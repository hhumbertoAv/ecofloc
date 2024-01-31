# CPU Energy Performance Tool (`cpuEnergyPerf`) Installation and Usage

## Installation

To install the `cpuEnergyPerf` application on your computer, follow these steps:

1. **Compile the Source Code:**
   - Open your terminal.
   - Navigate to the directory containing the `cpuEnergyPerf` source code.
   - Execute the command `make` to compile the application.

2. **Uninstallation:**
   - To uninstall, run `make clean` in the same directory. This will remove compiled binaries and clean up the directory.

## Usage

To run the `cpuEnergyPerf` program, you need to execute it as root and specify certain parameters:

- **Execute as Root:**
  - Run the program using `sudo ./ePerfCPU.out` with the necessary options.

- **Options:**
  1. `-p`: Specify the Process ID (PID) of the process you want to analyze.
  2. `-i`: Set the measurement interval (i.e., how frequently the application will measure power consumption).
  3. `-t`: Define the total duration of the analysis period.

  Example Command:

`#./ePerfCPU.out -p [PID] -i [interval in seconds] -t [total time in seconds]`
