# Smart Road Surveillance System

## IoT-Based Road Surveillance Using Edge AI, TinyML and Cloud Computing

A prototype intelligent road-surveillance system that combines edge-based computer vision, IoT communication, cloud monitoring, and automated road-status decisions.

The prototype uses an ESP32-CAM and a lightweight FOMO (Faster Objects, More Objects) model deployed through Edge Impulse to detect potholes, vehicles, and ambulances. Detection results are processed locally and transmitted to Firebase Realtime Database, where they can be monitored through a web dashboard.

The project is designed as a proof-of-concept for a scalable real-world road-surveillance architecture. The ESP32-CAM is used to validate the concept at prototype scale; a practical deployment would use higher-resolution cameras, more capable edge-computing hardware, and larger computer-vision models.

---

## 1. Project Overview

Road infrastructure requires continuous monitoring for conditions such as potholes, traffic congestion, and emergency-vehicle movement.

Traditional road monitoring can depend heavily on manual inspection or centralized processing. This project explores an alternative architecture in which initial visual processing is performed close to the camera using edge AI.

The prototype demonstrates a complete pipeline:

    Road Scene
        |
        v
    Camera
        |
        v
    Edge AI Inference
        |
        +---- Pothole Detection
        |
        +---- Vehicle Detection
        |
        +---- Ambulance Detection
        |
        v
    Decision Logic
        |
        +---- Traffic Information
        +---- Speed Limit Decision
        +---- Local Alerts
        |
        v
    Firebase Realtime Database
        |
        v
    Web Dashboard
        |
        v
    Administrative Control
        |
        v
    ESP32

The system therefore demonstrates both edge-to-cloud monitoring and cloud-to-edge control.

---

# 2. Objectives

The main objectives of the project are:

- Detect potholes using computer vision.
- Detect and count vehicles.
- Detect emergency vehicles such as ambulances.
- Demonstrate traffic-density estimation.
- Generate a dynamic speed-limit decision based on detected traffic conditions.
- Provide local alerts through an OLED display and buzzer.
- Send detection and road-status information to Firebase.
- Provide a web-based monitoring dashboard.
- Support administrative speed-limit overrides.
- Demonstrate an architecture that can be scaled beyond the prototype hardware.

---

# 3. Prototype Implementation

The current implementation uses an ESP32-CAM as the sensing and edge-computing device.

### Prototype Hardware

- ESP32-CAM
- SH1106 1.3-inch OLED
- Buzzer
- Breadboard
- Power supply
- Connecting wires

### Prototype Software

- Arduino IDE
- C++
- Edge Impulse
- FOMO object-detection model
- Firebase Realtime Database
- HTML
- CSS
- JavaScript

The ESP32-CAM captures images from the road scene and performs inference using the deployed TinyML model.

The detected objects are then used by the system's decision logic to determine the appropriate road status.

---

# 4. Machine Learning

The prototype uses the FOMO (Faster Objects, More Objects) object-detection model through Edge Impulse.

The current detection classes are:

    Pothole
    Vehicle
    Ambulance

The model is converted into an Arduino-compatible library and deployed directly on the ESP32-CAM.

The basic inference pipeline is:

    Camera Frame
        |
        v
    Image Processing
        |
        v
    FOMO Model
        |
        v
    Object Detection
        |
        +---- Pothole
        +---- Vehicle
        +---- Ambulance

The important characteristic of this implementation is that inference is performed on the edge device rather than sending every camera frame to a remote server for processing.

---

# 5. System Architecture

The system consists of three primary layers.

## 5.1 Edge Layer

The edge layer contains the ESP32-CAM and connected peripherals.

Responsibilities include:

- Image acquisition
- ML inference
- Object detection
- Vehicle counting
- Traffic-status calculation
- Local alert generation
- Communication with Firebase

The OLED and buzzer provide immediate local feedback.

---

## 5.2 Communication Layer

