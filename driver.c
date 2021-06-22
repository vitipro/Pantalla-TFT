//FIB, DSBM, Enric X. Martin Rull, March 2016

#include <wiringPi.h>
#include <stdio.h>
#include "ascii5x7.h"
#include <stdlib.h>
#include <math.h>

//Connections, TFT pin = board pin

#define Reset_Pin   6
#define CS_Pin      10
#define SCLK_Pin    14
#define SDO_Pin     13
#define SDI_Pin     12
#define Orig_X      0
#define Orig_Y      0
#define Size_X      240
#define Size_Y      320

//TFT Interface Functions

void Config_Pins()
{
    wiringPiSetup();
    pinMode(CS_Pin, OUTPUT);
    pinMode(Reset_Pin, OUTPUT);
    pinMode(SCLK_Pin, OUTPUT);
    pinMode(SDI_Pin, OUTPUT);
    pinMode(SDO_Pin, INPUT);

    /** Initial values  **/
    digitalWrite(CS_Pin, HIGH);
    digitalWrite(Reset_Pin, HIGH);
    digitalWrite(SCLK_Pin, LOW);
    delay(200);     //200 ms o 0.2s
}

/** NI IDEA DE COM PASSAR LA FUNCIÓ A C
 * def Free_Pins():
 *	GPIO.cleanup()
 *
 */

void CS_TFT(int value)
{
    digitalWrite(CS_Pin, value);
}

void Reset_TFT(int value)
{
    digitalWrite(Reset_Pin, value);
}

void Clock_SPI()
{
    digitalWrite(SCLK_Pin, LOW);
    digitalWrite(SCLK_Pin, HIGH);
}

void Send_SPI(int value)
{
    digitalWrite(SDI_Pin, value);
    Clock_SPI();
}

int Recv_SPI()
{
    Clock_SPI();
    return digitalRead(SDO_Pin);
}

void Send_SPI_8(int value)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        if (value & 0x80) Send_SPI(1);
        else Send_SPI(0);
        value = value << 1;
    }
}


//SPI Level Functions

#define SPI_START 0x70
#define SPI_RD 0x01
#define SPI_WR 0x00
#define SPI_DATA 0x02
#define SPI_INDEX 0x00

void Write_SPI_TFT_Cmd(int reg)
{
    //Reg is 8 bit
    CS_TFT(0);
    //Send Start,Write,Index
    Send_SPI_8(0x70);
    //Send the value
    Send_SPI_8(reg);
    CS_TFT(1);
}

void Write_SPI_TFT_Dat(int value)
{
    //value is 16 bit
    CS_TFT(0);
    //Send Start,Write,Data
    Send_SPI_8(0x72);
    //Send the value
    Send_SPI_8(value>>8);
    Send_SPI_8(value);
    CS_TFT(1);
}

void Write_SPI_TFT_Reg(int reg, int value)
{
    //Sets a value to a reg
    Write_SPI_TFT_Cmd(reg);
    Write_SPI_TFT_Dat(value);
}

int Read_SPI_TFT_Reg(int reg)
{
    //Sets a value to a reg
    Write_SPI_TFT_Cmd(reg);
    //Caldria llegir 16 bits
    return(0x0000);
}


