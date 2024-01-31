# RAM Energy Performance Tool (`ePerfRAM.sh`) Installation and Usage

## Installation

To use the `ePerfRAM.sh` script on your computer, follow these steps:

1. **Download the Script:**
   - Download the `ePerfRAM.sh` script from the provided source.
   - Ensure the script has execute permissions and run it as root by executing `#chmod +x ePerfRAM.sh` in your terminal.

## Usage

To run the `ePerfRAM.sh` script, specify the Process ID (PID) of the process to monitor and the measurement interval:

- **Execute the Script:**
  - Run `./ePerfRAM.sh` with the necessary options.

- **Options:**
  1. `-p`: Specify the Process ID (PID) of the process to be analyzed.
  2. `-t`: Define the measurement interval, i.e., how frequently the script will check for the process and measure energy consumption.

  Example Command:

`/bin/bash ./ePerfRAM.sh -p [PID] -t [interval in seconds]`

## Important Considerations

- The energy measurement is executed continuously until the specified process (PID) terminates. There is no need to set a total analysis time.
- If the script returns unexpected results or an error, ensure that the PID exists and the measurement interval is correctly specified.
