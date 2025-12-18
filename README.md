# Autoclicker

A terminal-based autoclicker for Linux, written in C.

## Description

This program allows you to automate mouse clicks. It provides multiple modes for clicking, including a feature to record and play back a mouse movement pattern. The application runs in the background and is controlled by global hotkeys.

**Disclaimer:** This project is designed for Linux and interacts directly with low-level input devices. It requires running with sufficient permissions (e.g., as root or by adding the user to the `input` group) to read keyboard events and create a virtual mouse device.

## Features

*   **Multiple Clicking Modes:**
    *   Normal: Click indefinitely at a set interval.
    *   Timer: Click for a specified duration.
    *   Amount: Click a specific number of times.
    *   Pattern: Replay a recorded sequence of mouse movements and clicks.
*   **Mouse Pattern Recording:** Record a sequence of movements and clicks to be replayed later.
*   **Runtime Configuration:** Change settings like click interval and mode on the fly using hotkeys.
*   **Persistent Configuration:** Your settings are saved to a `.config` file in the same directory as the executable and loaded on startup.

## WARNING: create_pattern

Recording and playing a pattern technically works, the coordinates are being captured correctly, saved and replayed. However due to limitations on my window manager wayland I could not fully test that feature.

Due to the limitations I didn't bother to dynamically get the screensize.
Currently it is hardcoded in constants.h.

Also the pattern currently always starts at 0,0 due to the lack of abs coordinates.
Additionally there could be scaling applied by your window manager, distoring the coordinates.

## Building from Source

This project uses CMake.

1.  **Clone the repository:**
    ```sh
    git clone <repository-url>
    cd autoclicker
    ```

2.  **Create a build directory:**
    ```sh
    mkdir build && cd build
    ```

3.  **Run CMake and build the project:**
    ```sh
    cmake ..
    make
    ```
    An executable named `autoclicker` will be created in the `build` directory.

## Usage

Run the executable with superuser privileges:
```sh
sudo ./autoclicker
```

The application runs in the terminal and displays the current configuration.

### Default Hotkeys


*   **ESC:** To exit
*   **F2:** Start/Stop autoclicker
*   **F3:** To show info
*   **F5:** Record a mouse pattern.
    *   **Left-click** to record a position.
    *   **Right-click** to finish recording.
*   **F6:** To play a pattern.
*   **F7:** To change keybindings
*   **ESC:** Exit the program.

These keybindings are configurable at runtime and are saved in the `.config` file.

## Documentation

The project is documented using Doxygen. To generate the documentation:

1.  **Install Doxygen:**
    ```sh
    sudo apt-get install doxygen graphviz # Example for Debian/Ubuntu
    ```
2.  **Generate the documentation:**
    ```sh
    doxygen Doxyfile
    ```
    The output will be in the `build/docs/` directory.

## License

This project is not licensed. Please add a license file.
