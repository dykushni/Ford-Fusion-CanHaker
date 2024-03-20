#include <SPI.h>
#include <mcp2515.h>
#include <EEPROM.h> 


struct can_frame canMsg4C0;
struct can_frame canMsg4C01;
struct can_frame canMsg4C8;
struct can_frame canMsgRead;
          
#define KEY 7
byte value;  


struct canRead {
  long rpm ;
  long speed ;
  int temp;
  uint8_t brake;
  int volt;
};

struct Race {

  bool preStart = false;
  bool redyToStart = false;
  bool raceStart = false;
  bool raceStartTimer = false;


  /*bool start = false;
  bool gotov = false;
  bool view = false;*/
  unsigned long raceTimeRady;
  unsigned long raceTimeStart;
  unsigned long raceTimeCurrent;
  unsigned long raceTime60;
  unsigned long raceTime100;
  unsigned long raceTimeFinish;
  
  };

struct Race zamer;
struct canRead Frame;


unsigned long    putErrpomTime ; 
bool putErrpom  = 0;
int keyArray[]={0,420,428,201,202,60,61,1};

/*
0 
420 temp
428 voltmeter
201 rpm
202 speed
1 -rds exit
101 redy
102 go
60 60km
61 100km
*/


int keySet=0;

MCP2515 mcp2515(10);

void setup() 
{
  // put your setup code here, to run once:


  while (!Serial);
  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS,MCP_8MHZ);
  pinMode(KEY, INPUT_PULLUP);
  pinMode(9, OUTPUT);

  mcp2515.setConfigMode();
  

mcp2515.setFilterMask(MCP2515::MASK0, false, 0x42F);
mcp2515.setFilterMask(MCP2515::MASK1, false, 0x20F);
mcp2515.setFilter(MCP2515::RXF0, false, 0x420);
mcp2515.setFilter(MCP2515::RXF2, false, 0x428);
mcp2515.setFilter(MCP2515::RXF1, false, 0x201);

  mcp2515.setNormalMode();

  canMsg4C8.can_id  = 0x4c8; canMsg4C8.can_dlc = 8;canMsg4C8.data[0] = 0x30; canMsg4C8.data[1] = 0x20; canMsg4C8.data[2] = 0x00; canMsg4C8.data[3] = 0x00; canMsg4C8.data[4] = 0x00;canMsg4C8.data[5] = 0x00; canMsg4C8.data[6] = 0x00;canMsg4C8.data[7] = 0x00;

  //canMsg4C8.data[7]={0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  canMsg4C0.can_id  = 0x4C0; canMsg4C0.can_dlc = 8; canMsg4C0.data[0] = 0x10; canMsg4C0.data[1] = 0x0D; //summa 
  canMsg4C0.data[2] = 0x34; 
  
  canMsg4C01.can_id  = 0x4c0; canMsg4C01.can_dlc = 8; canMsg4C01.data[0] = 0x21;


   EEPROM.get(0 , keySet); 

}
  
long spliteByte(uint16_t buf0,uint16_t buf1)  
{

    long  number = 0;
    number = (buf0 *256 + buf1) ; 
    return number;
  
  }

