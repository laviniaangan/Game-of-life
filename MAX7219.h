#define MAX7219

void SPI_Write_Byte(unsigned short n)
{
    unsigned short t, Mask, Flag;
    CLK_Pin = 0;      // Set CLK_Pin to 0, initializing the clock for SPI.
    Mask = 128;       // Initialize Mask with the value 128 (binary 10000000).

    for (t = 0; t < 8; t++) {  // Loop through each of the 8 bits in 'n'.
        Flag = n & Mask;      // Extract the current bit from 'n' and store it in 'Flag'.

        if (Flag == 0) {      // If the current bit is 0:
            DIN_Pin = 0;      // Set the data pin (DIN_Pin) to 0.
        } else {              // Otherwise (the current bit is 1):
            DIN_Pin = 1;      // Set the data pin (DIN_Pin) to 1.
        }

        CLK_Pin = 1;      // Set the clock pin (CLK_Pin) to 1.
        CLK_Pin = 0;      // Set the clock pin (CLK_Pin) back to 0, possibly to signal data transmission.

        Mask = Mask >> 1; // Right-shift the Mask to prepare for the next bit.
    }
}



void MAX7219_INIT() {

// Set BCD decode mode
CS_Pin = 0; // CS pin is pulled LOW
SPI_Write_Byte(0x09); // Select Decode Mode register
SPI_Write_Byte(0x00); // Select BCD mode for digits DIG0-DIG7
CS_Pin = 1; // CS pin is pulled HIGH

// Set display brighness
CS_Pin = 0; // CS pin is pulled LOW
SPI_Write_Byte(0x0A); // Select Intensity register
SPI_Write_Byte(0x05); // Set brightness
CS_Pin = 1; // CS pin is pulled HIGH

// Set display refresh
CS_Pin = 0; // CS pin is pulled LOW
SPI_Write_Byte(0x0B); // Select Scan-Limit register
SPI_Write_Byte(0x07); // Select digits DIG0-DIG3
CS_Pin = 1; // CS pin is pulled HIGH

// Turn on the display
CS_Pin = 0; // CS pin is pulled LOW
SPI_Write_Byte (0x0C);
SPI_Write_Byte (0x01);
CS_Pin = 1; // CS pin is pulled HIGH

// Disable Display-Test
CS_Pin = 0; // CS pin is pulled LOW
SPI_Write_Byte(0x0F); // Select Display-Test register
SPI_Write_Byte(0x00); // Disable Display-Test
CS_Pin = 1; // CS pin is pulled HIGH

}