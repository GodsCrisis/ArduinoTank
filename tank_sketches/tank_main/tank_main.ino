#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdint.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Piny
#define SERVO_PWM 9
#define MOTOR_A_L 2
#define MOTOR_A_R 4
#define MOTOR_A_PWM 3
#define MOTOR_B_L 12
#define MOTOR_B_R 13
#define MOTOR_B_PWM 11
#define NEOPIXEL_PWM 6

// NeoPixel
#define PIXEL_COUNT 4

struct __attribute__((packed)) radio_payload_t {
  uint8_t pos;     // 0…255
  uint16_t slide;  // 0…1023
  uint16_t rot;    // 0…1023
  uint8_t sw;      // 0 or 1
};

RF24 radio(7, 8);
Servo servo;
Adafruit_NeoPixel neopixel(PIXEL_COUNT, NEOPIXEL_PWM, NEO_GRBW + NEO_KHZ800);

// Stan animacji dla neopixela
enum AnimationState {
  ANIM_IDLE,
  ANIM_EXPLOSION
};

void neopixelSetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) {
  for (int i = 0; i < PIXEL_COUNT; i++) {
    neopixel.setPixelColor(i, neopixel.Color(r, g, b, w));
  }
}

AnimationState animState = ANIM_IDLE;
unsigned long animStartTime = 0;
uint8_t animPhase = 0;

void setup() {
  Serial.begin(9600);
  printf_begin();

  /*RADIO START*/
  const byte address[] = "1Tank";
  bool result = radio.begin();
  radio.setChannel(76);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();
  radio.printDetails();
  /*RADIO END*/

  /*MOTORS START*/
  pinMode(SERVO_PWM, OUTPUT);
  servo.attach(SERVO_PWM);
  servo.write(90);
  digitalWrite(4, HIGH);
  /*MOTORS END*/

  /*NEOPIXEL START*/
  pinMode(NEOPIXEL_PWM, INPUT_PULLUP);
  neopixel.begin();
  neopixel.clear();
  neopixelSetColor(0,0,0); // TODO: na poczatku jest zielone jedno zapalone i nie wiem dlaczego 
  neopixel.show();
  /*NEOPIXEL END*/
}

void updateExplosionAnimation() {
  if (animState != ANIM_EXPLOSION) return;

  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - animStartTime;

  // Explosion animation sequence with corrected colors
  if (elapsed < 50) {
    neopixelSetColor(255, 165, 0);  // Orange
    neopixel.show();
  } else if (elapsed < 100) {
    neopixelSetColor(255, 100, 0);  // Darker orange
    neopixel.show();
  } else if (elapsed < 150) {
    neopixelSetColor(255, 50, 0);  // Red-orange
    neopixel.show();
  } else if (elapsed < 200) {
    neopixelSetColor(255, 0, 0);  // Bright red
    neopixel.show();
  } else if (elapsed < 250) {
    neopixelSetColor(150, 0, 0);  // Dark red
    neopixel.show();
  } else if (elapsed < 300) {
    neopixelSetColor(50, 0, 0);  // Very dark red
    neopixel.show();
  } else {
    // koniec animacji
    neopixel.clear();
    neopixel.show();
    animState = ANIM_IDLE;
  }
}

void startExplosionAnimation() {
  animState = ANIM_EXPLOSION;
  animStartTime = millis();
}

void loop() {
  // Update NeoPixel
  updateExplosionAnimation();

  // Radio komunikacja
  if (radio.available()) {
    radio_payload_t payload;
    radio.read(&payload, sizeof(payload));
    printf("pos=%u  slide=%u  rot=%u  btn=%u\n",
           payload.pos, payload.slide, payload.rot, payload.sw);
    radio.flush_rx();

    // Serwo
    servo.write(map(payload.pos, 96, 160, 0, 180));

    // Silniki
    int mapped_slider = map(payload.slide, 0, 1023, -255, 255);
    int mapped_rot = map(payload.rot, 0, 1023, -127, 127);

    if (mapped_slider >= 0) {
      analogWrite(MOTOR_A_PWM, min(mapped_slider - mapped_rot, 255));
      analogWrite(MOTOR_B_PWM, min(mapped_slider + mapped_rot, 255));
      digitalWrite(MOTOR_A_L, HIGH);
      digitalWrite(MOTOR_A_R, LOW);
      digitalWrite(MOTOR_B_L, HIGH);
      digitalWrite(MOTOR_B_R, LOW);
    } else {
      analogWrite(MOTOR_A_PWM, min(-mapped_slider - mapped_rot, 255));
      analogWrite(MOTOR_B_PWM, min(-mapped_slider + mapped_rot, 255));
      digitalWrite(MOTOR_A_R, HIGH);
      digitalWrite(MOTOR_A_L, LOW);
      digitalWrite(MOTOR_B_R, HIGH);
      digitalWrite(MOTOR_B_L, LOW);
    }

    // Odtwórz animację na przycisk
    if (payload.sw == 1 && animState == ANIM_IDLE) {
      startExplosionAnimation();
    }
  }

  delay(10);
}