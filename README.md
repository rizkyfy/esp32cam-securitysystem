# esp32cam-securitysystem
"Sistem Keamanan IoT Berbasis ESP32-CAM Terintegrasi Telegram dan Google Drive"
Adalah Sebuah sistem keamanan berbasis IOT dengan memanfaatkan sistem kamera yang ada pada ESP32-Cam dan juga menggunakan sistem sensor gerakan Pada Pir Sensor.

🌟 Fitur Utama
- Live Streaming - Menampilkan live streaming video dari kamera ESP32-CAM pada halaman web.
- Motion Detection - Menampilkan live streaming video dari kamera ESP32-CAM pada halaman web.
- Auto Upload - Secara otomatis mengunggah foto hasil deteksi gerakan ke Google Drive.
- Telegram Notification - Memberikan notifikasi melalui Telegram saat perangkat online.
- Web Interface - Memungkinkan pengguna untuk me-refresh stream video dan mengaktifkan mode fullscreen melalui tombol pada halaman web.
- Fullscreen Mode - Tampilan layar penuh untuk monitoring
- Real-time Clock - Menampilkan tanggal dan waktu pada stream

🔧 Fungsi-Fungsi Utama
- Koneksi WiFi: Menghubungkan ESP32-CAM ke jaringan WiFi menggunakan SSID dan password yang telah ditentukan.
- Inisialisasi Kamera: Menginisialisasi modul kamera ESP32-CAM dengan konfigurasi yang sesuai, seperti resolusi, format gambar, dan kualitas JPEG.
- Web Server: Membuat web server yang memungkinkan akses ke stream video dari kamera melalui browser web.
- Streaming Video: Menyediakan stream video secara langsung dari kamera ESP32-CAM ke halaman web.
- Deteksi Gerakan: Menggunakan sensor PIR untuk mendeteksi gerakan. Jika gerakan terdeteksi, sistem akan mengambil foto dan mengunggahnya ke Google Drive.
- Upload Foto ke Google Drive: Mengunggah foto yang diambil oleh kamera ke Google Drive menggunakan script Google Apps Script.
- Notifikasi Telegram: Mengirim notifikasi ke bot Telegram saat ESP32-CAM berhasil terhubung ke WiFi.

🛠️ Komponen yang Dibutuhkan
1. ESP32-CAM (AI Thinker)
2. Sensor PIR HC-SR501
3. Adaptor 5V
4. Kabel jumper

📋 Library yang Diperlukan
- WiFi.h
- WebServer.h
- esp_camera.h
- HTTPClient.h
- base64.h
- UniversalTelegramBot.h
- WiFiClientSecure.h

🔧 Instalasi Libary 
- Arduino IDE Setup
Pastikan Anda sudah menginstall:
Arduino IDE
ESP32 Board Manager
Library yang diperlukan (bisa diinstall via Library Manager)

🤖 Konfigurasi Telegram Bot
1. Buka Telegram dan cari @BotFather
2. Ketik /newbot dan ikuti instruksi
3. Simpan Bot Token yang diberikan
4. Cari @myidbot dan ketik /getid untuk mendapatkan Chat ID

📂Setup Google Drive Upload
1. Buka Google Apps Script
2. Masukan Kode yang sudah di berikan
3. Terapkan Lalu di Deployment baru
4. Masukan Kode atau Kredensial ke code Arduino nya

🚀 Cara Penggunaan
1. Upload kode ke ESP32-CAM
2. Buka Serial Monitor (115200 baud rate)
3. Tunggu hingga terhubung ke WiFi
4. Catat IP Address yang muncul
5. Buka browser dan akses http://IP_ADDRESS
6. Nikmati live streaming CCTV Anda!

📄 Lisensi
Project ini menggunakan lisensi MIT - lihat file LICENSE untuk detail.

👨‍💻 Author
Rizky Febriandi Y

GitHub: @rizkyfy
Email: rizkyfy49@gmail.com

🙏 Acknowledgments

ESP32-CAM Community
UniversalTelegramBot Library
Google Apps Script Documentation


⭐ Jika project ini membantu Anda, jangan lupa untuk memberikan star!

Made with ❤️ using ESP32-CAM
