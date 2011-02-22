#include "Ax12.h"

// Private Methods //////////////////////////////////////////////////////////////

int Ax12Class::read_error(void)
{
  Time_Counter = 0;
  while(SerialX->available() < 5 & Time_Counter < TIME_OUT){  // Wait for Data
    Time_Counter++;
    delay(1);
    if( SerialX->peek() != 255 ){
      SerialX->read();
    }
  }
	
  while (SerialX->available() > 0){
    Incoming_Byte = SerialX->read();
    if ( Incoming_Byte == 255 & SerialX->peek() == 255 ){
      SerialX->read();                                  // Start Bytes
      SerialX->read();                                   // Ax-12 ID
      SerialX->read();                                  // Length
      Error_Byte = SerialX->read();                       // Error
      return (Error_Byte);
    }
  }

  return (-1);											 // No Ax Response
}

// Public Methods //////////////////////////////////////////////////////////////

void Ax12Class::begin(HardwareSerial * s , long baud, unsigned char D_Pin)
{
  SerialX = s;
  SerialX->begin(baud);	
  pinMode(D_Pin,OUTPUT);
  Direction_Pin = D_Pin;
}	

int Ax12Class::ping(unsigned char ID)
{
  Checksum = ~(ID + AX_READ_DATA + AX_PING);  
      
  digitalWrite(Direction_Pin,HIGH);
  SerialX->write(AX_START);                     
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_READ_DATA);
  SerialX->write(AX_PING);    
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);
    
  return (read_error());              
}

int Ax12Class::setID(unsigned char ID, unsigned char New_ID)
{    
  Checksum = ~(ID + AX_ID_LENGTH + AX_WRITE_DATA + AX_ID + New_ID);  
	
  digitalWrite(Direction_Pin,HIGH);     // Set Tx Mode
  SerialX->write(AX_START);                // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_ID_LENGTH);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_ID);
  SerialX->write(New_ID);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);      // Set Rx Mode
    
  return (read_error());                // Return the read error
}

int Ax12Class::setBD(unsigned char ID, unsigned char Baud_Rate)
{    
  Checksum = ~(ID + AX_BD_LENGTH + AX_WRITE_DATA + AX_BAUD_RATE + Baud_Rate);       
	
  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_BD_LENGTH);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_BAUD_RATE);
  SerialX->write(Baud_Rate);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);      // Set Rx Mode
    
  return (read_error());                // Return the read error
}

int Ax12Class::move(unsigned char ID, long Position)
{
  unsigned char Position_H,Position_L;
  Position_H = Position >> 8;           // 16 bits - 2 x 8 bits variables
  Position_L = Position;
    
  Checksum = ~(ID + AX_GOAL_LENGTH + AX_WRITE_DATA + AX_GOAL_POSITION_L + Position_L + Position_H);
    
  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_GOAL_LENGTH);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_GOAL_POSITION_L);
  SerialX->write(Position_L);
  SerialX->write(Position_H);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode
	
  return (read_error());                 // Return the read error
}

int Ax12Class::moveSpeed(unsigned char ID, long Position, long Speed)
{
  char Position_H,Position_L,Speed_H,Speed_L;
  Position_H = Position >> 8;    
  Position_L = Position;                // 16 bits - 2 x 8 bits variables
  Speed_H = Speed >> 8;
  Speed_L = Speed;                      // 16 bits - 2 x 8 bits variables
    
  Checksum = (ID + AX_GOAL_SP_LENGTH + AX_WRITE_DATA + AX_GOAL_POSITION_L);
  Checksum += (Position_L + Position_H + Speed_L + Speed_H);
  Checksum = ~Checksum;

	
  digitalWrite(Direction_Pin,HIGH);     // Set Tx Mode
  SerialX->write(AX_START);                // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_GOAL_SP_LENGTH);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_GOAL_POSITION_L);
  SerialX->write(Position_L);
  SerialX->write(Position_H);
  SerialX->write(Speed_L);
  SerialX->write(Speed_H);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode
    
  return (read_error());               // Return the read error
}

int Ax12Class::torqueStatus( unsigned char ID, bool Status)
{
  Checksum = ~(ID + AX_TORQUE_LENGTH + AX_WRITE_DATA + AX_TORQUE_ENABLE + Status);
    
  digitalWrite(Direction_Pin,HIGH);   // Set Tx Mode
  SerialX->write(AX_START);              // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_TORQUE_LENGTH);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_TORQUE_ENABLE);
  SerialX->write(Status);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);    // Set Rx Mode
    
  return (read_error());              // Return the read error
}

