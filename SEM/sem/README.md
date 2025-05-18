# Fractal Renderer and Control Interface

This project is a semester assignment for the B3B36PRG course at FEL ČVUT. It consists of two components:

- **`prgsem`**: Main program responsible for graphical user interface (GUI), user interaction, and managing computation requests.
- **`comp_module`**: Standalone computational module that performs fractal computations on request via named pipes.
                     (computaion mostly preformed by computation.c)

## Features

- Modular architecture: control and computation are split into separate processes
- Communication via named pipes (FIFOs)
- SDL2-based graphical interface
- Supports fractal generation and live updates
- Dynamic panning and fractal prameters alteration
- Image and video export using `stb_image_write`

---

## Build Instructions

- Inside SEM/sem run 
    - make clean && make
    - ./create_pipes.sh

- Now in seperate terminals (in the same repository) run
    - ./prgsem
    - ./comp_module


### Dependencies

Make sure the following packages are installed:

- GCC with AddressSanitizer support
- SDL2 and SDL2_image development libraries
- pthreads
- `make`

For example, on Debian/Ubuntu:

```bash
sudo apt install build-essential libsdl2-dev libsdl2-image-dev
```

Using GitHub Codespaces the output window will be opened on port 8080
