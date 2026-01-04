# Autoclicker

A terminal-based autoclicker for Linux, written in C.

## Description

This program allows you to automate mouse clicks. It provides multiple modes for clicking, including a feature to record and play back a mouse movement pattern. The application runs in the background and is controlled by global hotkeys.

**Disclaimer:** This project is designed for Linux and interacts directly with low-level input devices. It requires running with sufficient permissions (e.g., as root or by adding the user to the `input` group) to read keyboard events and create a virtual mouse device.

Use this command to add yourself to the input group, not requiring sudo:
```
sudo usermod -aG input $(whoami)
```

## Features

*   **Multiple Clicking Modes:**
    *   Normal: Click indefinitely at a set interval.
    *   Timer: Click for a specified duration.
    *   Amount: Click a specific number of times.
    *   Pattern: Replay a recorded sequence of mouse movements and clicks.
*   **Mouse Pattern Recording:** Record a sequence of movements and clicks to be replayed later.
*   **Runtime Configuration:** Change settings like click interval and mode on the fly using hotkeys.
*   **Persistent Configuration:** Your settings are saved to a `.config` file in the same directory as the executable and loaded on startup.

## Building from Source

This project uses CMake.

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/karl2561/autoclicker
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

Run the executable with superuser privileges or without if you are in the input group:
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

## Warning: create_pattern

Recording and playing a pattern technically works, the coordinates are being captured correctly, saved and replayed.

However due to limitations on my window manager wayland I could not fully test nor implement it.
Current limitations:
  - Hardcoded screensize (height & width inside constants.h)
  - Hardcoded starting coordinates (0, 0) due to lack of absolute coordinates.
  - Scaling applied by the window manager is ignored, distoring the coordinates.

This feature could greatly improved with several ideas allowing:
  - To store multiple patterns (easily achieved by storing them each as a file inside a dir, listing them all for the user to pick, overwrite, name or delete)
  - Not only storing the position but also the KEY_ID to not only allow mouse presses, but also any other input
  - Storing the delay between presses (or setting it programmatically, leaving it blank to default to the current setting) to allow for custom delays
  - Being able to create composite patterns which repeat subpatterns a certain amount or chaining different subpattern together.
  - This allows to easily share patterns (they just must exist int the specified dir)
  - Interface to programmatically create patterns (entering coordinates, delay and key for each keystrokes

## License

Licensed unter MIT License
