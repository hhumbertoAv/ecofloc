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
#include "ePerfSD.h"


////////////////////////////////////////////////////////////////////////////////////
//       Main :)
///////////////////////////////////////////////////////////////////////////////////



int main(int argc, char *argv[]) {
    
    //Options
    int opt;
    int pid;
    int interval_milliseconds = -1;
    int total_time_seconds = -1;
    int verbose = 1;
    
    static struct option long_options[] = {
            {"no-verbose", no_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };

    int long_index = 0;
    while((opt = getopt_long(argc, argv, "p:i:t:", long_options, &long_index)) != -1) {
            switch(opt) {
                case 'p':
                pid = atoi(optarg);
                break;
            case 'i':
                interval_milliseconds = atoi(optarg);
                break;
            case 't':
                total_time_seconds = atoi(optarg);
                break;
            case 'v':
                verbose = 0;
                break;
            default:
                fprintf(stderr, "Usage: %s -p PID -i interval_milliseconds -t total_time_seconds [--no-verbose]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }


   

    calculate_sd_power(pid,interval_milliseconds, total_time_seconds);


    return EXIT_SUCCESS;
}

