#pragma once

// Inputs
#define I_START_BUTTON (!(ReadPin(E, 2))) // Black 1
#define I_DRIVE_DOOR (!(ReadPin(K, 3))) // Black 8
#define I_AC_THERMO (!(ReadPin(F, 0))) // Black 9
#define I_DOOR_SPEED (!(ReadPin(K, 1))) // Black 20
#define I_PASS_DOOR (!(ReadPin(C, 3))) // Black 28
#define I_COOLANT_TEMP (!(ReadPin(K, 2))) // Black 31
#define I_ENGINE_RUNNING (!(ReadPin(K, 0))) // Black 32

#define I_RIGHT_TURN (!(ReadPin(D, 4))) // Blue 5
#define I_BRAKE (!(ReadPin(D, 6))) // Blue 6
#define I_FUEL_PUMP_1 (!(ReadPin(J, 0))) // Blue 8
#define I_LEFT_TURN (!(ReadPin(C, 2))) // Blue 9
#define I_COOLING_FAN (!(ReadPin(J, 3))) // Blue 18
#define I_FUEL_PUMP_2 (!(ReadPin(J, 1))) // Blue 19
#define I_REVERSE_LIGHT_SWITCH (!(ReadPin(D, 7))) // Blue 31

// Outputs
#define O_START_BUTTON_LIGHT(state) WritePin(E, 3, state) // Black 2
#define O_PASS_WINDOW_DIR(state) WritePin(F, 4, state) // Black 11
#define O_FLOOR(state) WritePin(H, 5, state) // Black 13
#define O_DEFROST(state) WritePin(H, 3, state) // Black 14
#define O_DASH(state) WritePin(H, 1, state) // Black 15
#define O_DRIVERS_WINDOW_DIR(state) WritePin(F, 6, state) // Black 22
#define O_DRIVERS_WINDOW_CTRL(state) WritePin(F, 5, state) // Black 33
#define O_PASS_WINDOW_CTRL(state) WritePin(F, 2, state) // Black 34

#define O_AC_KICK(state) WritePin(L, 3, state) // Blue 13

// ADC Pins
#define PASS_WINDOW_CS ADC1D // Black 10  PF3 ADC3
#define DRIVERS_WINDOW_CS ADC3D // Black 21  PF1 ADC1
#define STEERING_WHEEL_SWITCHES ADC7D // Black 35	PF7	ADC7
