
#ifndef UART_UART_LOWLEVEL_H_
#define UART_UART_LOWLEVEL_H_

#define UART_NEWLINE 0x0A
#define UART_ASCII_PROT_START_BYTE 0x2E
#define UART_BYTE_LIMIT 32

char uart_rx_msg[96];
uint8_t uart_rx_msg_len;

char uart_tx_msg[96];
uint8_t uart_tx_byte_idx;

void UART_Init(void);

// The following message handler is called when a UART
// message completes being received i.e.  with a newline
// character
extern void UART_ReceiveMessageHandler(void);

#endif /* UART_UART_LOWLEVEL_H_ */
