# predictive_maintenance_2025

An IoT + AI project that detects early signs of equipment failure using ESP32 and ML models.

## Folder Structure

- `IoT_firmware/`: Arduino code for ESP32 with MPU6050 and Firebase upload.
- `signal_processing/`: Python scripts for filtering and analyzing acceleration signals.
- `AI-ML/`: Training scripts and models for predicting faults.
- `web/`: Web interface to visualize real-time sensor data.
- `docs/`: System architecture diagrams and documentation.
- `data/`: Contains system architecture diagrams, flowcharts, and other relevant documentation that describe the design and structure of the predictive maintenance project.  
This folder serves as a central place for visual aids and technical documents to help team members and stakeholders understand the overall system.


## Tech Stack

- **Hardware**: ESP32 WROOM 32, MPU6050
- **Cloud**: Firebase Realtime Database
- **AI/ML**: ค่อยใส่ยังไม่รู้
- **Web**: ไม่รู้

## Getting Started

```bash
git clone https://github.com/Theolnwzaa007/predictive_maintenance_2025.git
cd ml_model
pip install -r requirements.txt
python train_model.py