//TFT Level Functions
void SPI_TFT_Reset()
{
    //Reset the TFT
    Reset_TFT(0);
    delay(250);
    Reset_TFT(1);
    delay(6);

    int driverCode = Read_SPI_TFT_Reg(0x00);

    //Start Initial Sequence
    Write_SPI_TFT_Reg(0xEA, 0x0000);    //Reset Power Control 1
    Write_SPI_TFT_Reg(0xEB, 0x0020);   //Power Control 2
    Write_SPI_TFT_Reg(0xEC, 0x000C);    //Power Control 3
    Write_SPI_TFT_Reg(0xED, 0x00C4);    //Power Control 4
    Write_SPI_TFT_Reg(0xE8, 0x0040);    //Source OPON_N
    Write_SPI_TFT_Reg(0xE9, 0x0038);    //Source OPON_I
    Write_SPI_TFT_Reg(0xF1, 0x0001);
    Write_SPI_TFT_Reg(0xF2, 0x0010);
    Write_SPI_TFT_Reg(0x27, 0x00A3);    //Display Control 2

    // Power On sequence
    Write_SPI_TFT_Reg(0x1B, 0x001B);    //Power Control 2
    Write_SPI_TFT_Reg(0x1A, 0x0001);    //Power Control 1
    Write_SPI_TFT_Reg(0x24, 0x002F);    //Vcom Control 2
    Write_SPI_TFT_Reg(0x25, 0x0057);    //Vcom Control 3
    Write_SPI_TFT_Reg(0x23, 0x008D);    //Vcom Control 1

    // Gamma settings
    Write_SPI_TFT_Reg(0x40,0x00);
    Write_SPI_TFT_Reg(0x41,0x00);
    Write_SPI_TFT_Reg(0x42,0x01);
    Write_SPI_TFT_Reg(0x43,0x13);
    Write_SPI_TFT_Reg(0x44,0x10);
    Write_SPI_TFT_Reg(0x45,0x26);
    Write_SPI_TFT_Reg(0x46,0x08);
    Write_SPI_TFT_Reg(0x47,0x51);
    Write_SPI_TFT_Reg(0x48,0x02);
    Write_SPI_TFT_Reg(0x49,0x12);
    Write_SPI_TFT_Reg(0x4A,0x18);
    Write_SPI_TFT_Reg(0x4B,0x19);
    Write_SPI_TFT_Reg(0x4C,0x14);
    Write_SPI_TFT_Reg(0x50,0x19);
    Write_SPI_TFT_Reg(0x51,0x2F);
    Write_SPI_TFT_Reg(0x52,0x2C);
    Write_SPI_TFT_Reg(0x53,0x3E);
    Write_SPI_TFT_Reg(0x54,0x3F);
    Write_SPI_TFT_Reg(0x55,0x3F);
    Write_SPI_TFT_Reg(0x56,0x2E);
    Write_SPI_TFT_Reg(0x57,0x77);
    Write_SPI_TFT_Reg(0x58,0x0B);
    Write_SPI_TFT_Reg(0x59,0x06);
    Write_SPI_TFT_Reg(0x5A,0x07);
    Write_SPI_TFT_Reg(0x5B,0x0D);
    Write_SPI_TFT_Reg(0x5C,0x1D);
    Write_SPI_TFT_Reg(0x5D,0xCC);

    //Power + Osc
    Write_SPI_TFT_Reg(0x18,0x0036);      //OSC Control 1
    Write_SPI_TFT_Reg(0x19,0x0001);      //OSC Control 2
    Write_SPI_TFT_Reg(0x01,0x0000);      //Display Mode Control
    Write_SPI_TFT_Reg(0x1F,0x0088);      //Power Control 6
    delay(5);
    Write_SPI_TFT_Reg(0x1F,0x0080);      //Power Control 6
    delay(5);
    Write_SPI_TFT_Reg(0x1F,0x0090);      //Power Control 6
    delay(5);
    Write_SPI_TFT_Reg(0x1F,0x00D0);      //Power Control 6
    delay(5);

    //Appearance
    Write_SPI_TFT_Reg(0x17,0x0005);      //Colmod 16Bit/Pixel
    Write_SPI_TFT_Reg(0x36,0x0000);      //Panel Characteristic
    Write_SPI_TFT_Reg(0x28,0x0038);      //Display Control 3
    delay(40);
    Write_SPI_TFT_Reg(0x28,0x003C);      //Display Control 3
    delay(2);
    Write_SPI_TFT_Reg(0x16,0x0008);	    //Orientation
    Write_SPI_TFT_Reg(0x03,(Orig_X>>0)); //Set Dimensions
    Write_SPI_TFT_Reg(0x02,(Orig_X>>8));
    Write_SPI_TFT_Reg(0x05,((Size_X-1)>>0));
    Write_SPI_TFT_Reg(0x04,((Size_X-1)>>8));
    Write_SPI_TFT_Reg(0x07,(Orig_Y>>0));
    Write_SPI_TFT_Reg(0x06,(Orig_Y>>8));
    Write_SPI_TFT_Reg(0x09,((Size_Y-1)>>0));
    Write_SPI_TFT_Reg(0x08,((Size_Y-1)>>8));
}

void SPI_TFT_pixel(int x, int y, int color)
{
    //Sets a Pixel X,Y to a Color
    Write_SPI_TFT_Reg(0x03,(x>>0));
    Write_SPI_TFT_Reg(0x02,(x>>8));
    Write_SPI_TFT_Reg(0x07,(y>>0));
    Write_SPI_TFT_Reg(0x06,(y>>8));
    Write_SPI_TFT_Cmd(0x22);
    Write_SPI_TFT_Dat(color);
}

void setBackground(int color)
{
    for (int i = 0; i < Size_X; ++i) {
        for (int j = 0; j < Size_Y; ++j) SPI_TFT_pixel(i, j, color);
    }
}

void printRectangle(int x1, int x2, int y1, int y2, int color)
{
    for (int i = x1; i < x2; ++i) {
        for (int j = y1; j < y2; ++j) SPI_TFT_pixel(i, j, color);
    }
}

void printVerticalLines(int color1, int color2)
{
    int i = 0, print1 = 1;
    int numLines = 8;
    for (int line = 0; line < numLines; ++line) {
        if (print1) {
            printRectangle(0, Size_X, i, i + 39, color1);
            print1 = 0;
        }
        else if (!print1) {
            printRectangle(0, Size_X, i, i + 39, color2);
            print1 = 1;
        }
        i += 40;
    }
}

