/**
 *
 * Kontrolowanie silniczka dc przez potencjometr typu slider
 *
 */

#define SERIAL_OUTPUT 0 // debug

const int potPin = A0; // pot
const int in1Pin = 9;  // mostek-h in1, musi byc pin PWM czyli te z ~
const int in2Pin = 10; // mostek-h in2, musi byc pin PWM czyli te z ~

int potValue = 0;
int motorSpeed = 0;
int deadZone = 20;            // deadzone do zatrzymania silniczka gdy pot jest w srodku
float speedMultiplier = 1.0f; // mnoznik predkosci

void setup()
{
    pinMode(potPin, INPUT);  // pot input
    pinMode(in1Pin, OUTPUT); // mostek-h in1 output
    pinMode(in2Pin, OUTPUT); // mostek-h in2 output

#if SERIAL_OUTPUT
    Serial.begin(9600);
#endif
}

void loop()
{
    // 0 - 1023
    potValue = analogRead(potPin);

    // mapowanie 0 - 1023 na -255 - 255
    motorSpeed = map(potValue, 0, 1023, -255, 255); // https://docs.arduino.cc/language-reference/en/functions/math/map/

    // deadzone
    if (motorSpeed > -deadZone && motorSpeed < deadZone)
    {
        motorSpeed = 0;
    }

    // mnozenie predkosci
    motorSpeed *= speedMultiplier;
    motorSpeed = constrain(motorSpeed, -255, 255); // https://docs.arduino.cc/language-reference/en/functions/math/constrain/

    // kierunek obrotu
    if (motorSpeed > 0)
    {
        // clockwise
        analogWrite(in1Pin, motorSpeed); // PWM do IN1
        analogWrite(in2Pin, 0);          // IN2 low
    }
    else if (motorSpeed < 0)
    {
        // counter-clockwise
        analogWrite(in1Pin, 0);           // IN1 low
        analogWrite(in2Pin, -motorSpeed); // PWM do IN2
    }
    else
    {
        // stop
        analogWrite(in1Pin, 0);
        analogWrite(in2Pin, 0);
    }

#if SERIAL_OUTPUT
    Serial.print("Pot: ");
    Serial.print(potValue);
    Serial.print(" Motor: ");
    Serial.println(motorSpeed);
#endif

    delay(10);
}
