
void setup_HC() {
  // Khởi tạo chân HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  DEBUG_PRINTLN("HC settup ");
}

// -----------------------------------------------------------------
// === LOOP (Lặp lại) ===
// -----------------------------------------------------------------

void HC_loop() {
  distance = measureDistanceCM();
  fill_percent = calculateFillPercent(distance);
  
  DEBUG_PRINTLN("Khoang cach (cm): ");
  if (distance > 0) {
      DEBUG_PRINTLN(distance);
      DEBUG_PRINTLN(" | Phan tram day: ");
      DEBUG_PRINTLN(fill_percent, 1);
      DEBUG_PRINTLN("%");
  } else {
      DEBUG_PRINTLN("LOI: Cam bien timeout (Qua xa hoac loi dau noi/nguon).");
      fill_percent = -1;
  }
  
  delay(1000); // Đọc mỗi 1 giây
}

// -----------------------------------------------------------------
// === HÀM ĐỌC KHOẢNG CÁCH (HC-SR04) ===
// -----------------------------------------------------------------

long measureDistanceCM() {
  // 1. Đảm bảo chân TRIG ở mức thấp 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // 2. Phát xung 10µs
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // 3. Đo thời gian xung phản hồi (ECHO)
  // Timeout 30000 µs (30ms) ~ 5m
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  
  // Nếu duration = 0, nghĩa là bị timeout
  if (duration == 0) return -1;
  
  // 4. Tính toán khoảng cách (cm)
  // Khoảng cách = (Thời gian * Tốc độ âm thanh: 0.0343 cm/µs) / 2
  long distance_cm = duration * 0.0343 / 2;
  return distance_cm;
}


// -----------------------------------------------------------------
// === HÀM TÍNH PHẦN TRĂM ĐẦY ===
// -----------------------------------------------------------------

float calculateFillPercent(long distance_cm) {
    if (distance_cm <= 0) return 0.0f; // Trả về 0% nếu lỗi đọc
    
    // 1. Chiều cao khả dụng (khoảng cách từ miệng đến đáy)
    float available_height = BIN_HEIGHT_CM - SENSOR_OFFSET_CM;
    
    // 2. Chiều sâu rác (Khoảng cách từ rác đến đáy thùng)
    float trash_depth = available_height - (float)distance_cm;
    
    // 3. Đảm bảo giá trị không âm
    trash_depth = (trash_depth < 0.0f) ? 0.0f : trash_depth;
    
    // 4. Tính phần trăm
    float fill_percentage = (trash_depth / available_height) * 100.0f;
    
    // Giới hạn tối đa 100%
    fill_percentage = (fill_percentage > 100.0f) ? 100.0f : fill_percentage;
    
    return fill_percentage;
}