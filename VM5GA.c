/*
 * This is for reading the rfid tags from the VM5GA RFID reader.
 * VM5GA is a UHF reader.
 * VM5GA will be connected to serial port 2 of Arduino mega.
 * I am going to write pretty bad code , because of time constraint. Will have to come back to clean up.
 */


//DEFINE KEYWORDS HERE
//const byte INFO[] = {0xBB , 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};
//const char INFO_RESPONSE[] =
//const char SUCCESS[] = "success";

byte info[] = {0xBB , 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};
byte china800MHZregion[] = {0xBB,	0x00,	0x07,	0x00,	0x01,	0x04,	0x0C,	0x7E};
byte freq840MHZ[] = {0xBB, 0x00, 0xAB, 0x00, 0x01, 0x00, 0xAC, 0x7E };
byte infoResponse[] = {0xBB, 0x01, 0x03, 0x00, 0x08, 0x02, 0x4D, 0x61, 0x67, 0x69, 0x63, 0x52, 0x66, 0xA7, 0X7E};
//String infoResponseString = "";
String readString = "";
char infoResponse1[] = {0xBB, 0x01, 0x03, 0x00, 0x08, 0x02, 0x4D, 0x61, 0x67, 0x69, 0x63, 0x52, 0x66, 0xA7, 0X7E};
char china800MHZregionResponse1[] = {0xBB,	0x01,	0x07,	0x00,	0x01,	0x00,	0x09,	0x7E};
char freq840MHZResponse1[] = {0xBB,	0x01,	0xAB,	0x00,	0x01,	0x00,	0xAD,	0x7E};

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(8, OUTPUT);   //connected to EN pin
 digitalWrite(8,HIGH);  //make EN pin high
 //Serial.println("Writing info");
 //Serial2.write(info, sizeof(info));

 //Serial2.write(info, sizeof(info));
// for (int i =0;i<sizeof(infoResponse);i++){
//	 infoResponseString += infoResponse1[i];
 //}

 //Serial.println(infoResponseString); //prints string to serial port out
}


// uncompiled, untested
//byte message[] = {0xAA, 0xBB, 0x06, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03 };

//Serial.write(message, sizeof(message));
//BB 00 03 00 01 02 06 7E ---> in hex
//187 000 003 000 001 002 006 126  ---> in decimal
void loop(){
    //check if the module is alive.
	while(!isVM5GAAlive());  //only go ahead if module is alive.

	//clear the VM5GA buffer.
	clearVM5GABuffer();

	//set frequency to 840.125MHz. Not supported in India. (865-867 MHz) need to change !!
    setFrequency();

    //clear the VM5GA buffer.
    clearVM5GABuffer();


	delay(5000);

/*
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
*/
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
	//will try to get product info and check the response .
	//query product info.


	 //log the info
	 Serial.println("Querying product info");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(info, sizeof(info));
     //check for the response.
	 if(checkResponseVM5GA(infoResponse1,sizeof(infoResponse),5000)){
		 Serial.println("Module found !!");
		 return 1;
	 }else{
		 Serial.println("Module not found");
		 return 0;
	 }

}


/*FUNCTION NAME:
  * checkResponseVM5GA
  *
  * DESCRIPTION:
  * This checks for the given response from VM5GA.
  *
  * PARAMETERS:
  * 1.response[]  - the response to search
  * 2.responseSize  - the size of response ,sizeof(response)
  * 3.timeout     - the maximum time to lookfor
  *
  * RETURN VALUES:
  * 1 - if pattern found
  * 0 - if pattern not found or timeout
  */
boolean checkResponseVM5GA(char response[],int responseSize,int timeout){
   unsigned long timeoutStart = millis(); //for the timeout
   char data[30];       //this is the buffer - if response is longer than 20, then increase this
   responseSize--;      //since we're going to get an extra character from the sizeof()


   //fill the data buffer from Rx
   for(byte i=0; i<responseSize; i++){
     while(!Serial2.available()){//wait until a new byte is sent down from the VM5GA.
      if((millis()-timeoutStart)>timeout){//if nothing happens within the timeout period, get out of here
        Serial.println("timeout1");     //timeout !! log the info
              return 0; //get out of here
              }
      }

      data[i]=Serial2.read();// save the byte to the buffer data
      readString += data[i];
     }

// really don't know why this is showing not a match.
//   Serial.println("Printing data");
//   Serial.println(readString);
//  readString = "";
//   if(readString == infoResponseString){
//	   Serial.println("Correct module info!!");
//   }else{
//	   Serial.println("Wrong module info!!");
//   }



   //the buffer if filled so start checking the response and refreshing the buffer
   //stay in here until the keyword found or a timeout occurs
    while(1){
        //run through the entire buffer and look for the keyword
        //this check is here, just in case the first thing out of the VM5GA was the response, meaning the buffer was actually filled with the response
        for(byte i=0; i<responseSize; i++){
          if(response[i]!=data[i]){//if it doesn't match, break out of the search now
            break;
          }
          if(i==(responseSize-1)){//we got all the way through the buffer without breaking, must be a match!
          return 1; //return a 1 and get outta here!
          }
        }

   //start rolling/refreshing the buffer
     for(byte i=0; i<(responseSize-1); i++){
        data[i]=data[i+1];// so the data at 0 becomes the data at 1, and so on.... the last value is where we'll put the new data.
     }

    //timing control
      while(!Serial2.available()){
        if((millis()-timeoutStart)>timeout){
          Serial.println("timeout2");  //timeout !! log the info
              return 0;
            }
       }
      data[responseSize-1]=Serial2.read();//save the new data in the last position in the buffer.
    }
}



/*FUNCTION NAME:
  * clearVM5GABuffer
  *
  * DESCRIPTION:
  * This is used to clear the rx buffer , which is filled by response from VM5GA.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */

void clearVM5GABuffer(){
  char temp;
  while(Serial2.available()){
    temp = Serial2.read();
    delay(1);
   }
}




/*FUNCTION NAME:
  * setFrequency
  *
  * DESCRIPTION:
  * This is used to set the frequency of operation to CHINA 800MHz region.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */

void setFrequency(){
  // first set region to CHINA 800MHZ.
  //	BB	00	07	00	01	(04) C	7E .. 04 for china 800Mhz.

  //log the info
	Serial.println("Setting region to CHINA 800MHZ");
  // clear VM5GA buffer first.
	clearVM5GABuffer();
	Serial2.write(china800MHZregion, sizeof(china800MHZregion));
  //check for the response.
    if(checkResponseVM5GA(china800MHZregionResponse1,sizeof(china800MHZregionResponse1),5000)){
		Serial.println("Region set to CHINA 800MHZ");   //log the info
	}else{
		Serial.println("Unable to set region to CHINA 800MHZ");  // log the info
	}

    //set the frequency to 840.125MHz
    //log the info
    Serial.println("Setting frequency to 840.125MHz");
    // clear VM5GA buffer first.
    clearVM5GABuffer();
    Serial2.write(freq840MHZ, sizeof(freq840MHZResponse1));
    //check for the response.
    if(checkResponseVM5GA(freq840MHZResponse1,sizeof(freq840MHZResponse1),5000)){
   		Serial.println("Frequency set to 840.125MHz");   //log the info
   	}else{
   		Serial.println("Unable to set frequency to 840.125MHz");  // log the info
   	}


}
