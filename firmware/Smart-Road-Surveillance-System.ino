/*
 * Smart Road Surveillance System - Main ESP32-CAM Code
 *
 * This code combines:
 * 1. Edge Impulse ML Model (Object Detection)
 * 2. Wi-Fi Connectivity
 * 3. Firebase Realtime Database integration (for dashboard)
 * 4. SSD1306 OLED Display (for driver alerts)
 * 5. Buzzer (for audible alerts)
 *
 * It automatically detects vehicles, potholes, and emergency vehicles,
 * calculates a dynamic speed limit, and sends all data to a web dashboard.
 *
 * NOTE: This version (v4) REMOVES the Firebase stream (admin override listener)
 * to fix compilation errors with older Firebase_ESP_Client libraries.
 * The dashboard override button will not work with this code.
 */

// --- START OF USER CONFIGURATION ---

// 1. This is your Edge Impulse project header
#include <PRATHAM-Smart_Road_inferencing.h> 

// This include is required for 'ei::image'
#include "edge-impulse-sdk/dsp/image/image.hpp"

// 2. Your Wi-Fi credentials
#define WIFI_SSID "PRV11R"
#define WIFI_PASSWORD "PRAT2005"

// 3. Your Firebase project details
#define API_KEY "AIzaSyCy5BEx_fMiO3q0tN4EEXWHxXsxLFkjKvk"
#define DATABASE_URL "https://pratham-smart-road-default-rtdb.firebaseio.com"

// --- END OF USER CONFIGURATION ---

/* --- Hardware & Library Includes --- */
#include <WiFi.h>
#include "esp_camera.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h> // <-- OLD DRIVER
#include <Adafruit_SH110X.h>    // <-- NEW DRIVER (for SH1106 / SH110X chips)

// Helper functions for Firebase
// *** FIX ***
// Removed "addons/" prefix. This might fix the "No such file or directory" error
// if your library version has these files in the root 'src' folder.
#include "C:\\Users\\prath\\Documents\\Arduino\\libraries\\Firebase_ESP32_Client\\src\\addons\\TokenHelper.h"
#include "C:\\Users\\prath\\Documents\\Arduino\\libraries\\Firebase_ESP32_Client\\src\\addons\\RTDBHelper.h"

/* --- Edge Impulse Camera Settings --- */
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#else
#error "Camera model not selected"
#endif

// Camera buffer settings
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS   320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS   240
#define EI_CAMERA_FRAME_BYTE_SIZE         3
uint8_t *snapshot_buf; //points to the output of the capture

/* --- Hardware Pin Definitions --- */
// OLED Display (I2C)
#define OLED_SDA 14 // GPIO 14
#define OLED_SCL 15 // GPIO 15
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // <-- OLD DRIVER
Adafruit_SH1106G display(128, 64, &Wire, -1);  // <-- NEW DRIVER

// Buzzer
#define BUZZER_PIN 13

/* --- Firebase Objects --- */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool firebase_ready = false;
// Admin override variables are removed as we are not using the stream

/* --- Project Logic Variables --- */
#define CHECK_INTERVAL 2000
unsigned long last_check_time = 0;

int vehicleCount = 0;
int potholeCount = 0;
int ambulanceCount = 0;

bool potholeAlertActive = false;
bool emergencyAlertActive = false;

// FIX: Removed 'struct' from this line
ei_impulse_result_bounding_box_t* bounding_boxes = nullptr;

size_t bounding_box_count = 0;
int ei_err = 0;

/* --- Edge Impulse Camera Config --- */
static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_QVGA,
    .jpeg_quality = 12,
    .fb_count = 1,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/* --- Function Declarations --- */