void send_panel (  canRead Frame , int key =202) 
{

int digi[6];
int tim[6];

uint8_t message[] = { 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x20,0x2D }; //11 - 

    if( key == 420) 
    {   // temperatura 420
           int bufCanTemp=0;
           
          
            
            if(Frame.temp < 0)  bufCanTemp = abs(Frame.temp); 
            else bufCanTemp = Frame.temp;
        
       //    Serial.print("\nTemperatura 420 :"); Serial.print(bufCanTemp);
              
          for (int i = 2; i >= 0; i--) 
            {
                digi[i] = bufCanTemp % 10;  
                bufCanTemp = bufCanTemp/10;
            
            }
    
              canMsg4C0.data[3] = 0x54;
              canMsg4C0.data[4] = 0x45;
              canMsg4C0.data[5] = 0x4D;
              canMsg4C0.data[6] = 0x50;
              canMsg4C0.data[7] = 0x20;    
                 
              if(Frame.temp < 0) {       canMsg4C01.data[1] = 0x2D;}   
              else if (Frame.temp > 0 && Frame.temp < 100) {   canMsg4C01.data[1] = 0x20;           }  
              else   {    canMsg4C01.data[1] = message[digi[0]]; }
        
          
              if( (Frame.temp > -10  && Frame.temp < 10 )) { canMsg4C01.data[2] = 0x20;  }
                        
             canMsg4C01.data[2]  = message[digi[1]]; 
             canMsg4C01.data[3]  = message[digi[2]];
   
 }else if( key == 201) 
      {
          long bufCan = 0;
          bufCan = Frame.rpm;
          //bufCan=bufCan/4;
           
    //       Serial.print("\nRPM 201 :"); Serial.println(Frame.rpm);
  
          
          for (int i = 3; i >= 0; i--) 
            {
                digi[i] = bufCan % 10;
                bufCan = bufCan/10;
                
            }

              canMsg4C0.data[3]   = 0x52;
              canMsg4C0.data[4]   = 0x50;
              canMsg4C0.data[5]   = 0x4D;
              canMsg4C0.data[6] = 0x20;
           
              if(Frame.rpm > 999)canMsg4C0.data[7]   = message[digi[0]];
              else canMsg4C0.data[7] = 0x20;
              
              canMsg4C01.data[1]  = message[digi[1]]; 
              canMsg4C01.data[2]  = message[digi[2]]; 
              canMsg4C01.data[3]  = message[digi[3]]; 
       
        }else if(key == 202 )
        {

            //speed only
             long bufCanSpeed =0;
             bufCanSpeed = Frame.speed;
             bufCanSpeed=bufCanSpeed/100;
  //           Serial.print("\nbufCanSpeed 202 :"); Serial.println(bufCanSpeed);
           
             
             for (int i = 2; i >= 0; i--) 
            {
                digi[i] = bufCanSpeed % 10;
                bufCanSpeed = bufCanSpeed/10;
            }
           
              canMsg4C0.data[3] = 0x20;
              canMsg4C0.data[4] = 0x20;
             if(Frame.speed > 9999 ) canMsg4C0.data[5]   = message[digi[0]];
             else canMsg4C0.data[5] = 0x20;


             if(Frame.speed > 999 ) canMsg4C0.data[6]   = message[digi[1]];
             else canMsg4C0.data[6] = 0x20;
             
             
              canMsg4C0.data[7]   = message[digi[2]]; 
              canMsg4C01.data[1] = 0x20; 
              canMsg4C01.data[2] = 0x4B; 
              canMsg4C01.data[3] = 0x4D; 

       }
          
          else if(key == 428 ) 
          {

           // volmeter
           
           int bufCanVolt=0;
           bufCanVolt= Frame.volt;
  
          for (int i = 2; i >= 0; i--) 
            {
                digi[i] = bufCanVolt % 10;  
                bufCanVolt = bufCanVolt/10;
            
            }
            
            //56 4f 4c 54
            
              canMsg4C0.data[3]   = 0x56;
              canMsg4C0.data[4]   = 0x4f;
              canMsg4C0.data[5]   = 0x4c;
              canMsg4C0.data[6]   = 0x54;
             
              if(Frame.volt > 99)  canMsg4C0.data[7]   = message[digi[0]] ; else canMsg4C0.data[7]   = 0x20;
              
              canMsg4C01.data[1]  =  message[digi[1]]; 
              canMsg4C01.data[2]  =  0x2E;; 
              canMsg4C01.data[3]  =  message[digi[2]];
            
            }

            else if(key == 1 ) {    return  ;  }
            else if(key == 101 ) 
          
          {
            // ready

              canMsg4C0.data[3]   = 0x52;
              canMsg4C0.data[4]   = 0x45;
              canMsg4C0.data[5]   = 0x41;
              canMsg4C0.data[6]   = 0x44;
              canMsg4C0.data[7]   = 0x59; 
              canMsg4C01.data[1]  = 0x20; 
              canMsg4C01.data[2]  = 0x2E; 
              canMsg4C01.data[3]  = 0x2E; 
            
            
            }

            else if(key == 61 || key == 60) {
       
             long bufCanSpeed =0;
             long bufCurrentTime=0;
            
             if( key == 61) bufCurrentTime = zamer.raceTime100;
             else  bufCurrentTime = zamer.raceTime60;
                      
             Serial.print("\nbufCurrentTime_to :"); Serial.print(bufCurrentTime);
    
          for (int i = 4; i >= 0; i--) 
            {
                tim[i] = bufCurrentTime % 10;
                bufCurrentTime = bufCurrentTime/10;
            }
            
              canMsg4C0.data[3]   =  message[tim[0]];
              canMsg4C0.data[4]   =  message[tim[1]];;
              canMsg4C0.data[5]   =  0x2E;
              canMsg4C0.data[6]   = message[tim[2]] ;
              canMsg4C0.data[7]   = 0x20; 
             
             
             if( key == 61) {
                         
             canMsg4C01.data[1] = 0x31;
             canMsg4C01.data[2] = 0x30;
             canMsg4C01.data[3] = 0x30; 
             } else {
             canMsg4C01.data[1] = 0x20;
             canMsg4C01.data[2] = 0x36;
             canMsg4C01.data[3] = 0x30; 
              }}
              
            else if(key == 102 ) 
          {
            // ready go go go 
             
             zamer.raceTimeCurrent = millis()-zamer.raceTimeStart;
             long bufCanSpeed =0;
             long bufCurrentTime=0;
             
             bufCanSpeed = Frame.speed;
             bufCanSpeed=bufCanSpeed/100;

             bufCurrentTime = zamer.raceTimeCurrent;
                        
             Serial.print("\nbufCanSpeed_to 100:"); Serial.print(bufCanSpeed);
             Serial.print("\nbufCurrentTime_to 100:"); Serial.print(bufCurrentTime);
          
                 
             for (int i = 2; i >= 0; i--) 
            {
                digi[i] = bufCanSpeed % 10;
                bufCanSpeed = bufCanSpeed/10;
            }


          for (int i = 4; i >= 0; i--) 
            {
                tim[i] = bufCurrentTime % 10;
                bufCurrentTime = bufCurrentTime/10;
            }
 
              canMsg4C0.data[3]   =  message[ tim[0]];
              canMsg4C0.data[4]   =  message[tim[1]];;
              canMsg4C0.data[5]   =  0x2E;
              canMsg4C0.data[6]   = message[tim[2]] ;
              canMsg4C0.data[7]   = 0x20; 
             
             if(Frame.speed > 9999 )  canMsg4C01.data[1]   = message[digi[0]];
             else  canMsg4C01.data[1] = 0x20;


             if(Frame.speed > 999 )  canMsg4C01.data[2]   = message[digi[1]];
             else  canMsg4C01.data[2] = 0x20;
             canMsg4C01.data[3]  = message[digi[2]]; 
     
             }
           
          else  
      
          {

              canMsg4C0.data[3]   = 0x46;
              canMsg4C0.data[4]   = 0x49;
              canMsg4C0.data[5]   = 0x58;
              canMsg4C0.data[6]   = 0x20;
              canMsg4C0.data[7]   = 0x43; 
              canMsg4C01.data[1]  = 0x41; 
              canMsg4C01.data[2]  = 0x4E; 
              canMsg4C01.data[3]  = 0x20;
              }

              canMsg4C01.data[4] = 0x20;
              canMsg4C01.data[5] = 0x49;
              canMsg4C01.data[6] = 0x40;
              canMsg4C01.data[7] = 0x20;
              
  
              mcp2515.sendMessage(&canMsg4C0);
              mcp2515.sendMessage(&canMsg4C8); 
              delay(2);
              mcp2515.sendMessage(&canMsg4C01); 
              
              digi[0] = '\0';
      
 
 }


