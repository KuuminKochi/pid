attachInterrupt(digitalPinToInterrupt(pin), ISR, mode) (recommended)
## Parameters
interrupt: the number of the interrupt. Allowed data types: int.
pin: the Arduino pin number.
ISR: the ISR to call when the interrupt occurs; this function must take no parameters and return nothing. This function is sometimes referred to as an interrupt service routine.
mode: defines when the interrupt should be triggered. Four constants are predefined as valid values:

**LOW** to trigger the interrupt whenever the pin is low,
**CHANGE** to trigger the interrupt whenever the pin changes value
**RISING** to trigger when the pin goes from low to high,
**FALLING** for when the pin goes from high to low.

```cpp
const byte ledPin = 13;
const byte interruptPin = 2;  // input pin that the interruption will be attached to
volatile byte state = LOW;  // variable that will be updated in the ISR

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}

void loop() {
  digitalWrite(ledPin, state);
}

void blink() {
  state = !state;
}
```

> [!NOTE] PULLUP Note
> The reason we use INPUT_PULLUP is to have clean signal. Electrical noise easily turns the pin HIGH, so to prevent that, we want the pin to be HIGH by default, and pulled down when signal in.


UNO R3, Nano, Mini, other 328-based	can access digital pins 2, 3 for interrupts

# Related
[millis](millis.md) 