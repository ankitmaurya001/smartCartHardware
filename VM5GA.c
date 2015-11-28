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
byte signalStrength20dm[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E };
byte signalStrength26dbm[] ={0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E };
byte singlePollingCmd[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
byte multiplePollingCmd[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x28, 0x74, 0x7E }; // poll 40 times
byte stopMultiplePollingCmd[] = {0xBB, 0x00, 0x28, 0x00, 0x00, 0x28, 0x7E};
//String infoResponseString = "";
String readString = "";
char infoResponse1[] = {0xBB, 0x01, 0x03, 0x00, 0x08, 0x02, 0x4D, 0x61, 0x67, 0x69, 0x63, 0x52, 0x66, 0xA7, 0X7E};
char china800MHZregionResponse1[] = {0xBB,	0x01,	0x07,	0x00,	0x01,	0x00,	0x09,	0x7E};
char freq840MHZResponse1[] = {0xBB,	0x01,	0xAB,	0x00,	0x01,	0x00,	0xAD,	0x7E};
char signalStrength20dmResponse1[] = {0xBB,	0x01,	0xB6,	0x00,	0x01,	0x00,	0xB8,	0x7E};
char tagID1Response[] ={0xBB, 0x02, 0x22, 0x00, 0x11, 0xC3, 0x30, 0x00,
		0x34, 0x08, 0x33, 0xB2, 0xDD, 0xD9, 0x01, 0x40,
		0x00, 0x00, 0x00, 0x00, 0x34, 0x6F, 0xE3, 0x7E };
char stopMultiplePollingCmdResponse1[]={0xBB, 0x01,	0x28, 0x00,	0x01, 0x00,	0x2A, 0x7E};

//vahiyaat jugaad
int tag1Found =0;
int tag2Found =0;
int tag3Found =0;
int tag4Found =0;


//gloabal variable for the current tag.
int tagNo;


void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(8, OUTPUT);   //connected to EN pin
  digitalWrite(8,HIGH);  //make EN pin high
  setupVM5GA();          //setup VM5GA

}


// uncompiled, untested
//byte message[] = {0xAA, 0xBB, 0x06, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03 };