bool ei_camera_init(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
void update_firebase_and_display(void);
void run_classifier(void);
void play_alert_sound(void);

// Stream callbacks are removed

/******************************************************************
 *
 * SETUP FUNCTION
 *
 ******************************************************************/
void setup() {
    Serial.begin(115200);
    Serial.println("Smart Road System Initializing...");

    // 1. Initialize Hardware (OLED, Buzzer)
    Wire.begin(OLED_SDA, OLED_SCL);
    // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // <-- OLD DRIVER
    if (!display.begin(0x3C, true)) { // <-- NEW DRIVER (Address 0x3C, reset)
        // Serial.println(F("SSD1306 allocation failed")); // <-- OLD
        Serial.println(F("SH110X allocation failed")); // <-- NEW
    } else {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(0, 0);
        display.println("Initializing...");
        display.display();
    }
    
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    // 2. Initialize Camera
    if (ei_camera_init() == false) {
        Serial.println("Failed to initialize Camera!");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("FATAL: Camera Init");
        display.println("Failed. Check pins.");
        display.display();
        while(1); // Halt
    } else {
        Serial.println("Camera initialized");
    }

    // 3. Connect to Wi-Fi
    Serial.print("Connecting to Wi-Fi: ");
    Serial.print(WIFI_SSID);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connecting to:");
    display.println(WIFI_SSID);
    display.display();
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    display.println("WiFi Connected!");
    display.display();
    delay(1000);

    // 4. Initialize Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);
    
    // Stream callback is removed
    // Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);

    // Initialize Firebase
    Firebase.begin(&config, &auth);
    
    // *** START OF FIX ***
    // We must explicitly sign in to get an auth token
    Serial.println("Signing in to Firebase...");
    display.println("Signing in...");
    display.display();
    
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Firebase sign-in OK");
        firebase_ready = true; // Now we are ready
    } else {
        Serial.printf("Firebase sign-in failed: %s\n", fbdo.errorReason().c_str());
        display.println("FB Sign-in FAILED");
        display.display();
        firebase_ready = false;
    }
    // *** END OF FIX ***

    // 5. Start Firebase Stream (REMOVED)
    // We are no longer listening for override commands
    Serial.println("Firebase initialized.");
    
    // 6. Final confirmation message
    Serial.println("Firebase and services initialized. System is running.");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("SYSTEM ONLINE");
    display.println("Looking for...");
    display.println("Vehicles");
    display.println("Potholes");
    display.println("Ambulances");
    display.display();
    
    last_check_time = millis();
}


/******************************************************************
 *
 * MAIN LOOP
 *
 ******************************************************************/
void loop() {
    
    if (millis() - last_check_time > CHECK_INTERVAL) {
        last_check_time = millis();

        // 1. Run the classifier to get object counts
        run_classifier();

        // 2. Process data, update display, and send to Firebase
        if (ei_err == EI_IMPULSE_OK) {
            update_firebase_and_display();
        } else {
            Serial.printf("Classifier failed with error %d\n", ei_err);
        }
    }
    
    // Short delay to keep things stable
    delay(10);
}


/******************************************************************
 *
 * PROCESS DATA & UPDATE CLOUD
 *
 ******************************************************************/
