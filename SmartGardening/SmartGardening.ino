/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <RTClib.h>
#include<string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUZZER 11
#define RELAY1 14
#define MANUAL_BUTTON 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static bool tone_flag=false;



//Button Varibles
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 10;    // the debounce time; increase if the output flickers
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin


//Timer varibles
//To calculate the remaining time to turn off the water
unsigned long TimertimeStart=0;
unsigned long motorofftimeRemaining=0;

//Timer1
const char char_Timer1_StartTime[] = "12:15:00";
unsigned int Timer1_inSeconds = 40;     // 10sec equals to 100ml
const char char_Timer1_EndTime[]   = "12:15:40";

// To calculate amount of time button is pressed
unsigned long ButtonPressedTime = 0;      // time at which button is pressed
unsigned long ButtonPressedDuration = 0;  // how long the button is pressed

typedef enum{
  BUTTON_PRESSED=0,
  BUTTON_RELEASED,
  TIMER_START,
  TIMER_EXPIRED
  }system_state_t;



  // 'watertap', 90x76px
 static const unsigned char PROGMEM watertap []= {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xc0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x1f, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
  0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x1f, 0x07, 0xf0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0e, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xf0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf0, 0x0d, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x0d, 0xff, 0xfc, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x1d, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
  0xff, 0xff, 0xff, 0xfd, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfd, 
  0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xfc, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 
  0xff, 0xff, 0xff, 0xfd, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xfd, 
  0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xfe, 0x0d, 0xff, 0xfc, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xff, 0x83, 0xff, 0xf0, 0x0d, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 
  0x00, 0xff, 0xc0, 0x0d, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x0e, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
  0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
  0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x3f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x07, 0xc0, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x20, 
  0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x60, 0x3e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x70, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xe0, 0xf8, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe1, 0xf8, 
  0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe1, 0xfc, 0x7c, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe1, 0xf8, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xe0, 0xf8, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x70, 
  0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x7c, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(57600);

  //Configure manual button as input
  pinMode(MANUAL_BUTTON, INPUT);
  
  //Set the RTC time to System Time
  if (!rtc.isrunning()) {
    //Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
   
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   //rtc.adjust(DateTime(2020, 11, 05, 13, 49, 0));

   delay(2000); // Pause for 2 seconds

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Clear the buffer.
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 0);
  // Display static text
  display.println("Smart");

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  // Display static text
  display.println("Gardening");
  
  display.display();

  //Sound Effect
   cutomtone2();
   

  delay(2000); // Pause for 2 seco
   // Clear the buffer.
   display.clearDisplay();
  
   //Configure the Motor output:
   pinMode(RELAY1, OUTPUT);
   
   //Default off state of relay
   digitalWrite(RELAY1, LOW); 

  //Initialize the time variable
   //memset(char_Timer1_StartTime[8], '\0', 1);
   //memset(char_Timer1_EndTime[8], '\0', 1);

    //Create the Final time string 
    //TimerEndTimeStringCreate(char_Timer1_StartTime,char_Timer1_EndTime,Timer1_inSeconds);
   
}
 
