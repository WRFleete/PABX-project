 
 
#define PSTN_Ring 2
#define PSTN_offhook 3
#define RingGen_EN 4
#define HS_sel_B0 5
#define HS_sel_B1 6
#define HS_sel_B2 7
#define DTMF_B0 8
#define DTMF_B1 9
#define DTMF_B2 10
#define DTMF_B3 11
#define hold_Relay 13
#define Status_LED 12
#define I_loop_Vsense A0
#define DTMF_0 10
#define DTMF_1 1
#define DTMF_2 2
#define DTMF_3 3
#define DTMF_4 4
#define DTMF_5 5
#define DTMF_6 6
#define DTMF_7 7
#define DTMF_8 8
#define DTMF_9 9
#define DTMF_Star 11
#define DTMF_Hash 12
#define DTMF_A 13
#define DTMF_B 14
#define DTMF_C 15
#define timeout 50
#define onHookTimeout 250
int I_loopVal;
byte HS_selval;
byte DTMF_val;
int on_hook_val;
int HS_off_hook ;
bool inHold = 0;
byte countHookPulse = 0;
bool pulseNumReady = 0;
int pulTimeout = 0;
int onHookTimer = 0;
bool messageSent = 0;
void setup() {
  // put your setup code here, to run once:

pinMode (PSTN_Ring,INPUT);
pinMode (PSTN_offhook,OUTPUT);
pinMode (RingGen_EN,OUTPUT);
pinMode (HS_sel_B0,OUTPUT);
pinMode (HS_sel_B1,OUTPUT);
pinMode (HS_sel_B2,OUTPUT);
pinMode(DTMF_B0,INPUT);
pinMode(DTMF_B1,INPUT);
pinMode(DTMF_B2,INPUT);
pinMode(DTMF_B3,INPUT);
pinMode(Status_LED,OUTPUT);
pinMode(I_loop_Vsense,INPUT);
Serial.begin(9600);
on_hook_val = analogRead(I_loop_Vsense);
}

