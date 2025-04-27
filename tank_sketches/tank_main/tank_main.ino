#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdint.h>
#include <Servo.h>

// Piny
#define SERVO_PWM 9

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
  servo.attach(SERVO_PWM);
  /*MOTORS END*/
}

void loop()
{
  if(radio.available())
  {
    char buf[32];
    radio.read(&buf, 32);
    int servo_dt = (int)strtol(buf, NULL, 16);
    //printf("otrzymano: %s\n", buf);
    servo.write(servo_dt);
    radio.flush_rx(); // Opróżnienie bufora odbiorczego
  }
  delay(10);
}