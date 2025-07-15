# predictive_maintenance_2025

An IoT + AI project that detects early signs of equipment failure using ESP32 and ML models.

## Folder Structure

- `firmware/`: Arduino code for ESP32 with MPU6050 and Firebase upload.
- `signal_processing/`: Python scripts for filtering and analyzing acceleration signals.
- `ml_model/`: Training scripts and models for predicting faults.
- `web_app/`: Web interface to visualize real-time sensor data.
- `docs/`: System architecture diagrams and documentation.

## Tech Stack

- **Hardware**: ESP32, MPU6050
- **Cloud**: Firebase Realtime Database
- **AI/ML**: Python, scikit-learn, TensorFlow
- **Web**: HTML, JS (or Streamlit)

## Getting Started

```bash
git clone https://github.com/Theolnwzaa007/predictive_maintenance_2025.git
cd ml_model
pip install -r requirements.txt
python train_model.py
