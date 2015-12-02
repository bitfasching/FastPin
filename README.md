# FastPin Library for Arduino

Fast and yet easy-to-use methods for binary GPIO access, using the pin identifiers you're familiar with. The ports are looked up only once at initialization. Reading and writing afterwards work directly on the registers.

## Usage

Set up your FastPin passing the pin's number and I/O mode, as you would for `pinMode()`:
```
FastPin LED(13, OUTPUT);
```
Once initialized, use `read()` or `write()` according to its I/O mode:
```
LED.write(HIGH);
LED.write(LOW);
```
As the lookup is done at runtime, this even works if the pin is not a known constant at compile time. So you can select a pin dynamically using your program logic, initialize it and enjoy fast access afterwards.

## Speed

This implementation is slower than port manipulation with compile time constants,
but is one order of magnitude faster than Arduino's default functions.

Measurements on an ATmega328 running at 16 MHz, setting one pin high and low in an infinite loop:

- digitalWrite: 5.05 µs per write, 10.3 µs per cycle (0.097 MHz)
- FastPin: 0.32 µs per write, 0.76 µs per cycle (1.3 MHz)
- direct: 0.13 µs per write, 0.38 µs per cycle (2.6 MHz)

In this test, the time for a complete cycle includes the loop overhead.

## Caveats

### PWM and Interrupts

The `read()` and `write()` methods skip some steps that `digitalRead()` or `digitalWrite()` would do:

- PWM timers are not stopped. If you want to write to a pin that is currently active as a PWM output, stop it by calling digitalWrite() on it.
- Interrupts are not disabled. If you have interrupt routines writing to the same pin, you might want to disable them yourself.

### I/O Mode

FastPin doesn't remember the pin mode after initialization.
It can't warn you if you use the wrong function for a given pin. As only one register pointer is kept (either for input or output), writing on a pin that was set up as an input has no effect. If you'd like to enable the internal pull-up for an input, do so during initialization.

## Supported Platforms

FastPin has been tested on the ATmega328 (Uno, Nano). As the pin numbers are translated using the same Arduino macros as the default digital I/O functions, FastPin should work on every platform which is supported by the Arduino core library.