void loop() {
 
 I_loopVal = analogRead(I_loop_Vsense);
  if(!digitalRead(PSTN_Ring)){  //if the external phone line is ringing
    while(!digitalRead(PSTN_Ring)){
  digitalWrite(RingGen_EN,HIGH);
//for(byte i = 0; i < 8; i++){
  setHS(0); //ring HS 0 on the intercom system
  digitalWrite(Status_LED,HIGH);}
  delay(100);
 // digitalWrite(Status_LED,LOW);
    digitalWrite(RingGen_EN,LOW);
    delay(60);
 int test_off_hook = analogRead(I_loop_Vsense);
 delay(60);
 while(test_off_hook < (on_hook_val -120) || inHold){ //wait for a phone on the intercom to go off hook
  digitalWrite(Status_LED,HIGH);
  digitalWrite(RingGen_EN,LOW); //turn off the ring generator
  digitalWrite( PSTN_offhook,HIGH); //take the POTS/PSTN line off hook
  //Serial.println(test_off_hook);
  delay(60);
  if(readDTMF() == DTMF_Star){delay(60);if(inHold){inHold = 0;}else{inHold = 1;}} 
  //decide if the "hold" relay should connect the intercom loop to the isolation transformer for the main phone line
  //the DTMF star key should toggle the "hold" function. NB you should be able to inject audio into the phone while in hold
  //hold nor dialing onto the main phone line will not work with pulse phones, works with DTMF only 
  
          if(!inHold){digitalWrite( hold_Relay,HIGH); delay(60);
                 test_off_hook = analogRead(I_loop_Vsense);}else{checkTransferCall (); digitalWrite( hold_Relay,LOW);}
  

  if(test_off_hook >= (on_hook_val - 120)){ //when the intercom phones hang up
    test_off_hook = on_hook_val;
  digitalWrite(PSTN_offhook,LOW); //hang up the main phone line
  digitalWrite( hold_Relay,LOW); //disconnect the transformer from the intercom loop
  digitalWrite(Status_LED,LOW); 
  }
  
  }
  
  //}
 digitalWrite(RingGen_EN,LOW);  
 }

 
   
 if (I_loopVal < (on_hook_val - 30)){ //if main phone line wasnt ringing check if a handset is off hook


if(onHookTimer >= onHookTimeout){ //stuff for resetting pulse count for pulse dialing
    pulTimeout = 50;
    countHookPulse = 0;
    onHookTimer = 0;
    Serial.println("Resetting pulse count");
    messageSent = false;
    }
 
  /**if(!digitalRead(PSTN_Ring)){
  //digitalWrite(RingGen_EN,HIGH);
//for(byte i = 0; i < 8; i++){
  digitalWrite(PSTN_offhook, HIGH);
        digitalWrite(Status_LED,HIGH);
        digitalWrite( hold_Relay,HIGH);
        int off_hook = analogRead(I_loop_Vsense);
        int test = analogRead(I_loop_Vsense);
        delay (250);
        while(off_hook >= (test-7) || inHold){

          if(readDTMF() == DTMF_Star){delay(60);if(inHold){inHold = 0;}else{inHold = 1;}}
          if(!inHold){digitalWrite( hold_Relay,HIGH);delay(60);
                 test = analogRead(I_loop_Vsense);}else{digitalWrite( hold_Relay,LOW);}
              
          //Serial.println(test);
          delay(60);
          }     
        digitalWrite(PSTN_offhook, LOW);
        digitalWrite( hold_Relay,LOW);
        //digitalWrite(Status_LED,LOW);
        
  
  //}
 digitalWrite(RingGen_EN,LOW);  
 }**/

  
  I_loopVal = analogRead(I_loop_Vsense);
  //Serial.println(I_loopVal);
  HS_off_hook = analogRead(I_loop_Vsense);
  digitalWrite(Status_LED,HIGH);
 delay (20);
 pulTimeout ++;
 
 DTMF_val = readDTMF();
 if(pulTimeout >= timeout){
  //pulseNumReady = true;
  if(countHookPulse >0 ){
  DTMF_val = 10 - countHookPulse; //process the pulse count as a "DTMF value"
  if(DTMF_val == 0){DTMF_val = 10;} //zero dialed on pulse dial (10 pulses)
  countHookPulse = 0;
 
}
  countHookPulse = 0;
  pulTimeout = 0;
  messageSent = false;
  }

    if(DTMF_val > 0){
      Serial.println(DTMF_val);
      //digitalWrite(Status_LED,HIGH);
      //delay(30);
      digitalWrite(Status_LED,LOW);
      delay (30);

         if(DTMF_val == 9){ //dialing 9 enters a test mode to cycle the ringers on all phones on the intercom loop
          delay(60);
          testMode();
          
          }

        
      if(DTMF_val <= 8){ //dialing 1- 8 dials one of the 8 HS on the loop
      setHS(DTMF_val - 1); // set the '138 to the HS to ring
     
      int off_hook = analogRead(I_loop_Vsense);
        delay (60);
        while(off_hook >= (HS_off_hook - 40)){ //wait for a handset to pick up...cont...
         // int off_hook = analogRead(I_loop_Vsense);
          
          //Serial.println(test);
          //and pulse the ringer (can adjust for different ring pattern)
          digitalWrite(Status_LED,HIGH);
          digitalWrite(RingGen_EN,HIGH);
          delay(500);
          //digitalWrite(Status_LED,LOW);
          digitalWrite(RingGen_EN,LOW); 
          delay(250);
          digitalWrite(Status_LED,HIGH);
          digitalWrite(RingGen_EN,HIGH);
           delay(500);
           digitalWrite(RingGen_EN,LOW); 
         
          delay(1200);

          
          int test = analogRead(I_loop_Vsense);
          delay (60);
                   if(test >= (on_hook_val - 20)){//...cont... or the caller hangs up
            off_hook = 0;
            }
          while(test < (off_hook - 20)){ //while in the call just wait for the handsets to hang up
            delay(60);
          test = analogRead(I_loop_Vsense);
          delay (60);
          if(off_hook >= (test - 20)){
            off_hook = 0;
            }
            }
          
        }

        
      }else{
     
        
        
        if(DTMF_val == DTMF_0){ //dialing 0 will take the main phoneline off hook
        digitalWrite(PSTN_offhook, HIGH);
        digitalWrite(Status_LED,HIGH);
        digitalWrite( hold_Relay,HIGH); //route the main line to the intercom loop (via the isolation transformer)
        delay(60);
        int off_hook = analogRead(I_loop_Vsense);
        int test = analogRead(I_loop_Vsense);
        delay (250);
        while(off_hook >= (test-70) || inHold){ //if a handset is off hook or in hold keep the main line off hook
//decide if the "hold" relay should connect the intercom loop to the isolation transformer for the main phone line
  //the DTMF star key should toggle the "hold" function. NB you should be able to inject audio into the phone while in hold
  //hold nor dialing onto the main phone line will not work with pulse phones, works with DTMF only 
          if(readDTMF() == DTMF_Star){delay(60);if(inHold){inHold = 0;}else{inHold = 1;}}
          if(!inHold){digitalWrite( hold_Relay,HIGH);delay(60);
                 test = analogRead(I_loop_Vsense);}else{
                      checkTransferCall ();
                  digitalWrite( hold_Relay,LOW);}
              
          //Serial.println(test);
          delay(60);
          }     
        digitalWrite(PSTN_offhook, LOW); //when the intercom line phones hang up. hang up the main phone line (unless it was in hold)
        digitalWrite( hold_Relay,LOW);
        //digitalWrite(Status_LED,LOW);
        }
        
        
        }
          
          
        

        
  }


}else{

  //
    //pulTimeout = 0;
    
     onHookTimer ++;
     if(onHookTimer >= onHookTimeout){
    //pulTimeout = 50;
    onHookTimer = 256;
    countHookPulse = 0;
    if(!messageSent){
      Serial.println("On hook"); //verbose diag info 
      messageSent = true;
      }
    //onHookTimer = 0;
    }
  digitalWrite(Status_LED,LOW);
  if(!digitalRead(Status_LED) && pulTimeout > 0){
    pulTimeout = 0;
    onHookTimer = 0;
    delay(60);
  //pulseNumReady = false;
    countHookPulse ++;
    Serial.println("pulse count:");
    Serial.println(countHookPulse);
    if(countHookPulse >10){countHookPulse = 0;}}
  }

//digitalWrite(RingGen_EN,HIGH);
//delay(1000);
 //digitalWrite(RingGen_EN,LOW);

/**for(HS_selval = 0; HS_selval < 8;HS_selval ++){
  setHS(HS_selval);
   Serial.println(HS_selval);
 

delay(60);
}**/


  
}

