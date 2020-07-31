# Prove_Low_Voltage_System_Code

This code was written for the Low Voltage System within the Provelab Solar Car. This code was written to run on a raspberry pi 2, receive input for sensors and control linear actuators via motor drivers.

In order to compile and this code, you must be on a raspberry pi (or on some Linux distribution with the wiringPi library installed). Simply change to the directory and type `make`.

The compilation should output four executables: "manager" "serialRead" "processData" "motorGo"

Each of these executables handle separate jobs and were written to act modularly, that each executable can be copied and applied in other similar applications with little headache.

In order to run the program, run the executable "manager". This executable ties all the other executables together to accomplish the tasks of the Low Voltage System.
Change into the directory and run `./manager`
