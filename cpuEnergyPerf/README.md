# CPU Energy Performance Tool (`cpuEnergyPerf`) Installation and Usage

## Installation

To install the `cpuEnergyPerf` application on your computer, follow these steps:

1. **Compile the Source Code:**
   - Open your terminal.
   - Navigate to the directory containing the `cpuEnergyPerf` source code.
   - Execute the command `make` to compile the application.

2. **Uninstallation:**
   - To uninstall, run `make clean` in the same directory. This will remove compiled binaries and clean up the directory.

## System Requirements

Before installing `cpuEnergyPerf`, ensure that you have `rdmsr` installed on your system. This tool is required for reading model-specific registers (MSR) necessary for energy measurement.

- **For Arch-based distributions:**
  - Run as root `# pacman -S msr-tools`

- **For Debian-based distributions:**
  - Run as root `# apt-get install msr-tools`

## Usage

To run the `cpuEnergyPerf` program, you need to execute it as root and specify certain parameters:

- **Execute as Root:**
  - Run the program using `sudo ./ePerfCPU.out` with the necessary options.

- **Options:**
  1. `-p`: Specify the Process ID (PID) of the process you want to analyze.
  2. `-i`: Set the measurement interval (i.e., how frequently the application will measure power consumption).
  3. `-t`: Define the total duration of the analysis period.

  Example Command:

  `# sudo ./ePerfCPU.out -p [PID] -i [interval in seconds] -t [total time in seconds]`

## Important Considerations

- If the `cpuEnergyPerf` tool reports a power consumption result of `0`, this could indicate that the measurement interval and/or the total analysis time are too short. In such cases, try incrementing the measurement interval and extending the total analysis time.

- If you get an error related to the non-existence of "/sys/devices/system/cpu/...", please ensure you are running the OS on a real machine and not in a virtual environment. This consideration is pertinent for this first version of the tool.


