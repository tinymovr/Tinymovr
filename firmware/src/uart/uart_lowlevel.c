
#include <src/common.h>
#include <src/uart/uart_lowlevel.h>

#define UART_ASCII_PROT_START_BYTE 0x2E
#define UART_NEWLINE 0x0A
#define UART_BYTE_LIMIT 32

SerialMessageType msg_type = MSG_TYPE_UNKNOWN;

typedef enum {
    MSG_TYPE_UNKNOWN = 0,
    MSG_TYPE_ASCII = 1,
    MSG_TYPE_BINARY = 2
} SerialMessageType;

void ResetRxQueue()
{
	uart_rx_msg_byte_count = 0;
	uart_tx_msg_byte_count = 0;
	msg_type = MSG_TYPE_UNKNOWN;
}

void USARTB_IRQHandler(void)
{
	uint8_t int_type = pac5xxx_uart_interrupt_identification2(UART_REF);
    uint8_t data = pac5xxx_uart_read2(UART_REF);

    if (int_type == UARTIIR_INTID_TX_HOLD_EMPTY)
	{
		pac5xxx_uart_write2(UART_REF, uart_tx_msg[uart_tx_msg_byte_count]);
		uart_tx_msg_byte_count++;

		// Terminate transmission upon newline or transmit overflow
        if ((uart_tx_msg[uart_tx_msg_byte_count - 1u] == UART_NEWLINE) ||
				(uart_tx_msg_byte_count > UART_BYTE_LIMIT))
		{
			// Disable transmit interrupt
	        pac5xxx_uart_int_enable_THREI2(UART_REF, UART_INT_DISABLE);
			// Enable receive data interrupt for next incoming message
			pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_ENABLE);
		  	uart_tx_msg_byte_count = 0;
		}
	}
    else
	{	
		// Check first byte or return
        if (uart_rx_msg_byte_count == 0u)
		{
        	if (data == UART_ASCII_PROT_START_BYTE)
			{
        		msg_type = MSG_TYPE_ASCII;
			}
        	else
        	{
        		msg_type = MSG_TYPE_UNKNOWN;
        	}
		}

		if (msg_type != MSG_TYPE_UNKNOWN)
		{
			// Store data in buffer and increment index
			uart_rx_msg[uart_rx_msg_byte_count] = data;
			uart_rx_msg_byte_count++;

			if ((msg_type == MSG_TYPE_ASCII) &&
				(uart_rx_msg[uart_rx_msg_byte_count - 1u] == UART_NEWLINE))
			{
				UART_ReceiveMessageHandler();
				ResetRxQueue();
				// Disable receive data interrupt
				pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_DISABLE);
				// Reset RX FIFO, to clear RDAI interrupt
				pac5xxx_uart_rx_fifo_reset2(UART_REF);
			}
			else if (uart_rx_msg_byte_count > UART_BYTE_LIMIT)
			{
				ResetRxQueue();
			}
			else
			{
				// No action
			}
		}
	}
}