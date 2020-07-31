#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <ctype.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <math.h>

/*
 * If -r read
 * If -w write
 */


/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

#define READ_OPTION 'r'
#define WRITE_OPTION 'w'
#define MAXNUM_OPTION 'c'
#define PORT_OPTION 'p'

#define BUFFER_LEN 210

#define OUTPUT_PIN 7


static int READ = 0;
static int WRITE = 0;
static int NUM_CHARACTERS = 0;

static char port_name[200];

int open_port(char* port_name)
{
	int fd; /* File descriptor for the port */

	fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1){
   /*
    * Could not open the port.
    */
		fprintf(stderr, "open_port: Unable to open %s\n", port_name);
		exit(1);
	}
	else{
		fcntl(fd, F_SETFL, 0);
	}
	return fd;
}
/* returns number of bytes in buffer */
int numBytes(int fd){

	int bytes;

	ioctl(fd, FIONREAD, &bytes);

	return bytes;

}

void setSerialOptions( int fd ){

	struct termios options;

	/*
	 * Get the current options for the port...
	 */

	tcgetattr(fd, &options);

	/*
	 * Set the baud rates to 9600...
	 */

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	/*
	 * Enable the receiver and set local mode...
	 */

	options.c_cflag |= (CLOCAL | CREAD);
	
	/*
	*  Raw Input Text
	*/

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	/*
	*  Set 8 data bits per char
	*/
	options.c_cflag |= CS8;

	/*
	*  Raw Output
	*/
	options.c_oflag &= ~OPOST;

	/*
	 * VTIME is amount of time to wait for the characters
	 * VMin is the amount of characters minimum to read
	 * options.c_cc.VMIN optios.c_cc.VTIME
	 */

	options.c_cflag |= (CLOCAL | CREAD);

	/*
	 * Set the new options for the port...
	 */

	tcsetattr(fd, TCSANOW, &options);

}

void processOption(char* arg){
	int len = strlen(arg);
	char temp[10];
	int temp_index = 0;
	for (int i = 1; i < len; i++){
		switch (arg[i]){
			case READ_OPTION:
				READ = 1;
				break;
			case WRITE_OPTION:
				WRITE = 1;
				break;
			case MAXNUM_OPTION:
				while(  isdigit(arg[i+1])  ){
					temp[temp_index++] = arg[i+1];
					i++;
				}
				temp[temp_index] = '\0';
				if(strlen(temp) > 0){
					NUM_CHARACTERS = atoi(temp);
				}
				break;
			default:
				fprintf(stderr, "Invalid Option: %c\n", arg[i]);
		}

	}
}








void processImplementedOptions(int argc, char** argv){
	for (int i = 0; i < argc; i++){
		if (  strcmp(argv[i],"-p") == 0   ){
			port_name[0] = '\0';
			strcpy(port_name, argv[i+1]);
			i++;
		}else if (argv[i][0] == '-'){
			processOption(argv[i]);
		}
	}
}



// This function tells you if you have the start index of the requested word
int isNextWord(char* buffer, int offset, char* str){

	for (int j = 0; j < strlen(str); j++, offset++  ){

		if(buffer[offset] == str[j]){
			continue;
		}else{
			return 0;
		}

	}
	return 1;
}


// Returns zero if not on right place, returns 1 if last read char was newline
int findCorrectPlace(int fd, int bytes_left_in_buffer){
	char byte;
	while (bytes_left_in_buffer >= 1){ // locate correct place
		read(fd, &byte, 1);
		bytes_left_in_buffer--;
		if (byte == '\n'){
			return 1;
		}
	}
	return 0;
}

/*
$VNINS,000241.350006,0000,8080,-051.757,+019.129,+047.031,+00.00000000,+000.00000000,+00000.000,+000.000,+000.000,+000.000,90.0,00.0,0.00*6F

*/
void serialRead( int fd ){

	int bytes_in_buffer;
//	char byte;
	char yaw_c[9];
	char pitch_c[9];
	char roll_c[9];
	char buffer[BUFFER_LEN];
//	int index = 0;
	int isExtended = 0;
	double yaw, pitch, roll;
	yaw_c[8] = '\0';
	pitch_c[8] = '\0';
	roll_c[8] = '\0';
	while (1){
		bytes_in_buffer = numBytes(fd);
		if( bytes_in_buffer >= BUFFER_LEN ){
			read(fd , buffer, BUFFER_LEN);
			// process current data
			for (int i = 0; i < BUFFER_LEN-60; i++){
				if(isNextWord(buffer, i ,"$VNINS")){
					for(int j = 0; j < 8; j++){
						yaw_c[j] = buffer[i+31+j];
						pitch_c[j] = buffer[i+40+j];
						roll_c[j] = buffer[i+49+j];
					}
					yaw = atof(yaw_c);
					pitch = atof(pitch_c);
					roll = atof(roll_c);
					fprintf(stdout, "yaw: %f\n", yaw);
					fprintf(stdout, "pitch: %f\n", pitch);
					fprintf(stdout, "roll: %f\n", roll);

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
			}

			// END ProcData
//		fprintf(stdout,"%c",buffer[index]);
		}
	}
}




/*		if( bytes_in_buffer >= 1 ){
			read(fd , &byte, 1);
			fprintf(stdout,"%c",byte);
		} */








int main(int argc, char** argv){

/* Process Implemented Options */
	processImplementedOptions(argc,argv);


/* Open Port */
	int fd = open_port(port_name);


/* Set the Serial Port Options */
	setSerialOptions(fd);

	sleep(3);

/* WiringPi Setup */
        wiringPiSetupPhys();
        pinMode(OUTPUT_PIN,OUTPUT);

// Next make ReadWrite function
	if(READ){
		serialRead(fd);
	}

	close(fd);

	return 0;
}