The ESP32 communicates with Firebase using Wi-Fi.

Detection and road-status information is transmitted to Firebase in a structured format.

The communication is bidirectional.

### ESP32 to Cloud

    ESP32
      |
      v
    Firebase
      |
      v
    Dashboard

### Cloud to ESP32

    Dashboard
      |
      v
    Firebase
      |
      v
    ESP32

This allows the cloud to be used not only for monitoring but also for sending administrative commands back to the edge device.

---

## 5.3 Application Layer

The application layer consists of Firebase and the web dashboard.

The dashboard can display information such as:

- Vehicle count
- Traffic condition
- Current speed limit
- Pothole detection
- Ambulance detection
- Road-status information

The administrator can also provide a manual speed-limit override through Firebase.

---

# 6. Dynamic Speed-Limit Logic

Vehicle detections are used to estimate traffic density.

The prototype follows the general process:

    Vehicle Detection
          |
          v
    Vehicle Count
          |
          v
    Traffic Density
          |
          v
    Speed Limit Decision
          |
          v
    Firebase + Dashboard

The system also supports a manual override.

When a manual speed value is provided through Firebase, the ESP32 can use the administrator-defined value instead of the automatically calculated value.

This demonstrates a basic closed-loop IoT control architecture.

---

# 7. Real-World Deployment Approach

The ESP32-CAM implementation is intentionally treated as a prototype rather than as the final hardware architecture.

An ESP32-CAM has limited computational resources, limited camera capability, and a restricted field of view. These limitations make it unsuitable for directly monitoring a complete real-world multi-lane road environment at production scale.

The purpose of the prototype is therefore to validate the underlying architecture and demonstrate that the complete edge-to-cloud workflow can operate on constrained hardware.

A realistic deployment would replace the prototype sensing and computing layer with infrastructure designed for real road environments.

## 7.1 Real-World Camera System

Instead of a single ESP32-CAM, roadside installations could use:

- High-resolution IP cameras
- Industrial cameras
- Wide-angle cameras
- Multiple cameras covering different lanes
- Cameras with suitable night-time and weather performance

The exact camera configuration would depend on:

- Road width
- Number of lanes
- Camera mounting height
- Required detection distance
- Lighting conditions
- Weather conditions
- Required detection accuracy

For example, a multi-lane road would generally require a camera configuration capable of covering the complete area of interest rather than relying on the narrow field of view of a small embedded camera.

---

## 7.2 Real-World Edge Computing

The ESP32 would be replaced by a more powerful edge-computing platform.

A realistic architecture could use hardware such as:

- NVIDIA Jetson-class edge devices
- Raspberry Pi-class systems with suitable AI accelerators
- Industrial edge-AI computers
- GPU/NPU-enabled embedded systems

The edge device would receive video from the cameras and perform computer-vision inference locally.

The architecture would therefore become:

    Roadside Cameras
           |
           v
    Edge AI Computer
           |
           v
    Computer Vision Models
           |
           +---- Vehicle Detection
           +---- Pothole Detection
           +---- Ambulance Detection
           +---- Traffic Analysis
           |
           v
    Decision Engine
           |
           v
    Cloud Platform
           |
           v
    Monitoring Dashboard

---

# 8. Real-World AI Model

The FOMO model is suitable for demonstrating lightweight object detection on constrained hardware.

For a real deployment, the model would likely need to be replaced or supplemented by a more capable computer-vision architecture.

A larger model could provide improved capability for:

- Vehicle detection
- Vehicle classification
- Pothole detection
- Emergency-vehicle detection
- Multi-object tracking
- Lane-level analysis
- Traffic-flow analysis

The model selection would depend on the required accuracy, inference latency, hardware capability, and deployment cost.

The important architectural principle remains the same:

    Camera
       |
       v
    Edge Inference
       |
       v
    Detection
       |
       v
    Decision