int Ax12Class::ledStatus(unsigned char ID, bool Status)
{    
  Checksum = ~(ID + AX_LED_LENGTH + AX_WRITE_DATA + AX_LED + Status);
    
  digitalWrite(Direction_Pin,HIGH);   // Set Tx Mode
  SerialX->write(AX_START);              // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_LED_LENGTH);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_LED);
  SerialX->write(Status);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);    // Set Rx Mode
    
  return (read_error());              // Return the read error
}

int Ax12Class::readTemperature(unsigned char ID)
{	
  Checksum = ~(ID + AX_TEM_LENGTH  + AX_READ_DATA + AX_PRESENT_TEMPERATURE + AX_BYTE_READ);
    
  digitalWrite(Direction_Pin,HIGH); 
  SerialX->write(AX_START);
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_TEM_LENGTH);
  SerialX->write(AX_READ_DATA);
  SerialX->write(AX_PRESENT_TEMPERATURE);
  SerialX->write(AX_BYTE_READ);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode
	
  Temperature_Byte = 0;
  Time_Counter = 0;
  while(SerialX->available() < 6 & Time_Counter < TIME_OUT){
    Time_Counter++;
    delay(1);
    if( SerialX->peek() != 255 ){
      SerialX->read();
    }   
  }
	
  while (SerialX->available() > 0){
    Incoming_Byte = SerialX->read();
    if ( Incoming_Byte == 255 & SerialX->peek() == 255 ){
      SerialX->read();                            // Start Bytes
      SerialX->read();                            // Ax-12 ID
      SerialX->read();                            // Length
      if( (Error_Byte = SerialX->read()) != 0 )   // Error
	return (Error_Byte*(-1));
      Temperature_Byte = SerialX->read();         // Temperature
    }
  }
  return (Temperature_Byte);               // Returns the read temperature
}

unsigned int Ax12Class::readPosition(unsigned char ID)
{	
 Serial.println("debut");
 //purge
 while (SerialX->available() > 0){
 SerialX->read();
 }
  Checksum = ~(ID + AX_POS_LENGTH  + AX_READ_DATA + AX_PRESENT_POSITION_L + AX_BYTE_READ_POS);
  
  digitalWrite(Direction_Pin,HIGH); 
  SerialX->write(AX_START);
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_POS_LENGTH);
  SerialX->write(AX_READ_DATA);
  SerialX->write(AX_PRESENT_POSITION_L);
  SerialX->write(AX_BYTE_READ_POS);
  SerialX->write(Checksum);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode
	
  delayMicroseconds(TX_DELAY_TIME);

while (SerialX->available() > 0){
	Serial.println(Serial2.read(),HEX);
}

  /*Position_Long_Byte = 0;Position_Low_Byte=0;Position_High_Byte=0;
  Time_Counter = 0;
  while((SerialX->available() < 7) && (Time_Counter < TIME_OUT)){
    Time_Counter++;
    delay(1);
    if( SerialX->peek() != 255 ){
      SerialX->read();
    }   
  }
	
  //delayMicroseconds(2*TX_DELAY_TIME);
  while (SerialX->available() > 0){
    Incoming_Byte = SerialX->read();
    Serial.print(Incoming_Byte,HEX);Serial.print(",");	
    if ( Incoming_Byte == 0xFF && SerialX->peek() == 0xFF ){
      Serial.print(SerialX->read(),HEX);Serial.print(",");              // Start Bytes
      Serial.print(SerialX->read(),HEX);Serial.print(",");                             // Ax-12 ID
      Serial.print(SerialX->read(),HEX);Serial.print(",");                             // Length
      if( SerialX->peek() != 0 ) {  // Error
	Error_Byte=SerialX->read();
 	Serial.print(Error_Byte,HEX);Serial.print(","); 
	}
	else{
	 Serial.print(SerialX->read(),HEX);Serial.print(","); 
	}
    
      Position_Low_Byte = SerialX->read();            // Position Bytes
      Position_High_Byte = SerialX->read();
	Serial.print(Position_Low_Byte,HEX);Serial.print(","); 
	Serial.print(Position_High_Byte,HEX);Serial.print(","); 
      Position_Long_Byte = Position_High_Byte << 8; 
      Position_Long_Byte = Position_Long_Byte + Position_Low_Byte;
	Serial.print(SerialX->read(),HEX);Serial.println("."); //cheksum
    }
  }*/
 Serial.println("fin");
  return (Position_Long_Byte);     // Returns the read position
}

