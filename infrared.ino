/*
 * Andrés Botero
 * Modificado de: http://playground.arduino.cc/Code/InfraredReceivers
 * 
 *
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>

#define TIMER_RESET  TCNT1 = 0
#define SAMPLE_SIZE  68

int IRpin = 2;
unsigned int TimerValue[SAMPLE_SIZE];
char direction[SAMPLE_SIZE];
byte change_count;
long  byte1[8],byte2[8],byte3[8], byte4[8];
byte A, B, C, D;

long time;

void setup() {
	Serial.begin(9600);
	Serial.println("Analyze IR Remote");
	TCCR1A = 0x00;          // COM1A1=0, COM1A0=0 => Disconnect Pin OC1 from Timer/Counter 1 -- PWM11=0,PWM10=0 => PWM Operation disabled
	TCCR1B = 0x03;          // 16MHz clock with prescaler means TCNT1 increments every 4uS  
	TIMSK1 = 0x00;          
	pinMode(IRpin, INPUT);
}

void loop() {
	change_count = 0;
	while(digitalRead(IRpin) == HIGH) {}                                 
	TIMER_RESET;
	TimerValue[change_count] = TCNT1;
	direction[change_count++] = '0';
	while (change_count < SAMPLE_SIZE) {
		if (direction[change_count-1] == '0') {
			while(digitalRead(IRpin) == LOW) {
				if (TCNT1 > 100000) return; // timeout
			}
			TimerValue[change_count] = TCNT1;
			direction[change_count++] = '1';
		} else {
			while(digitalRead(IRpin) == HIGH) {
				if (TCNT1 > 100000) return; // timeout
			}
			TimerValue[change_count] = TCNT1;
			direction[change_count++] = '0';
		}
	}
	/*
	change_count = 0;
	while (change_count < SAMPLE_SIZE) {
		time = (long) TimerValue[change_count] * 4;
		Serial.print(time); 
		Serial.print('\t');
		Serial.println(direction[change_count++]);
	}
	Serial.println("Bit stream end!");
	*/
	/******* Decoding *******/
	change_count = 4;
	for (int idx = 0; idx < 8; ++idx) {
		byte1[idx] = (long) TimerValue[change_count] - (long) TimerValue[change_count-1];
		change_count += 2;
	}
	for (int idx = 0; idx < 8; ++idx) {
		byte2[idx] = (long) TimerValue[change_count] - (long) TimerValue[change_count-1];
		change_count += 2;
	}
	for (int idx = 0; idx < 8; ++idx) {
		byte3[idx] = (long) TimerValue[change_count] - (long) TimerValue[change_count-1];
		change_count += 2;
	}
	for (int idx = 0; idx < 8; ++idx) {
		byte4[idx] = (long) TimerValue[change_count] - (long) TimerValue[change_count-1];
		change_count += 2;
	}
  
		
	A = decode(byte1);
	B = decode(byte2);
	C = decode(byte3);
	D = decode(byte4);
	bool valid = true;

	valid = valid && (A == (byte)~B);
	valid = valid && (C == (byte)~D);
	if (!valid) {
		Serial.println("INVALID");
		return;
	}

	// Serial.println("Result:");
	char tmp[8];
	sprintf(tmp, "%02X", A);
	Serial.print(tmp);
	// Serial.println(A, BIN);
	// Serial.println(~A, BIN);
	// sprintf(tmp, "%02X", B);
	// Serial.println(tmp);
	// Serial.println(B, BIN);
	// Serial.println(~B, BIN);
	sprintf(tmp, "%02X", C);
	Serial.println(tmp);
	// Serial.println(C, BIN);
	// sprintf(tmp, "%02X", D);
	// Serial.println(tmp);
	// Serial.println(D, BIN);

	delay(200);
}

byte decode(long arr[8]) {
	byte out;
	for(int i=0;i<8;i++){
		bool val = arr[i]>250;
		bitWrite(out, 7-i, val);
	}
	return out;
}
