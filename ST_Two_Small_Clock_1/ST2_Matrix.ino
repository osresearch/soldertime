 //*******************************************************************************************************************
// 						Called by Timer 1 Interrupt to draw next column in LED matrix
//*******************************************************************************************************************
// Only light one ROW (and one column) ie one pixel at a time. = lower current draw, but lower refresh rate.


void LEDupdateTWO()                                                  // ONE ROW of selected column at a time
{  

  if(ROWBITINDEX >6)
  {
    Mcolumn = Mcolumn+1;                                              // Prep for next column
    if(Mcolumn >19)
    {
      Mcolumn =0;
    }

    PORTB = (PORTB & B10000000);                                    // Clear last column
    PORTC = (PORTC & B11110000) | B00001111;

    if(Mcolumn <16)                                                 // Matrix column (from 0 to 19)
    {
      PORTB = (PORTB & B01111111); //| (0<<PORTB7);                 // Decode digit Col. 1 to 16 - Select De-Mux chip

      PORTD = (PORTD & B00001111) | (Mcolumn << 4);                 // Decode address to 74HC154
    }
    else
    {
      PORTB =  (1<<PORTB7);                                          // Decode digit Col. 17 to 20 - UN-Select De-Mux chip

      PORTC = (PORTC & B11110000) | ~(1<<(Mcolumn-16));              // Using PC0 to PC4 to address col. 17 to 20 directly
    }

    ROWBITINDEX = 0;

  }
  else
  {
    PORTB = (PORTB & B10000000);
    if(bitRead(LEDMAT[Mcolumn],ROWBITINDEX))
    {
      //      PORTB = (PORTB & B10000000);
      bitSet(PORTB,ROWBITINDEX);
    }

    if(Mcolumn <16)                                                 // Matrix column (from 0 to 19)
    {
      delayMicroseconds(120);
    }
    //    else
    //    {
    //   bitClear(PORTB,ROWBITINDEX);
    //    }

    ROWBITINDEX = ROWBITINDEX +1;
  }
}

//*******************************************************************************************************************
// 						Called by Timer 1 Interrupt to draw next column in LED matrix
//*******************************************************************************************************************
// This version of LED refresh / drawing lights full column at once = higher current draw (but can be brighter)

void LEDupdate()                                                  // All ROWs of selected column at the same time
{  

  PORTB = (PORTB & B10000000);                                    // Clear last column
  PORTC = (PORTC & B11110000) | B00001111;

  if(Mcolumn <16)                                                 // Matrix column (from 0 to 19)
  {
    PORTB = (PORTB & B01111111); //| (0<<PORTB7);                 // Decode digit Col. 1 to 16 - Select De-Mux chip

    PORTD = (PORTD & B00001111) | (Mcolumn << 4);                 // Decode address to 74HC154
  }
  else
  {
    PORTB =  (1<<PORTB7);                                          // Decode digit Col. 17 to 20 - UN-Select De-Mux chip

    PORTC = (PORTC & B11110000) | ~(1<<(Mcolumn-16));              // Using PC0 to PC4 to address col. 17 to 20 directly
  }

  // ---  

  PORTB = (PORTB & B10000000) | (LEDMAT[Mcolumn]);                  // Light LEDs - turn on ROWs

  // ---

  Mcolumn = Mcolumn+1;                                              // Prep for next column
  if(Mcolumn >19)
  {
    Mcolumn =0;
  }
}

//*******************************************************************************************************************
// 						                            Used by STII Small Desk Clock
//*******************************************************************************************************************
// Only light one ROW (and one column) ie one pixel at a time. = lower current draw, but lower refresh rate.


// Where:
// PORTC2 - S1 (ADC1)
// PORTC3 - S2 (ADC2)
// When S1=H and S2=L Decoder 1 = Selected
// When S1=L and S2=H Decoder 2 = Selected
// When S1=L and S2=L Decoder 3 = Selected
// When S1=H and S2=H None Selected = all columns are OFF

// PORTD4 - A
// PORTD5 - B
// PORTD6 - C
// PORTD7 - Free pin (only with the "Small desk clock")

// PORTB 0 to 6 = ROWS 1 to 7

void LEDupdateTHREE()                                                  // ONE ROW of selected column at a time
{  

  if(ROWBITINDEX >6)
  {
    ROWBITINDEX = 0;
    
    Mcolumn = Mcolumn+1;                                              // Prep for next column
    if(Mcolumn >19)
    {
      Mcolumn =0;
    }

//    PORTB = (PORTB & B10000000);                                    // Clear last column
    PORTB = (PORTB & B10000000);
    PORTC = (PORTC & B11110011) | B00001100;                        // Turn off decoders
    


    if(Mcolumn < 8)                                                 // Matrix column (from 0 to 7)
    {
      PORTD = (PORTD & B10001111) | (Mcolumn << 4);                 // Decode address to 74HC138
      PORTC = (PORTC & B11110011) | B00000100;                      // Select Chip 1
    }
    
    if(Mcolumn > 7 && Mcolumn < 16)
    {
      PORTD = (PORTD & B10001111) | ((Mcolumn - 8) << 4);           // Decode address to 74HC138
      PORTC = (PORTC & B11110011) | B00001000;                      // Select Chip 2
    }

    if(Mcolumn > 15)
    {
      PORTD = (PORTD & B10001111) | ((Mcolumn - 16) << 4);           // Decode address to 74HC138
      PORTC = (PORTC & B11110011);                                   // Select Chip 3
    }
  }
  else
  {
//    PORTB = (PORTB & B10000000);
    PORTB = (PORTB & B10000000);
    if(bitRead(LEDMAT[Mcolumn],ROWBITINDEX))
    {
      //      PORTB = (PORTB & B10000000);
      bitSet(PORTB,ROWBITINDEX);
    }

    ROWBITINDEX = ROWBITINDEX +1;
    
    delayMicroseconds(50);                                            // Test to see if this makes LEDs brighter
  }
}


