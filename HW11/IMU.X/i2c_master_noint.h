#ifndef I2C_MASTER_NOINT_H__
#define I2C_MASTER_NOINT_H__
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

#include <xc.h>

// Constant register values for the MCP23017 I/O Board
#define MCP23017 0b01000000 // slave address for a READ --> bitwise OR with a 1 to WRITE
#define IODIRA 0x00 
#define IODIRB 0x01 
#define IPOLA 0x02 
#define IPOLB 0x03 
#define GPINTENA 0x04 
#define GPINTENB 0x05 
#define GPPUA 0x0C 
#define GPPUB 0x0D 
#define GPIOA 0x12 
#define GPIOB 0x13 
#define OLATA 0x14 
#define OLATB 0x15

void i2c_master_setup(void); // set up I2C1 as master
void i2c_master_start(void); // send a START signal
void i2c_master_restart(void); // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void); // receive a byte of data
void i2c_master_ack(int val); // send an ACK (0) or NACK (1)
void i2c_master_stop(void); // send a stop

void setRegister(unsigned char address, unsigned char reg, unsigned char value);
unsigned char readRegister(unsigned char address, unsigned char reg);

#endif