/** \file
 * Solder:Time Desk Clock
 * SpikenzieLabs.com
 *
 * By: MARK DEMERS, May 2013
 * Updated: Trammell Hudson <hudson@trmm.net>, Feb 2014
 * VERSION 1.1
 *
 * Brief:
 * Sketch used in the Solder: Time Desk Clock Kit, more info and build
 * instructions at http://www.spikenzielabs.com/stdc
 *
 * Updated information at http://trmm.net/SolderTime_Desk_Clock
 *
 *  LEGAL:
 * This code is provided as is. No guaranties or warranties are given
 * in any form. It is your responsibilty to determine this codes
 * suitability for your application.
 * 
 * Changes:
 * A. Modified LEDupdateTHREE() void used by ST:2 Watch to function
 * with the new circuits in the Solder:Time Desk Clock
 * B. Modified port dirctions on some pins to deal with new circuits.
 * C. Changed sleep mode into a "change back to display time" mode
 */ 

#include <Wire.h>
#include <EEPROM.h>

#include <TimerOne.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Worm animation
int c =0;
int y = 3;
int target = 3;
int targdist =0;
boolean targdir = true;
int wormlenght = 15;
boolean soundeffect = false;

// int  i  =0;
// int  i2  =0;
// int  vite = 2;
// uint8_t incro = 0;
// uint8_t column = 0;
uint8_t TEXT = 65;
uint8_t i2cData = 0;
// int nextcounter = 0;

int STATE = 0;
int SUBSTATE = 0;
int MAXSTATE = 6;
boolean NextStateRequest = false;
boolean NextSUBStateRequest = false;
boolean JustWokeUpFlag = false;
boolean JustWokeUpFlag2= false;
boolean OptionModeFlag = false;

int ROWBITINDEX = 0;
int scrollCounter =0;
int ScrollLoops = 3;
int scrollSpeed = 300;                                    // was 1200
int blinkCounter = 0;
boolean blinkFlag = false;
boolean blinkON = true;
boolean blinkHour = false;
boolean blinkMin = false;
#define blinkTime  500                                   // was 1000

boolean displayFLAG = true;

unsigned long SleepTimer;
unsigned long currentMillis;
unsigned long SleepLimit = 6000;
boolean SleepEnable = true;
int UpdateTime = 0;

#define BUTTON1 2
#define MODEBUTTON 2
#define BUTTON2 3
#define SETBUTTON 3
boolean  bval = false;

//char Str1[] = "Hi";
char IncomingMessage[24];
char MessageRead;
//uint8_t INBYTE;
uint8_t Message[275];
int IncomingIndex = 0;
int IncomingMessIndex =0;
int IncomingMax = 0;
int MessagePointer = 0;
int StartWindow = 0;
int IncomingLoaded =0;


char days[7][4] = {
  "Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
char months[12][4] = {
  "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

// Time Variables
uint8_t HourTens = 1;
uint8_t HourOnes = 2;
uint8_t MinTens = 0;
uint8_t MinOnes = 0;
uint8_t SecTens = 0;
uint8_t SecOnes = 0;

uint8_t Days =1;
uint8_t DateOnes = 1;
uint8_t DateTens =0;
uint8_t MonthOnes =1;
uint8_t MonthTens = 1;
uint8_t YearsOnes = 2;
uint8_t YearsTens = 1;

uint8_t DayCode =1;
uint8_t MonthCode =1;


boolean TH_Not24_flag = true;
boolean PM_NotAM_flag = false;
boolean NewTimeFormate = TH_Not24_flag;
uint8_t AMPMALARMDOTS = 0;

// Alarm
uint8_t AHourTens  = 1;
uint8_t AHourOnes = 2;
uint8_t AMinTens = 0;
uint8_t AMinOnes = 0;

boolean A_TH_Not24_flag = true;
boolean A_PM_NotAM_flag = false;

// StopWatch
  int OldTime = 0;
  int CurrentTime = 0;
  int TotalTime = 0;

uint8_t SWDigit4 = 0;  
uint8_t SWDigit3 = 0;   
uint8_t SWDigit2 = 0; 
uint8_t SWDigit1 = 0; 

int SWMINUTES = 0;
int SWSECONDS = 0;

int dayIndex = 0;

//uint8_t SetDigit  = 4;
//boolean MODEOVERRIDEFLAG = false;
boolean NextStateFlag = false;
boolean SetTimeFlag = false;
boolean ALARM1FLAG = false;
boolean ALARMON = false;

boolean scrollDirFlag = false;


//
volatile uint8_t Mcolumn = 0;
//volatile uint8_t McolumnTemp = 0;
//volatile uint8_t Mrow = 0;
volatile uint8_t LEDMAT[20];

volatile boolean MINUP = false;
volatile boolean MINDOWN = false;
volatile boolean TFH = false;

const int digitoffset = 95;                // 95             // was 16


// Constants
// DS1337+ Address locations
#define		RTCDS1337	B01101000               // was B11010000  
#define		RTCCONT		B00001110		//; Control
#define		RTCSTATUS       B00001111		//; Status

//#define		RTC_HSEC	B00000001		//; Hundredth of a secound
#define		RTC_SEC		B00000000		//; Seconds
#define		RTC_MIN		B00000001		//; Minuites
#define		RTC_HOUR	B00000010		//; Hours

#define		RTC_DAY  	B00000011		//; Day
#define		RTC_DATE	B00000100		//; Date
#define		RTC_MONTH	B00000101		//; Month
#define		RTC_YEAR	B00000110		//; Year

#define		RTC_ALARM1SEC	B00000111		//; Seconds
#define		RTC_ALARM1MIN	B00001000		//; Minuites
#define		RTC_ALARM1HOUR	B00001001		//; Hours
#define         RTC_ALARM1DATE  B00001010               //; Date

// Fonts have been moved to font.c
extern const uint8_t LETTERS[][5];
extern const uint8_t GRAPHIC[][5];
