#include<wiringPi.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MOTOR_B_ENABLE_PIN 11
#define MOTOR_B_PIN_1 13
#define MOTOR_B_PIN_2 15
#define MOTOR_B_PWM_PIN 32



int main(int argc, char** argv){

	if (argc < 2){
		fprintf(stderr,"Error: Not enough input arguments.\n");
		fprintf(stderr,"Usage: motorGo [CW || CCW || OFF]\n");
		exit(0);
	}


// Setup
	wiringPiSetupPhys();
	pinMode(MOTOR_B_ENABLE_PIN,OUTPUT);
	pinMode(MOTOR_B_PIN_1,OUTPUT);
	pinMode(MOTOR_B_PIN_2,OUTPUT);
	pinMode(MOTOR_B_PWM_PIN,PWM_OUTPUT);

// Initialize Pins to Zero Values
	digitalWrite(MOTOR_B_ENABLE_PIN,HIGH);
	digitalWrite(MOTOR_B_PIN_1,LOW);
	digitalWrite(MOTOR_B_PIN_2,LOW);
	pwmWrite(MOTOR_B_PWM_PIN,0);

// Turn on Motor CW
	if(strcmp("CW", argv[1] )  == 0){
		digitalWrite(MOTOR_B_ENABLE_PIN,HIGH);
		digitalWrite(MOTOR_B_PIN_1,HIGH);
		digitalWrite(MOTOR_B_PIN_2,LOW);
		pwmWrite(MOTOR_B_PWM_PIN,1023);
	}

// Turn on Motor CCW
	else if(strcmp("CCW", argv[1] )  == 0){
		digitalWrite(MOTOR_B_ENABLE_PIN,HIGH);
		digitalWrite(MOTOR_B_PIN_1,LOW);
		digitalWrite(MOTOR_B_PIN_2,HIGH);
		pwmWrite(MOTOR_B_PWM_PIN,1023);
	}

	return 0;
}
