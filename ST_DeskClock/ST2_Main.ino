/** \file
 * Main loop for Solder:Time Desk Clock.
 *
 * Run continuously after bootup.
 */

// Test for Sleep
static void
check_sleep_timer(void)
{
	currentMillis = millis();
	OptionModeFlag = false;

	if(SleepEnable && (currentMillis - SleepTimer) > SleepLimit)
	{
		// New for ST Desk Clock - goto Time vs Sleep
		if (STATE== 1)
		{
			SUBSTATE = 1;
			blinkON = true;
			blinkFlag = false;
			blinkMin = false;
			blinkHour = false;
		} else {
			STATE= 1; // was STATE= 99; 
			SUBSTATE = 0;
		}

		SleepTimer = millis();
	}
}


// Test for Mode Button Press
static void
check_mode_button(void)
{
	if (digitalRead(MODEBUTTON))
		return;

	if(ALARMON)
		CheckAlarm();

	if(ALARM1FLAG)
	{
		ALARM1FLAG = false;
		ALARMON = false;
		EnableAlarm1(false);
		STATE = 90;
		JustWokeUpFlag = false;
	} else
	if(JustWokeUpFlag)
	{
		// Used to supress "Time" text from showing when waking up.
		JustWokeUpFlag = false;
		JustWokeUpFlag2 = true;
	} else {
		NextStateRequest = true;
	}

	while(1)
	{
		// check for simultaneous mode and set buttons
		if (!digitalRead(SETBUTTON))
		{
			OptionModeFlag = true;
			NextStateRequest = false;
			NextSUBStateRequest = false;
			displayString("SPEC");
			delay(300);
		}      

		// wait for them to stop holding the button
		if (!digitalRead(MODEBUTTON))
			break;
	}

	delay(100);
	SleepTimer = millis();
}


// Test for SET Button Press
static void
check_set_button(void)
{
	if (digitalRead(SETBUTTON))
		return;

	// if the mode button is held down as well, do nothing
	if (OptionModeFlag)
		return;

	NextSUBStateRequest = true;

	while(1)
	{
		// this is repeated from above; can be merged?
		if (!digitalRead(MODEBUTTON))
		{
			OptionModeFlag = true;
			NextStateRequest = false;
			NextSUBStateRequest = false;
			displayString("SPEC");
			delay(300);
		}   
      
		// wait for them to stop holding the button
		if (digitalRead(SETBUTTON))
			break;
	}

	delay(100);
	SleepTimer = millis();
}
 

// Running Blink counter
static void
check_blink(void)
{
	if(!blinkFlag)
	{
		// Not blinking, just leave the LEDs lit
		blinkON = true;
		return;
	}

	blinkCounter++;
	if (blinkCounter >blinkTime) // was 150
	{
		blinkON = !blinkON;
		blinkCounter = 0;
	}
}  


void loop()
{
	check_sleep_timer();
	check_mode_button();
	check_set_button();
	check_blink();

  
//*******************************************************************************************************************
// 								                        Main Loop - State Machine 
//*******************************************************************************************************************

  switch (STATE) 
  {
  case 0:                                                                  // Set-Up
    STATE = 1;
    break;

  case 1:                                                                  // Display Time
    DisplayTimeSub(); 
    break;

  case 2:                                                                  // Set Time
    setTimeSub();
    break; 

  case 3:                                                                  // Config Alarm
   setAlarmSub();
    break;
 
   case 4:                                                                 // Stop Watch
    StopWatch();
    break;
 
    
  case 5:                                                                 // Serial Display                                 
  DisplaySerialData();
  break;

  case 6:                                                                 // Graphic Demo                                 
  graphican();
  break;

   // ---------------------------------------------------------------   
  
  case 90:                                                                  // Alarm Triggered
  
    blinkFlag = true;
    displayString("Beep");

  if(blinkON)
  {
    pinMode(SETBUTTON, OUTPUT);
    tone(SETBUTTON,4000) ;
    delay(100);
    noTone(SETBUTTON);
    digitalWrite(SETBUTTON, HIGH);
  }

    #if ARDUINO >= 101 
    pinMode(SETBUTTON, INPUT_PULLUP);
//    digitalWrite(SETBUTTON, HIGH);
     #else
//    digitalWrite(SETBUTTON, HIGH);
    pinMode(SETBUTTON, INPUT);
     #endif
    delay(250);

//    bval = !digitalRead(SETBUTTON);
    if(NextSUBStateRequest || NextStateRequest)
    {
      STATE = 0;
      SUBSTATE = 0;
 //     NextStateFlag = true;
      NextStateRequest = false;
      NextSUBStateRequest = false;      
      blinkFlag = false;
    }    
    break;

    // --------------------------------------------------------------- 

  case 99:                                                                    // Sleep
    displayString("Nite");
    delay(500);
    clearmatrix();
    GoToSleep();
    SleepTimer = millis();
    STATE = 0;
    SUBSTATE = 0;
    break;

    // --------------------------------------------------------------- 

  }
}
