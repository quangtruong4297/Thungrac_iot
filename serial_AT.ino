void setup_espcam() {
  Serial_CAM.begin(COMM_BAUD, SERIAL_8N1, COMM_RX_PIN, -1);
  DEBUG_PRINTLN("serial cam ok");
}
void serialEvent_cam() {
  String data = "";
  if (Serial_CAM.available()) {
    while (Serial_CAM.available()) {
      data = Serial_CAM.readStringUntil('\n');
      data.trim();
      break;
    }
  }
  if (data.startsWith("$")) {
    if (data == "$huuco") {
      trashType = "huu co";
      DEBUG_PRINTLN("--- CAM: huu co ---");
    } else if (data == "$voco") {
      trashType = "vo co";
      DEBUG_PRINTLN("--- CAM: vo co ---");
    } else if (data == "$khongxacdinh") {
      trashType = "Khong xac dinh";
      DEBUG_PRINT("--- CAM: Nhận diện không rõ: ");
      DEBUG_PRINTLN(data);
    }
  }
}
