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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <getopt.h>


#define PROC_IO_PATH_FORMAT "/proc/%d/io"
#define READ_BYTES_KEY "read_bytes:"
#define WRITE_BYTES_KEY "write_bytes:"

//Important ref: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/filesystems/proc.rst?id=HEAD#l1305


////////////////////////////////////////////////////////////////////////////////////
// CONF SECTION -> Depends on sdFeatures.txt (I/O watts and max bytes per second) 
///////////////////////////////////////////////////////////////////////////////////


// Struct to hold the SD features. Variables' mame correspond to file fileds 
typedef struct {
    float write_power; //in Watts
    float read_power; //in Watts
    long write_max_rate; //in bytes per second
    long read_max_rate; //in bytes per second
} StorageValues;


void extract_SD_features(const char* filename, StorageValues* values) {
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
            fprintf(stderr, "Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        
        if (line[0] == '#') {
                continue;
        }
            // Parse each line
        if (sscanf(line, "write_power=%f", &(values->write_power)) == 1) {
                // Successfully parsed write_power
        } else if (sscanf(line, "read_power=%f", &(values->read_power)) == 1) {
                // Successfully parsed read_power
        } else if (sscanf(line, "write_max_rate=%ld", &(values->write_max_rate)) == 1) {
                // Successfully parsed write_max_rate
        } else if (sscanf(line, "read_max_rate=%ld", &(values->read_max_rate)) == 1) {
                // Successfully parsed read_max_rate
        }
    }

    fclose(file);
}

////////////////////////////////////////////////////////////////////////////////////
//       POWER FORMULA SECTION 
///////////////////////////////////////////////////////////////////////////////////

// Struct to hold the power values
typedef struct {
 
    float read_power;
    float write_power;
    float total_power;

} PowerValues;

PowerValues power_formula(StorageValues *values, long read_rate, long write_rate) {
        
    PowerValues power_values;
    power_values.read_power = values->read_power * (float)read_rate / values->read_max_rate;
    power_values.write_power = values->write_power * (float)write_rate / values->write_max_rate;
    power_values.total_power = power_values.read_power + power_values.write_power;

    return power_values;
}


////////////////////////////////////////////////////////////////////////////////////
//       I/O RATES && POWER SECTION 
///////////////////////////////////////////////////////////////////////////////////


long get_io_bytes(int pid, const char* key) {

//This function opens the specified /proc/<PID>/io file, reads the file line by line until it finds the line that starts with the specified key ("read_bytes:" or "write_bytes:"), and returns the number of bytes.
    char path[256];
    snprintf(path, sizeof(path), PROC_IO_PATH_FORMAT, pid);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
            perror("Could not open /proc/<PID>/io file");
        exit(1);
    }

    char line_key[256];
    long bytes;
    // Read the file until the desired key is found
    while (fscanf(file, "%s %ld", line_key, &bytes) == 2) {
            if (strcmp(line_key, key) == 0) {
                break;
        }
    }

    fclose(file);
    // Return the number of bytes associated with the key
    return bytes;
}


typedef struct {
    double power;
    double time;
    double energy;
} SDPowerAndTime;


void calculate_power(int pid, int interval_milliseconds, double total_time,  StorageValues* s_values) {

    //This function calculate the power consumption from the transfer rate
    double accumulated_time = 0.0;
    
    float total_power=-1;
    long read_rate,write_rate;

    int iteration=0;


    // Read the initial read and write bytes from /proc/<pid>/io
    long initial_read_bytes = get_io_bytes(pid, READ_BYTES_KEY);
    long initial_write_bytes = get_io_bytes(pid, WRITE_BYTES_KEY);

    //Create a timespec struct to represent the interval at which we're collecting data (nanosleep req.)
    struct timespec interval = {interval_milliseconds / 1000, (interval_milliseconds % 1000) * 1000000};

    // Record the start time
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Run the loop for total_time_seconds
    //TODO -> See if the logic of calculete the iterations number is better -> see CPU.c logic
    

     do {
            // Sleep for the given interval
        nanosleep(&interval, NULL); 
        // After the sleep, read the final read and write bytes from /proc/<pid>/io
        long final_read_bytes = get_io_bytes(pid, READ_BYTES_KEY);
        long final_write_bytes = get_io_bytes(pid, WRITE_BYTES_KEY);

        // Calculate the interval in seconds as a float
        float interval_seconds = (float) interval_milliseconds / 1000;

        //Calculate the r/w rates by finding the difference between final and initial byte values and dividing by the interval
        read_rate = (long) ((final_read_bytes - initial_read_bytes) / interval_seconds);
        write_rate = (long) ((final_write_bytes - initial_write_bytes) / interval_seconds);

        // If verbose mode is on, or if read and write rates are not 0, print the read and write rates
        if (( read_rate != 0 || write_rate != 0)) {

            PowerValues p_values;
            power_formula(s_values,read_rate,write_rate);
            p_values=power_formula(s_values, read_rate, write_rate); 
            total_power = p_values.total_power;
            

            //printf("%s rate: %ld bytes/sec\n", READ_BYTES_KEY, read_rate);
            //printf("%s rate: %ld bytes/sec\n", WRITE_BYTES_KEY, write_rate);
            //printf("Total Power = %f\n", total_power);
        }


        //A next step in the iteration
        initial_read_bytes = final_read_bytes;
        initial_write_bytes = final_write_bytes;


        gettimeofday(&end, NULL);
        double iteration_time = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000000.0;
        accumulated_time += iteration_time;

    } while ((total_time < 0) || (accumulated_time <= total_time));


    
}



////////////////////////////////////////////////////////////////////////////////////
//       Calculate power 
///////////////////////////////////////////////////////////////////////////////////




double calculate_sd_power(int pid, int interval_milliseconds, double total_time_seconds){ 
    
    const char* filename = "sdFeatures.conf";
    StorageValues s_values;
    
    extract_SD_features(filename, &s_values); 
    //printf("write_power = %f, read_power = %f, write_max_rate = %ld, read_max_rate = %ld\n", 
      //     s_values.write_power, s_values.read_power, s_values.write_max_rate, s_values.read_max_rate);

    // pass the addresses of read_rate and write_rate to calculate_rate
    calculate_power(pid,interval_milliseconds, total_time_seconds, &s_values);

    


}

