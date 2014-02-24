 /** \file
 * Matrix driver for the Solder:Time Desk Clock.
 *
 * Called by Timer 1 Interrupt to draw next column in LED matrix
 *
 */

/** Select a decoder.
 * 0 == none, otherwise  1 through 3
 *
 * PORTC2 - S1 (ADC1)
 * PORTC3 - S2 (ADC2)
 * When S1=H and S2=L Decoder 1 = Selected
 * When S1=L and S2=H Decoder 2 = Selected
 * When S1=L and S2=L Decoder 3 = Selected
 * When S1=H and S2=H None Selected = all columns are OFF
 */
static void
led_decoder_select(
	unsigned decoder
)
{
	uint8_t bits = B1100; // default to off

	if (decoder == 1)
		bits = B0100;
	else
	if (decoder == 2)
		bits = B1000;
	else
	if (decoder == 3)
		bits = B0000;

	PORTC = (PORTC & B11110011) | bits;
}


/** Select an output column.
 * Valid vlaues are 0 - 8.
 */
static void
led_column_select(
	unsigned column
)
{
	column &= 0x7;
	PORTD = (PORTD & B10001111) | (column << 4);
}


/** Clear all seven outputs values from the current matrix.
 * PORTB8 is used for an additional pin, so we don't touch it.
 */
static void
led_clear(void)
{
	PORTB &= B10000000;
}



/*
 * Used by STII Small Desk Clock.
 *
 * Only light one ROW (and one column) ie one pixel at a time. 
 * lower current draw, but lower refresh rate.
 *
 * Where:
 * PORTC2 - S1 (ADC1)
 * PORTC3 - S2 (ADC2)
 * When S1=H and S2=L Decoder 1 = Selected
 * When S1=L and S2=H Decoder 2 = Selected
 * When S1=L and S2=L Decoder 3 = Selected
 * When S1=H and S2=H None Selected = all columns are OFF
 * PORTD4 - A
 * PORTD5 - B
 * PORTD6 - C
 * PORTD7 - Free pin (only with the "Small desk clock")
 * PORTB 0 to 6 = ROWS 1 to 7
 */
void
LEDupdateTHREE()
{  
	if (ROWBITINDEX < 7)
	{
		led_clear();

		if (bitRead(LEDMAT[Mcolumn], ROWBITINDEX))
			bitSet(PORTB, ROWBITINDEX);

		ROWBITINDEX++;
    
		// Test to see if this makes LEDs brighter
		//delayMicroseconds(50);
		return;
	}

	// We've displayed the entire row; prep for next column
	ROWBITINDEX = 0;
	if (++Mcolumn > 19)
	      Mcolumn = 0;

	// Clear current outputs and disable the decoders while we switch
	led_clear();
	led_decoder_select(0);

	// Each matrix has eight columns (from 0 to 7)
	const unsigned decoder = (Mcolumn / 8) + 1;
	const unsigned column = Mcolumn % 8;
	
	led_column_select(column);
	led_decoder_select(decoder);
}
