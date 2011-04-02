#include "Ax12.h"

// Private Methods //////////////////////////////////////////////////////////////

int Ax12Class::read_error(void)
{
  Time_Counter = 0;
  while((available() < 5) && (Time_Counter < TIME_OUT)){  // Wait for Data
    Time_Counter++;
    delayMicroseconds(1000);
    if( peek() != 255 ){
      read();
    }
  }

  while (available() > 0){
    Incoming_Byte = read();
    if ( (Incoming_Byte == 255) && (peek() == 255) ){
      read();                                  // Start Bytes
      read();                                   // Ax-12 ID
      read();                                  // Length
      Error_Byte = read();                       // Error
      return (Error_Byte);
    }
  }

  return (-1);											 // No Ax Response
}

// Public Methods //////////////////////////////////////////////////////////////

int Ax12Class::reset(unsigned char ID){
  Checksum = ~(ID + 2 + AX_RESET);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(2);
  write(AX_RESET);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

  return (read_error());
}

int Ax12Class::ping(unsigned char ID)
{
  Checksum = ~(ID + AX_READ_DATA + AX_PING);

  digitalWrite(Direction_Pin,HIGH);
  write(AX_START);
  write(AX_START);
  write(ID);
  write(AX_READ_DATA);
  write(AX_PING);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);

  return (read_error());
}

int Ax12Class::setID(unsigned char ID, unsigned char New_ID)
{
  Checksum = ~(ID + AX_ID_LENGTH + AX_WRITE_DATA + AX_ID + New_ID);

  digitalWrite(Direction_Pin,HIGH);     // Set Tx Mode
  write(AX_START);                // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(AX_ID_LENGTH);
  write(AX_WRITE_DATA);
  write(AX_ID);
  write(New_ID);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);      // Set Rx Mode

  return (read_error());                // Return the read error
}

int Ax12Class::setBR(unsigned char ID, unsigned char Baud_Rate)
{
  Checksum = ~(ID + AX_BR_LENGTH + AX_WRITE_DATA + AX_BAUD_RATE + Baud_Rate);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(AX_BR_LENGTH);
  write(AX_WRITE_DATA);
  write(AX_BAUD_RATE);
  write(Baud_Rate);
  write(Checksum);
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
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(AX_GOAL_LENGTH);
  write(AX_WRITE_DATA);
  write(AX_GOAL_POSITION_L);
  write(Position_L);
  write(Position_H);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

  return (read_error());                 // Return the read error
}

int Ax12Class::setMoveSpeed(unsigned char ID, long Position, long Speed)
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
  write(AX_START);                // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(AX_GOAL_SP_LENGTH);
  write(AX_WRITE_DATA);
  write(AX_GOAL_POSITION_L);
  write(Position_L);
  write(Position_H);
  write(Speed_L);
  write(Speed_H);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode

  return (read_error());               // Return the read error
}

int Ax12Class::setTorqueStatus( unsigned char ID, bool Status)
{
  Checksum = ~(ID + AX_TORQUE_LENGTH + AX_WRITE_DATA + AX_TORQUE_ENABLE + Status);

  digitalWrite(Direction_Pin,HIGH);   // Set Tx Mode
  write(AX_START);              // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(AX_TORQUE_LENGTH);
  write(AX_WRITE_DATA);
  write(AX_TORQUE_ENABLE);
  write(Status);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);    // Set Rx Mode

  return (read_error());              // Return the read error
}

int Ax12Class::setLedStatus(unsigned char ID, bool Status)
{
  Checksum = ~(ID + AX_LED_LENGTH + AX_WRITE_DATA + AX_LED + Status);

  digitalWrite(Direction_Pin,HIGH);   // Set Tx Mode
  write(AX_START);              // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(AX_LED_LENGTH);
  write(AX_WRITE_DATA);
  write(AX_LED);
  write(Status);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);    // Set Rx Mode

  return (read_error());              // Return the read error
}

