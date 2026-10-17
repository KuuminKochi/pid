// C++ code
// IG42 Specifications
// GEAR RATIO: 19
// RESOLUTION: 4x
// PULSE PER REVOLUTION: 7

// CPR 
const int CPR = 19 * 4 * 7; // CPR = 532

// PID
const float KP = 0.5;
const float KD = 0.01;

// Drive
const int IN_1 = 10;
const int IN_2 = 9;
const int THRESHOLD_ERROR = 3;
const int MIN_PWM = 50; // Motors need a minimum PWM to run.
const long RAMP_STEP = 5;

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
    // SETUP
    Serial.begin(9600);
    pinMode(ENCODER_A, INPUT_PULLUP);
    pinMode(ENCODER_B, INPUT_PULLUP);
    lastA = digitalRead(ENCODER_A);
    lastB = digitalRead(ENCODER_B);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoderCount, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B), updateEncoderCount, CHANGE);
    pinMode(IN_1, OUTPUT);
    pinMode(IN_2, OUTPUT);


    // No interrupts because the value affected by interrupts could be corrupted
    // if it's read and updated at the same time
    noInterrupts();
    long relTarget = encoderCount + degreeToCount(90);
    long currentCount = encoderCount;
    long lastU = 0;
    unsigned long prev = micros();
    float derivative = 0;
    interrupts();

    long error = relTarget - currentCount;
    long prevError = error;
    
    // THRESHOLD_ERROR for an acceptable error range. Can't always have a perfect u = 0
    while (abs(error) > THRESHOLD_ERROR) {
        // U CALCULATIONS AND CHECKS
        // PD Only. u = Kp e(t) + Kd de/dt
        long u = KP * error + KD * derivative;
        u = constrain(u, -255, 255);

        if (u > 0 && u < MIN_PWM) {
            u = MIN_PWM;
        } else if (u < 0 && u > -MIN_PWM) {
            u = -MIN_PWM;
        }

        
        // RAMP_UP is used to gradually speed up the motor to be easier on the gearbox and everything around it
        // Basically just means if the calculated u is too high, it'll switch u into it's previous recorded speed
        // plus RAMP_STEP up, so that it's gradual.
        if (u > lastU + RAMP_STEP) {
            u = lastU + RAMP_STEP;
        } else if (u < lastU - RAMP_STEP) {
            u = lastU - RAMP_STEP;
        }
        
        // records u for next loop
        lastU = u;

        noInterrupts();
        currentCount = encoderCount;
        interrupts();

        // Some calculations for Kd de/dt.
        // de/dt is taken as (error - prevError)/dt because you can't actually have infinitisimally small values.
        // So, you need to calculate it in its discrete form.
        // I calculated it here so that I can reuse it in the second loop and beyond.
        unsigned long now = micros();
        float dt = now - prev;
        prev = now;
        float dtSec = dt / 1e6;
        error = relTarget - currentCount;
        derivative = (error - prevError) / dtSec;
        prevError = error;

        drive(u);

        Serial.println(error);
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
