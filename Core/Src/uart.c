/*
 * uart.c
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#include "uart.h"


uint8_t msg[100];
uint8_t data[9];
uint8_t motion_data[36];
uint8_t receive_buffer1 = 0, receive_buffer2 = 0, receive_buffer3 = 0;
uint8_t check_esp = 0;
uint8_t status_but1 = 'l', status_but2 = 'l', status_d1 = 'l', status_d2 = 'l'; //load
uint8_t status_send_esp = 'l';

void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

void uart_Rs232SendString(uint8_t* str){
	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString("0");
		return;
	}
    uint8_t num_flag = 0;
    int i;
	if(num < 0) uart_Rs232SendString("-");
    for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((void*)msg,"%d",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString("0");
        }
        num %= mypow(10, i-1);
    }
}

void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((void*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString(".");
    sprintf((void*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}

void uart_init_rs485(){
	HAL_UART_Receive_IT(&huart3, data, 9);
	HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, 0);
}

void uart_init_esp(){
	HAL_UART_Receive_IT(&huart2, &receive_buffer2, 1);
	HAL_GPIO_WritePin(ESP12_PWR_GPIO_Port, ESP12_PWR_Pin, 1);
}

void uart_EspSendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart2, bytes, size, 10);
}

//void uart_EspSendString(uint8_t* str){
//	HAL_UART_Transmit(&huart2, (void*)msg, sprintf((void*)msg,"%s",str), 10);
//}
void checkByte2(uint8_t data){
	if(data == 03){
		HAL_UART_Transmit(&huart1, "Button ", 7, 100);
	}
	if(data == 04){
		HAL_UART_Transmit(&huart1, "Device ", 7, 100);
	}
}

void checkByte7(uint8_t data){
	if(data == 0x80){
		HAL_UART_Transmit(&huart1, " is ON. ", 8, 100);
	}
	if(data == 0x81){
		HAL_UART_Transmit(&huart1, " is OFF.", 8, 100);
	}
}

void statusButton1(data2, data4, data7){
	if(data2 == 03 && data4 == 01 && data7 == 0x80){
		status_but1 = 'H'; //ON
		status_send_esp = 'u';
	}
	if(data2 == 03 && data4 == 01 && data7 == 0x81){
		status_but1 = 'L'; //OFF
		status_send_esp = 'U';
	}
}

void statusButton2(data2, data4, data7){
	if(data2 == 03 && data4 == 02 && data7 == 0x80){
		status_but2 = 'H'; //ON
		status_send_esp = 'v';
	}
	if(data2 == 03 && data4 == 02 && data7 == 0x81){
		status_but2 = 'L'; //OFF
		status_send_esp = 'V';
	}
}

void statusDevice1(data2, data4, data7){
	if(data2 == 04 && data4 == 00 && data7 == 0x80){
		status_d1 = 'H'; //ON
		status_send_esp = 'm';
	}
	if(data2 == 04 && data4 == 00 && data7 == 0x81){
		status_d1 = 'L'; //OFF
		status_send_esp = 'M';
	}
}

void statusDevice2(data2, data4, data7){
	if(data2 == 04 && data4 == 01 && data7 == 0x80){
		status_d2 = 'H'; //ON
		status_send_esp ='n';
	}
	if(data2 == 04 && data4 == 01 && data7 == 0x81){
		status_d2 = 'L'; //OFF
		status_send_esp = 'N';
	}
}

void statusMotion(data0){
	if(data0 == 0x9C){
		status_send_esp = 's';
	}
}

void checkButton1(){
	if(status_but1 == 'l')
		HAL_UART_Transmit(&huart1, "Button 1 is loading...\n", 23, 100);
	if(status_but1 == 'H')
		HAL_UART_Transmit(&huart1, "Button 1 is OFF.\n", 17, 100);
	if(status_but1 == 'L')
		HAL_UART_Transmit(&huart1, "Button 1 is ON.\n", 16, 100);
}

void checkButton2(){
	if(status_but2 == 'l')
		HAL_UART_Transmit(&huart1, "Button 2 is loading...\n", 23, 100);
	if(status_but2 == 'H')
		HAL_UART_Transmit(&huart1, "Button 2 is OFF.\n", 17, 100);
	if(status_but2 == 'L')
		HAL_UART_Transmit(&huart1, "Button 2 is ON.\n", 16, 100);
}

void checkDevice1(){
	if(status_d1 == 'l')
		HAL_UART_Transmit(&huart1, "Device 1 is loading...\n", 23, 100);
	if(status_d1 == 'H')
		HAL_UART_Transmit(&huart1, "Device 1 is OFF.\n", 17, 100);
	if(status_d1 == 'L')
		HAL_UART_Transmit(&huart1, "Device 1 is ON.\n", 16, 100);
}

void checkDevice2(){
	if(status_d2 == 'l')
		HAL_UART_Transmit(&huart1, "Device 2 is loading...\n", 23, 100);
	if(status_d2 == 'H')
		HAL_UART_Transmit(&huart1, "Device 2 is OFF.\n", 17, 100);
	if(status_d2 == 'L')
		HAL_UART_Transmit(&huart1, "Device 2 is ON.\n", 16, 100);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		// rs232 isr
		// can be modified
		HAL_UART_Transmit(&huart1, &receive_buffer1, 1, 10);

		// turn on the receive interrupt
		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
	}

	if(huart->Instance == USART2){
		if(receive_buffer2 == 'O') check_esp = 1;
		else if(receive_buffer2 == 'a') light_status = 0;
		else if(receive_buffer2 == 'A') light_status = 1;
		HAL_UART_Receive_IT(&huart2, &receive_buffer2, 1);
		uart_EspSendBytes(&status_send_esp, 1);
	}

	if(huart->Instance == USART3){
        if (huart->RxXferSize == 9) {
        	HAL_UART_Receive_IT(&huart3, data, 9);
			statusButton1(data[2], data[4], data[7]);
			statusButton2(data[2], data[4], data[7]);
			statusDevice1(data[2], data[4], data[7]);
			statusDevice2(data[2], data[4], data[7]);
			statusMotion(data[0]);
	//		checkButton1();
	//		checkButton2();
	//		checkDevice1();
	//		checkDevice2();
	//		HAL_UART_Transmit(&huart1, "----------------------------------------", 50, 100);
			HAL_UART_Transmit(&huart1, data, 9, 10);
//        } else if (huart->RxXferSize == 36) {
//        	HAL_UART_Receive_IT(&huart3, motion_data, 36);
//        	status_send_esp = 's'; //sensor
        }
		uart_EspSendBytes(&status_send_esp, 1);
	}
}

uint8_t uart_EspCheck(){
	if(check_esp == 1) return 1;
	return 0;
}



