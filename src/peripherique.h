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


#ifndef PERIPHERIQUE_H_INCLUDED
#define PERIPHERIQUE_H_INCLUDED
#include "Arduino.h"

#define DEFAULT_SIZE_BUFF 2


class Peripherique {

private:
static char nbrInstance;
static Peripherique** tab; // tableau de pointeur vers toutes les instances de la classe

protected:
char pin;
char on;  // la fonction de callback est appelée avec une fréquence de 1/period si on=1
unsigned int period;
unsigned long lastUpdate;


public:

Peripherique(char pin_in,unsigned int period_in,char default_state=1);

~Peripherique()
{
  delete tab;
}

virtual void callback()=0;
void setPeriod(unsigned int period_in);
virtual void start();
virtual void stop();
char getState();

static void update();

};

///  --------- Timer -----------------//

class Timer:public Peripherique{

private:
    void (*ptFct)(void);
    void callback();

public:
    Timer(unsigned int period_in=0, void(*fct)(void)=NULL, char default_state=0);
    void bindCallback(void(*fct)(void),unsigned int period_in);
};


/// -------------- INPUT CLASSES ----------------------- ///

class DigitalIn:public Peripherique{

private:
    char value;
    char delayIT;
    char compteDelay;
    void (*ptFct)(void);
    int modeIT;

    void callback();

public:
    DigitalIn(char pin_in, unsigned int period_in);


    void startInterrupt(void(*fct)(void),int mode, char delay=0);
    void stopInterrupt();
    char getVal(); // retourne la dernière valeur lu avec la fct de callback
    char read(); // lit directement la valeur et la retourne

};

/// -------- AnalogIn ----------- ///

class AnalogIn: public Peripherique{

private:

    int *values ; // pointeur pour le tableau de valeur
    char size_buff ; //nbr de point de la moyenne glissante. doit être une puissance de 2
    int writeBuff; // indice pour le tableau
    float cstConv ;
    int valMoy ;

    void callback();

public:
 //---- constructeur ------//
    AnalogIn(char pin_analog,int period_in,float conv,char size_b);
    AnalogIn(char pin_analog,int period_in,float conv);

//------- destructeur ------//
    ~AnalogIn() {delete []values ;}

//-------- Methodes ---------//

 float getVal();
 int rawValue();
 void setConv(float paraConv);

    unsigned char getSizeBuff()
    {
        return size_buff;
    }
};


/// ----------------- OUTPUT CLASSES ------------------------------///


class DigitalOut{

protected:
char value;
char pin;

public:
DigitalOut(char pin_in, char init_value=0):pin(pin_in),value(init_value)
{
    pinMode(pin,OUTPUT);
    digitalWrite(pin,init_value);
}

virtual void On() {digitalWrite(pin,HIGH); value=1;}
virtual void Off(){digitalWrite(pin,LOW); value =0;}
char getVal(){return value;}

};


/// ------------------------ LED ------------------///

class Led: public Peripherique, public DigitalOut {

private:
int compteCycle;

void callback()
{
   value ^=1; // toggle value
   digitalWrite(DigitalOut::pin,value);
    if(compteCycle>0)compteCycle --;
    if(compteCycle==0)
    {
        stop();
        Off();
    }
}

public:
Led(char pin,char init_state=0):compteCycle(-1),DigitalOut(pin,init_state),Peripherique(pin,0,0){}

void blink(unsigned int period_in,int nbrCycle_in=-1);
void On(){DigitalOut::On(); on=0;} // on = 0 pour arreter l'appel de la callback
void Off(){DigitalOut::Off(); on=0;}

};


/// ------------------- Buzzer -------------------- ///

typedef struct Sound Sound;
struct Sound {

unsigned int *time; // indices pair => buzzer on          indices impair => buzzer off
char sizeTab;

};

class Buzzer:public Peripherique {

private:

unsigned int tempo; // delai entre 2 sons
char indice;
char compteCycle;
Sound *sound;

void callback()
{
    if(sound !=NULL)
    {

        if((indice+1)%2) analogWrite(pin,100);
        else analogWrite(pin,0);

        period = sound->time[indice];
        indice++;
        if(indice == (sound->sizeTab))
        {
            analogWrite(pin,0);
            period+=tempo;
            indice =0;
            if(compteCycle>0)compteCycle --;
            if(compteCycle==0) stop();
        }
    }

}

public:
Buzzer(char pin);

void playSound(Sound *s,unsigned int tempo, char nbrCycle=-1);
void stop();
char isPlaying();

};


#endif // PERIPHERIQUE_H_INCLUDED