void loop() {

    static char statedigramCurr = TIMER_EXPIRED;
    static char statedigramNext = TIMER_EXPIRED;

    DateTime now = rtc.now();
    
    //buffer can be defined using following combinations:
    //hh - the hour with a leading zero (00 to 23)
    //mm - the minute with a leading zero (00 to 59)
    //ss - the whole second with a leading zero where applicable (00 to 59)
    //YYYY - the year as four digit number
    //YY - the year as two digit number (00-99)
    //MM - the month as number with a leading zero (01-12)
    //MMM - the abbreviated English month name ('Jan' to 'Dec')
    //DD - the day as number with a leading zero (01 to 31)
    //DDD - the abbreviated English day name ('Mon' to 'Sun')
    
    char CurrentDate[] = "DD/MM/YYYY";
    //Serial.println(now.toString(CurrentDate));
    
    char CurrentTime[] = "hh:mm:ss";
    //Serial.print(strlen(CurrentTime),DEC);
    Serial.println(now.toString(CurrentTime));


    if(statedigramCurr == TIMER_EXPIRED){
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println(now.toString(CurrentDate));
        
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10, 30);
        // Display static text
        display.println(now.toString(CurrentTime));

        //Display Timer start time 
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 46);
        display.println("Irrigation Time:");

        
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 55);
        display.println(char_Timer1_StartTime);

        //Display Timer time in seconds
        char tembuf[5];
        memset(tembuf,0,sizeof(tembuf));
        sprintf(tembuf,"%d secs",Timer1_inSeconds);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(64, 55);
        display.println(tembuf);
    }
    else{   char remainingTimeBuff[6]={'\0'};
      
           //Display water tap image
            display.drawBitmap(0, 0, watertap, 90, 76, 1);

            if(statedigramCurr == TIMER_START){ 

                //To determine the event of 1 sec passed
                if((millis() - TimertimeStart)>=1000){
                      motorofftimeRemaining = motorofftimeRemaining - 1;
                      TimertimeStart = millis();
                    }
                  //Display remaining time in seconds when timer start
                    display.setTextSize(2);
                    display.setTextColor(SSD1306_WHITE);
                    display.setCursor(50, 45);
                    sprintf(remainingTimeBuff,"%02lds",motorofftimeRemaining);
                    display.println(remainingTimeBuff);
            }
            else if(statedigramCurr == BUTTON_PRESSED)
            {
              //Display the how much time button is pressed
                display.setTextSize(2);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(50, 45);
                sprintf(remainingTimeBuff,"%03lds",ButtonPressedDuration);
                display.println(remainingTimeBuff);
             }
             else
             {}
      }
     //Display image on screen 
      display.display();
      
    // read the state of the switch into a local variable:
  int reading = digitalRead(MANUAL_BUTTON);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
    }
  }

         
    switch(statedigramCurr){
      
      case BUTTON_PRESSED : relayon();
                            tone_flag = true;
                            if(buttonState == HIGH)
                                {statedigramNext =BUTTON_PRESSED;
                                ButtonPressedDuration = millis() - ButtonPressedTime;
                                }
                            else
                                statedigramNext =BUTTON_RELEASED;
                            break;
      case BUTTON_RELEASED: relayoff();
                            tone_flag = false;
                            statedigramNext = TIMER_EXPIRED;
                            ButtonPressedTime = 0;
                            break;
      case TIMER_START   :  relayon();
                             int ret;

                             ret = strcmp(char_Timer1_EndTime,CurrentTime);
                             if(ret < 0) {
                                Serial.println("str1 is less than str2");
                             } else if(ret > 0) {
                                Serial.println("str2 is less than str1");
                             } else {
                                Serial.println("str1 is equal to str2");
                             }

                             Serial.println("ret=");
                             Serial.print(ret,DEC);
                             Serial.print("\n");    // prints a tab
                             
                            if((ret == false)){
                            statedigramNext = TIMER_EXPIRED;
                            motorofftimeRemaining=0;
                            }
                            tone_flag = true;
                            break;     
      case TIMER_EXPIRED :  relayoff();
                            if(buttonState == HIGH){
                              statedigramNext =BUTTON_PRESSED;
                              ButtonPressedTime = millis();
                               }
                               
                             int ret1;
                             ret1 = strcmp(char_Timer1_StartTime,CurrentTime);
                             if(ret1 < 0) {
                                Serial.println("str1 is less than str2");
                             } else if(ret > 0) {
                                Serial.println("str2 is less than str1");
                             } else {
                                Serial.println("str1 is equal to str2");
                             }

                             Serial.println("ret1=");
                             Serial.print(ret1,DEC);
                             Serial.print("\n");    // prints a tab 
                                
                             if((strcmp(char_Timer1_StartTime,CurrentTime)== 0)){
                              
                                statedigramNext = TIMER_START;
                                TimertimeStart = millis();
                                 //Intialize the motor turn off time variable set time
                                 motorofftimeRemaining = Timer1_inSeconds;
                                }
                            tone_flag = false;
                           break;     
      default:
              break;    
      
      }
      statedigramCurr = statedigramNext;
      //Serial.print(statedigramCurr,DEC);
      //Serial.print("\n");    // prints a tab

       if(tone_flag == true)
       cutomtone1();
       
        delay(20); // Pause for 1 seco
       // Clear the buffer.
        display.clearDisplay();

       // save the reading. Next time through the loop, it'll be the lastButtonState:
       lastButtonState = reading;
}



void spacegun(int maximum){
  
  for(int i=0; i<= maximum;i++){
    digitalWrite(BUZZER,HIGH);
    delayMicroseconds(i+100);
    digitalWrite(BUZZER,LOW);
    delayMicroseconds(i+100); 
  }
}


void cutomtone1(){
    tone(BUZZER,3000,10);
}

void cutomtone2(){
    tone(BUZZER,3500,500);
}

void relayon(){
  digitalWrite(RELAY1, HIGH);
  }
void relayoff(){
  digitalWrite(RELAY1, LOW);
  }

void TimerEndTimeStringCreate(char buffer1[9],char buffer2[9],unsigned int timevar){
 
    char charbuff[2];
    memset(charbuff, '\0', sizeof(charbuff));
    sprintf(charbuff,"%02d",timevar);
    Serial.println(charbuff);
    
    // copying n bytes of src into dest. 
    strncpy(buffer2, buffer1, 6);
    strcat(buffer2, charbuff);
    Serial.println("You are here");
    Serial.println(buffer2);
    
   //Initialize the buffer
   memset(buffer1[8], '\0', 1);
   memset(buffer2[8], '\0', 1);
  }  
