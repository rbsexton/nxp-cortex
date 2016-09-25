/*
 * A Hacked-down version of the NXP Uart.c demo.
 */

#include "chip.h"
#include "board.h"
#include "string.h"

const uint32_t OscRateIn = 12000000;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

const char inst1[] = "LPC11xx UART example buffers\r\n";

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

// RX on 18, TX on 19.
static void Init_UART_PinMux(void) {
 Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_18 used for RXD */
 Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_19 used for TXD */
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Main UART program body
 * @return	Always returns 1
 */
int main(void)
{
	SystemCoreClockUpdate();
	Init_UART_PinMux();

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, 115200);
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);

	/* Send initial messages */
	const char *p = inst1;
	while(*p) {
		int ret = Chip_UART_Send(LPC_USART, p,1);
		if ( ret ) p++;
	}

	while(1) { ; } 
	}

