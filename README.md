# The rlInput static library

This repo contains the static library `rlInput` that provides a unified interface for input devices
(mouse, keyboard, gamepads).

## General
The core idea of this library is to provide a unified interface for mouse, keyboard and gamepad
(both XInput and DirectInput) input to be used in a gameloop, meaning that applications using it
aren't relying on event-based input but rather request the current state of the device
(in part as compared to the state at the last check) from time to time, on demand.

## Basic structure
The entire library is defined in the `rlInput` namespace.
It defines the classes `DirectInput`, `XInput`, `Keyboard` and `Mouse`. All of these are singletons,
meaning you'll have to call the static `Instance()` method to get their only instance.

## The unified interface
All these above mentioned classes provide the following methods:

| Method | Description |
|--------|-------------|
| `update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)` | Update method via Windows Messages.<br>Should be called every time the Window callback of the window using the device gets called. |
| `prepare()` | Prepare the received input for state queries. Must be called every time an updated state is needed, but only once per game loop. |
| `reset()` | Reset the internal status and pretend nothing is currently pressed/clicked. |

## Specializations
### General
Both `DirectInput` and `XInput` provide two ways of preparing inputs:
Via their respective `Gamepad` objects or directly through the singleton. Please only call one of
these in the game loop, but not both, as that would lead to invalid values.

### DirectInput
DirectInput is an interface for generic, mostly third-party gamepads.<br>
The main singleton provides a list of currently available devices via `DirectInput::GamepadMeta`
structs.

You can then create an instance of `DirectInput::Gamepad` using both one of the provided
`GamepadMeta` structs and a `HWND`. That `HWND` identifies the window associated with the gamepad,
it shouldn't get destroyed before the `Gamepad` is destroyed.

Due to the generic nature of DirectInput, not all of the provided values of the `Gamepad` class are
always used. To be specific, it's not possible to automatically decide what the axes actually mean
and which axes are actually set. That's why all possible axes are provided alongside the count
specified by the device.


### XInput
XInput is mostly used by Microsoft's XBox gamepads. The interface is way easier to use than
DirectInput.

<b>Hint:</b> you can access XBox gamepads both via DirectInput and XInput, though the use of XInput
is recommended, in part because XInput provides the option to use vibratione effects.<br>
You can check if a DirectInput controller is actually an XInput controller by calling the
`isXInput(...)` method of the `DirectInput` singleton.

Up to four XBox gamepads may be accessed at a time via XInput. Use the `gamepad(...)` method or use
the singleton like an `std::vector` (both `operator[]` and `begin()`, `end()` etc. are defined) with
indexes between 0 and 3. Check if a certain slot is connected via the `connected()` method of the
`Gamepad` class.


### Keyboard
Keyboard support is also provided. Get the state of a certain key by calling the `key(...)` method
or using the `operator[]` with a Microsoft virtual key code (`VK_[...]`).

Some helper functions that simplify state checks (like recording of text input) are also available.
See headers for details.


### Mouse
You can check the mouse state to. Note that the mouse position keeps getting tracked when any button
is held down when the mouse left the client area. This will only stop once all keys are released
again.



## Misc

| Category              | Value                     |
|-----------------------|---------------------------|
| Target Platform       | Windows Vista+ x64        |
| C++ Language Standard | ISO C++20 (MSVC)          |
