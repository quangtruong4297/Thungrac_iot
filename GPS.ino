void setup_GPS() {


  Serial_GPS.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  DEBUG_PRINTLN("Serial_GPS (Serial 1) khoi tao tren GPIO 2/4.");
}

// -----------------------------------------------------------------

// Hàm đọc dữ liệu từ Serial_GPS và phân tích bằng TinyGPSPlus
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do {
    // Đọc tất cả các byte khả dụng từ Serial_GPS và đưa vào bộ mã hóa của TinyGPSPlus
    while (Serial_GPS.available()) {
      gps.encode(Serial_GPS.read());
    }
  } while (millis() - start < ms);
}

// Hàm in ra thông tin GPS (Chỉ in khi có giá trị hợp lệ)
static void printGPSInfo() {
  DEBUG_PRINTLN("-----------------------------------------");
  
  // Kiểm tra vị trí (Location)
  if (gps.location.isValid()) {
    DEBUG_PRINT("Vi do: ");
    DEBUG_PRINTLN(gps.location.lat(), 6);
    location_lat = gps.location.lat();
    DEBUG_PRINT("Kinh do: ");
    DEBUG_PRINTLN(gps.location.lng(), 6);
    location_long = gps.location.lng();
  } else {
    DEBUG_PRINT("Vi tri: Khong hop le/Chua co lock. Da doc duoc ");
    DEBUG_PRINT(gps.charsProcessed());
    DEBUG_PRINTLN(" ky tu.");
    location_lat = -1;
    location_long = -1;
  }
  
  // Kiểm tra số vệ tinh
  DEBUG_PRINT("So ve tinh: ");
  DEBUG_PRINTLN(gps.satellites.value());
  
  // Kiểm tra thời gian
  if (gps.time.isValid()) {
    DEBUG_PRINT("Gio UTC: ");
    DEBUG_PRINT(gps.time.hour());
    DEBUG_PRINT(":");
    DEBUG_PRINT(gps.time.minute());
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(gps.time.second());
    time_GPS = String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
  } else {
    DEBUG_PRINTLN("Thoi gian: Khong hop le");
    time_GPS = "null";
  }
}