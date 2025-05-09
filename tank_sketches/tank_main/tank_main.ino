#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdint.h>
#include <Servo.h>

// Piny
#define SERVO_PWM 9
#define MOTOR_A_L 2
#define MOTOR_A_R 4
#define MOTOR_A_PWM 3
#define MOTOR_B_L 12
#define MOTOR_B_R 13
#define MOTOR_B_PWM 11
// #define DFPLAYER_IO1 13

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
  //while (!Serial);  // Czekaj na otwarcie monitora szeregowego
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
  servo.write(90);
  digitalWrite(4,HIGH);
  //digitalWrite(2,LOW);
  /*MOTORS END*/

  /*DFPLAYER*/
  //digitalWrite(DFPLAYER_IO1,LOW);
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
    servo.write(map(payload.pos,96,160,0,180));
    int mapped_slider = map(payload.slide,0,1023,-255,255);
    int mapped_rot = map(payload.rot,0,1023,-127,127);
    if(mapped_slider >= 0)
    {
      analogWrite(MOTOR_A_PWM,min(mapped_slider - mapped_rot,255));
      analogWrite(MOTOR_B_PWM,min(mapped_slider + mapped_rot,255));
      digitalWrite(MOTOR_A_L,HIGH);
      digitalWrite(MOTOR_A_R,LOW);
      digitalWrite(MOTOR_B_L,HIGH);
      digitalWrite(MOTOR_B_R,LOW);
    }
    else
    {
      analogWrite(MOTOR_A_PWM,min(-mapped_slider - mapped_rot,255));
      analogWrite(MOTOR_B_PWM,min(-mapped_slider + mapped_rot,255));
      digitalWrite(MOTOR_A_R,HIGH);
      digitalWrite(MOTOR_A_L,LOW);
      digitalWrite(MOTOR_B_R,HIGH);
      digitalWrite(MOTOR_B_L,LOW);
    }
  }
  delay(10);
}