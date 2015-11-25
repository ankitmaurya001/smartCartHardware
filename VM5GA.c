/*
 * This is for reading the rfid tags from the VM5GA RFID reader.
 * VM5GA is a UHF reader.
 * VM5GA will be connected to serial port 2 of Arduino mega.
 */


byte info[] = {0xBB , 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};


void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(8, OUTPUT);   //connected to EN pin
 digitalWrite(8,HIGH);  //make EN pin high
 Serial.println("Writing info");
 Serial.write(info, sizeof(info));
 Serial2.write(info, sizeof(info));

}


// uncompiled, untested
//byte message[] = {0xAA, 0xBB, 0x06, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03 };

//Serial.write(message, sizeof(message));
//BB 00 03 00 01 02 06 7E ---> in hex
//187 000 003 000 001 002 006 126  ---> in decimal
void loop(){
	String stringOne;
	//get the module info.
//if(Serial.available()){
//	stringOne = String(Serial.read(),HEX);
//	Serial.println(stringOne);
//}
//	if(Serial.available()){
//		Serial.println(Serial.read());
//		stringOne = String(Serial2.read(),HEX);
//			Serial.println(stringOne);
//	}
}