int Ax12Class::setRange(unsigned char ID,long min ,long max)
{

  //min
  unsigned char min_H,min_L;
  min_H = min >> 8;           // 16 bits - 2 x 8 bits variables
  min_L = min;

  Checksum = ~(ID + 5 + AX_WRITE_DATA + AX_CW_ANGLE_LIMIT_L + min_L + min_H);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(5);
  write(AX_WRITE_DATA);
  write(AX_CW_ANGLE_LIMIT_L);
  write(min_L);
  write(min_H);
  write(Checksum);
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
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(5);
  write(AX_WRITE_DATA);
  write(AX_CCW_ANGLE_LIMIT_L);
  write(max_L);
  write(max_H);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

  return (read_error());                 // Return the read error
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
int Ax12Class::setAlarmShutdown(unsigned char ID,unsigned char data){

  Checksum = ~(ID + 4 + AX_WRITE_DATA + AX_ALARM_SHUTDOWN + data);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(4);
  write(AX_WRITE_DATA);
  write(AX_ALARM_SHUTDOWN);
  write(data);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode
  return (read_error());
}

int Ax12Class::setAlarmLed(unsigned char ID,unsigned char data){

  Checksum = ~(ID + 4 + AX_WRITE_DATA + AX_ALARM_LED + data);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(4);
  write(AX_WRITE_DATA);
  write(AX_ALARM_LED);
  write(data);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode
  return (read_error());
}

int Ax12Class::write(unsigned char ID, unsigned char variable , unsigned int value)
{
  unsigned char value_H,value_L;
  value_H = value >> 8;           // 16 bits - 2 x 8 bits variables
  value_L = value;

  Checksum = ~(ID + 5 + AX_WRITE_DATA + variable + value_L + value_H);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(5);
  write(AX_WRITE_DATA);
  write(variable);
  write(value_L);
  write(value_H);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

  return (read_error());                 // Return the read error
}

int Ax12Class::write(unsigned char ID, unsigned char variable , unsigned char value)
{
  Checksum = ~(ID + 4 + AX_WRITE_DATA + variable + value);

  digitalWrite(Direction_Pin,HIGH);      // Set Tx Mode
  write(AX_START);                 // Send Instructions over Serial
  write(AX_START);
  write(ID);
  write(4);
  write(AX_WRITE_DATA);
  write(variable);
  write(value);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);       // Set Rx Mode

  return (read_error());                 // Return the read error
}


unsigned char Ax12Class::readTemperature(unsigned char ID)
{
  Checksum = ~(ID + AX_TEM_LENGTH  + AX_READ_DATA + AX_PRESENT_TEMPERATURE + AX_BYTE_READ);
  unsigned char Temperature_Byte=0;
  digitalWrite(Direction_Pin,HIGH);
  write(AX_START);
  write(AX_START);
  write(ID);
  write(AX_TEM_LENGTH);
  write(AX_READ_DATA);
  write(AX_PRESENT_TEMPERATURE);
  write(AX_BYTE_READ);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode

  delayMicroseconds(TX_DELAY_TIME);

  for (int i = 0 ; i<8 && available() > 0;i++){
    read();
    //Serial.println(read(),HEX);
  }

  /*while (available() > 0){
    Serial.println(read(),HEX);
    }*/

  //delayMicroseconds(2*TX_DELAY_TIME);
  while (available() > 0){
    Incoming_Byte = read();
    //Serial.print(Incoming_Byte,HEX);Serial.print(",");
    if ( Incoming_Byte == 0xFF && peek() == 0xFF ){
      read();read();read();
      //Serial.print(read(),HEX);Serial.print(",");              // Start Bytes
      //Serial.print(read(),HEX);Serial.print(",");                             // Ax-12 ID
      //Serial.print(read(),HEX);Serial.print(",");                             // Length
      if( peek() != 0 ) {  // Error
	Error_Byte=read();
 	//Serial.print(Error_Byte,HEX);Serial.print(",");
      }
      else{read();
	//Serial.print(read(),HEX);Serial.print(",");
      }
      Temperature_Byte = read();            // Position Bytes
    }
  }
  return (Temperature_Byte);               // Returns the read temperature
}

unsigned int Ax12Class::readPosition(unsigned char ID)
{
  //purge
  while (available() > 0){
    read();
  }
  Checksum = ~(ID + AX_POS_LENGTH  + AX_READ_DATA + AX_PRESENT_POSITION_L + AX_2BYTE_READ);

  digitalWrite(Direction_Pin,HIGH);
  write(AX_START);
  write(AX_START);
  write(ID);
  write(AX_POS_LENGTH);
  write(AX_READ_DATA);
  write(AX_PRESENT_POSITION_L);
  write(AX_2BYTE_READ);
  write(Checksum);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode

  delayMicroseconds(TX_DELAY_TIME);
  for (int i = 0 ; i<8 && available() > 0;i++){
    read();
    //Serial.println(read(),HEX);
  }

  /*while (available() > 0){
    Serial.println(read(),HEX);
    }*/
  unsigned int Position_Long_Byte=0;
  //delayMicroseconds(2*TX_DELAY_TIME);
  while (available() > 0){
    Incoming_Byte = read();
    //Serial.print(Incoming_Byte,HEX);Serial.print(",");
    if ( Incoming_Byte == 0xFF && peek() == 0xFF ){
      read();read();read();
      //Serial.print(read(),HEX);Serial.print(",");              // Start Bytes
      //Serial.print(read(),HEX);Serial.print(",");                             // Ax-12 ID
      //Serial.print(read(),HEX);Serial.print(",");                             // Length
      if( peek() != 0 ) {  // Error
	Error_Byte=read();
 	//Serial.print(Error_Byte,HEX);Serial.print(",");
      }
      else{read();
	//Serial.print(read(),HEX);Serial.print(",");
      }

      unsigned char Position_Low_Byte = read();            // Position Bytes
      unsigned char Position_High_Byte = read();
      //Serial.print(Position_Low_Byte,HEX);Serial.print(",");
      //Serial.print(Position_High_Byte,HEX);Serial.print(",");
      Position_Long_Byte = Position_High_Byte << 8;
      Position_Long_Byte = Position_Long_Byte + Position_Low_Byte;
      //Serial.print(read(),HEX);Serial.println("."); //cheksum
    }
  }
  //Serial.println("fin");
  return (Position_Long_Byte);     // Returns the read position
}

