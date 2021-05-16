#include "ws2812b.h"
#include <stdio.h>
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
    
    ws2812b_setup();

    __builtin_enable_interrupts();
    
    int numLEDs = 10;
    
    wsColor color_array[numLEDs];
    
    signed short LED1;   // initial hue of LED1
    signed short LED2;  // initial hue of LED2
    signed short LED3; // initial hue of LED3
    signed short LED4; // initial hue of LED4
    signed short LED5;
    signed short LED6;
    signed short LED7;
    signed short LED8;
    signed short LED9;
    signed short LED10;
    
    int heartbeat = 0;
    short dir=0;            // (1 or 0) clockwise/counter-clockwise path along color-wheel
    short inv=0;            // (1 or 0) clockwise/counter-clockwise path, inverts color orientation
    float cycle=3;          // cycle --> scales cycle time
    float b=.05;             // brightness 
    float gradient=1;       // gradient --> scales how close respective LED hues are together
    short cap=360*cycle;    // sets the max counter for ticking thru hues
    
    if (inv==1) {
        LED1 = 324/gradient;
        LED2 = 288/gradient;
        LED3 = 252/gradient;
        LED4 = 216/gradient;
        LED5 = 180/gradient;
        LED6 = 144/gradient;
        LED7 = 108/gradient;
        LED8 = 72/gradient;
        LED9 = 36/gradient;
        LED10 = 0;
    } else if (inv==0) {
        LED1 = 0;
        LED2 = 36/gradient;
        LED3 = 72/gradient;
        LED4 = 108/gradient;
        LED5 = 144/gradient;
        LED6 = 180/gradient;
        LED7 = 216/gradient;
        LED8 = 252/gradient;
        LED9 = 288/gradient;
        LED10 = 324/gradient;
    }     
    while (1) {        
        if (dir==1) {
            if (LED1==cap){LED1=0;}
            if (LED2==cap){LED2=0;}
            if (LED3==cap){LED3=0;}
            if (LED4==cap){LED4=0;}
            if (LED5==cap){LED5=0;}
            if (LED6==cap){LED6=0;}
            if (LED7==cap){LED7=0;}
            if (LED8==cap){LED8=0;}
            if (LED9==cap){LED9=0;}
            if (LED10==cap){LED10=0;}
            
            color_array[0] = HSBtoRGB(LED1/cycle,1,b);
            color_array[1] = HSBtoRGB(LED2/cycle,1,b);
            color_array[2] = HSBtoRGB(LED3/cycle,1,b);
            color_array[3] = HSBtoRGB(LED4/cycle,1,b);
            color_array[4] = HSBtoRGB(LED5/cycle,1,b);
            color_array[5] = HSBtoRGB(LED6/cycle,1,b);
            color_array[6] = HSBtoRGB(LED7/cycle,1,b);
            color_array[7] = HSBtoRGB(LED8/cycle,1,b);
            color_array[8] = HSBtoRGB(LED9/cycle,1,b);
            color_array[9] = HSBtoRGB(LED10/cycle,1,b);
            
            ws2812b_setColor(color_array, numLEDs);
            
            LED1++;
            LED2++;
            LED3++;
            LED4++;
            LED5++;
            LED6++;
            LED7++;
            LED8++;
            LED9++;
            LED10++;
            
        } else if (dir==0) {
            if (LED1==0){LED1=cap;}
            if (LED2==0){LED2=cap;}
            if (LED3==0){LED3=cap;}
            if (LED4==0){LED4=cap;}
            if (LED5==0){LED5=cap;}
            if (LED6==0){LED6=cap;}
            if (LED7==0){LED7=cap;}
            if (LED8==0){LED8=cap;}
            if (LED9==0){LED9=cap;}
            if (LED10==0){LED10=cap;}
            
            color_array[0] = HSBtoRGB(LED1/cycle,1,b);
            color_array[1] = HSBtoRGB(LED2/cycle,1,b);
            color_array[2] = HSBtoRGB(LED3/cycle,1,b);
            color_array[3] = HSBtoRGB(LED4/cycle,1,b);
            color_array[4] = HSBtoRGB(LED5/cycle,1,b);
            color_array[5] = HSBtoRGB(LED6/cycle,1,b);
            color_array[6] = HSBtoRGB(LED7/cycle,1,b);
            color_array[7] = HSBtoRGB(LED8/cycle,1,b);
            color_array[8] = HSBtoRGB(LED9/cycle,1,b);
            color_array[9] = HSBtoRGB(LED10/cycle,1,b);
            
            ws2812b_setColor(color_array, numLEDs);
            
            LED1--;
            LED2--;
            LED3--;
            LED4--;
            LED5--;
            LED6--;
            LED7--;
            LED8--;
            LED9--;
            LED10--;
            
        }
        
        if (heartbeat==cap/2) {
            heartbeat=0;
            LATAbits.LATA4 = !LATAbits.LATA4;
        }
        
        heartbeat++;
    }
}