The prototype demonstrates this principle using a lightweight model, while the real-world implementation can replace the model without fundamentally changing the overall architecture.

---

# 9. Realistic Road Deployment Architecture

A possible real-world architecture is:

                         ROAD ENVIRONMENT
                                |
                 +--------------+--------------+
                 |                             |
                 v                             v
          Roadside Camera 1              Roadside Camera 2
                 |                             |
                 +--------------+--------------+
                                |
                                v
                       EDGE AI COMPUTER
                                |
             +------------------+------------------+
             |                  |                  |
             v                  v                  v
        Vehicle Model     Pothole Model    Emergency Model
             |                  |                  |
             +------------------+------------------+
                                |
                                v
                         Decision Engine
                                |
              +-----------------+----------------+
              |                 |                |
              v                 v                v
        Traffic Analysis   Road Alerts      Speed Logic
              |                 |                |
              +-----------------+----------------+
                                |
                                v
                         Cloud Platform
                                |
                                v
                         Web Dashboard
                                |
                                v
                      Traffic Administrator

The prototype developed in this project represents the smaller-scale version of this architecture.

---

# 10. Prototype-to-Real-World Mapping

| Prototype | Real-World Equivalent |
|---|---|
| ESP32-CAM | Edge AI computer |
| Small camera | High-resolution roadside camera |
| FOMO TinyML model | Larger computer-vision model |
| Single camera | Multiple cameras where required |
| Miniature road environment | Actual road infrastructure |
| Firebase Realtime Database | Cloud/IoT backend |
| Web dashboard | Traffic-management dashboard |
| OLED | Local/roadside information system |
| Buzzer | Appropriate real-world alert mechanism |
| Prototype speed logic | Production traffic-management logic |

The purpose of this mapping is to show that the prototype is not intended to claim production-scale performance from ESP32 hardware. Instead, it validates the architecture and core workflow that can later be implemented using production-grade hardware.

---

# 11. Edge Processing vs Cloud Processing

A major design consideration is where computer-vision processing should occur.

### Cloud-Only Processing

    Camera
       |
       v
    Internet
       |
       v
    Cloud AI
       |
       v
    Result

This approach can require continuous transmission of video and may introduce additional network dependency and latency.

### Edge Processing

    Camera
       |
       v
    Edge AI
       |
       v
    Detection
       |
       +-----------> Cloud
       |
       +-----------> Local Decision

The proposed architecture follows the second approach.

Only relevant detection and road-status information needs to be transmitted to the cloud rather than continuously sending the complete camera stream for every decision.

The prototype demonstrates this principle using TinyML on the ESP32-CAM.

---

# 12. Firebase Data Flow

The prototype uses Firebase Realtime Database for communication between the edge device and dashboard.

A simplified structure is:

    road_status/
    |
    +-- vehicle_count
    |
    +-- traffic_density
    |
    +-- speed_limit
    |
    +-- manual_speed_override
    |
    +-- pothole_detected
    |
    +-- ambulance_detected

The exact database structure can be modified as the system evolves.

---

# 13. Project Workflow

The complete prototype workflow is:

1. ESP32-CAM captures the road scene.
2. The image is processed locally.
3. The FOMO model performs object detection.
4. Detected vehicles are counted.
5. Pothole and ambulance detections generate alerts.
6. Traffic information is processed by the decision logic.
7. A speed-limit value is generated.
8. The OLED and buzzer provide local feedback.
9. Road-status information is sent to Firebase.
10. The dashboard displays the information.
11. An administrator can provide a manual speed override.
12. The ESP32 receives and applies the override.

---

