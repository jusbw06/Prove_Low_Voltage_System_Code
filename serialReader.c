#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <ctype.h>
#include <stdlib.h>


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
#define EXEC_OPTION "-exec"

#define BUFFER_LEN 1000

static int BS = 1;
static int READ = 0;
static int WRITE = 0;
static int NUM_CHARACTERS = 0;
static int EXEC_INDEX = 0;
static char** exec_list = NULL;
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

// arg_index is start of new command
void populateExecList(int argc, char** argv, int arg_index){

	int list_len = argc - arg_index;
	exec_list = (char**) malloc( (list_len+1) * sizeof(char*));
	for (int i=0; i < list_len; i++){
		exec_list[i] = (char*) malloc( 20 * sizeof(char) );
		exec_list[i][0] = '\0';
		strcpy(exec_list[i],argv[arg_index+i]);
	}
	exec_list[list_len] = NULL;

}


void processImplementedOptions(int argc, char** argv){
	for (int i = 1; i < argc; i++){
		if (  strcmp(argv[i],"-p") == 0   ){
			port_name[0] = '\0';
			strcpy(port_name, argv[i+1]);
			i++;
		}else if( strcmp(argv[i],"-exec") == 0  ){
			EXEC_INDEX = i;
			populateExecList(argc,argv, i+1);
			break;
		}else if (argv[i][0] == '-'){
			processOption(argv[i]);
		}
	}
}

void execRead(int fd){

	dup2(fd, STDIN_FILENO);
	execv( exec_list[0], exec_list );

}
// Put back in line counter
void serialRead( int fd ){
	char bytes[BUFFER_LEN];
	bytes[BS] = '\0';
	while (1){
		while ( numBytes(fd) > BS ){
			read(fd , bytes, BS );
			fprintf(stdout, "%s", bytes);
		}
	}
}

void serialWrite(int fd){
	int byte;
	int count = 0; // line count
	while (  ( byte = getc(stdin) ) != EOF  &&  count < NUM_CHARACTERS ){
			write(fd, &byte , 1);			
		if(byte == '\n'){
			count++;
		}			
	}
}
int main(int argc, char** argv){

/* Process Implemented Options */
	processImplementedOptions(argc,argv);


/* Open Port */
	int fd = open_port(port_name);


/* Set the Serial Port Options */
	setSerialOptions(fd);

	sleep(3);

// Next make ReadWrite function
	if (READ && EXEC_INDEX > 0){
/*
		int i = 0;
		while (exec_list[i] != NULL){
			fprintf(stdout,"%s\n", exec_list[i]);
			i++;
		}
*/

		execRead(fd);
	}
	if(READ){
		serialRead(fd);
	}
	if (WRITE){
		serialWrite(fd);
	}

	close(fd);

	return 0;
}
