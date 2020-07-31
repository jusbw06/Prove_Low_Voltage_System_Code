#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>

#define VERBOSE_OPTION 'v'

static int VERBOSE = 0;
static int isExtended = 0;

void processOption(char* arg){
	int len = strlen(arg);
	for (int i = 1; i < len; i++){
		switch (arg[i]){
			case VERBOSE_OPTION:
				VERBOSE = 1;
				break;
			default:
				fprintf(stderr, "Invalid Option: %c\n", arg[i]);
		}

	}
}

void processImplementedOptions(int argc, char** argv){
	for (int i = 1; i < argc; i++){
		if (argv[i][0] == '-'){
			processOption(argv[i]);
		}
	}
}

void initializeMotor(){
	int pid = fork();
	// ischild
	if (pid == 0){
	         execl("motorGo","motorGo","OFF",NULL);
	}

}

void toggleMotor(int pitch){

	//turn on motor
	if(abs(pitch) > 30 && isExtended == 0){
		int pid = fork();
		// ischild
		if (pid == 0){
	       	         execl("motorGo","motorGo","CW",NULL);
	        }
		isExtended = 1;
	}else if(abs(pitch) <= 30 && isExtended == 1){
		int pid = fork();
		// ischild
		if (pid == 0){
		         execl("motorGo","motorGo","CCW",NULL);
	        }
		isExtended = 0;
	}


}


void processData(){

	char null_buffer[100];
	char yaw_c[9];
	char pitch_c[9];
	char roll_c[9];
	double yaw,pitch,roll;
	int byte;
	yaw_c[8] = '\0';
	pitch_c[8] = '\0';
	roll_c[8] = '\0';
	while ( ( byte = fgetc(stdin) ) != EOF ){
// MODIFY for speed         < ==========================
		if ( byte == '$'){
			fread(null_buffer,sizeof(char),30,stdin);
			fread(yaw_c, sizeof(char), 8, stdin);
			fgetc(stdin);
			fread(pitch_c, sizeof(char), 8, stdin);
			fgetc(stdin);
			fread(roll_c, sizeof(char), 8, stdin);
		}
//			fprintf(stdout, "yaw: %s\n", yaw_c);
//			fprintf(stdout, "pitch: %s\n", pitch_c);
//			fprintf(stdout, "roll: %s\n", roll_c);
		yaw = atof(yaw_c);
		pitch = atof(pitch_c);
		roll = atof(roll_c);
		if (VERBOSE){
			fprintf(stdout, "yaw: %f\n", yaw);
			fprintf(stdout, "pitch: %f\n", pitch);
			fprintf(stdout, "roll: %f\n", roll);
		}

	// Extend or Retract Motor
		toggleMotor(pitch);


	}

}



int main(int argc, char** argv){


// Process Options
	processImplementedOptions(argc, argv);

// Process Data and Toggle Motor
	initializeMotor();
	processData();


	return 0;

}




