// Include dan Libarry
#include <WiFi.h>
#include <WebServer.h>
#include <esp_camera.h>
#include <HTTPClient.h>
#include <base64.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

// Konfigurasi Telegram
#define BOT_TOKEN "" // Bot Token Telegram (Generate memakai bot di telegram)
#define CHAT_ID "" // Chat id atau grup yang akan di jadikan tempat notifikasi
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Konfigurasi WiFi
const char* ssid = ""; // SSID Wifi rumah atau hotspot
const char* password = ""; // Password Wifi

// Google Drive Upload
const char* googleScriptUrl = ""; // Ini kredensial pada google script yang udah di buat atau id penerapan nya ( Note Harus pake yang https://script.google.com/macros/s/Codenya/exec )

// Camera
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// PIR Sensor
#define PIR_PIN 13 // Sesuaikan aja pin dari Pin sensor ini disini saya pake 13
unsigned long lastDetectionTime = 0;
const unsigned long detectionDelay = 2000;
const unsigned long cooldownPeriod = 10000;
bool canDetect = true;

// Web Server
WebServer server(80);
bool streamActive = false;

// Fungsi unggah foto ke Google Drive
void uploadPhotoToGoogleDrive(const uint8_t* image, size_t len) {
  HTTPClient http;
  http.begin(googleScriptUrl);
  http.addHeader("Content-Type", "application/json");

  String base64Image = base64::encode(image, len);
  String payload = "{\"image\": \"" + base64Image + "\"}";

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response: %d\n", httpResponseCode);
    Serial.println(http.getString());
  } else {
    Serial.printf("HTTP POST gagal: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}

// Fungsi mengambil dan mengunggah foto
void captureAndUploadPhoto() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Kamera gagal menangkap gambar.");
    return;
  }
  Serial.println("Foto berhasil diambil, mengunggah ke Google Drive...");
  uploadPhotoToGoogleDrive(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

// HTML webpage
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <title>ESP32-CAM CCTV System</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: 'Courier New', monospace;
      background-color: #000;
      margin: 0;
      padding: 20px;
      color: white;
    }

    .container {
      max-width: 1200px;
      margin: 0 auto;
    }

    .header {
      background-color: #1a1a1a;
      color: #00ff00;
      padding: 15px;
      text-align: center;
      border-radius: 5px;
      margin-bottom: 20px;
      font-size: 24px;
      text-transform: uppercase;
      letter-spacing: 2px;
    }

    .video-container {
      position: relative;
      background-color: #000;
      border: 2px solid #333;
      border-radius: 5px;
      overflow: hidden;
      margin-bottom: 20px;
    }

    .video-overlay {
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      padding: 10px;
      background-color: rgba(0, 0, 0, 0.7);
      color: #00ff00;
      font-family: 'Courier New', monospace;
      font-size: 14px;
      display: flex;
      justify-content: space-between;
      z-index: 10;
    }

    .stream-container {
      width: 100%;
      position: relative;
    }

    #stream {
      width: 100%;
      display: block;
    }

    .controls {
      background-color: #1a1a1a;
      padding: 15px;
      border-radius: 5px;
      text-align: center;
    }

    button {
      background-color: #333;
      color: #00ff00;
      border: 1px solid #00ff00;
      padding: 10px 20px;
      margin: 5px;
      cursor: pointer;
      font-family: 'Courier New', monospace;
      text-transform: uppercase;
      transition: all 0.3s ease;
    }

    button:hover {
      background-color: #00ff00;
      color: #000;
    }

    .status {
      position: absolute;
      top: 10px;
      right: 10px;
      padding: 5px 10px;
      background-color: rgba(0, 255, 0, 0.2);
      border: 1px solid #00ff00;
      color: #00ff00;
      border-radius: 3px;
    }

    .recording-dot {
      display: inline-block;
      width: 10px;
      height: 10px;
      background-color: #ff0000;
      border-radius: 50%;
      margin-right: 5px;
      animation: blink 1s infinite;
    }

    @keyframes blink {
      0% {
        opacity: 1;
      }

      50% {
        opacity: 0;
      }

      100% {
        opacity: 1;
      }
    }
  </style>
