// C++ code
// IG42 Specifications
// GEAR RATIO: 19
// RESOLUTION: 4x
// PULSE PER REVOLUTION: 7

// CPR 
const int CPR = 19 * 4 * 7; // CPR = 532

// PID
const float KP = 0.25;

// Drive
const int IN_1 = 10;
const int IN_2 = 9;
const int THRESHOLD_ERROR = 3;

// Interrupt Pins
const int ENCODER_A = 2;
const int ENCODER_B = 3;

// Used in the calculations for CPR = PPR * Decoding Factor * Gear Ratio
// If count++, then its moving forward. If count--, moving backward
// count++ means A is leading. count-- means B is leading.
volatile long encoderCount = 0;
volatile bool lastA = LOW;
volatile bool lastB = LOW;

void setup()
{
    Serial.begin(9600);
    pinMode(ENCODER_A, INPUT_PULLUP);
    pinMode(ENCODER_B, INPUT_PULLUP);
    lastA = digitalRead(ENCODER_A);
    lastB = digitalRead(ENCODER_B);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoderCount, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B), updateEncoderCount, CHANGE);
    pinMode(IN_1, OUTPUT);
    pinMode(IN_2, OUTPUT);


    noInterrupts();
    long relTarget = encoderCount + degreeToCount(90);
    long currentCount = encoderCount;
    interrupts();

    long error = relTarget - currentCount;
    while (abs(error) > THRESHOLD_ERROR) {
        long u = KP * error;
        drive(u);

        noInterrupts();
        currentCount = encoderCount;
        interrupts();
        error = relTarget - currentCount;
    };

    brake();
}

void loop()
{
}

// ISR
void updateEncoderCount() {
    // Read the current state of both pins immediately
    bool currentA = digitalRead(ENCODER_A);
    bool currentB = digitalRead(ENCODER_B);
    
    // Check if Encoder A changed state
    if (currentA != lastA) {
        if (currentA == currentB) {
            encoderCount--; // Reverse
        } else {
            encoderCount++; // Forward
        }
    }
    // Check if Encoder B changed state
    else if (currentB != lastB) {
        if (currentA == currentB) {
            encoderCount++; // Forward
        } else {
            encoderCount--; // Reverse
        }
    }
    
    // Save the current states for the next pulse edge
    lastA = currentA;
    lastB = currentB;
}

long countToDegree(long count) {
    return count * 360 / CPR;
}

long degreeToCount(long degree) {
    return degree * CPR / 360;
}

void brake() {
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, HIGH);
}

void drive(long u) {
    u = constrain(u, -255, 255);
    if (u > 0) {
        analogWrite(IN_1, u);
        digitalWrite(IN_2, LOW);
    } else if (u < 0) {
        analogWrite(IN_2, -u);
        digitalWrite(IN_1, LOW);
    } else {
        brake();
    }
}
