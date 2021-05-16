#include "ST7789.h"
#include "spi.h"
#include "font.h"
#include <xc.h>
#include <stdio.h>
#include <math.h>
#include <sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

void progressBar(char x, char y, char length, char width, unsigned short color);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // Configure A4 as the "heartbeat" output
    LATAbits.LATA4 = 0; // A4 is initially off
    TRISBbits.TRISB4 = 1; // B4 is an input
    
    initSPI();     // initialize SPI params
    LCD_init();    // setup LCD screen

    __builtin_enable_interrupts();
    
    char message[50];
    int counter = 0;
    char tick = 0;
    char progress=0;
    int dt = 24000000;
    
    LCD_clearScreen(BACKGROUND);
    
    if (0) {
        sprintf(message,"Hello world!");
        drawString(message,28,32,WHITE);

        sprintf(message,"FPS = ");
        drawString(message,28,72,WHITE);

        sprintf(message,"Counter: ");
        drawString(message,28,48,WHITE);

        progressBar(27,58,100,8,WHITE);
    }
    
    while (1) {
        if(0) {
            _CP0_SET_COUNT(0);

            progressBar(27+progress,58,1,8,BLUE);

            sprintf(message,"%d",progress);
            drawString(message,78,48,WHITE);

            sprintf(message,"%0.2f",(float) 24000000/dt);
            drawString(message,64,72,WHITE);

            counter++;
            tick++;

            switch (tick) {case 20: tick=0; LATAbits.LATA4 = !LATAbits.LATA4;}
            //switch (counter) {case 5: progress++; counter=0;}
            progress++;

            if (progress==100) {
                progressBar(27,58,100,8,WHITE);
                progress=0;
            }

            dt = _CP0_GET_COUNT();
        } else {
            for (counter=0;counter<30;counter++) {
                _CP0_SET_COUNT(0);
                sprintf(message,"123456789012345678901234567890123456789012345678");
                drawString(message,0,counter*8,WHITE);
                dt = _CP0_GET_COUNT();
                sprintf(message,"RPS=%0.2f",(float)24000000/dt);
                drawString(message,0,8,RED);
                sprintf(message," 12345678901234567890123456789012345678");
            }
        }
    }
}

void progressBar(char x, char y, char length, char width, unsigned short color) {
    char j,k;
    for (j=0; j<length; j++) {
        for (k=0; k<width; k++) {
            LCD_drawPixel(x+j,y+k,color);
        }   
    }
}