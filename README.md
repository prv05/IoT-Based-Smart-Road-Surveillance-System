# Smart Road Surveillance System

IoT-based road surveillance using Edge AI, TinyML, and Cloud Computing.

A prototype system that combines edge-based computer vision, IoT communication, and cloud monitoring to detect potholes, vehicles, and ambulances in real time, and to make automated road-status decisions.

## Overview

An **ESP32-CAM** running a lightweight **FOMO** (Faster Objects, More Objects) model via **Edge Impulse** performs on-device inference to detect potholes, vehicles, and ambulances. Results are sent to **Firebase Realtime Database** and displayed on a web dashboard, which can also send administrative commands (e.g. speed-limit overrides) back to the device.

This is a proof-of-concept validating an edge-to-cloud architecture that can later scale to production-grade cameras and edge-AI hardware.

```
Camera → Edge AI Inference → Decision Logic → Firebase → Web Dashboard → Admin Control
                                    ↑                                          |
                                    └──────────────── override ────────────────┘
```

## Features

- Pothole detection
- Vehicle detection and counting
- Ambulance (emergency vehicle) detection
- Dynamic speed-limit logic based on traffic density
- Local alerts via OLED display and buzzer
- Real-time Firebase sync with a web dashboard
- Manual speed-limit override from the dashboard

## Tech Stack

| Layer | Tools |
|---|---|
| Hardware | ESP32-CAM, SH1106 OLED, buzzer |
| Firmware | Arduino IDE, C++ |
| ML | Edge Impulse, FOMO model |
| Backend | Firebase Realtime Database |
| Dashboard | HTML, CSS, JavaScript |

## Getting Started

### Requirements
- Arduino IDE with ESP32 board support
- ESP32-CAM
- Firebase account
- Edge Impulse Arduino library

### Setup
1. Clone the repository.
2. Open `firmware/` in Arduino IDE and install required libraries.
3. Import the Edge Impulse model library.
4. Configure your Wi-Fi and Firebase credentials.
5. Select the ESP32-CAM board, then compile and upload.
6. Open `dashboard/` and connect it to your Firebase instance.

> Do not commit Wi-Fi passwords, Firebase credentials, or API keys.

## Project Structure

```
smart-road-surveillance/
├── firmware/       # ESP32-CAM code + Edge Impulse model
├── dashboard/       # Web dashboard (HTML/CSS/JS)
├── firebase/        # Database structure
├── docs/            # Architecture diagrams, report
└── demo/            # Sample detection images
```

## Limitations

This is a prototype, not a production system:
- ESP32-CAM has limited compute and field of view
- Detection accuracy depends on training data and conditions
- Not tested across real multi-lane, multi-weather road environments


## Roadmap

- Multi-camera, multi-lane support
- Upgrade to Jetson/Raspberry Pi-class edge hardware
- Larger CV models with vehicle classification and tracking
- GPS-based pothole localization
- Number-plate recognition and speed estimation

## contributor

**Pratham Vernekar**
Computer Science and Engineering, RV University

**Rakshitha N Badiger**
Computer Science and Engineering, RV University

**Nandan Kumar S**
Computer Science and Engineering, RV University

## License

Developed for academic and research purposes. Add an open-source license if this repository is intended for public reuse.
