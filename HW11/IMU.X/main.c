#include "font.h"
#include "i2c_master_noint.h"
#include "st7789.h"
#include "lsm6ds33.h"
#include "spi.h"
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

void yline(unsigned short x, unsigned short y, unsigned short color);
void yline(unsigned short x, unsigned short y, unsigned short color);
void xBar(signed short Xaxis_data, unsigned short color);
void yBar(signed short Yaxis_data, unsigned short color);

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
    
    i2c_master_setup(); // enable I2C communication
    imu_setup();        // setup accelerometer, gyroscope, and reading from multiple registers in a row
    
    initSPI();
    LCD_init();         // initialize LCD screen 

    __builtin_enable_interrupts();
    
    char message[48];
    int dt=24000000;
    char x,y;
    char HBcounter=0;
   
    signed short IMUdata[7];
    
    LCD_clearScreen(BACKGROUND);
    
    for(x=119;x<=121;x++){for(y=119;y<=121;y++){
        
            LCD_drawPixel(x,y,MAGENTA);}
    
    }
    
    if(0) {
        sprintf(message,"FPS = ");
        drawString(message,15,225,WHITE);

        sprintf(message,"T =      C");
        drawString(message,15,16,YELLOW);

        sprintf(message,"a[X] = ");
        drawString(message,15,32,RED);

        sprintf(message,"a[Y] = ");
        drawString(message,15,48,GREEN);

        sprintf(message,"a[Z] = ");
        drawString(message,15,64,BLUE);
    } else {
        sprintf(message,"FPS = ");
        drawString(message,15,225,WHITE);

        sprintf(message,"a[X] =       g");
        drawString(message,15,15,RED);

        sprintf(message,"a[Y] =       g");
        drawString(message,155,15,GREEN);
    }

    while(1) {
        _CP0_SET_COUNT(0);
        
        sprintf(message,"%0.2f ",(float) 24000000/dt);
        drawString(message,45,225,WHITE);
        
        imu_read(IMU_ADDR,OUT_TEMP_L,IMUdata,7);

        if (0) {
            sprintf(message,"%0.1f",0.0038149302*(IMUdata[0])+25);
            drawString(message,35,16,YELLOW);
            sprintf(message,"%d   ",IMUdata[4]);
            drawString(message,50,32,RED);
            sprintf(message,"%d   ",IMUdata[5]);
            drawString(message,50,48,GREEN);
            sprintf(message,"%d   ",IMUdata[6]);
            drawString(message,50,64,BLUE);
        } else {
            sprintf(message,"%0.2f ",(float)IMUdata[4]/16384.0);
            drawString(message,50,15,RED);
            sprintf(message,"%0.2f ",(float)IMUdata[5]/16384.0);
            drawString(message,190,15,GREEN);
            xBar(IMUdata[5],MAGENTA);
            yBar(IMUdata[4],MAGENTA);
        }
        
        switch (HBcounter) {
            case 10:
                LATAbits.LATA4 = !LATAbits.LATA4;
                HBcounter=0;
        }
        
        HBcounter++;
        
        dt=_CP0_GET_COUNT();
    }
}

void xline(unsigned short x, unsigned short y, unsigned short color) {
    int k;
    for (k=0;k<3;k++) {
        LCD_drawPixel(x,y-1+k,color);
    }
}

void yline(unsigned short x, unsigned short y, unsigned short color) {
    int k;
    for (k=0;k<3;k++) {
        LCD_drawPixel(x-1+k,y,color);
    }
}

void yBar(signed short Yaxis_data, unsigned short color) {
    int i,y;
    y = (int)((float)(Yaxis_data) / 16384.0 * 120.0 + 120.0);
    if (y<119) {
        for (i=0;i<=y;i++) {yline(120,i,BACKGROUND);}
        for (i=y;i<119;i++) {yline(120,i,color);}
        for (i=122;i<=240;i++) {yline(120,i,BACKGROUND);}
    } else if (y>121) {
        for (i=0;i<119;i++) {yline(120,i,BACKGROUND);}
        for (i=122;i<=y;i++) {yline(120,i,color);}
        for (i=y+1;i<=240;i++) {yline(120,i,BACKGROUND);}
    }
}

void xBar(signed short Xaxis_data, unsigned short color) {
    int i,x;
    x = (int)((float)(Xaxis_data) / 16384.0 * 120.0 + 120.0);
    if (x<119) {
        for (i=0;i<=x;i++) {xline(i,120,BACKGROUND);}
        for (i=x;i<119;i++) {xline(i,120,color);}
        for (i=122;i<=240;i++) {xline(i,120,BACKGROUND);}
    } else if (x>121) {
        for (i=0;i<119;i++) {xline(i,120,BACKGROUND);}
        for (i=122;i<=x;i++) {xline(i,120,color);}
        for (i=x+1;i<=240;i++) {xline(i,120,BACKGROUND);}
    }
}
