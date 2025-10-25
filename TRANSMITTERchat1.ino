// === LiFi TRANSMITTER: στέλνει ολόκληρη γραμμή από Serial ===
const uint8_t  TX_PIN = 13;          // Laser/LED μέσω τρανζίστορ
const uint16_t selangmasa = 1;       // ms (αύξησέ το αν χρειάζεται αντοχή)
const uint8_t  MAX_PAYLOAD = 60;     // μέγιστο μήκος λέξης/φράσης

inline void txOn()  { digitalWrite(TX_PIN, HIGH); }  // Φως ON
inline void txOff() { digitalWrite(TX_PIN, LOW);  }  // Φως OFF

void sendBit(uint8_t b) {
  // 2-4-4: start=ON(2), data=(ON αν 1, OFF αν 0)(4), stop=OFF(4)
  txOn();                       delay(2 * selangmasa);
  (b ? txOn() : txOff());       delay(4 * selangmasa);
  txOff();                      delay(4 * selangmasa);
}

void sendByte(uint8_t v) {                // LSB-first
  for (uint8_t i = 0; i < 8; i++) sendBit((v >> i) & 1);
}

void sendFrame(const uint8_t* data, uint8_t n) {
  // Preamble
  sendByte(0x55); sendByte(0x55);
  // Start Of Frame
  sendByte(0x02);
  // Length
  sendByte(n);

  // Payload + checksum
  uint16_t sum = n;
  for (uint8_t i = 0; i < n; i++) { sendByte(data[i]); sum += data[i]; }
  sendByte((uint8_t)(sum & 0xFF)); // checksum
  // End Of Frame
  sendByte(0x03);
}

void setup() {
  pinMode(TX_PIN, OUTPUT);
  txOff();
  Serial.begin(115200);
  delay(300);
  Serial.println(F("LiFi TX: Πληκτρολόγησε κείμενο και πάτα Enter για αποστολή.Newline or Both"));
}

void loop() {
  static char buffer[MAX_PAYLOAD + 1];
  static uint8_t len = 0;

  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;              // αγνόησε CR
    if (c == '\n') {                      // Enter -> στείλε πλαίσιο
      if (len > 0) {
        sendFrame((const uint8_t*)buffer, len);
        Serial.print(F("Στάλθηκαν ")); Serial.print(len); Serial.println(F(" byte."));
        len = 0;
      }
    } else if (len < MAX_PAYLOAD) {
      buffer[len++] = c;
      buffer[len]   = '\0';
    } else {
      // Γεμάτο: στείλε αμέσως
      sendFrame((const uint8_t*)buffer, len);
      Serial.println(F("Στάλθηκαν (auto) πλήρες buffer."));
      len = 0;
    }
  }
}
