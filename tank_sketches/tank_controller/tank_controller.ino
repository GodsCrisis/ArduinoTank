#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>

#define ENCODER_CLK 8
#define ENCODER_DT  9
#define ENCODER_SW  10

// CE = GP15, CSN = GP5
RF24 radio(15, 5);
const byte address[] = "1Tank";
int enc_pos = 0;
int enc_clk_last;
int enc_clk_val;
int enc_dt_val;
boolean is_clockwise;

void setup() {
  //while (!Serial); // Czekaj na Serial
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT);
  // Ustawienie SPI na niestandardowych pinach
  SPI.setSCK(6);    // SCK -> GP6
  SPI.setMOSI(7);   // MOSI -> GP7
  SPI.setMISO(4);   // MISO -> GP4
  radio.begin();

  radio.setChannel(76);
  radio.setPALevel(RF24_PA_LOW);       // Zmienione na LOW dla stabilności
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();               // Tryb nadawania
  //radio.printDetails();
  enc_clk_last = digitalRead(ENCODER_CLK);
}

void loop() {
  //delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  enc_clk_val = digitalRead(ENCODER_CLK);
  if (enc_clk_val != enc_clk_last) {
    enc_dt_val = digitalRead(ENCODER_DT);
    if (enc_dt_val != enc_clk_val) {
      enc_pos ++;
      is_clockwise = true;
    } else {
      is_clockwise = false;
      enc_pos--;
    }
    char out_text[32] = "\0";
    if (is_clockwise) {
      //sprintf(out_text, "cw");
    } else {
      //sprintf(out_text, "ccw");
    }
    //sprintf(out_text, "%s %d",out_text,enc_pos);
    sprintf(out_text,"%08X",enc_pos);
    if(digitalRead(ENCODER_SW) != HIGH)
    {
      //sprintf(out_text, "%s klik", out_text);
    }
    out_text[32] = '\0';
    radio.write(&out_text, sizeof(out_text));
    digitalWrite(LED_BUILTIN, HIGH);
  }
  radio.flush_tx();
  enc_clk_last = enc_clk_val;
  //delay(50); 
}