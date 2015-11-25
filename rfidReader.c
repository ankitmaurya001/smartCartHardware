/*
 * This is for reading the 125Khz RFID tag.
 * I am doing a vahiyaaat juggad currently as I don't have time . Will get back later to this.
 * The tags read in dec are
 * 2-56-54-48-48-54-56-57-50-56-48-70-67-3 (14 DEC)
 * The first byte is 2 so I will check for 2 as starting and read 13 next values , that will
 * be my tag value, not checking anything else.
 *
 * The Serial2 of mega is connected to the RFID.
 *
 */



int tag[14];
int bytesRead = 0;
int productNo = 0;

int tagID0[14] = {2,48,55,48,48,57,50,65,53,55,48,52,48,3};
int tagID1[14] = {2,48,55,48,48,57,50,52,57,68,53,48,57,3};
int tagID2[14] = {2,48,55,48,48,48,69,50,49,51,56,49,48,3};


//wrapping in namespace as the stupid arduino IDE doesn't understand struct
namespace productInfo{
	struct {
		int tagID[14];
		String productName;
		String productPrice;
		String phoneNumber;
		}productNo[3];
}


int readTag();
int getProduct(int tagIDTemp[]);






void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(8, OUTPUT);   //connected to buzzer
  digitalWrite(8,LOW);  //switch off the buzzer
  //initialize the structure

  //product 0
  productInfo::productNo[0].productName 	= "Levi's Jeans";
  productInfo::productNo[0].productPrice 	= "2500";
  productInfo::productNo[0].phoneNumber 	= "7829834215";
  for(int i=0;i<14;i++){
	  productInfo::productNo[0].tagID[i] = tagID0[i];
    }

  //product 1
  productInfo::productNo[1].productName 	= "Wrogn Tshirt";
  productInfo::productNo[1].productPrice 	= "1999";
  productInfo::productNo[1].phoneNumber 	= "7829834215";
  for(int i=0;i<14;i++){
  	  productInfo::productNo[1].tagID[i] = tagID1[i];
      }

  //product 2
   productInfo::productNo[2].productName 	= "Adidas Tshirt";
   productInfo::productNo[2].productPrice 	= "1499";
   productInfo::productNo[2].phoneNumber 	= "7829834215";
   for(int i=0;i<14;i++){
   	  productInfo::productNo[2].tagID[i] = tagID2[i];
       }

   for(int i=0;i<3;i++){
	   Serial.print("TAG:");
	   Serial.println(i);
	   for(int j=0;j<14;j++){
		   Serial.print(productInfo::productNo[i].tagID[j]);
		   Serial.print("-");
	   }
	   Serial.println("");
   }
}

void loop() {
	while((productNo = readTag())==0);     //first look for the tag, and proceed only if tag is read and in database.
//	productNo = getProduct(tag);
//	if(productNo!=-1){
//	Serial.println(productNo);
//	}
//	productNo = readTag();
//	if(productNo!=0){
	Serial.println("");
//	Serial.print(productNo-1);
//	}
	Serial.println("Product found:");
	Serial.print(productInfo::productNo[productNo-1].productName);



}


/*FUNCTION NAME:
  * readTag
  *
  * DESCRIPTION:
  * This reads a RFID tag one at a time.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * tagNo+1 - if able to read and tag is found.
  * 0       - if not able to read, or able to read but tag not found in the database.
  */
int readTag(){
	digitalWrite(8,LOW);  //switch off the buzzer
		//check for incoming values
		if(Serial2.available()){
			tag[bytesRead] = Serial2.read(); //read the first  byte
			if (tag[bytesRead]==2){ //see if its the starting of the tag
				bytesRead++;
				//read the complete tag
				while(bytesRead<=14){
					if(Serial2.available()){
						tag[bytesRead] = Serial2.read();
						bytesRead++;
					}
					if(bytesRead==14){  //as when bytesRead =14 then Serial.available() will be false ,hence bytesRead
										// wont increment.
						bytesRead++;
					}
				}
			}
		}

		//print the tag
		if(bytesRead==15){
			   Serial.println("");
			   Serial.print("Tag ID:");
			   Serial.print(tag[0],DEC);
			   Serial.print("-");
			   Serial.print(tag[1],DEC);
			   Serial.print("-");
			   Serial.print(tag[2],DEC);
			   Serial.print("-");
			   Serial.print(tag[3],DEC);
			   Serial.print("-");
			   Serial.print(tag[4],DEC);
			   Serial.print("-");
			   Serial.print(tag[5],DEC);
			   Serial.print("-");
			   Serial.print(tag[6],DEC);
			   Serial.print("-");
			   Serial.print(tag[7],DEC);
			   Serial.print("-");
			   Serial.print(tag[8],DEC);
			   Serial.print("-");
			   Serial.print(tag[9],DEC);
			   Serial.print("-");
			   Serial.print(tag[10],DEC);
			   Serial.print("-");
			   Serial.print(tag[11],DEC);
			   Serial.print("-");
			   Serial.print(tag[12],DEC);
			   Serial.print("-");
			   Serial.print(tag[13],DEC);
			   Serial.print("-");
			   bytesRead = 0;
			   digitalWrite(8,HIGH);
			   delay(100);
		//	   return 1;
			   return (getProduct(tag));   //you read the tag and search for the tag in the database.
		}else{
			return 0;  //read something else
		}
}




/*FUNCTION NAME:
  * getProduct
  *
  * DESCRIPTION:
  * This checks the RIFD tag with the stored tags.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * tagNo - if product found in database
  * -1	  - if product not found in database
  */

int getProduct(int tagIDTemp[]){
	int matched =1;
	for(int i=0;i<3;i++){
		matched =1;
		for(int j=0;j<14;j++){
			if(tagIDTemp[j] !=productInfo::productNo[i].tagID[j]){
				matched =0;
				break;
			}
		}
		if(matched==1){ //we got out of the loop without breaking, a match!!
			return (i+1);
		}
	}if(matched==0){
		return 0; //product not found;
	}

}
