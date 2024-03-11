/*Licensed to the Apache Software Foundation (ASF) under one
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
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s --cpu|--sd|--ram|--nic [options]\n", argv[0]);
        return 1;
    }

    char *app;
    char *args[8]; // Adjust size based on the maximum number of arguments any app can take, including the app name and NULL terminator.

    if (strcmp(argv[1], "--cpu") == 0 || strcmp(argv[1], "--sd") == 0) {
        // CPU and SD share the same options structure.
        if (argc != 8) { //  total 8 args
            printf("Usage for CPU/SD: %s %s -p [PID] -i [interval] -t [duration]\n", argv[0], argv[1]);
            return 1;
        }
        app = (strcmp(argv[1], "--cpu") == 0) ? "./ePerfCPU.out" : "./ePerfSD.out";
        args[0] = app;
        printf("BETOOOO: %s\n", args[0]);
        printf("Current working directory: %s\n", getenv("PWD"));

        args[1] = argv[2]; // "-p"
        args[2] = argv[3]; // PID
        args[3] = argv[4]; // "-i"
        args[4] = argv[5]; // interval
        args[5] = argv[6]; // "-t"
        args[6] = argv[7]; // duration
        args[7] = NULL; // NULL-terminate the arguments.
    } else if (strcmp(argv[1], "--ram") == 0 || strcmp(argv[1], "--nic") == 0) {
        // RAM and NIC share the same options structure but differ from CPU/SD.
        if (argc != 6) { // Expecting 4 arguments plus the program name, total 5
            printf("Usage for RAM/NIC: %s %s -p [PID] -t [interval]\n", argv[0], argv[1]);
            return 1;
        }
        app = (strcmp(argv[1], "--ram") == 0) ? "./ePerfRAM.sh" : "./ePerfNIC.sh";
        args[0] = app;
        args[1] = argv[2]; // "-p"
        args[2] = argv[3]; // PID
        args[3] = argv[4]; // "-t"
        args[4] = argv[5]; // interval
        args[5] = NULL; // NULL-terminate the arguments.
    } else {
        printf("Invalid option: %s\n", argv[1]);
        return 1;
    }

    // Execute the chosen application with the provided arguments.
    if (execvp(app, args) == -1) {
        perror("Error executing the application");
        return 1;
    }

    return 0; // This point is never reached.
}
