/*********************************************
Project : Led Matrix Display
**********************************************
Chip type: ATmega164A
Clock frequency: 20 MHz
Compilers:  CVAVR 2.x
*********************************************/

#include <mega164a.h>
#include <stdio.h>
#include <delay.h>  
#include <string.h> 
#include <stdlib.h>
#include "defs.h"    
#include <stdint.h>

#define MODULES 4
#define ROWS 8
#define COLS 8

unsigned char grid[MODULES][ROWS][COLS]; // Matrice 3D pentru a reprezenta grila "Game of life"

void SPI_Write_Byte(unsigned short num)
{
    unsigned short t, Mask, Flag;
    CLK_Pin = 0;
    Mask = 128;
    for (t=0; t<8; t++) { 
        Flag = num & Mask; 
        if(Flag == 0) { 
            DIN_Pin = 0; 
        } else { 
            DIN_Pin = 1; 
            } 
        CLK_Pin = 1; 
        CLK_Pin = 0; 
        Mask = Mask >> 1;
    }
}

void MAX7219_INIT() {
unsigned char module;
    for ( module = 0; module < MODULES; module++) {
        CS_Pin = 0;
        SPI_Write_Byte(0x09); //Registrul de mod de decodare
        SPI_Write_Byte(0x00); // Dezactiveazã decodarea BCD
        CS_Pin = 1;

        CS_Pin = 0;
        SPI_Write_Byte(0x0A);// Registrul de intensitate
        SPI_Write_Byte(0x05); // Seteazã intensitatea la un nivel mediu
        CS_Pin = 1;

        CS_Pin = 0;
        SPI_Write_Byte(0x0B); //Registrul de limitã de scanare
        SPI_Write_Byte(0x07);// Activeazã toate cele 8 cifre
        CS_Pin = 1;

        CS_Pin = 0;
        SPI_Write_Byte(0x0C);// Registrul de pornire
        SPI_Write_Byte(0x01);// Porneste afisajul
        CS_Pin = 1;

        CS_Pin = 0;
        SPI_Write_Byte(0x0F); //Registrul de test
        SPI_Write_Byte(0x00); // Dezactiveazã testul de afi?aj
        CS_Pin = 1;
    }
}

void Write_Byte(unsigned char module, unsigned short myColumn, unsigned short myValue)
{   unsigned char m;
    CS_Pin = 0; // selecteaza max7219.
    for ( m = 0; m < MODULES; m++) {
        if (m == module) {
            SPI_Write_Byte(myColumn); // trimite valoarea myColumn catre max7219 
            SPI_Write_Byte(myValue); // trimite valoarea myValue catre max7219
        } else {
            SPI_Write_Byte(0x00); 
            SPI_Write_Byte(0x00); 
        }
    }
    CS_Pin = 1; // deselecteaza max7219.
}

void Clear_Matrix(void)
{    unsigned char module;
     unsigned short x;
    for ( module = 0; module < MODULES; module++) {
        for ( x = 1; x < 9; x++) {
            Write_Byte(module, x, 0x00);
        }
    }
}

void Set_LED(unsigned char module, unsigned char col, unsigned char row, unsigned char state) {
    unsigned char ledValue = 1 << (31 - col); // Generam valoarea corecta pentru coloana dorita
    if (state) {
        Write_Byte(module, row + 1, ledValue); // 'row + 1' pentru ca rândurile în functie de MAX7219 încep de la 1
    } else {
        Write_Byte(module, row + 1, 0x00); // Stinge LED-ul în pozitia respectiva
    }
}

// Functie pentru actualizarea grilei de joc Game of Life bazata pe reguli
void updateGrid() {
    unsigned char newGrid[MODULES][ROWS][COLS]; 
    unsigned char i, j, module;

    for (module = 0; module < MODULES; module++) {
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++) {
                unsigned char neighbors = 0;  
                char x, y, newX, newY;

                // Numara vecinii care sunt vii
                for (x = -1; x <= 1; x++) {
                    for (y = -1; y <= 1; y++) {
                        if (x == 0 && y == 0)
                            continue;

                        newX = i + x;
                        newY = j + y;

                        // Gestioneazã marginile grilei
                        if (newX < 0)
                            newX = ROWS - 1;
                        else if (newX >= ROWS)
                            newX = 0;

                        if (newY < 0)
                            newY = COLS - 1;
                        else if (newY >= COLS)
                            newY = 0;

                        if (grid[module][newX][newY] == 1)
                            neighbors++;
                    }
                }

                // Aplicam regulile jocului Game of Life 
                if (grid[module][i][j] == 1) {
                    if (neighbors < 2 || neighbors > 3)
                        newGrid[module][i][j] = 0; // Celula moare
                    else
                        newGrid[module][i][j] = 1; // Celula supravietuieste
                } else {
                    if (neighbors == 3)
                        newGrid[module][i][j] = 1; // Celula invie
                    else
                        newGrid[module][i][j] = 0; // Celula ramane moarta
                }
            }
        }
    }

    // Actualizeazã grila originalã cu newGrid
    for (module = 0; module < MODULES; module++) {
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++) {
                grid[module][i][j] = newGrid[module][i][j];
            }
        }
    }
}

void updateDisplayFromGrid() {
    unsigned char row, col, module;
    for (module = 0; module < MODULES; module++) {
        for (row = 0; row < ROWS; row++) {
            unsigned char rowData = 0;
            for (col = 0; col < COLS; col++) {
                if (grid[module][row][col])
                    rowData |= (1 << col);
            }
            Write_Byte(module, row + 1, rowData);
        }
    }
}

void initializeGridRandomly() {
    unsigned char i, j, module;
    for (module = 0; module < MODULES; module++) {
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++) {
                grid[module][i][j] = rand() % 2; // Initializare random cu 0 sau 1
            }
        }
    }
}

void main() {
    DDRB = 0xFF;

    MAX7219_INIT();
    Clear_Matrix();

    // Initializeaza grila cu valori random
    initializeGridRandomly();

    while (1) {
        // Apeleaza functia pentru a modifica grila
        updateGrid();
        // Actualizeaza afisajul MAX7219 cu noua grila
        updateDisplayFromGrid();
        delay_ms(2000); // Timp de intarziere intre generari
    }
}
