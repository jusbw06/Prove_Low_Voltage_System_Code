#include <unistd.h>
#include<stdio.h>
#include<string.h>

#define USB_PATH "/dev/ttyUSB5"

#define VERBOSE_OPTION 'v'

static int VERBOSE = 0;

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

int main(int argc, char** argv){

	processImplementedOptions( argc, argv);

	int serial_pid;

	serial_pid = fork();
	//ischild
	if (serial_pid == 0){
		if (VERBOSE){
			execl("serial_read","serial_read","-r","-p",USB_PATH,"-exec","processData","-v",NULL);
		}
		execl("serial_read","serial_read","-r","-p",USB_PATH,"-exec","processData",NULL);
	}
	

	pause();


	return 0;

}

