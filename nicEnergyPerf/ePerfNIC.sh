<<lic
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
lic

#!/bin/bash

################################################################################
# Script Name: ePerfNIC.sh
# Description: This script calculates a PID's NIC power consumption.
# From variables: PID transfer rate and NIC MAX transfer rate
# Usage: ePerfNIC.sh -p PID -t windowTime milliseconds
# Note: A safe windowTime is 100 msecs, otherwise empty values may exist
    #WARN: Lower values make the script calculate low initial values
# Important REFs and values for the NIC: 
  # source to intel confidential (?) sheet specs: 
  #https://www.tonymacx86.com/attachments/cnvi-and-9560ngw-documentation-pdf.342854/
  #https://fccid.io/B94-9560D2WZ/User-Manual/Users-Manual-3800018.pdf

  #Power TpT – 11n HB-40 Rx 11n (at max TpT) 550 mW
  #TpT – 11ac HB-80 Tx 11ac (at max TpT) 1029 mW

  #11ac 160 MHz 2SS Rx Conductive, best attenuation, TCP/IP 1204 Mbps - 150500 KBps
  #11ac 160 MHz 2SS TX Conductive, best attenuation, TCP/IP 1220 Mbps - 152500 KBps
# NIC_INFO: the intel 6 AX201 has Capabilities: [c8] Power Management version 3 (lspci -v)
#...we can custom it depending on the kernel driver for further optimizations
################################################################################


pid=0
windowTime=0

nic_energy=0
nic_power_AVG=0

getInput()
{  
  while getopts "t:p:" opt; do
    case ${opt} in
      t )
        windowTime=$OPTARG
        ;;
      p )
        pid=$OPTARG
        ;;
      \? )
        echo "Invalid option: -$OPTARG" >&2
        exit 1
        ;;
      : )
        echo "Option -$OPTARG requires an argument." >&2
        exit 1
        ;;
    esac
  done
}

verifyInput()
{

  if [ $windowTime -lt 100 ]; then
    echo "I need a bigger windowTime :(..."
    exit
  fi
  if [ ! -e "/proc/$1/stat" ]; then #If the process does not exist -> exit
    echo "Non-existent PID"
    exit
  fi

}

getNICCons()
{

count=0
uplCount=0
dlCount=0 

while IFS=' ' read -r upload download; do

    uplCount=$(echo "$uplCount + $upload" | bc)
    dlCount=$(echo "$dlCount + $download" | bc)
    count=$(echo "$count + 1" | bc) 

# the last to words of nethogs output are upl and downl rates. STDBUFF is for disable buffering
done < <(timeout $(echo "$windowTime * 0.001" | bc) nethogs wlo1 -t -P $pid -v 0 -d 0.1 | \
      stdbuf -i0 -o0 -e0 grep $pid | stdbuf -oL awk '{print $(NF-1),$NF}')
      #nethogs with steps of 100 msecs


upload_rate_avg=$(echo "scale=10; $uplCount / $count" | bc)
download_rate_avg=$(echo "scale=10; $dlCount / $count" | bc)

#from the datasheet (look above)
max_download_power=0.55 #W
max_upload_power=1.029 #W

#from the datasheet (look above)
max_download_rate=150500 #KBps
max_upload_rate=152500 #KBps

upload_power=$(echo "scale=10; $max_upload_power*($upload_rate_avg / $max_upload_rate)" | bc)
download_power=$(echo "scale=10; $max_download_power*($download_rate_avg / $max_download_rate)" | bc)

nic_power_AVG=$(echo "scale=10; $upload_power + $download_power" | bc)
nic_energy=$(echo "scale=10; $nic_power_AVG*$windowTime*0.001" | bc)


}

verifyPrintOutput()
{
    #verify if it's a non empty numerical value  
  if [[ ! -z $nic_energy ]] && \
     [[ $nic_energy =~ ^[0-9]*([.][0-9]+)?$ ]] && \
     [[ ! -z $nic_power_AVG ]] && \
     [[ $nic_power_AVG =~ ^[0-9]*([.][0-9]+)?$ ]]; then

        echo nic_energy_J: $nic_energy
        echo nic_avgPower_W: $nic_power_AVG
  else
        echo error somewhere
    fi
}

main()
{
  getInput "$@"
  verifyInput
  getNICCons 2> /dev/null
  verifyPrintOutput
}

main "$@"
