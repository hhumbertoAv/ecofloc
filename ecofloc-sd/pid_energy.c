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


#include "pid_energy.h"

/* 
* storage_features is declared as extern in pid_energy.h and defined in main.c. 
* Negative values indicate an error, such as when the features file is unreachable.
*/

sd_features storage_features;
volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig)
{
    keep_running = 0;
}

double pid_energy(int pid, int interval_ms, int timeout_s)
{

    double total_energy = 0.0;
    
    long initial_read_bytes = read_bytes(pid);
    long initial_written_bytes = written_bytes(pid);

    signal(SIGINT, handle_sigint);

    time_t start_time;
    start_time = time(NULL);

    //All to milliseconds to get the iterations to perform
    int total_iterations = (int)(timeout_s * 1000.0 / interval_ms);

     //68 years :)
    if (timeout_s < 0) timeout_s = INT_MAX;

    /*
    * PATCH: Instead of stopping the loop based on timeout expiration, EcoFloc now iterates 
    * based on the computed number of iterations. 
    * This approach prevents inconsistencies when handling multiple PIDs. 
    * Example: If processing all PIDs in an iteration takes longer than the specified interval, 
    * relying solely on elapsed time could cause an early exit before completing the intended cycles.
    */

    //while (keep_running && (time(NULL) - start_time) <= timeout_s)
    int iteration=1;
    while (keep_running && iteration <= total_iterations)
    {
        struct timespec interval = {interval_ms / 1000, (interval_ms % 1000) * 1000000};
        nanosleep(&interval, NULL);

        long final_read_bytes = read_bytes(pid);
        long final_written_bytes = written_bytes(pid);

        double interval_seconds = (double) interval_ms / 1000.0;

        /* 
        * In some cases the final bytes > initial bytes in /proc
        * TODO -> Verify if that is for threads management. A mutex is pertinent here? 
        */
        if(final_read_bytes < initial_read_bytes)
            final_read_bytes = initial_read_bytes = 0.0;
        if(final_written_bytes < initial_written_bytes)
            final_written_bytes = initial_written_bytes = 0.0;
        
            

        long read_rate = (long)((final_read_bytes - initial_read_bytes) / interval_seconds);
        long write_rate = (long)((final_written_bytes - initial_written_bytes) / interval_seconds);

        double read_power = storage_features.read_power * (double)read_rate / storage_features.read_max_rate;
        double write_power = storage_features.write_power * (double)write_rate / storage_features.write_max_rate;

        double avg_interval_power = read_power + write_power;

        double interval_energy = avg_interval_power * interval_seconds; // Energy in joules for the interval

        write_results(pid, time(NULL) - start_time, avg_interval_power,interval_energy, iteration);

        total_energy += interval_energy;

        initial_read_bytes = final_read_bytes;
        initial_written_bytes = final_written_bytes;

        iteration++;
    }

    return total_energy;
}