# 14. Project Structure

    smart-road-surveillance/
    |
    +-- README.md
    |
    +-- firmware/
    |   +-- smart_road_surveillance.ino
    |   +-- model/
    |       +-- edge-impulse-model.h
    |
    +-- dashboard/
    |   +-- index.html
    |   +-- style.css
    |   +-- script.js
    |
    +-- firebase/
    |   +-- database-structure.json
    |
    +-- dataset/
    |   +-- README.md
    |
    +-- docs/
    |   +-- architecture.png
    |   +-- flowchart.png
    |   +-- circuit-diagram.png
    |   +-- project-report.pdf
    |
    +-- demo/
    |   +-- pothole-detection.jpg
    |   +-- vehicle-detection.jpg
    |   +-- ambulance-detection.jpg
    |   +-- dashboard.jpg
    |
    +-- .gitignore
    +-- LICENSE

---

# 15. Installation

## Requirements

- Arduino IDE
- ESP32 board support
- ESP32-CAM
- Firebase account
- Edge Impulse Arduino library
- Required Arduino libraries
- Web browser

## Firmware Setup

1. Clone the repository.

2. Open the firmware project in Arduino IDE.

3. Install the required libraries.

4. Import the Edge Impulse model library.

5. Configure Wi-Fi credentials.

6. Configure Firebase credentials.

7. Select the appropriate ESP32-CAM board.

8. Compile and upload the firmware.

9. Open the Serial Monitor to verify operation.

## Dashboard Setup

Open the dashboard files and configure the Firebase connection.

Do not commit private Firebase credentials, Wi-Fi passwords, API keys, or service-account files to the repository.

---

# 16. Limitations

The current implementation is a prototype and has several limitations.

### Hardware

- ESP32-CAM has limited computational resources.
- The camera has limited field of view and image quality compared with dedicated road-surveillance cameras.
- The prototype does not provide complete real-road coverage.

### Machine Learning

- The lightweight model is constrained by embedded hardware.
- Detection performance depends on the training dataset and operating conditions.
- Real-world deployment would require substantially more testing across different lighting, weather, road surfaces, camera angles, and traffic conditions.

### Infrastructure

- The prototype uses a controlled environment.
- It does not represent a complete traffic-management infrastructure.
- Production deployment would require appropriate networking, power, physical protection, monitoring, security, and maintenance.

These limitations are expected for a proof-of-concept implementation.

---

# 17. Future Development

Future versions can extend the prototype toward a real-world system through:

- Higher-resolution roadside cameras
- Multiple camera deployment
- More powerful edge-AI hardware
- Larger computer-vision models
- Multi-object tracking
- GPS-based pothole localization
- Historical road-condition analytics
- Vehicle classification
- Number-plate recognition
- Speed estimation
- Lane-level traffic analysis
- Improved emergency-vehicle detection
- Distributed edge nodes
- Integration with larger traffic-management systems
- V2X communication

---

# 18. Research Significance

The primary purpose of this project is to demonstrate the feasibility of combining:

    IoT
    +
    Edge AI
    +
    TinyML
    +
    Computer Vision
    +
    Cloud Computing
    +
    Real-Time Monitoring

The ESP32-CAM implementation provides a constrained environment in which the proposed architecture can be experimentally validated.

The architecture is designed to be hardware-independent at a higher level. The sensing and inference layer can be upgraded from an ESP32-CAM prototype to production-grade cameras and edge-AI hardware while retaining the core concepts of local inference, decision-making, cloud monitoring, and remote control.

---

# 19. Prototype vs Production

This project should be understood as a prototype implementation rather than a production-ready traffic-management system.

The prototype answers the question:

> Can a low-cost edge device demonstrate an integrated road-surveillance pipeline combining computer vision, IoT communication, cloud monitoring, and remote control?

A real-world implementation would address a different engineering scale:

> How can the same architecture be deployed reliably across real roads using appropriate cameras, edge-AI hardware, networking, models, security, and traffic-management infrastructure?

The current project focuses on validating the first stage while providing an architecture that can be extended toward the second.

---

# 20. Author

Pratham Vernekar

Computer Science and Engineering  
RV University

---

# 21. License

This project is developed for academic and research purposes.

Add an open-source license if the repository is intended for public reuse.
