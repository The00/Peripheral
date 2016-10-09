/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Theophile Leurent
 April 2016
 rev 1.0
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "peripherique.h"

/// ------------------- Peripherique ---------------------------///

char Peripherique::nbrInstance=0;
Peripherique** Peripherique::tab=NULL;

Peripherique::Peripherique(char pin_in,unsigned int period_in,char default_state):pin(pin_in),period(period_in),lastUpdate(0)
{
    int i;
    on=default_state;
    Peripherique **transit = new Peripherique*[nbrInstance+1] ; // on crée un nouveau tableau avec 1 élément de plus

    for(i=0;i<nbrInstance;i++) transit[i]=tab[i]; // on recopie l'ancien tableau dans le nouveau

    delete tab; // on supprime l'ancien

    tab = transit; // on réaffecte tab, transit devient le nouveau tableau
    tab[nbrInstance]= this; // on ajoute l'instance crée à la fin du tableau

    nbrInstance++;
}

////////////////////////////////////
// 	function  void update()
//
// 	parameter: none
//	
// 	return : none
//
//  note : 	This function must be called in the loop function when using one of the class of the librairie.
//			The loop function should not call blocking or polling function.
////////////////////////////////////
void Peripherique::update()
{
    int i;
    unsigned long time_now=millis();

    for(i=0;i<nbrInstance;i++)
    {
        if(tab[i]->on &&(time_now-(tab[i]->lastUpdate)) >= tab[i]->period) // si il faut actualiser et si le peripherique est activé
        {
            tab[i]->lastUpdate = time_now;
            tab[i]->callback();
        }
    }
}

////////////////////////////////////
// 	function  void setPeriod(unsigned int period_in)
//
// 	parameter: period_in: set the calling period(in ms) of the callback function of the object.
//	
// 	return : none
//
//  note : 	
////////////////////////////////////
void Peripherique::setPeriod(unsigned int period_in)
{
     if(period_in>0) period=period_in ;
}

////////////////////////////////////
// 	function  void start()
//
// 	parameter: none
//	
// 	return : none
//
//  note : 	enable the callback function of the object to be called with the period set with setPeriod()
////////////////////////////////////
void Peripherique::start()
{
    on=1;
}

////////////////////////////////////
// 	function  void stop()
//
// 	parameter: none
//	
// 	return : none
//
//  note : disable the callback function of the object.
////////////////////////////////////
void Peripherique::stop()
{
    on=0;
}


////////////////////////////////////
// 	function  void getState()
//
// 	parameter: none
//	
// 	return : 1 if the callback function is enabled, 0 if not.
//
//  note : 	
////////////////////////////////////
char Peripherique::getState()
{
    return on;
}

/// ------------------ Timer ------------------------------///



Timer::Timer(unsigned int period_in, void(*fct)(void), char default_state):ptFct(fct),Peripherique(-1,period_in, default_state){}

void Timer::callback()
{
    if(ptFct != NULL) (*ptFct)();
}


////////////////////////////////////
// 	function  void bindCallback()
//
// 	parameter: fct: 		address of the callback function
//			   period_in:	period (in ms) at which fct will be called 
//	
// 	return : none
//
//  note : 	
////////////////////////////////////
void Timer::bindCallback(void(*fct)(void),unsigned int period_in)
{
    period = period_in;
    ptFct = fct;
}

/// ------------------- AnalogIn ----------r-----------------///



AnalogIn::AnalogIn(char pin_analog,int period_in,float conv):cstConv(conv),valMoy(0),writeBuff(0),size_buff(DEFAULT_SIZE_BUFF),Peripherique(pin_analog,period_in)
    {
         values = new int[size_buff] ;
    }

    AnalogIn::AnalogIn(char pin_analog,int period_in,float conv,char nbrP) :cstConv(conv),valMoy(0),writeBuff(0),Peripherique(pin_analog,period_in)
   {
    char i;
    char count_bit = 0;

    for(i=0;i<8;i++) count_bit += ((nbrP & (1<<i))>>i); // vérifie que le nombre de point est bien un multiple de 2

    if(count_bit == 1) size_buff = nbrP;
    else size_buff = DEFAULT_SIZE_BUFF; // sinon on utilise la valeur par défaut

    values = new int[size_buff] ;

    for(i=0;i<size_buff;i++) values[i]=0;
}

