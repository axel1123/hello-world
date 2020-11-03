// This is a prube
///////////----------
/*****************************************************************/
/****************************** E2I ******************************/
/*****************************************************************/
/******************* Programa de Sensor BMA400 *******************/

/*
ADXL362_MotionActivatedSleep para el ADXL362 - Micropower 3-axis accelerometer
Conecciones:
 CONECCIONES:

 DEVICE   | MSP430
 -------------------
   MOSI   |  P1.7     
   MISO   |  P1.6
   SCLK   |  P1.5
   CS     |  P2.0
   INT1   |  P1.3
   GND    |  GND
   VIO    |  3.3v
   VS     |  3.3v

    DEVICE   | MSP430
 -------------------
   MOSI   |  d11     
   MISO   |  d12
   SCLK   |  d13
   CS     |  d10
   INT1   |  d3
   GND    |  GND
   VIO    |  3.3v
   VS     |  3.3v
 
*/ 


#include <SPI.h>
#include <SD.h>



#include "BMA400.h"
/*_____________________________ Fin _____________________________*/

/************************ Pin de Int Sen *************************/
#define interruptPin 1 
//#define WAKEUP_PIN P1_3
/*_____________________________ Fin _____________________________*/

int CS = 2;//para el cs del bma
File myFile;

/********************* ACC Sensor Variables **********************/
uint8_t Ascale = AFS_4G, SR = SR_200Hz, power_Mode = normal_Mode, OSR = osr3, acc_filter = acc_filt1;

long mssg;
long mssg2;
String Dato[10];
String valorCadena = "";

//uint8_t mssg;
int16_t ax, ay, az;       
const int WAKEUP_PIN = 1;
bool valInterrupt = 1;
BMA400 BMA400;
/*_____________________________ Fin _____________________________*/

