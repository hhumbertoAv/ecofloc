# Storage Device Energy Performance Tool (`ePerfSD`) Installation and Usage

## Installation

To install the `ePerfSD` application on your computer, follow these steps:

1. **Compile the Source Code:**
   - Open your terminal.
   - Navigate to the directory containing the `ePerfSD` source code.
   - Execute the command `make` to compile the application.

2. **Uninstallation:**
   - To uninstall, run `make clean` in the same directory. This will remove compiled binaries and clean up the directory.

## Usage

To run the `ePerfSD` program, you need to execute it as root and specify certain parameters:

- **Execute as Root:**
  - Run the program using `sudo ./ePerfSD.out` with the necessary options.

- **Options:**
  1. `-p`: Specify the Process ID (PID) of the process you want to analyze.
  2. `-i`: Set the measurement interval (i.e., how frequently the application will measure power consumption).
  3. `-t`: Define the total duration of the analysis period.

  Example Command:

`./ePerfSD.out -p [PID] -i [interval in seconds] -t [total time in seconds]`

  
## Important Considerations

If the `ePerfSD` tool reports a power consumption result of `0`, this could indicate that the measurement interval and/or the total analysis time are too short. In such cases, try incrementing the measurement interval and extending the total analysis time.