void update_firebase_and_display(void) {
    if (!firebase_ready) {
        Serial.println("Firebase not ready, skipping update.");
        return;
    }

    // --- 1. Decide Speed Limit ---
    int newSpeed = 65; // Default speed
    if (vehicleCount > 2) {
        newSpeed = 30; // Heavy traffic
    } else if (vehicleCount > 1) {
        newSpeed = 45; // Moderate traffic
    }
    
    // --- 2. Check for Alerts ---
    bool newPotholeAlert = false;
    bool newEmergencyAlert = false;

    if (potholeCount > 0 && !potholeAlertActive) {
        newPotholeAlert = true;
        potholeAlertActive = true; // Set flag so we only alert once
    } else if (potholeCount == 0) {
        potholeAlertActive = false; // Reset flag when clear
    }

    if (ambulanceCount > 0 && !emergencyAlertActive) {
        newEmergencyAlert = true;
        emergencyAlertActive = true;
    } else if (ambulanceCount == 0) {
        emergencyAlertActive = false;
    }
    
    // --- 3. Update OLED Display ---
    // Since stream is removed, we only show the new dynamic speed
    int displaySpeed = newSpeed; 
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    
    display.println("DYNAMIC SPEED"); // No longer checks for admin override

    display.setTextSize(3);
    display.setCursor(20, 20);
    display.print(displaySpeed);
    display.setTextSize(2);
    display.print(" ");
    display.print("mph");

    // Show alerts on bottom line
    display.setTextSize(1);
    display.setCursor(0, 55);
    if (newPotholeAlert || potholeAlertActive) {
        display.print("! POTHOLE AHEAD !");
    }
    if (newEmergencyAlert || emergencyAlertActive) {
        display.print("! EMERGENCY VEH !");
    }
    
    display.display();
    
    // --- 4. Trigger Buzzer ---
    if (newPotholeAlert || newEmergencyAlert) {
        play_alert_sound();
    }

    // --- 5. Send Data to Firebase ---
    FirebaseJson json;
    
    // A. Road Status (Speed and Vehicle Count)
    json.set("road_status/dynamic_speed_limit", newSpeed);
    json.set("road_status/current_vehicles", vehicleCount);
    // manual_speed_override is removed
    
    // B. Alerts Status (Counts)
    json.set("alerts/pothole_count", potholeCount);
    json.set("alerts/emergency_vehicle_count", ambulanceCount);
    
    // C. Latest Alert (for the feed)
    unsigned long timestamp = millis();
    if (newPotholeAlert) {
        json.set("alerts/latest_alert/message", "Pothole Detected");
        json.set("alerts/latest_alert/type", "pothole");
        json.set("alerts/latest_alert/timestamp", timestamp);
    } else if (newEmergencyAlert) {
        json.set("alerts/latest_alert/message", "Emergency Vehicle Nearby");
        json.set("alerts/latest_alert/type", "emergency");
        json.set("alerts/latest_alert/timestamp", timestamp);
    }

    // *** FIX: Removed & from fbdo and json ***
    if (!Firebase.updateNode(fbdo, F("/"), json)) {
        Serial.printf("Firebase update failed: %s\n", fbdo.errorReason().c_str());
    }
}
/******************************************************************
 *
 * FIREBASE STREAM CALLBACK (REMOVED)
 *
 ******************************************************************/
// All streamCallback functions are removed to fix compilation errors


/******************************************************************
 *
 * HARDWARE & ML HELPER FUNCTIONS
 *
 ******************************************************************/

void play_alert_sound(void) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
}

void run_classifier(void) {
    vehicleCount = 0;
    potholeCount = 0;
    ambulanceCount = 0;

    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
    if(snapshot_buf == nullptr) {
        ei_printf("ERR: Failed to allocate snapshot buffer!\n");
        ei_err = -1;
        return;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
        ei_printf("Failed to capture image\r\n");
        free(snapshot_buf);
        ei_err = -2;
        return;
    }

    ei_impulse_result_t result = { 0 };
    ei_err = run_classifier(&signal, &result, false);

    if (ei_err != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", ei_err);
        free(snapshot_buf);
        return;
    }

    bounding_box_count = result.bounding_boxes_count;
    for (uint32_t i = 0; i < bounding_box_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value < 0.6) { // Confidence threshold
            continue;
        }

        if (strcmp(bb.label, "vehicle") == 0) {
            vehicleCount++;
        }
        else if (strcmp(bb.label, "pothole") == 0) {
            potholeCount++;
        }
        else if (strcmp(bb.label, "emergency_vehicle") == 0) {
            ambulanceCount++;
        }
    }
    
    free(snapshot_buf);
}

bool ei_camera_init(void) {
    static bool is_initialised = false;
    if (is_initialised) return true;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x\n", err);
      return false;
    }
    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
    
    is_initialised = true;
    return true;
}

bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ei_printf("Camera capture failed\n");
        return false;
    }

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
    esp_camera_fb_return(fb);
    if(!converted){
        ei_printf("Conversion failed\n");
        return false;
    }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS) || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }

    if (do_resize) {
        // FIX: Added 'ei::image::processing' namespace
        ei::image::processing::crop_and_interpolate_rgb888(
        out_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        out_buf,
        img_width,
        img_height);
    }
    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;
    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }
    return 0;
}