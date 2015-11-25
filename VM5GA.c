/*
 * This is for reading the rfid tags from the VM5GA RFID reader.
 * VM5GA is a UHF reader.
 * VM5GA will be connected to serial port 2 of Arduino mega.
 * I am going to write pretty bad code , because of time constraint. Will have to come back to clean up.
 */


byte info[] = {0xBB , 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};
byte infoResponse[] = {0xBB, 0x01, 0x03, 0x00, 0x08, 0x02, 0x4D, 0x61, 0x67, 0x69, 0x63, 0x52, 0x66, 0xA7, 0X7E};
String readString = "";

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(8, OUTPUT);   //connected to EN pin
 digitalWrite(8,HIGH);  //make EN pin high
 //Serial.println("Writing info");
 Serial2.write(info, sizeof(info));

 //Serial2.write(info, sizeof(info));

}


// uncompiled, untested
//byte message[] = {0xAA, 0xBB, 0x06, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03 };

//Serial.write(message, sizeof(message));
//BB 00 03 00 01 02 06 7E ---> in hex
//187 000 003 000 001 002 006 126  ---> in decimal
void loop(){
    //check if the module is alive.
	while(!isVM5GAAlive());
	 while (Serial2.available()) {
	   delay(10);  //small delay to allow input buffer to fill

	   char c = Serial2.read();  //gets one byte from serial buffer

	   readString += c;
	 } //makes the string readString

	 if (readString.length() >0) {
	   Serial.println(readString); //prints string to serial port out

	   readString=""; //clears variable for new input
	 }
}



/*FUNCTION NAME:
  * isVM5GAAlive
  *
  * DESCRIPTION:
  * This checks whether VM5GA is alive.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * 1  - if alive
  * 0  - if not alive
  */

int isVM5GAAlive(){


}