unsigned char Ax12Class::readVoltage(unsigned char ID)
{
  Checksum = ~(ID + AX_VOLT_LENGTH  + AX_READ_DATA + AX_PRESENT_VOLTAGE + AX_BYTE_READ);

  digitalWrite(Direction_Pin,HIGH);
  write(AX_START);
  write(AX_START);
  write(ID);
  write(AX_VOLT_LENGTH);
  write(AX_READ_DATA);
  write(AX_PRESENT_VOLTAGE);
  write(AX_BYTE_READ);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode
  delayMicroseconds(TX_DELAY_TIME);

  for (int i = 0 ; i<8 && available() > 0;i++){
    read();
    //Serial.println(read(),HEX);
  }

  /*while (available() > 0){
    Serial.println(read(),HEX);
    }*/
  unsigned char Voltage_Byte=0;
  //delayMicroseconds(2*TX_DELAY_TIME);
  while (available() > 0){
    Incoming_Byte = read();
    //Serial.print(Incoming_Byte,HEX);Serial.print(",");
    if ( Incoming_Byte == 0xFF && peek() == 0xFF ){
      read();read();read();
      //Serial.print(read(),HEX);Serial.print(",");              // Start Bytes
      //Serial.print(read(),HEX);Serial.print(",");                             // Ax-12 ID
      //Serial.print(read(),HEX);Serial.print(",");                             // Length
      if( peek() != 0 ) {  // Error
	Error_Byte=read();
 	//Serial.print(Error_Byte,HEX);Serial.print(",");
      }
      else{read();
	//Serial.print(read(),HEX);Serial.print(",");
      }
      Voltage_Byte = read();            // Position Bytes
    }
  }
  return (Voltage_Byte);               // Returns the read Voltage
}

unsigned char Ax12Class::readBR(unsigned char ID){
  unsigned char BR=0;
  Checksum = ~(ID + AX_BR_LENGTH  + AX_READ_DATA + AX_BAUD_RATE + AX_BYTE_READ);

  digitalWrite(Direction_Pin,HIGH);
  write(AX_START);
  write(AX_START);
  write(ID);
  write(AX_BR_LENGTH);
  write(AX_READ_DATA);
  write(AX_BAUD_RATE);
  write(AX_BYTE_READ);
  write(Checksum);
  delayMicroseconds(TX_DELAY_TIME);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode
  delayMicroseconds(TX_DELAY_TIME);

  for (int i = 0 ; i<8 && available() > 0;i++){
    read();
  }

  while (available() > 0){
    Incoming_Byte = read();
    if ( Incoming_Byte == 0xFF && peek() == 0xFF ){
      read();read();read();
      if( peek() != 0 ) {  // Error
	Error_Byte=read();
      }
      else{
	read();
      }
      BR = read();
    }
  }
  return (BR);
}

unsigned int Ax12Class::readPresentLoad(unsigned char ID){
	
	unsigned int res = read(ID,AX_PRESENT_LOAD_L,AX_2BYTE_READ);
	res = res && 0x3FF;
	return res;
}

unsigned int Ax12Class::read(unsigned char ID,unsigned char value,unsigned char type){
  //purge
  while (available() > 0){
    read();
  }
  Checksum = ~(ID + 4 + AX_READ_DATA + value + type);

  digitalWrite(Direction_Pin,HIGH);
  write(AX_START);
  write(AX_START);
  write(ID);
  write(4);
  write(AX_READ_DATA);
  write(value);
  write(type);
  write(Checksum);
  digitalWrite(Direction_Pin,LOW);     // Set Rx Mode

  delayMicroseconds(TX_DELAY_TIME);
  //purge 2
  for (int i = 0 ; i<8 && available() > 0;i++){
    read();
  }

  unsigned int byte=0;

  while (available() > 0){
    unsigned char incoming_Byte = read();
    if ( incoming_Byte == 0xFF && peek() == 0xFF ){
      read();read();read();

      if( peek() != 0 ) {  // Error
		Error_Byte=read();
      }
      else{read();}

      unsigned char low_Byte = read();

      if(type==AX_2BYTE_READ){
		unsigned char high_Byte = read();
		byte = high_Byte << 8;
		byte = byte + low_Byte;
      }else{
		byte = low_Byte;
      }
    }
  }
  return (byte);     // Returns the read byte
}
