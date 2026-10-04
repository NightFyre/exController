# exController

A lightweight single-header C++ controller library using XInput.

## Features

- Controller input
- Button states
- Analog sticks and triggers
- Vibration
- Optional Dear ImGui support

## Usage

```cpp
#define EXCONTROLLER_IMPLEMENTATION
#include "exController.h"

NF::exController controller;

controller.Update();

if (controller.IsPressed(NF::exController::Button::A))
{
    // A pressed
}
```
