# KeyboardLatencyTester

Yet another keyboard latency tester, powered by imgui.

## Features

- Key up/down histogram
- Keypress duration distribution graph
- Multiple rendering/input backend support (see below)
- Save data to csv

# Backend support

This project theoretically support all the backends that imgui supports. However, I do not have the time and effort to support all the platform, so I only picked ones that's meaningful to me.

| Graphics  | Input/Windowing | Status | Comment |
| --------- | --------------- | ------ | ------- |
| D3D11     | Win32 | Supported, w/ binary | For Windows |
| D3D12 | Win32 | Supported, w/ binary | For Windows |
| OpenGL | glfw | Supported, w/o binary | Mainly for Linux. (Linux folks can figure out how to compile it right?)
| Metal | glfw | Supported, w/o binary | For macOS |

# Future plans

- Alternatively use [libusb](https://github.com/libusb/libusb) as input backend. (For accurately measure latencies directly from USB controller, and identify latencies from windowing systems/compositors)
