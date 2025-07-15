#include <Wire.h>
#include <MPU6050_light.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi และ Firebase
#define WIFI_SSID "YOUR"
#define WIFI_PASSWORD "YOUR"
#define API_KEY "YOUR"
#define DATABASE_URL "YOUR"
#define USER_EMAIL "YOUR"
#define USER_PASSWORD "YOUR"

// Struct สำหรับเก็บข้อมูล accelerometer
struct AccData {
  float x;
  float y;
  float z;
  unsigned long timestamp;
};

#define MAX_SAMPLES 200

// Queue สำหรับ double buffering
QueueHandle_t bufferQueueA;
QueueHandle_t bufferQueueB;
QueueHandle_t writeQueue;
QueueHandle_t sendQueue;

volatile bool readyToSend = false;

SemaphoreHandle_t switchMutex;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

MPU6050 mpu(Wire);

void setup() {
  Serial.begin(115200);

  // เชื่อมต่อ WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Firebase Init
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // เริ่ม MPU6050
  Wire.begin();
  byte status = mpu.begin();
  while (status != 0) {
    Serial.println("MPU6050 not connected!");
    delay(1000);
  }
  mpu.calcGyroOffsets();

  // สร้าง Queue และ Mutex
  bufferQueueA = xQueueCreate(MAX_SAMPLES, sizeof(AccData));
  bufferQueueB = xQueueCreate(MAX_SAMPLES, sizeof(AccData));
  writeQueue = bufferQueueA;
  sendQueue = bufferQueueB;

  switchMutex = xSemaphoreCreateMutex();

  // สร้าง Task
  xTaskCreatePinnedToCore(TaskSensor, "TaskSensor", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskUploader, "TaskUploader", 10000, NULL, 1, NULL, 1);
}

void TaskSensor(void* pvParameters) {
  while (true) {
    mpu.update();

    AccData data;
    data.timestamp = millis();
    data.x = mpu.getAccX() * 9.81;
    data.y = mpu.getAccY() * 9.81;
    data.z = mpu.getAccZ() * 9.81;

    // ลองใส่ลง Queue
    if (xQueueSend(writeQueue, &data, 0) == errQUEUE_FULL) {
      // ถ้าเต็ม ให้สลับ buffer
      if (xSemaphoreTake(switchMutex, portMAX_DELAY)) {
        if (!readyToSend) {
          QueueHandle_t temp = writeQueue;
          writeQueue = sendQueue;
          sendQueue = temp;
          readyToSend = true;
          Serial.println("Buffer switched! Ready to send.");
        }
        xSemaphoreGive(switchMutex);
      }

      // หลังสลับ buffer ใส่ใหม่แบบ blocking
      xQueueSend(writeQueue, &data, portMAX_DELAY);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);  // sampling ทุก 10 ms
  }
}

void TaskUploader(void* pvParameters) {
  AccData data;
  FirebaseJsonArray jsonArray;

  while (true) {
    if (readyToSend) {
      Serial.println("Upload started");
      unsigned long startTime = millis();

      if (xSemaphoreTake(switchMutex, portMAX_DELAY)) {
        while (xQueueReceive(sendQueue, &data, 0) == pdTRUE) {
          FirebaseJson jsonItem;
          jsonItem.set("timestamp", data.timestamp);
          jsonItem.set("X", data.x);
          jsonItem.set("Y", data.y);
          jsonItem.set("Z", data.z);
          jsonArray.add(jsonItem);
        }
        readyToSend = false;
        xSemaphoreGive(switchMutex);
      }

      if (Firebase.RTDB.push(&fbdo, "/sensor/batchAcceleration", &jsonArray)) {
        Serial.println("Batch upload success!");
      } else {
        Serial.print("Batch upload failed: ");
        Serial.println(fbdo.errorReason());
      }

      jsonArray.clear();
      Serial.printf("Upload took %lu ms\n", millis() - startTime);
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);  // เว้นระยะรอ
  }
}

void loop() {
}