void setHS(byte HSnum){ //sets the 74 series 138 output
  if(HSnum <=7){
  digitalWrite (HS_sel_B0,bitRead(HSnum,0));
digitalWrite (HS_sel_B1,bitRead(HSnum,1));
digitalWrite (HS_sel_B2,bitRead(HSnum,2));
  }
  
  }

  byte readDTMF(){ //returns the DTMF value off the decoder chip
    byte DTMF_code;
   if(digitalRead(DTMF_B0)){bitSet(DTMF_code,0);}else{bitClear(DTMF_code,0);}
 if(digitalRead(DTMF_B1)){bitSet(DTMF_code,1);}else{bitClear(DTMF_code,1);}
 if(digitalRead(DTMF_B2)){bitSet(DTMF_code,2);}else{bitClear(DTMF_code,2);}
 if(digitalRead(DTMF_B3)){bitSet(DTMF_code,3);}else{bitClear(DTMF_code,3);}
 return DTMF_code;
 }
 
 void testMode(){ //ringer test for the intercom handsets
  Serial.println("ringer test");
  Serial.println("waiting for HS to hang up");
  while(analogRead(I_loop_Vsense) < on_hook_val - 10){
    
    delay(250);
    digitalWrite(Status_LED,LOW);
    
    }
  while(analogRead(I_loop_Vsense) >= on_hook_val - 10){ //while all HS are on hook
    for(byte i = 0; i < 8; i++){ //cycle through HS 1 - 8
      
      setHS(i);
      Serial.print("ringing HS ");
      Serial.println(i + 1);
      // for(byte j = 0; j < i+1; j++){
      digitalWrite(RingGen_EN,HIGH); //give the ringer circuit a kick
          delay(500);
          //digitalWrite(Status_LED,LOW);
          digitalWrite(RingGen_EN,LOW); 
          delay(1000);//}
          
      
      }
    Serial.println("pick up HS now to abort test"); //cycle until a HS is picked up
    delay(1000);
    }
  
  }

  void checkTransferCall () { 
   //when in hold using the main phone line (either from dialing out or if it was ringing)
   //you can dial another HS on the loop to "transfer" a call
     
    
    if(readDTMF() > 0){
                        byte DTMF_val = readDTMF();
                      if(DTMF_val <= 8){
      setHS(DTMF_val - 1);
     
      
        delay (60);
        
         // int off_hook = analogRead(I_loop_Vsense);
          
          //Serial.println(test);
          
          //digitalWrite(Status_LED,LOW);
          digitalWrite(RingGen_EN,HIGH);
          delay(500);
          //digitalWrite(Status_LED,LOW);
          digitalWrite(RingGen_EN,LOW);
          delay(1200);
          
          
          
        

        
      }}
                  
                  //digitalWrite(Status_LED,HIGH);
                  
                  }


 