void printCharacter(int initPos, int posX, int posY, int color)
{
    char value, mask = 0b00000001;
    int X = posX, Y = posY;
    for (int pos = initPos; pos < initPos + 5; ++pos) {
        value = Font5x7[pos];
        for (int bit = 0; bit < 8; ++bit) {
            char printPixel = value & mask;
            if (printPixel) SPI_TFT_pixel(Size_X - X, Y, color);
            mask *= 2;
            ++X;
        }
        mask = 0b00000001;
        ++Y;
        X = posX;
    }
}

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

int readImage(const char *fileName,byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel)
{
        //Open the file for reading in binary mode
        FILE *imageFile = fopen(fileName, "rb");
        //Read data offset
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        int ret = fread(&dataOffset, 4, 1, imageFile);
        //Read width
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        ret = fread(width, 4, 1, imageFile);
        //Read height
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        ret = fread(height, 4, 1, imageFile);
        //Read bits per pixel
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        ret = fread(&bitsPerPixel, 2, 1, imageFile);
        //Allocate a pixel array
        *bytesPerPixel = ((int32)bitsPerPixel) / 8;

        //Rows are stored bottom-up
        //Each row is padded to be a multiple of 4 bytes.
        //We calculate the padded row size in bytes
        int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);
        //We are not interested in the padded bytes, so we allocate memory just for
        //the pixel data
        int unpaddedRowSize = (*width)*(*bytesPerPixel);
        //Total size of the pixel data in bytes
        int totalSize = unpaddedRowSize*(*height);
        *pixels = (byte*)malloc(totalSize);
        //Read the pixel data Row by Row.
        //Data is padded and stored bottom-up
        int i = 0;
        //point to the last row of our pixel array (unpadded)
        byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
        for (i = 0; i < *height; i++)
        {
                //put file cursor in the next row from top to bottom
	        fseek(imageFile, dataOffset+(i*paddedRowSize), SEEK_SET);
	        //read only unpaddedRowSize bytes (we can ignore the padding bytes)
	        ret = fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
	        //point to the next row (from bottom to top)
	        currentRowPointer -= unpaddedRowSize;
        }

        fclose(imageFile);
	return ret;
}

int main(int argc, char *argv[])
{
    Config_Pins();
    SPI_TFT_Reset();
    setBackground(0x0000);

    // Test 1 - Print a set of pixels with a certain color
    /*
       setBackground(0x0000);                                    // sets the background to black
       printRectangle(100, 200, 80, 160, 0xFFFF);                // prints a white rectangle with vertices at x = 100, 200 and y = 80, 160
    */

    // Test 2 - Print the screen with vertical alternative lines of 2 colors
    // printVerticalLines(0x4528, 0xFFFF);       // green and white

    // Test 3 - Print a character with a certain color
    /*
       setBackground(0x0000);
       int initPos = 15;                               // initial position of the character in Font5x7
       printCharacter(initPos, 10, 10, 0xFFFF);        // white '#'
    */

    // Test 4 - Print a phrase
    /*
       setBackground(0x0000);
       int numChars = 15;
       int posX = 10, posY = 10;
       int characters[15] = {40, 69, 76, 76, 79, 0, 55, 79, 82, 76, 68, 1, 0, 26, 36};
       for (int i = 0; i < numChars; ++i) {
           printCharacter(characters[i] * 5, posX, posY, 0xFFFF);    // "Hello World! :D" in white
           posY += 6;
       }
    */

    // Test 5 - Display an image 1
    /*
       byte *pixels;
       int32 width, height, bytesPerPixel;
       readImage("tiger.bmp", &pixels, &width, &height, &bytesPerPixel);       // tiger image
       int index = 0;
       for (int i = 0; i < Size_X; ++i) {
           for (int j = 0; j < Size_Y; ++j) {
               int R = pixels[index + 2];
               int G = pixels[index + 1];
               int B = pixels[index];		                                     // in bitmaps, R and B are swapped
               int pixelColor = (((R & 0xF8) << 8) + ((G & 0xFC) << 3) + (B >> 3));  // RGB888 to RGB565
               SPI_TFT_pixel(-1 * (i - Size_X - 1), j, pixelColor);                  // this is necessary because the image is stored upside-down
               index += 3;
           }
       }
    */

    // Test 6 - Display an image 2
    /*
       byte *pixels;
       int32 width, height, bytesPerPixel;
       readImage("house.bmp", &pixels, &width, &height, &bytesPerPixel);        // house image
       int index = 0;
       for (int i = 0; i < Size_X; ++i) {
           for (int j = 0; j < Size_Y; ++j) {
               int R = pixels[index + 2];
               int G = pixels[index + 1];
               int B = pixels[index];                                                // in bitmaps, R and B are swapped
               int pixelColor = (((R & 0xF8) << 8) + ((G & 0xFC) << 3) + (B >> 3));  // RGB888 to RGB565
               SPI_TFT_pixel(-1 * (i - Size_X - 1), j, pixelColor);                  // this is necessary because the image is stored upside-down
               index += 3;
           }
       }
    */

}
