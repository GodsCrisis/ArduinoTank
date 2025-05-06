#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdint.h>
#include <Servo.h>

// Piny
#define SERVO_PWM 9

struct __attribute__((packed)) radio_payload_t {
  uint8_t  pos;    // 0…255
  uint16_t slide;  // 0…1023
  uint16_t rot;    // 0…1023
  uint8_t  sw;     // 0 or 1
};

// CE = D7, CSN = D8 (dowolne piny cyfrowe)
RF24 radio(7, 8);
Servo servo;

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // Czekaj na otwarcie monitora szeregowego
  printf_begin();   // Inicjalizacja printf dla nrf24
  
  /*RADIO START*/
  const byte address[] = "1Tank";
  bool result = radio.begin();
  radio.setChannel(76);               // Ten sam kanał co w Pico
  radio.setPALevel(RF24_PA_LOW);      // Mniejsze zakłócenia
  radio.setDataRate(RF24_250KBPS);    // Taki sam jak u nadajnika
  radio.openReadingPipe(0, address);  // Adres odbiorczy
  radio.startListening();             // Przejście w tryb odbioru
  radio.printDetails();               // Wyświetlenie szczegółów połączenia
  /*RADIO END*/

  /*MOTORS START*/
  pinMode(SERVO_PWM, OUTPUT);
  //servo.attach(SERVO_PWM);
  /*MOTORS END*/
}

void loop()
{
  if(radio.available())
  {
    radio_payload_t payload;
    radio.read(&payload, sizeof(payload));
    printf("pos=%u  slide=%u  rot=%u  btn=%u\n",
                payload.pos, payload.slide, payload.rot, payload.sw);
    radio.flush_rx(); // Opróżnienie bufora odbiorczego
  }
  delay(10);
}