void loop() 
{
  
 
 int key=202;
 
 
 if( mcp2515.readMessage(&canMsgRead) == MCP2515::ERROR_OK ) {


     if (canMsgRead.can_id==0x420)    
          {   
             Frame.temp  =  canMsgRead.data[0]-40;      
             Frame.brake = canMsgRead.data[6]; 
           }
     else if (canMsgRead.can_id==0x201  )    
          {
               
                Frame.speed = spliteByte( canMsgRead.data[4], canMsgRead.data[5]);
                Frame.rpm   = spliteByte( canMsgRead.data[0], canMsgRead.data[1]);
               
          }
      else if (canMsgRead.can_id==0x428  )  Frame.volt   = canMsgRead.data[1];
       // else key = 99;

       if(Frame.rpm > 3800 || Frame.speed == 0) key = 201;
       
       if(Frame.temp < 45 || Frame.temp > 103) key = 420;
       
       if((Frame.volt < 120  && Frame.volt > 1 ) || Frame.rpm < 500 )key = 428;
  

if ( digitalRead(KEY) == HIGH) 
{
       delay(100);
         if ( digitalRead(KEY) == HIGH) 
        { 
          delay(100);
         
          if(keySet >= 7) keySet =0 ;
               else keySet++;
         
           putErrpomTime =  millis() ; 
           putErrpom  = 1;
           digitalWrite(9, HIGH); 
              
          }
       
 }
 
if (keySet != 0) { key = keyArray[keySet]; }     

           if(putErrpom == 1 && millis()-putErrpomTime > 30000 && keySet < 5)
           { 

                  digitalWrite(9, HIGH); 
                  EEPROM.put(0, keySet); 
                  putErrpom = 0; 
                  digitalWrite(9, LOW);
           } 
 
   

/*
  bool preStart = false; 1700 оборотов включили режим до 100
  bool redyToStart = false;  ждем старта 
  bool raceStart = false; гонка идет 
  bool raseStop = false; гонка з
  bool raceStartTimer = false;
*/


        
        
    if(Frame.speed == 0 &&  Frame.rpm > 1700 && zamer.preStart == false && zamer.raceStart == false && millis()>10000  ) 
        
        {
        //подготовка к замеру 
        zamer.preStart = true;
        zamer.raceTimeRady = millis();
        key = 101;
        Serial.println("\nReady to race PreStart");
          
         }

      
     if ( Frame.speed == 0 &&  zamer.preStart == true && zamer.raceStart == false)  { 
        
        //    ждем старта показываем заставку 
        key = 101;  
        Serial.println("\nredyyy time start ..."); 
        zamer.raceStartTimer = true;
      }  

 
     
     if ((zamer.raceStart == false  && zamer.preStart == true  && millis()- zamer.raceTimeRady>20000)) //prostoi 20 cek
      {

         zamer.preStart = false ;
         zamer.raceStart = false;
         zamer.raceStartTimer = false;
         Serial.println("Stop Rady");

        } 

    if (  Frame.speed == 0 &&  zamer.raceStart == true ) 
         
         {

         zamer.preStart = false ;
         zamer.raceStart = false;
         zamer.raceStartTimer = false;
         Serial.println("Stop  Race");
        // keySet = 6;
         }      

      

     if ( ( zamer.preStart == true && Frame.speed > 0) ||  zamer.raceStart == true ) 
         {
              
              Serial.println("\nRace .... ");
              zamer.raceStart = true;
            
              key=102;
             
              // гонка идет 
           
            
              if(zamer.raceStartTimer == true )  
              {
              // начало замера
                zamer.raceTimeStart =millis();
                zamer.raceTime60 = 0;
                zamer.raceTime100 = 0;
                zamer.raceTimeFinish =0;
                zamer.raceStartTimer = false;
                Serial.println("\nStart Timer");
               }

               if(millis() -  zamer.raceTimeStart > 30000) { 
                  
                  zamer.preStart = false ;
                  zamer.raceStart = false;
                  zamer.raceStartTimer = false;
                
                }
                

               if( Frame.speed >= 6000 &&  zamer.raceTime60 == 0) 
               {
                    Serial.println("60!!!!!!");
                    zamer.raceTime60 = millis() -  zamer.raceTimeStart;
                 }

               if( Frame.speed >= 10000  &&  zamer.raceTime100 == 0) 
               {
                    Serial.println("100 Finish!!!!!!");
                  
                    zamer.raceTime100 = millis() -  zamer.raceTimeStart;
                
                      send_panel(Frame, key);
                      keySet = 6;
                      
                      zamer.preStart = false;
                      zamer.raceStart = false;
                      
                 
                }

            //    Serial.println(key); 
          }



          
         
 //Serial.println(key); 
 //Serial.println(keySet); 
 send_panel(Frame,key);

 //6000cd fix
  delay(8);
  
 }

 if(mcp2515.getStatus() == 0)digitalWrite(9, HIGH);
 else digitalWrite(9, LOW);
  //Serial.println(mcp2515.checkError());


  
}
