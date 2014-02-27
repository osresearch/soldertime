 /** \file
 * Matrix driver for the Solder:Time Desk Clock.
 *
 * Called by Timer 1 Interrupt to draw next column in LED matrix
 *
 */


// Store a padded version of the array to avoid multiplies
static volatile uint8_t led_matrix[WIDTH][8];

void
led_draw(
	uint8_t col,
	uint8_t row,
	uint8_t bright
)
{
	led_matrix[col][row] = bright;
}


void
led_draw_col(
	uint8_t col,
	uint8_t bits,
	uint8_t bright
)
{
	for (uint8_t row=0, mask=1; row < 7 ; row++, mask <<=1)
	{
		uint8_t bit = bits & mask;
		led_draw(col, row, bit ? bright : 0);
	}
}


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
	static uint8_t row = 0;
	static uint8_t column = 0;

	if (row_mask != 0)
	{
		// Read the pixel data for the current column and output
		// the one pixel that is currently high.
		row_mask >>= 1;
		uint8_t bright = led_matrix[column][--row];
#define reverse_video 0
		if (reverse_video)
			bright = 0xFF - bright;

		if (bright)
			led_output(row_mask);
		else
			led_output(0);

    
		// hold the LED on for a scaled period of time
		// (with interrupts disabled, since this is in
		// an ISR) and then turn it off.
		// This is lame, but avoids glitching brightness.
		for (uint8_t i = 0; i < bright ; i++)
		{
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
		}
		led_output(0);
		for (uint8_t i = bright; i != 0 ; i++)
		{
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
		}

		return;
	}

	// Turn off before we switch columns and decoders
	led_output(0);

	// We've displayed the entire row; prep for next column
	row_mask = 0x80;
	row = 7;

	if (++column > 19)
		column = 0;

	// Each matrix has eight columns (from 0 to 7)
	const unsigned decoder_id = (column / 8) + 1;
	const unsigned decoder_column = column % 8;
	
	led_decoder_select(decoder_id);
	led_column_select(decoder_column);

}
