#include "Ax12.h"

Ax12Class Ax12;

void setup(){	
	//delay(1000);
	Serial.begin(9600);
	Ax12.begin(&Serial2 , 115200, 40);
	Serial.println("Start");

	///init
	//Ax12.begin(&Serial2 , 1000000, 40);
	//Ax12.setID(1,4);
	//
	//Ax12.setBR(2,0x10);
	//Ax12.setBR(1,0x10);
	//Ax12.setBR(4,0x1);
	//Ax12.reset(0xFE);
	//Ax12.setID(1,2);
}

void loop(){
	if(Serial.available()>0){
	int val=Serial.read();
		if(val=='a'){
			 Ax12.write(1,AX_GOAL_POSITION_L,(unsigned int)212 );
			 //Ax12.move(2,734-100 );
			 //Ax12.move(3,290+100 );
			 
			 //Ax12.move(1,734-100 );
			 Ax12.move(2,290+100 );
			 Ax12.move(3,734-100 );
			 Ax12.move(4,290+100 );
			 Ax12.move(7,290+100 );
		}else if (val=='q'){
			Ax12.move(1,734);
			 Ax12.move(2,290);
			 Ax12.move(3,734);
			 Ax12.move(4,290 );
			 Ax12.move(7,290 );
			 //Ax12.move(2,734-100 );
			 //Ax12.move(7, 290+100 );
		}else if (val=='w'){
			 Ax12.move(1,512 );
			 Ax12.move(7,290 );
		}else if(val=='z'){
			 Ax12.move(1,100 );
			 Ax12.move(4,734-100 );
		}else if (val=='s'){
			 Ax12.move(1,0 );
			 Ax12.move(4, 290+100 );
		}else if(val=='c'){
			Serial.println("c");
			Serial.print("position : ");
			Serial.println(Ax12.read(1,AX_PRESENT_POSITION_L,AX_2BYTE_READ) ,DEC);
			Serial.print("BR : ");
			Serial.println(Ax12.read(1,AX_BAUD_RATE,AX_BYTE_READ) ,DEC);
			Serial.print("temperature : ");
			Serial.println(Ax12.read(1,AX_PRESENT_TEMPERATURE,AX_BYTE_READ) ,DEC);
			Serial.print("Voltage : ");
			Serial.println(Ax12.read(1,AX_PRESENT_VOLTAGE,AX_BYTE_READ),DEC);
			Serial.print("load : ");
			Serial.println(Ax12.read(1,AX_PRESENT_LOAD_L,AX_2BYTE_READ),DEC);
		}else if (val=='x'){
			 Serial.println("x");
			 Ax12.move(1,734 );
			 Ax12.move(4,290 );
		}
	}
}


