/*
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

//TODO -> Calculate the energy of each process, considering the time of each step (is concerns) 


#include"cpupower.h"


////////////////////////////////////////////////////////////////////////////////////
// THE CPU /////
///////////////////////////////////////////////////////////////////////////////////


CPUPowerAndTime calculate_cpu_power_for_pid_and_children(int pid, double interval) {

    CPUPowerAndTime cpu_results; //struct defined in cpupower.h

    // Calculate power for the given PID
    cpu_results= calculate_cpu_power(pid, interval);

    // Path to the /proc/PID/task/TID/children file
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", pid, pid);

    // Open the children file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return cpu_results;
    }

    int child_pid;
    // Read PIDs from the file and calculate power for each child process
    while (fscanf(file, "%d", &child_pid) == 1) {
        
        CPUPowerAndTime child_cpu_results = calculate_cpu_power_for_pid_and_children(child_pid, interval);

        
        cpu_results.power += child_cpu_results.power;
        cpu_results.time += child_cpu_results.time;
        cpu_results.energy += child_cpu_results.energy;

    }

    // Close the file
    fclose(file);
    return cpu_results;
}




//This is the main function for the CPU power
void cpu_power(int pid, double interval, double timeout) {

  
    //struct timeval start, now;
    //PATCHED gettimeofday(&start, NULL);
    
    time_t start, end;
    start=time(NULL);
    CPUPowerAndTime total_results;
    total_results.power=0;
    total_results.time=0;
    total_results.energy=0;



//TIME PATCH HERE...WHEN BIG EXPERMENTS ARE PERFORMED, THE FINAL VALUE OF TIME HAS NO SENSE (var overload?)



//////////////////////////////////////////////////////////////////////////////////////////////////////




    long long int counter=0; //to find the watts avg 

   // PIDPower *pid_power_table = NULL;

    do {
        
        // check if the process still exists
        if (kill(pid, 0) == -1) {
              printf("Process %d was killed\n", pid);
              break;
        }

        counter++;


        //CPUPowerAndTime results=calculate_cpu_power_for_pid_and_children(pid,interval);



        //Let's check children processes apfer
       //*****************************************************************
         CPUPowerAndTime results; //struct defined in cpupower.h

        // Calculate power for the given PID
        results = calculate_cpu_power(pid, interval);
        //********************************************************************





        total_results.power += results.power;
       //PATCHED total_results.time += results.time;
        //PATCHED total_results.energy += results.energy;


        //printf("PID %d\ntotal W %f\ntotal T %f\ntotal E %f\n"
          //  ,pid,results.power,results.time,results.energy);
        

        //gettimeofday(&now, NULL);
        end=time(NULL);

    }while(timeout<0 || difftime(end,start)<timeout);
    //} while ((timeout < 0 || (now.tv_sec - start.tv_sec + (now.tv_usec - start.tv_usec)/1e6) < timeout));
  



    

////////////////////////////////////////////////////////////////////////////////////////

        total_results.time=difftime(end,start);
/////////////////////////////////////////////////////////////////////////

        total_results.power=total_results.power/counter;
        total_results.energy=total_results.power*total_results.time;




        printf("PID %d\ntotal W %f\ntotal T %f\ntotal E %f\n"
          ,pid,total_results.power,total_results.time,total_results.energy);

}






int main(int argc, char **argv) {
    
    int pid = 0;
    double interval_ms = 0.0;
    double total_time_s = -1.0;
    int opt;
    while ((opt = getopt(argc, argv, "p:i:t:")) != -1) {
            switch (opt) {
        case 'p':
            pid = atoi(optarg);
            break;
        case 'i':
            interval_ms = atof(optarg);
            break;
        case 't':
            total_time_s = atof(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s -p PID -i INTERVAL_MILLISECONDS -t TIMEOUT_SECONDS \n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    cpu_power(pid, interval_ms, total_time_s);

    return 0;
}



