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


/** Write an output to the currently selected column/decoder.
 * There are seven rows per column, mapped to bits PORTB0-6.
 * PORTB7 is used for an additional pin, so we don't touch it.
 */
static void
led_output(
	uint8_t bits
)
{
	PORTB = (PORTB & 0x80) | (bits & 0x7F);
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
	static uint8_t row_mask = 0;
	static uint8_t column = 0;

	if (row_mask != 0)
	{
		led_output(LEDMAT[column] & row_mask);
		row_mask >>= 1;
    
		// Test to see if this makes LEDs brighter
		//delayMicroseconds(50);
		return;
	}

	// We've displayed the entire row; prep for next column
	row_mask = 1 << 7;
	if (++column > 19)
		column = 0;


	// Each matrix has eight columns (from 0 to 7)
	const unsigned decoder_id = (column / 8) + 1;
	const unsigned decoder_column = column % 8;
	
	// Clear current outputs and disable the decoders while we switch
	led_output(0);
	led_decoder_select(decoder_id);
	led_column_select(decoder_column);
}