int Ax12Class::readVoltage(unsigned char ID)
{    
  Checksum = ~(ID + AX_VOLT_LENGTH  + AX_READ_DATA + AX_PRESENT_VOLTAGE + AX_BYTE_READ);
    
  digitalWrite(Direction_Pin,HIGH); 
  SerialX->write(AX_START);
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(AX_VOLT_LENGTH);
  SerialX->write(AX_READ_DATA);
  SerialX->write(AX_PRESENT_VOLTAGE);
  SerialX->write(AX_BYTE_READ);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode 
	
  Voltage_Byte = 0;
  Time_Counter = 0;
  while(SerialX->available() < 6 & Time_Counter < TIME_OUT){
    Time_Counter++;
    delay(1);
    if( SerialX->peek() != 255 ){
      SerialX->read();
    }   
  }
	
  while (SerialX->available() > 0){
    Incoming_Byte = SerialX->read();
    if ( Incoming_Byte == 255 & SerialX->peek() == 255 ){
      SerialX->read();                            // Start Bytes
      SerialX->read();                            // Ax-12 ID
      SerialX->read();                            // Length
      if( (Error_Byte = SerialX->read()) != 0 )   // Error
	return (Error_Byte*(-1));
      Voltage_Byte = SerialX->read();             // Voltage
    }
  }
  return (Voltage_Byte);               // Returns the read Voltage
}

int Ax12Class::setRange(unsigned char ID,long min ,long max)
{    

  //min
  unsigned char min_H,min_L;
  min_H = min >> 8;           // 16 bits - 2 x 8 bits variables
  min_L = min;
  
  Checksum = ~(ID + 5 + AX_WRITE_DATA + AX_CW_ANGLE_LIMIT_L + min_L + min_H);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(5);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_CW_ANGLE_LIMIT_L);
  SerialX->write(min_L);
  SerialX->write(min_H);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

  read_error();
  delayMicroseconds(TX_DELAY_TIME);

  //max
  unsigned char max_H,max_L;
  max_H = max >> 8;           // 16 bits - 2 x 8 bits variables
  max_L = max;
  
  Checksum = ~(ID + 5 + AX_WRITE_DATA + AX_CCW_ANGLE_LIMIT_L + max_L + max_H);
    
  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(5);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_CCW_ANGLE_LIMIT_L);
  SerialX->write(max_L);
  SerialX->write(max_H);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode
	
  return (read_error());                 // Return the read error
}

int Ax12Class::reset(unsigned char ID){ 
  Checksum = ~(ID + 2 + AX_RESET);
    
  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(2);
  SerialX->write(AX_RESET);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

}

/*
Bit	Function
Bit 7	0
Bit 6	If set to 1, torque off when an Instruction Error occurs
Bit 5	If set to 1, torque off when an Overload Error occurs
Bit 4	If set to 1, torque off when a Checksum Error occurs
Bit 3	If set to 1, torque off when a Range Error occurs
Bit 2	If set to 1, torque off when an Overheating Error occurs
Bit 1	If set to 1, torque off when an Angle Limit Error occurs
Bit 0	If set to 1, torque off when an Input Voltage Error occurs
*/
int Ax12Class::alarmShutdown(unsigned char ID,unsigned char data){
  
  Checksum = ~(ID + 4 + AX_WRITE_DATA + AX_ALARM_SHUTDOWN + data);
    
  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(4);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_ALARM_SHUTDOWN);
  SerialX->write(data);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode
}

int Ax12Class::alarmLed(unsigned char ID,unsigned char data){
  
  Checksum = ~(ID + 4 + AX_WRITE_DATA + AX_ALARM_LED + data);
    
  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  SerialX->write(AX_START);                 // Send Instructions over Serial
  SerialX->write(AX_START);
  SerialX->write(ID);
  SerialX->write(4);
  SerialX->write(AX_WRITE_DATA);
  SerialX->write(AX_ALARM_LED);
  SerialX->write(data);
  SerialX->write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode
}



