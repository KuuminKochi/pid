# PID motor position control (Arduino)

Quadrature-encoder position control for an IG42 gear motor (CPR = 19 × 4 × 7 = 532) through an L293D.

- `pid.ino` – PD controller with minimum PWM and ramp-up
- `p_only.ino` – P-only version
- `notes/` – REC programming notes ([start here](notes/REC%20Programming%20Notes.md)), plus [Interrupts](notes/Interrupts.md) and [millis](notes/millis.md)

## Simulation

[Tinkercad circuit](https://www.tinkercad.com/things/2K8BaEvplPL-pid-controller?sharecode=6XP1BLMA8daMoybr5TneVgzM050QMHvxMpEXaznK7oE)
