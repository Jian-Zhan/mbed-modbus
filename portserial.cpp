/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "mbed.h"                   // Cam

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- System Variables ---------------------------------*/
Serial rs485(RS485_TX, RS485_RX);

extern UART_HandleTypeDef uart_handlers[3];
UART_HandleTypeDef *huart = &uart_handlers[2]; // USART3

DigitalOut RS485_RNE(PC_4), RS485_DE(PC_5); // Needed for MAX3485
DigitalOut LED_RNE(LED2); // Indicator LED

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    __disable_irq();
    // printf("Rx: %d, Tx: %d", xRxEnable, xTxEnable);
    if (xRxEnable) {
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
        RS485_RNE = 0;
        LED_RNE = 0;
    }
    else {
        __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
        RS485_RNE = 1;
        LED_RNE = 1;
    }

    if (xTxEnable) {
        __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
        RS485_DE = 1;
    }
    else {
        __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
        RS485_DE = 0;
    }
    // printf(" done.\n");
    __enable_irq();
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    rs485.baud(115200);
    rs485.format(8, SerialBase::None, 1);
    //rs485.abort_read();
    //rs485.abort_write();

    __disable_irq();
    rs485.attach(&prvvUARTRxISR, SerialBase::RxIrq);
    rs485.attach(&prvvUARTTxReadyISR, SerialBase::TxIrq);
    __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
    __enable_irq();
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    rs485.putc(ucByte);
    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == 0);
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
    * pucByte = rs485.getc();
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    // printf("TX ready\n");
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    // printf("RX received\n");
    pxMBFrameCBByteReceived(  );
}