</head>

<body>
  <div class="container">
    <div class="header">
      ESP32-CAM CCTV System
    </div>

    <div class="video-container">
      <div class="video-overlay">
        <div id="datetime"></div>
        <div class="status">
          <span class="recording-dot"></span>
          LIVE
        </div>
      </div>
      <div class="stream-container">
        <img src="/stream" id="stream" alt="Loading stream...">
      </div>
    </div>

    <div class="controls">
      <button onclick="refreshStream()">Refresh Stream</button>
      <button onclick="toggleFullscreen()">Fullscreen</button>
    </div>
  </div>

  <script>
    function updateDateTime() {
      const now = new Date();
      const dateStr = now.toLocaleDateString('id-ID', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit'
      });
      const timeStr = now.toLocaleTimeString('id-ID', {
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
      });
      document.getElementById('datetime').textContent =
        `CAM01 | ${dateStr} ${timeStr}`;
    }

    function refreshStream() {
      const stream = document.getElementById('stream');
      stream.src = '/stream?' + new Date().getTime();
    }

    function toggleFullscreen() {
      const container = document.querySelector('.video-container');
      if (!document.fullscreenElement) {
        container.requestFullscreen().catch(err => {
          alert(`Error attempting to enable fullscreen: ${err.message}`);
        });
      } else {
        document.exitFullscreen();
      }
    }

    // Update date/time every second
    setInterval(updateDateTime, 1000);
    updateDateTime(); // Initial update

    // Auto-refresh stream every 5 minutes
    setInterval(refreshStream, 300000);

    // Handle stream errors
    document.getElementById('stream').onerror = function() {
      console.log("Stream error, reconnecting...");
      setTimeout(refreshStream, 5000);
    };
  </script>
</body>

</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleStream() {
  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n";
  response += "Connection: close\r\n"; // Menambahkan header Connection: close
  response += "\r\n";
  client.print(response);

  streamActive = true;

  while (client.connected() && streamActive) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      client.print("--frame\r\nContent-Type: text/plain\r\n\r\nCamera Error\r\n");
      delay(1000);
      continue;
    }

    client.print("--frame\r\n");
    client.print("Content-Type: image/jpeg\r\n");
    client.print("Content-Length: ");
    client.print(String(fb->len));
    client.print("\r\n\r\n");
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);

    // Ngecek sensor gerakan jika ada gerakan
    if (canDetect && digitalRead(PIR_PIN) == HIGH) {
      unsigned long currentTime = millis();
      if (currentTime - lastDetectionTime > detectionDelay) {
        Serial.println("Gerakan terdeteksi! Mengambil foto...");
        captureAndUploadPhoto();
        lastDetectionTime = currentTime;
        canDetect = false;
      }
    }

    // Cek cooldown pada sensor ( jika cooldown tidak di batasi maka akan terjadi penggulangan memotret pada esp32 cam )
    if (!canDetect && (millis() - lastDetectionTime > cooldownPeriod)) {
      canDetect = true;
      Serial.println("Sensor aktif kembali");
    }
    delay(100);
  }

  streamActive = false;
  Serial.println("Client disconnected from stream"); // Tambahkan pesan ini
}

void startCameraServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.enableCORS(true);
  server.begin();
  Serial.println("Camera server started");
}

void setup() {
  Serial.begin(115200);

  // Setup PIR sensor
  pinMode(PIR_PIN, INPUT);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int wifiRetries = 0;
  while (WiFi.status() != WL_CONNECTED && wifiRetries < 20) {
    delay(500);
    Serial.print(".");
    wifiRetries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Fungsi mengirim notifikasi jika esp32 online ke Telegram
  client.setInsecure();
  String message = "📷 ESP32-CAM Online!\nIP Address: ";
  message += WiFi.localIP().toString();
  bot.sendMessage(CHAT_ID, message, "");

  // Ini Settingan Esp32-Cam yang ringan ( Bisa di ubah aja sesuai kebutuhan dan selera )
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 5000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 5;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 5;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
    return;
  }

  startCameraServer();
  Serial.println("System ready");
}

void loop() {
  server.handleClient();
}
