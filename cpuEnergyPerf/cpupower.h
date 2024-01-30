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

#include"cpuusage.h"
#include <signal.h>


////////////////////////////////////////////////////////////////////////////////////
// FORMULA W=PCVVF
// 1) Percentage of time that the CPU gives to the process and its threads
// 2) Capacitance
// 3) Voltage 
// 4) Frequency of involved cores (if threads)
///////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// CPU capacitance 
// -> VALUES from the CPU datasheet 
///////////////////////////////////////////////////////////////////////////////////

 double get_cpu_capacitance() {
        
    char buf[256];
    double cpu_freq_tdp;
    double cpu_tdp = 28.0;
    double cpu_voltage_tdp = 1.5;
    double cpu_capacitance;

    // Open and read the base_frequency file
    FILE* freq_file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/base_frequency", "r");
    if (freq_file == NULL) {
            perror("Error opening base_frequency file");
        return -1.0;
    }
    if (fgets(buf, sizeof(buf), freq_file) == NULL) {
            perror("Error reading base_frequency file");
        return -1.0;
    }
    cpu_freq_tdp = strtod(buf, NULL)/1000; //-> MHz to avoid big numbers
    fclose(freq_file);

    // Calculate cpu_capacitance using formula in the thesis
    cpu_capacitance = (0.7 * cpu_tdp) / (cpu_freq_tdp * cpu_voltage_tdp * cpu_voltage_tdp);

    return cpu_capacitance;
}



////////////////////////////////////////////////////////////////////////////////////
// voltage & CPU frequency  -> I only take into account the involved cores
///////////////////////////////////////////////////////////////////////////////////

// Just remember that pointers are outputs of the function 

void get_cpu_frequency_and_voltage_for_pid(int pid, double *avg_freq, double *avg_voltage,
                                                        int *total_threads) {
    
    char stat_path[256];
    int cpu = 0;
    double total_freq = 0;
    double total_voltage = 0;
    *total_threads = 0;
    DIR* task_dir;
    struct dirent* task_dirent;

    sprintf(stat_path, "/proc/%d/task", pid);
    task_dir = opendir(stat_path);
    if(task_dir == NULL) {
            perror("Error opening directory");
            return;
    }
  
    //Iterate threads info to find frequency and voltage
    while((task_dirent = readdir(task_dir)) != NULL) {


            if(task_dirent->d_type == DT_DIR && atoi(task_dirent->d_name) > 0) {
            FILE* stat_file;
            sprintf(stat_path, "/proc/%d/task/%s/stat", pid, task_dirent->d_name);
            stat_file = fopen(stat_path, "r");
            
            if(stat_file != NULL) {
                //Which cpu executes the thread? 
                fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*llu %*llu %*d %*d %*d %*d %*d %*d %*llu %*u %*d %*d %*d %*d %*d %*d %*d %d", &cpu);
                fclose(stat_file);

                char command[128];
                char freq_buffer[128];
                char voltage_buffer[128];
                FILE *freq_pipe;
                FILE *voltage_pipe;
                
                //Frequency of the cpu that executes the thread
                sprintf(command, "cat /sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", cpu);
                freq_pipe = popen(command, "r");
                if (freq_pipe == NULL) {
                    perror("Error opening pipe");
                    return;
                }
                fgets(freq_buffer, 128, freq_pipe);
                pclose(freq_pipe);
                total_freq += atof(freq_buffer)/1000; // MHz to avoid big numbers 

                //echo "scale=2; $(sudo rdmsr -p 5 0x198 -u --bitfield 47:32)/8192" | bc

                //voltage of the core that executes the thread (rdmsr -p coreID)
                sprintf(command, "sudo rdmsr -p%d 0x198 -u --bitfield 47:32", cpu);
                voltage_pipe = popen(command, "r");
                if (voltage_pipe == NULL) {
                    perror("Error opening pipe");
                    return;
                }
                fgets(voltage_buffer, 128, voltage_pipe);
                pclose(voltage_pipe);

                total_voltage += atof(voltage_buffer)/8192; // Adjust voltage according to rdmsr scaling
                (*total_threads)++;

            }
        }
    }
    closedir(task_dir);

    //Average freq and voltage 
    *avg_freq = total_freq / *total_threads; 
    *avg_voltage = total_voltage / *total_threads;
}


////////////////////////////////////////////////////////////////////////////////////
// Calculate power 
////////////////////////////////////////////////////////////////////////////////////



typedef struct {
    double power;
    double time;
    double energy;
} CPUPowerAndTime;


CPUPowerAndTime calculate_cpu_power(int pid, int interval_ms) {
    
    int cores_count;
    double freq, voltage;
    long double total_power=0;

    //That controls the total time of the while
    struct timeval start, end;


    CPUPowerAndTime result;
    result.power=0;
    result.time=0;
    result.energy=0;



     // Define a signal set
    sigset_t signal_set;

    // Add all signals to the set
    sigfillset(&signal_set);    





   sigprocmask(SIG_BLOCK, &signal_set, NULL);


    gettimeofday(&start, NULL);
    // check if the process still exists
    if (kill(pid, 0) == -1) {
           printf("Process %d was killed\n", pid);
           return result;
    }

    double cpu_capacitance = get_cpu_capacitance();
        
    //The sleep logic is inside the next funcion. That's because the usage is relative to the CPU time
    double pid_cpu_usage = get_pid_cpu_usage(pid,interval_ms);
    get_cpu_frequency_and_voltage_for_pid(pid, &freq, &voltage, &cores_count);

    if(cores_count>12) cores_count=12; // TODO -> THREADS or CORES???
    double power = pid_cpu_usage * cpu_capacitance * freq * voltage * voltage * cores_count;
       
      
    gettimeofday(&end, NULL);
    
     sigprocmask(SIG_UNBLOCK, &signal_set, NULL);

    //ERROR HERE??????????????

    //ERROR HERE??????????????
    //ERROR HERE??????????????
    //ERROR HERE??????????????
    //ERROR HERE?????????????? TIME HAS NO SENSE WHEN FINISH THE EXPERIMENT 

    double iteration_time = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000000.0;
    
    if((power > 0) && (power < 30))//Naif way to chek value coherence..to improve 
        result.power=power;
    result.time=iteration_time;
    result.energy=result.power*result.time;


    //printf("pid %d\npower %f\n",pid, result.power);
    return result;


}
    