/************************* Main Funccion *************************/
void setup()
{

  SPI.begin();
  Serial.begin(9600);

  pinMode(3,OUTPUT);//cs del sd
  //digitalWrite(3,LOW);
  pinMode(CS, OUTPUT);

  digitalWrite(CS, HIGH);

  if (!SD.begin(3)) { // the (8) is the CS pin you chose to use, on my sheild that pin is hardwired to "8"
    return;
  }
  // create a new file 
  // starts with Crash00, and next one would be Crash01 
  char filename[] = "Crash.txt";
  for (uint8_t i = 0; i < 100; i++) {
    //filename[6] = i/10 + '0';
    //filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      myFile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  SPI.setDataMode(SPI_MODE3);
  digitalWrite(CS,HIGH);
  
  BMA400.begin(2);
  Serial.println("BMA400 accelerometer...");
  byte c = BMA400.getChipID();  
  Serial.print("BMA400 "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.println(" ");
  delay(1000); 



  //SPI.setDataMode(SPI_MODE3);
  //digitalWrite(CS,HIGH);

  if(c == 0x90){
   Serial.println("BMA400 is online..."); Serial.println(" ");
   BMA400.resetBMA400();                                                  
   BMA400.initBMA400(Ascale, SR, normal_Mode  , OSR, acc_filter);           
   BMA400.SetWakeupInterruption();     
   attachInterrupt(interruptPin,interrupt,RISING);               
  }
  else {
  if(c != 0x90) Serial.println(" BMA400 not functioning!");
  }
}
/*_____________________________ Fin _____________________________*/

void Reception_Data(){
  //Serial.println(mssg);
  if(mssg > 500){
    mssg2 = mssg;
  }
  valorCadena = String(mssg);

  int large_valorCadena = valorCadena.length();

  if (large_valorCadena > 6){
    
    for(int i = 0; i <= large_valorCadena; i++){
      for(int j = 0; j <= large_valorCadena; j++){
        if(j == i + 2){
          if(j % 2 == 0){
            Dato[i] = valorCadena.substring(i,j);
            Serial.println(Dato[i]);
            //Serial.print("wii");
            //Serial.println(large_valorCadena);
            
            //delay(4000);
            }
          }
        }
      }
  }
  if (large_valorCadena == 1){
    Serial.print("Parametros: ");
    Serial.println(mssg2);
    }
  }
  
void Parameters(){
  
    //BMA400.resetBMA400();  
  //if(mssg != 0){
    
   //Serial.flush();
   //Serial.println(mssg);
   if(Dato[0].toInt() == 10){    
    Ascale = AFS_2G;
    } else if(Dato[0].toInt() == 11){
    Ascale = AFS_4G;
      } else if (Dato[0].toInt() == 12){
        Ascale = AFS_8G; 
        } else if (Dato[0].toInt() == 13){
          Ascale = AFS_16G;
          //} 
  }

   if(Dato[2].toInt() == 20){
    SR = SR_12_5Hz;
    } else if(Dato[2].toInt() == 21){
      SR = SR_25Hz;
      } else if(Dato[2].toInt() == 22){
      SR = SR_50Hz;
      } else if(Dato[2].toInt() == 23){
      SR = SR_100Hz;
      } else if(Dato[2].toInt() == 24){
      SR = SR_200Hz;
      } else if(Dato[2].toInt() == 25){
      SR = SR_800Hz;
      }

    if(Dato[4].toInt() == 30){
      power_Mode = sleep_Mode;
      } else if(Dato[4].toInt() == 31){
        power_Mode = lowpower_Mode;
        } else if(Dato[4].toInt() == 32){
        power_Mode = normal_Mode;
        }

    if(Dato[6].toInt() == 40){
      OSR = osr0;
      } else if(Dato[6].toInt() == 41){
      OSR = osr1;
      } else if(Dato[6].toInt() == 42){
      OSR = osr2;
      } else if(Dato[6].toInt() == 43){
      OSR = osr3;
      } 

    if(Dato[8].toInt() == 50){
      acc_filter = acc_filt1;
      } else if(Dato[8].toInt() == 51){
      acc_filter = acc_filt2;
      } else if(Dato[8].toInt() == 52){
      acc_filter = acc_filt_lp;
      }

  
      
   //Ascale = Ascale;
    
   //Serial.println("*****************");
   Serial.println(Ascale);
   //Serial.println("*****************");
   delay(2000);
   //byte c = BMA400.getChipID();
   //if(c == 0x90){
   //Serial.println("BMA400 is online..."); //Serial.println(" ");
   //BMA400.resetBMA400();                                                  
          
   //BMA400.SetWakeupInterruption();     
   //attachInterrupt(interruptPin,interrupt,RISING);               
  //}
  //else {
  //if(c != 0x90) Serial.println(" BMA400 not functioning!");
  //}
  
  }

/************************* Main Funccion *************************/
void Read(){
  //digitalWrite(2,LOW);
  //digitalWrite(3,HIGH);
  BMA400.readBMA400AccelData(ax,ay,az);
 // digitalWrite(WAKEUP_PIN ,LOW);
  Serial.print("a"); Serial.print(ax);Serial.print(",");Serial.print(ay);Serial.print(",");Serial.println(az); 

   sd();
  }
/*_____________________________ Fin _____________________________*/

/************************* Main Funccion *************************/

void sd(){
  //digitalWrite(2,HIGH);
  //digitalWrite(3,LOW);
  }
void sleepNow(){
  Serial.print("\nBMA400 went to sleep - Put MSP to sleep now \n");
  delay(100);
//  suspend();
  }
/*_____________________________ Fin _____________________________*/

/************************ Main Funccion **************************/
void interrupt(){
//  wakeup();
  //delay(100);
  }
/*_____________________________ Fin _____________________________*/

/************************** Main Loop ****************************/
void loop() { 

 // digitalWrite(7,HIGH);
  //digitalWrite(2,LOW);
  //valInterrupt = digitalRead(interruptPin);
  //if(valInterrupt == 1){
       /* 
      if (Serial.available() > 0){
          mssg = Serial.parseInt();
          //sleep(100);
          Reception_Data();
          Parameters();
          BMA400.initBMA400(Ascale, SR, normal_Mode, OSR, acc_filter);
          //BMA400.SetWakeupInterruption();     
        }
        */
    //interrupt();
    //digitalWrite(WAKEUP_PIN ,LOW);
    
    Read();
    /*} 
  else{
    digitalWrite(WAKEUP_PIN, HIGH);
    //sleepNow();
    //Read();
      }
      */
}
/*_____________________________ Fin _____________________________*/
