#include <segment7.h>
#include <peripherique.h>

#define DISPLAY_A A1
#define DISPLAY_B A2
#define DISPLAY_C 3
#define DISPLAY_D 7
#define DISPLAY_E 6
#define DISPLAY_F 10
#define DISPLAY_G 9
#define DOT_DISPLAY 4

#define BUZZER_PIN 11
#define MODE_BUTTON 0 // bouton poussoir pour basculer le mode d'affichage
#define SELECT_BUTTON 1 // bouton poussoir pour choisir quelle tension afficher
#define ALARM_SWITCH 2 // alarme on/off
#define BATT_LED 8
#define RSSI_LED 5
#define RSSI_IN A6
#define BATT_IN A3

#define MAX_RSSI 1.1 // full signal
#define MIN_RSSI 0.47  // no signal
#define CONV_RSSI 1
#define MAX_LIPO 12.6
#define MIN_LIPO 10
#define CONV_BATT 2.6376

#define THRESHOLD_BATT 11
#define THRESHOLD_RSSI 0.7
#define DELAY_ALARM_RSSI 500
#define DELAY_ALARM_BATT 100

#define SIZE_BUFF_A_IN 8
#define ANALOG_SAMPLE_TIME 100  

//////////////// OBJETS /////////////////
 
Segment7 display(ANODE_C, DISPLAY_A, DISPLAY_B, DISPLAY_C, DISPLAY_D ,DISPLAY_E, DISPLAY_F, DISPLAY_G, DOT_DISPLAY);

Buzzer buzzer(BUZZER_PIN);

Led ledrssi(RSSI_LED);
Led ledbatt(BATT_LED);

DigitalIn select(SELECT_BUTTON,20);
DigitalIn mode(MODE_BUTTON,20);
DigitalIn alarmswitch(ALARM_SWITCH,50);

AnalogIn rssi(RSSI_IN,ANALOG_SAMPLE_TIME,CONV_RSSI,SIZE_BUFF_A_IN);
AnalogIn batt(BATT_IN,ANALOG_SAMPLE_TIME,CONV_BATT,SIZE_BUFF_A_IN);

////////////// Définition des sons et alarmes ///////////////

unsigned int tab[] = {100,80,200,100};
Sound welcome = {tab,4};

unsigned int tab_ae[] = {100,100};
Sound alarmEnabled = {tab_ae,2};

unsigned int tab_a1[]={50,50,50,50};
Sound alarm_rssi = {tab_a1,4};

unsigned int tab_a2[]={100,100};
Sound alarm_batt = {tab_a2,2};
/////////////////////////////////////////////////////////////

char displayMode = 1;
char channel = 0; // RSSI =0 ,batt =1
char rssiEmergency = 0;
char battEmergency = 0;
float rssiVal = 0;
float battVal = 0;
float valToDisplay = 0;


void toggleDisplayMode()
{
 displayMode ^=1;
}

void toggleChannel()
{
  channel ^=1;
  if(channel == 0) { if(!battEmergency) ledbatt.Off(); if(!rssiEmergency) ledrssi.On();}    
  else if(channel == 1) { if(!rssiEmergency) ledrssi.Off(); if(!battEmergency) ledbatt.On();} 
 }

void play()
{
  buzzer.playSound(&alarmEnabled,10,1);
  if(rssiEmergency) buzzer.playSound(&alarm_rssi,DELAY_ALARM_RSSI);
  if(battEmergency) buzzer.playSound(&alarm_batt,DELAY_ALARM_BATT);
}

int scaleForDigit(float min_in, float max_in, float value)
{
  float a =9/(max_in-min_in);
  float b =-a*max_in+9;

  return (int)(a*value+b); 
}

int scaleLipo(float val)
{
  if(val >= 12.45) return 9;
  if(val >= 12.10) return 8;
  if(val >= 11.83) return 7;
  if(val >= 11.7) return 6;
  if(val >= 11.55) return 5;
  if(val >= 11.425) return 4;
  if(val >= 11.3) return 3;
  if(val >= 11.18) return 2;
  if(val >= 10.95) return 1;
  if(val >= 9.9) return 0;

  return 0;
}



void setup() {

buzzer.playSound(&welcome,10,1);

select.startInterrupt(toggleChannel,RISING,6);
mode.startInterrupt(toggleDisplayMode,RISING,6);
alarmswitch.startInterrupt(play,FALLING,6);

}

void loop() {
  
  Peripherique::update();
  display.refresh();

  rssiVal = rssi.getVal();
  battVal = batt.getVal();

  if((rssiVal < THRESHOLD_RSSI) && !rssiEmergency)
  {
    rssiEmergency = 1;
    ledrssi.blink(300);
    buzzer.playSound(&alarm_rssi,DELAY_ALARM_RSSI);
  }

   if((rssiVal > THRESHOLD_RSSI) && rssiEmergency)
  {
    rssiEmergency = 0;
    if(channel == 0) ledrssi.On();
    else ledrssi.Off();
    
    buzzer.stop();
  }

    if((battVal < THRESHOLD_BATT) && !battEmergency)
  {
    battEmergency = 1;
    ledbatt.blink(300);
    buzzer.playSound(&alarm_batt,DELAY_ALARM_BATT);
  }

   if((battVal > THRESHOLD_BATT) && battEmergency)
  {
    battEmergency = 0;
    if(channel == 1) ledbatt.On();
    else ledbatt.Off();
    
    buzzer.stop();
  }


  if(alarmswitch.getVal()) buzzer.stop();
  
  if(channel == 0 && displayMode == 0) valToDisplay = scaleForDigit(MIN_RSSI, MAX_RSSI, rssiVal);
  if(channel == 0 && displayMode == 1) valToDisplay = rssiVal;
  if(channel == 1 && displayMode == 0) valToDisplay =scaleLipo(battVal);
  if(channel == 1 && displayMode == 1) valToDisplay = battVal;

  display.writeNumber(valToDisplay,(mode_dis)displayMode);
}