//Serial.write(message, sizeof(message));
//BB 00 03 00 01 02 06 7E ---> in hex
//187 000 003 000 001 002 006 126  ---> in decimal
void loop(){
  /*  //check if the module is alive.
	while(!isVM5GAAlive());  //only go ahead if module is alive.

	//clear the VM5GA buffer.
	clearVM5GABuffer();
	//rest
	delay(10);

	//set frequency to 840.125MHz. Not supported in India. (865-867 MHz) need to change !!
    setFrequencyVM5GA();

    //clear the VM5GA buffer.
    clearVM5GABuffer();
    //rest
    delay(10);

    //set signal strength to 26dbm.
    setSignalStrengthVM5GA();

    //clear the VM5GA buffer.
     clearVM5GABuffer();
    //rest
     delay(10);

     //do single polling and see what happens
     singlePolling();
     //clear the VM5GA buffer.
     clearVM5GABuffer();
     //rest
     delay(10);
*/
	//going will multiple polling , because it looked better to me . Well it's 5:23 am so can't really say :P
    //try multiple polling and see what happens
	//keep polling untill you find a tag!!
     while(!(tagNo =multiplePolling()));
     //clear the VM5GA buffer.
     clearVM5GABuffer();
     //rest
     delay(10);


	//delay(1000);

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
  * setFrequencyVM5GA
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

void setFrequencyVM5GA(){
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



/*FUNCTION NAME:
  * setSignalStrenthVM5GA
  *
  * DESCRIPTION:
  * This sets signal strength of VM5GA to 26dbm.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void setSignalStrengthVM5GA(){

	 //log the info
	 Serial.println("Setting signal strength to 26dbm");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(signalStrength26dbm, sizeof(signalStrength26dbm));
     //check for the response.
	 if(checkResponseVM5GA(signalStrength20dmResponse1,sizeof(signalStrength20dmResponse1),5000)){
		 Serial.println("Signal strength set to 26dbm");
	 }else{
		 Serial.println("Unable to set signal strength to 26dbm");
	 }

}


/*FUNCTION NAME:
  * stopMultiplePollingVM5GA
  *
  * DESCRIPTION:
  * This function stops multiple polling of VM5GA.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void stopMultiplePollingVM5GA(){
	 //log the info
	 Serial.println("Stopping multiple polling");
	 // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(stopMultiplePollingCmd, sizeof(stopMultiplePollingCmd));
	 //check for the response.
	 if(checkResponseVM5GA(stopMultiplePollingCmdResponse1,sizeof(stopMultiplePollingCmdResponse1),5000)){
		Serial.println("Multiple polling stopped");
	 }else{
		Serial.println("Unable to stop multiple polling");
	}

}





/*FUNCTION NAME:
  * singlePolling
  *
  * DESCRIPTION:
  * This function polls the tag one single time.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void singlePolling(){

	 //log the info
	 Serial.println("Polling one single time");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(singlePollingCmd, sizeof(singlePollingCmd));
	 delay(100);
	 //print the response.
	 //printResponseofVM5GA();
     //check for the response.
	// if(checkResponseVM5GA(tagID1Response,sizeof(tagID1Response),10000)){
	//	 Serial.println("Tag ID with 34 found");
	// }else{
	//	 Serial.println("Tag ID with 34 not found");
	// }
	 int tag =0;
	 tag = readTagVM5GA();
	 switch (tag) {
	     case 1:
	        Serial.println("Tag 0x31 found ");
	       break;
	     case 2:
	    	 Serial.println("Tag 0x32 found ");
	       break;
	     case 3:
	    	 Serial.println("Tag 0x33 found ");
	    	 break;
	     case 4:
	    	 Serial.println("Tag 0x34 found ");
	    	 break;
	     default:
	       // Serial.println("No tag found");
	     break;
	   }
}


/*FUNCTION NAME:
  * multiplePolling
  *
  * DESCRIPTION:
  * This function polls the tag multiple times. (40 here)
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * tagNo - When a new tag is found.
  * 0 - if no tag found.
  */

int multiplePolling(){

	 //log the info
	 Serial.println("Multiple polling 40 times");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(multiplePollingCmd, sizeof(multiplePollingCmd));
	 delay(20);
	 //print the response.
	 //printResponseofVM5GA();
     //check for the response.
	// if(checkResponseVM5GA(tagID1Response,sizeof(tagID1Response),10000)){
	//	 Serial.println("Tag ID with 34 found");
	// }else{
	//	 Serial.println("Tag ID with 34 not found");
	// }
	 //read 40 times
	 for(int i=0;i<40;i++){
	 int tag =0;
	 tag = readTagVM5GA();
	 switch (tag) {
	     case 1:
	    	if(tag1Found ==0){  //come only once
	        Serial.println("Tag 0x31 found ");
	        tag1Found = 1;
	        stopMultiplePollingVM5GA();
	        return 1;
	    	}
	       break;
	     case 2:
	    	 if(tag2Found ==0){ //come only once
	    	 Serial.println("Tag 0x32 found ");
	    	 tag2Found = 1;
	    	 stopMultiplePollingVM5GA();
	    	 return 2;
	    	 }
	       break;
	     case 3:
	    	 if(tag3Found ==0){ //come only once
	    	 Serial.println("Tag 0x33 found ");
	    	 tag3Found = 1;
	    	 stopMultiplePollingVM5GA();
	    	 return 3;
	    	 }
	    	 break;
	     case 4:
	    	 if(tag4Found ==0){ //come only once
	    	 Serial.println("Tag 0x34 found ");
	    	 tag4Found =1;
	    	 stopMultiplePollingVM5GA();
	    	 return 4;
	    	 }
	    	 break;
	     default:
	       // Serial.println("No tag found");
	     break;
	   }
	 }
	 return 0; //nothing found
}

/*FUNCTION NAME:
  * printResponseOfVM5GA
  *
  * DESCRIPTION:
  * This function prints the response of VM5GA in readable format.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */


void printResponseofVM5GA(){
	 while (Serial2.available()) {
		  delay(10);  //small delay to allow input buffer to fill
		  char start[] = {0XBB};
		   char c = Serial2.read();  //gets one byte from serial buffer

		   readString += c;
		 } //makes the string readString

		 if (readString.length() >0) {
		   Serial.println(readString); //prints string to serial port out
		   readString=""; //clears variable for new input
		 }
}





/*FUNCTION NAME:
  * readTagValues
  *
  * DESCRIPTION:
  * This function reads tag value with some vahiyaat jugaad. Need to clean this up!!
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * tag no.
  * 1 --- 0x31
  * 2 --- 0x32
  * 3 --- 0x33
  * 4 --- 0x34
  * 0 --- no valid tag.
  *
  */


int readTagVM5GA(){
	int i =0;
	int tagFound =0;
	while (Serial2.available()) {
		delay(5);  //small delay to allow input buffer to fill
		char badStart[] = {0XBB, 0X01 ,0XFF};
		char goodStart[] = {0X00}; //PC(LSB) should be zero
		char tags[] = {0x31, 0x32, 0x33, 0x34};
		char c = Serial2.read();  //gets one byte from serial buffer
        if(i==2 && c == badStart[2]){   //there should be a fucking better way to compare , no time hope this shit works.!!
            break;  //wrong response code from VM5GA.
        }
        if(i==7 && c == goodStart[0]){
        	char c = Serial2.read();
        	for(int j=0;j<4;j++){
        		if(c == tags[j]){
        		//tag found , save the id and break
        			tagFound = j+1;
        			break;
        		}
        	 }
        	break;  // breaking here as we have found some tag which is not in the inventory .
        }
		i++;

	}

	if(i==2 || i==7){
	   // read till end of this response i.e till 0x7E
	   while(Serial2.available()){
	    	delay(5); //small delay to allow input buffer to fill
	    	char end[] = {0X7E};
	    	char c = Serial2.read();
	    	if(c == end[0]){
	    		break;
	    	}
	    }
	}
	return tagFound;

}




/*FUNCTION NAME:
  * setupVM5GA
  *
  * DESCRIPTION:
  * This function initializes the VM5GA module.
  * 1)It checks if module is alive.
  * 2)Sets frequency region to CHINA 800MHz.
  * 3)Sets frequency to 840.125MHz.
  * 4)Sets signal strength to 26bdm.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void setupVM5GA(){

	 while(!isVM5GAAlive());  //only go ahead if module is alive.
	 //clear the VM5GA buffer.
	 clearVM5GABuffer();
	 //rest
	 delay(10);

	 //set frequency to 840.125MHz. Not supported in India. (865-867 MHz) need to change !!
	 setFrequencyVM5GA();
     //clear the VM5GA buffer.
	 clearVM5GABuffer();
	 //rest
	 delay(10);

	 //set signal strength to 26dbm.
	 setSignalStrengthVM5GA();
     //clear the VM5GA buffer.
	 clearVM5GABuffer();
	 //rest
	 delay(10);

}

