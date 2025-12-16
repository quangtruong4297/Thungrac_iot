// -----------------------------------------------------------------
// === LOOP (Hàm lặp lại) ===
// -----------------------------------------------------------------

void MQTT_public() {
  // 1. Kiểm tra kết nối MQTT
  if (!mqtt.connected()) {
    DEBUG_PRINTLN("=== MQTT NOT CONNECTED, RETRY... ===");
    uint32_t t = millis();
    if (t - lastReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
      lastReconnectAttempt = t;
      if (mqttConnect()) { lastReconnectAttempt = 0; }
    }
    return;
  }
  sendTelemetryData(); 
}

void MQTT_loop(){
  mqtt.loop();
}

// -----------------------------------------------------------------
// === CÁC HÀM HỖ TRỢ CHÍNH ===
// -----------------------------------------------------------------

void setupGPRS_MQTT() {
  Serial_GMS.begin(SIM_BAUD, SERIAL_8N1, SIM_RX_PIN, SIM_TX_PIN); 

  DEBUG_PRINTLN("Khoi tao Modem A7680C...");
  modem.restart(); 
  DEBUG_PRINT("Modem Info: ");
  DEBUG_PRINTLN(modem.getModemInfo());

  // === KIỂM TRA TÍN HIỆU SÓNG (RSSI) ===
  DEBUG_PRINTLN("--- Kiem tra cuong do song ---");
  int rssi = modem.getSignalQuality(); // Giá trị từ 0-31 (99 = No signal)
  
  DEBUG_PRINT("Cuong do song (RSSI 0-31): ");
  if (rssi == 99) {
      DEBUG_PRINTLN("Khong phat hien song (RSSI=99).");
  } else {
      DEBUG_PRINT(rssi);
      DEBUG_PRINTLN("/31");
  }
  // ======================================

  // Kết nối GPRS
  DEBUG_PRINT("Cho ket noi mang...");
  if (!modem.waitForNetwork()) {
    DEBUG_PRINTLN(" [THAT BAI]");
    return; 
  }
  DEBUG_PRINTLN(" [THANH CONG]");

  DEBUG_PRINT(F("Dang ket noi GPRS den "));
  DEBUG_PRINT(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    DEBUG_PRINTLN(" [THAT BAI]");
    return;
  }
  DEBUG_PRINTLN(" [THANH CONG]");

  // Thiết lập MQTT Broker
  mqtt.setServer(broker, TB_PORT);
  mqtt.setCallback(mqttCallback);
}

boolean mqttConnect() {
  DEBUG_PRINT("Dang ket noi MQTT den ");
  DEBUG_PRINT(broker);

  boolean status = mqtt.connect("ESP32_A7680C_Client", tbToken, NULL);

  if (status == false) {
    DEBUG_PRINT(" [THAT BAI], Code: ");
    DEBUG_PRINTLN(mqtt.state());
    return false;
  }
  DEBUG_PRINTLN(" [THANH CONG]");
  
  mqtt.publish(topicTelemetry, "{\"status\":\"SIM connected to MQTT\"}");
  
  return mqtt.connected();
}

// Kích thước tài liệu JSON tĩnh (cần đủ lớn để chứa tất cả dữ liệu)
// 300 bytes là đủ cho các trường bạn đang sử dụng.

// --- GỬI DỮ LIỆU CUỐI CÙNG ---
void sendTelemetryData() {

    // Khởi tạo StaticJsonDocument
    StaticJsonDocument<JSON_DOC_SIZE> doc;

    // Thêm các trường dữ liệu
    doc["distance_cm"] = distance;
    doc["fill_percent"] = fill_percent;
    doc["trash_type"] = trashType; // Từ ESP32-CAM

    // Thêm vị trí GPS (Chỉ thêm nếu hợp lệ)
    if (location_lat != 0.0 && location_lat != 0.0) {
        // ThingBoard yêu cầu các trường này để hiển thị trên bản đồ
        doc["latitude"] = location_lat;
        doc["longitude"] = location_long;
    } else {
        doc["location_status"] = "No Fix";
    }

    // 3. Chuyển đối tượng JSON thành chuỗi (String)
    String payload;
    serializeJson(doc, payload);
    
    DEBUG_PRINTLN("\n--- GUI DATA MQTT ---");
    DEBUG_PRINT("Payload: ");
    DEBUG_PRINTLN(payload);
    
    // 4. Gửi lên MQTT
    if (mqtt.connected()) {
        if (mqtt.publish(topicTelemetry, payload.c_str())) {
            DEBUG_PRINTLN("Publish MQTT thanh cong!");
        } else {
            DEBUG_PRINT("Publish MQTT that bai. Status: ");
            DEBUG_PRINTLN(mqtt.state());
        }
    }
    
    // ... (Phần debug cảnh báo giữ nguyên)
    if (distance <= 0) { DEBUG_PRINTLN("CANH BAO: HC-SR04 Loi doc cam bien."); }
    if (!gps.location.isValid()) { DEBUG_PRINTLN("CANH BAO: GPS Chua co vi tri (No Fix)."); }
    DEBUG_PRINTLN("-------------------------");
}


void mqttCallback(char* topic, byte* payload, unsigned int len) {
  DEBUG_PRINT("Tin nhan den: ");
  DEBUG_PRINTLN(topic);
}