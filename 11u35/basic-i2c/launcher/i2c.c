/*
 * @brief I2C example.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <stdlib.h>
#include <string.h>
#include "board.h"

// ---------------------------------------------------------
// C-Forth interconnection.
// ---------------------------------------------------------

static volatile uint32_t tick_cnt;

// There must be a matching forth structure for this.
typedef struct {
	volatile uint32_t *ticks;
} tSharedData;


// The address of this is given to the app launcher.
const tSharedData theshareddata = 
 	{ &tick_cnt };

const uint32_t OscRateIn = 12000000; // Required by the build system.

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define DEFAULT_I2C          I2C0

#define I2C_EEPROM_BUS       DEFAULT_I2C

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000
#define I2C_DEFAULT_SPEED    SPEED_100KHZ
#define I2C_FASTPLUS_BIT     0

#if (I2C_DEFAULT_SPEED > SPEED_400KHZ)
#undef  I2C_FASTPLUS_BIT
#define I2C_FASTPLUS_BIT IOCON_FASTI2C_EN
#endif

static I2C_ID_T i2cDev = DEFAULT_I2C;	/* Currently active I2C device */


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

// RX on 18, TX on 19.
static void Init_UART_PinMux(void) {
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, /* PIO0_18 used for RXD */
			 IOCON_FUNC1 | IOCON_MODE_INACT); 
		 
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19,  /* PIO0_19 used for TXD */
			 IOCON_FUNC1 | IOCON_MODE_INACT);
}

// The 
static void Init_I2C_PinMux(void) {
    Chip_SYSCTL_PeriphReset(RESET_I2C0);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4, IOCON_FUNC1 | I2C_FASTPLUS_BIT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5, IOCON_FUNC1 | I2C_FASTPLUS_BIT);
}

/* State machine handler for I2C0 and I2C1 */
static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	}
	else {
		Chip_I2C_SlaveStateHandler(id);
	}
}

/* Initialize the I2C bus */
static void i2c_app_init(I2C_ID_T id, int speed) {
    Init_I2C_PinMux();
  
    /* Initialize I2C */
    Chip_I2C_Init(id);
    Chip_I2C_SetClockRate(id, speed);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	SysTick Interrupt Handler
 * @return	Nothing
 * @note	Systick interrupt handler updates the button status
 */
void SysTick_Handler(void)
{
	tick_cnt++;
}

/**
 * @brief	I2C Interrupt Handler
 * @return	None
 */
void I2C_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}

const char greeting[] = "LPC11xx Boot!\r\n";

// Lets inline this for simplicity's sake
static void  LaunchUserAppNoNVIC(long unsigned int *base, long unsigned int *ic) {
	    __asm(
		"cpsid i\n"
  	  	"ldr   r2, [ %[base] , #0 ] @ Load up the stack pointer..\n"
		"mov sp, r2\n"
		"ldr r2, [ %[base], #4 ]\n"
        	"mov r0, %[ic] @ Put the RT Link in the right spot \n"
		"cpsie i\n"
		"bx  r2\n"
         	: : [base] "r" (base), [ic] "r" (ic) : "r0", "r2" );
	}

/**
 * @brief	Main program body
 * @return	int
 */
int main(void) {

    SystemCoreClockUpdate();

    // I2C Setup
    Chip_I2C_SetMasterEventHandler(i2cDev, Chip_I2C_EventHandler);
    NVIC_EnableIRQ(I2C0_IRQn);
    i2c_app_init(I2C0, I2C_DEFAULT_SPEED);

    /* Setup UART for 115.2K8N1 */
    Init_UART_PinMux();
    Chip_UART_Init(LPC_USART);
    Chip_UART_SetBaud(LPC_USART, 115200);
    Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
    Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(LPC_USART);
   
    /* Send initial messages */
    const char *p = greeting;
    while(*p) {
      int ret = Chip_UART_Send(LPC_USART, p,1);
      if ( ret ) p++;
    }
    LaunchUserAppNoNVIC( (long unsigned int *) 0x2000, &theshareddata);

}
