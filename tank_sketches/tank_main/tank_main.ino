#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdint.h>

// CE = D7, CSN = D8 (dowolne piny cyfrowe)
RF24 radio(7, 8);

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
  // ENABLE1  - pin D3
  // INPUT1   - pin D4
  // INPUT2   - pin D5

  uint8_t in1 = 0;
  uint8_t in2 = 255;
  //digitalWrite(3,0);
  //digitalWrite(4,in1);
  //digitalWrite(5,in2);
}

void loop()
{
  if(radio.available())
  {
    char buf[32];
    radio.read(&buf, 32);
    printf("otrzymano: %s\n", buf);
    radio.flush_rx(); // Opróżnienie bufora odbiorczego
  }
  delay(10);
}