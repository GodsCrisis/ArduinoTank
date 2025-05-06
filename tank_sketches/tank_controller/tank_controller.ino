#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>

#define ENCODER_CLK 8
#define ENCODER_DT  9
#define ENCODER_SW  10
#define POT_SLIDE 26
#define POT_ROT   27

struct __attribute__((packed)) radio_payload_t {
  uint8_t  pos;    // 0…255
  uint16_t slide;  // 0…1023
  uint16_t rot;    // 0…1023
  uint8_t  sw;     // 0 or 1
};

RF24 radio(15, 5);
const byte address[] = "1Tank";

volatile int32_t enc_pos = 128; //0x00 - 0x80 - 0xFF
volatile uint32_t last_interrupt = 0;
const uint32_t DEBOUNCE_US = 200;  //czestotliwosc interrupt (200 µs)

void handleEncoder() {
  uint32_t now = micros();
  if (now - last_interrupt < DEBOUNCE_US) return;
  last_interrupt = now;

  bool dt = digitalRead(ENCODER_DT);
  if (dt != digitalRead(ENCODER_CLK)) {
    enc_pos++;
    if(enc_pos > 255) enc_pos = 255;
  } else {
    enc_pos--;
    if(enc_pos < 0) enc_pos = 0;
  }
}

void setup() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);
  pinMode(ENCODER_SW,  INPUT_PULLUP);

  attachInterrupt(ENCODER_CLK, handleEncoder, CHANGE);
  // attachInterrupt(ENCODER_DT, handleEncoder, CHANGE);

  pinMode(POT_SLIDE, INPUT);
  pinMode(POT_ROT,   INPUT);

  SPI.setSCK(6);
  SPI.setMOSI(7);
  SPI.setMISO(4);

  radio.begin();
  radio.setChannel(76);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  noInterrupts();
  uint8_t pos = (uint8_t)enc_pos;
  interrupts();

  uint16_t slide = analogRead(POT_SLIDE);   //0x0000 - 0x03FF
  uint16_t rot   = analogRead(POT_ROT);     //0x0000 - 0x03FF
  bool     sw    = !digitalRead(ENCODER_SW);//0 - 1

  radio_payload_t payload{pos,slide,rot,sw};

  radio.write(&payload, sizeof(payload));
  radio.flush_tx();

  delay(10);
}
