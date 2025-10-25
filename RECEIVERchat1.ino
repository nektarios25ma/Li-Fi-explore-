// === LiFi RECEIVER: λαμβάνει πλαίσια & γράφει σε LCD I2C ===
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Αν η οθόνη σου είναι σε άλλο address (π.χ. 0x3F), άλλαξέ το εδώ:
LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t  RX_PIN = 0;          // ίδια συνδεσμολογία (D0)
const uint16_t selangmasa = 1;      // ms
const uint32_t START_TIMEOUT_MS = 3000;

int readBitWithTimeout(uint32_t timeout_ms) {
  // Περιμένει start (LOW), δειγματοληπτεί στο κέντρο (4*selangmasa)
  uint32_t t0 = millis();
  while (digitalRead(RX_PIN) != LOW) {
    if (millis() - t0 > timeout_ms) return -1;
  }
  delay(4 * selangmasa);
  int bit = !digitalRead(RX_PIN);   // LM393: ενεργό-LOW => αντιστροφή
  delay(4 * selangmasa);            // stop
  return bit;
}

bool readByte(uint8_t &outByte) {
  outByte = 0;
  for (uint8_t i = 0; i < 8; i++) {
    int b = readBitWithTimeout(START_TIMEOUT_MS);
    if (b < 0) return false;
    outByte |= (b & 1) << i;        // LSB-first
  }
  return true;
}

bool waitForByte(uint8_t target) {
  uint8_t b;
  uint32_t t0 = millis();
  while (millis() - t0 < START_TIMEOUT_MS) {
    if (readByte(b) && b == target) return true;
  }
  return false;
}

int readFrame(char *buffer, uint8_t maxLen) {
  if (!waitForByte(0x55)) return -1;
  if (!waitForByte(0x55)) return -1;
  if (!waitForByte(0x02)) return -1;

  uint8_t N;
  if (!readByte(N)) return -1;
  if (N == 0 || N >= maxLen) return -2;

  uint16_t sum = N;
  for (uint8_t i = 0; i < N; i++) {
    uint8_t b;
    if (!readByte(b)) return -1;
    buffer[i] = (char)b;
    sum += b;
  }
  uint8_t rxChecksum, eofByte;
  if (!readByte(rxChecksum)) return -1;
  if (!readByte(eofByte))    return -1;
  if (eofByte != 0x03)       return -3;
  if (((uint8_t)(sum & 0xFF)) != rxChecksum) return -4;

  buffer[N] = '\0';
  return (int)N;
}

void printOnLCD(const char* s) {
  lcd.clear();
  // Αν <=16 chars, γράψ’ το στην 1η γραμμή
  size_t L = strlen(s);
  if (L <= 16) {
    lcd.setCursor(0, 0); lcd.print(s);
    return;
  }
  // Διαφορετικά τύπωσε 1η και 2η γραμμή (κοπή στα 32)
  for (uint8_t i = 0; i < 16 && s[i]; i++) { lcd.setCursor(i, 0); lcd.print(s[i]); }
  for (uint8_t i = 0; i < 16 && s[i + 16]; i++) { lcd.setCursor(i, 1); lcd.print(s[i + 16]); }
}

void setup() {
  pinMode(RX_PIN, INPUT);
  //Serial.begin(115200);
  lcd.init();           // Αν η βιβλιοθήκη σου θέλει begin(): lcd.begin(16,2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("LiFi RX Ready");
  delay(3000);
  lcd.clear();
  delay(30);
}

void loop() {
  static char word[64];
  int n = readFrame(word, sizeof(word));
  
  if (n > 0) {
    printOnLCD(word);
    // flash στο LED 13 για επιβεβαίωση
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH); delay(60);
    digitalWrite(13, LOW);  delay(60);
  }
  delay(5);
}
