
char uart_rx_msg[48];
uint8_t uart_rx_msg_byte_count;

char uart_tx_msg[48];
uint8_t uart_tx_msg_byte_count;

// The following message handler is called when a UART
// message completes being received i.e.  with a newline
// character
extern void UART_ReceiveMessageHandler(void);