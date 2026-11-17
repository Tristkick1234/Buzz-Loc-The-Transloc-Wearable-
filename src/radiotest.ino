#include <SPI.h>
#include <RH_RF95.h>

// #define SENDER 
//comment out for reciever
#define PIN_SCK  21
#define PIN_MISO 20
#define PIN_MOSI 19
#define PIN_CS   18
#define PIN_RST  1
#define PIN_G0   0
#define RF_FREQ  915.0   // MHz (US ISM band)

RH_RF95 rf95(PIN_CS, PIN_G0);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, HIGH);
  delay(10);
  digitalWrite(PIN_RST, LOW);
  delay(10);
  digitalWrite(PIN_RST, HIGH);
  delay(10);

  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

  if (!rf95.init()) {
    Serial.println("Radio init failed - check wiring");
    while (1) delay(1000);
  }
  rf95.setFrequency(RF_FREQ);
  rf95.setTxPower(13, false);
  Serial.println("Radio OK");
}

void loop() {
#ifdef SENDER
  static int n = 0;
  char msg[32];
  snprintf(msg, sizeof(msg), "Hello #%d", n++);
  rf95.send((uint8_t *)msg, strlen(msg) + 1);
  rf95.waitPacketSent();
  Serial.printf("Sent: %s\n", msg);
  delay(1000);
#else
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      Serial.printf("Got: %s (RSSI %d)\n", (char *)buf, rf95.lastRssi());
    }
  }
#endif
}