void AnalogIn::callback()
    {
    int new_val=analogRead(pin);
    writeBuff = (writeBuff+1)&(size_buff-1);
    valMoy = valMoy +new_val-values[writeBuff];
    values[writeBuff]=new_val;
    }


//--------- Methodes ---------//


////////////////////////////////////
// 	function  float getVal()
//
// 	parameter: none
//	
// 	return : return the voltage measured after the low pass filter (moving average) and multipled with the cstConv
//
//  note : 	
////////////////////////////////////
 float AnalogIn::getVal()
 {
     return (float)valMoy*(cstConv/size_buff)*((float)5/1023);
 }

 
 ////////////////////////////////////
// 	function  int rawValue()
//
// 	parameter: none
//	
// 	return : perform a direct measure and return the result (no conversion, 0-1023)
//
//  note : 	
////////////////////////////////////
int AnalogIn::rawValue()
{
    return analogRead(pin);
}

////////////////////////////////////
// 	function  void setConv()
//
// 	parameter: paraConv: set the conversion value used in getVal()
//	
// 	return : none
//
//  note : 	
////////////////////////////////////
 void AnalogIn::setConv(float paraConv)
  {
      cstConv=paraConv;
  }


/// ---------------------- DigitalIn ----------------------- ///


DigitalIn::DigitalIn(char pin_in, unsigned int period_in):value(0),Peripherique(pin_in,period_in)
{
    pinMode(pin_in,INPUT_PULLUP);
    ptFct = NULL;
    modeIT = 0;
    delayIT = 0;
}

void DigitalIn::callback()
{
    if(!compteDelay) compteDelay--;

    else
     {
        int transit=digitalRead(pin);

        if((modeIT == RISING) && (transit == HIGH) && (value == LOW))
        {
            if(ptFct != NULL)(*ptFct)();
            if(delayIT) compteDelay = delayIT;
        }

        if((modeIT == FALLING) && (transit == LOW) && (value == HIGH))
        {
           if(ptFct != NULL)(*ptFct)();
           if(delayIT) compteDelay = delayIT;
        }

         if((modeIT == CHANGE) && (value ^ transit))
         {
            if(ptFct != NULL)(*ptFct)();
            if(delayIT) compteDelay = delayIT;
         }

     value = transit;
    }
}

////////////////////////////////////
// 	function  char getVal()
//
// 	parameter: none
//	
// 	return : return the value of the pin measured with the callback function
//
//  note : 	
////////////////////////////////////
char DigitalIn::getVal()
{
    return value;
}


////////////////////////////////////
// 	function  char read()
//
// 	parameter: none
//	
// 	return : perform a direct read of the pin and return the result
//
//  note : 	
////////////////////////////////////
char DigitalIn::read()
{
    return digitalRead(pin);
}


////////////////////////////////////
// 	function  void startInterrupt()
//
// 	parameter: fct: callback function for the interrupt
//			   mode: RISING, FALLING or CHANGE
//			   delay: number of bounce to wait before the isr can be called again after a interrupt
// 	return :  none
//
//  note : 	it's not real hardware interrupt. The value of the pin is readed periodically and the callback function is called if needed.
////////////////////////////////////
void DigitalIn::startInterrupt(void(*fct)(void),int mode,char delay)
{
    ptFct = fct;
    modeIT = mode;
    delayIT = delay;
}

void DigitalIn::stopInterrupt()
{
    ptFct = NULL;
    modeIT = 0;
}
/// ---------------------- Buzzer ----------------------- ///

Buzzer::Buzzer(char pin):indice(0),compteCycle(-1),sound(NULL),Peripherique(pin,1)
{
    pinMode(pin,OUTPUT);
}

void Buzzer::playSound(Sound *s,unsigned int temp, char nbrCycle)
{
    compteCycle=nbrCycle;
    tempo=temp;
    indice=0;
    sound=s;
    on=1;

}


void Buzzer::stop()
{
    analogWrite(pin,0);
    on=0;
}

char Buzzer::isPlaying()
{
    return on;
}

/// ------------------- Led ---------------------/////////////

void Led::blink(unsigned int period_in,int nbrCycle_in)
{
    compteCycle=nbrCycle_in*2;
    period=period_in/2;
    on=1;
    value=1;
}

