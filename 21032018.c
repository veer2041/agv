#include "lpc17xx.h"
#include "type.h"
#include "uart.h"
#include "gpio.h"
#include "string.h"

extern volatile uint32_t UART3Count;
extern volatile uint8_t UART3Buffer[BUFSIZE];
extern volatile uint32_t UART2Count;
extern volatile uint8_t UART2Buffer[BUFSIZE];
extern volatile uint32_t UART1Count;
extern volatile uint8_t UART1Buffer[BUFSIZE];
extern volatile uint32_t UART0Count;
extern volatile uint8_t UART0Buffer[BUFSIZE];

void server(void),mux(void),delay_ms(int a),senddata6();
void stop();

unsigned char string_rec[20];
unsigned char machine_num[50];
#define LED1      P2_2
#define LED2      P2_3
#define enable_frontmotor P0_4
#define enable_backmotor P0_5
#define frwd	  P0_7
#define rvrs	  P0_6
#define left_pin P0_11
#define right_pin P0_15

#define detection (1<<8)
#define distance (1<<9)
void agvmovement(int flag,int data);
/*****************************************************************************
**   Main Function  main()
This program has been test on Keil LPC1700 board.
*****************************************************************************/

		 
signed int tp_strcmp( unsigned char *s1, unsigned char *s2)
	{
   		for (; *s1 == *s2; s1++, s2++)
      		if (*s1 == '\0')
         	return(0);
   		return((*s1 < *s2) ? -1: 1);
	}


void clear_sting(unsigned char *str, unsigned int val ){
	int i = 0 ;
     for( i = 0 ; i < val ; i++ )
     {
          str[i] = '\0' ;
     }
	 return ;
}

void initpins(){
	GPIO_PinFunction(LED1,PINSEL_FUNC_0); // Configure Pin for Gpio
	GPIO_PinDirection(LED1,OUTPUT);        /* Configure the pins as Output to blink the Leds*/
   	GPIO_PinFunction(LED2,PINSEL_FUNC_0); // Configure Pin for Gpio
    GPIO_PinDirection(LED2,OUTPUT);
	GPIO_PinFunction(enable_backmotor,PINSEL_FUNC_0);
    GPIO_PinDirection(enable_backmotor,OUTPUT);	
	GPIO_PinFunction(enable_frontmotor,PINSEL_FUNC_0);
    GPIO_PinDirection(enable_frontmotor,OUTPUT);	
	GPIO_PinFunction(frwd,PINSEL_FUNC_0);
	GPIO_PinDirection(frwd,OUTPUT);       
	GPIO_PinFunction(rvrs,PINSEL_FUNC_0);
    GPIO_PinDirection(rvrs,OUTPUT);	
	GPIO_PinFunction(right_pin,PINSEL_FUNC_0);
    GPIO_PinDirection(right_pin,OUTPUT);
	GPIO_PinFunction(left_pin,PINSEL_FUNC_0);
    GPIO_PinDirection(left_pin,OUTPUT);							   
	LPC_GPIO0->FIODIR |= (0<<detection);
	LPC_GPIO0->FIODIR |= (0<<distance);
	GPIO_PinWrite(LED1,LOW);     
  	GPIO_PinWrite(LED2,LOW);     
	
}



void destination(int data,int flag){
	int x=0,j,t;
		 
			UARTSend( 0, "Destination\r\n", sizeof("Destination\r\n"));
		//	delay_ms(2000);
		
			switch(flag){
				case 1:	   /*Forward*/
							LPC_GPIO0->FIOSET |= (1<<5);
							LPC_GPIO0->FIOSET |= (1<<7);
							while(data!=x){
			 
								if(!(LPC_GPIO0->FIOPIN & distance))
									{
										x++;
										UARTSend( 0, "read\r\n", sizeof("read\r\n"));
										delay_ms(50);
									}
								while((LPC_GPIO0->FIOPIN & detection)){
										if((LPC_GPIO0->FIOPIN & detection)){
												LPC_GPIO0->FIOCLR |= (1<<5);
												LPC_GPIO0->FIOCLR |= (1<<7);
													UARTSend( 0, "obstacle\r\n", sizeof("obstacle\r\n"));
										//	senddata6();
												while(!(LPC_GPIO0->FIOPIN & detection)){
													UARTSend( 0, "abcd\r\n", sizeof("abcd\r\n"));
													LPC_GPIO0->FIOSET |= (1<<5);
													LPC_GPIO0->FIOSET |= (1<<7);
													break;
												}
												
										}		
									
									}


									if(data==x){
												LPC_GPIO0->FIOCLR |= (1<<5);
												LPC_GPIO0->FIOCLR |= (1<<6);
												LPC_GPIO0->FIOCLR |= (1<<7);  
												break;
									}
								}
								
				case 2:	 	/*Reverse*/	
							LPC_GPIO0->FIOSET |= (1<<5);
							LPC_GPIO0->FIOSET |= (1<<6);
								UARTSend( 0, "case2\r\n", sizeof("case2\r\n"));
							while(data!=x){
//			GPIO_PinWrite(frwd,SET);
//			GPIO_PinWrite(enable_motor,SET);
		 
							if(!(LPC_GPIO0->FIOPIN & distance))
								{
									x++;
									UARTSend( 0, "read\r\n", sizeof("read\r\n"));
									delay_ms(50);
								}

								while((LPC_GPIO0->FIOPIN & detection)){
										if((LPC_GPIO0->FIOPIN & detection)){
												LPC_GPIO0->FIOCLR |= (1<<5);
												LPC_GPIO0->FIOCLR |= (1<<7);
													UARTSend( 0, "obstacle\r\n", sizeof("obstacle\r\n"));
										//	senddata6();
												while(!(LPC_GPIO0->FIOPIN & detection)){
													UARTSend( 0, "abcd\r\n", sizeof("abcd\r\n"));
													LPC_GPIO0->FIOSET |= (1<<5);
													LPC_GPIO0->FIOSET |= (1<<7);
													break;
												}
												
										}		
									
									}

								if(data==x){
											LPC_GPIO0->FIOCLR |= (1<<5);
											LPC_GPIO0->FIOCLR |= (1<<6); 
											UARTSend( 0, "\r\ncleared",sizeof("\r\ncleared"));
											stop(); 
											break;
								}
							
				//
				//					j=collision();
				//								if(j==1){
				//									agvmovement(flag,x);
				//									break;	
				//								}	
							}	
				case 3:		/*Left*/
							LPC_GPIO0->FIOSET |= (1<<5);
							LPC_GPIO0->FIOSET |= (1<<7);
							LPC_GPIO0->FIOSET |= (1<<11);
							LPC_GPIO0->FIOSET |= (1<<4);
							while(data!=x){
//			GPIO_PinWrite(frwd,SET);
//			GPIO_PinWrite(enable_motor,SET);
		 
							if(!(LPC_GPIO0->FIOPIN & distance))
								{
									x++;
									UARTSend( 0, "read\r\n", sizeof("read\r\n"));
									delay_ms(50);
								}

								while((LPC_GPIO0->FIOPIN & detection)){
										if((LPC_GPIO0->FIOPIN & detection)){
												LPC_GPIO0->FIOCLR |= (1<<5);
												LPC_GPIO0->FIOCLR |= (1<<7);
													UARTSend( 0, "obstacle\r\n", sizeof("obstacle\r\n"));
										//	senddata6();
												while(!(LPC_GPIO0->FIOPIN & detection)){
													UARTSend( 0, "abcd\r\n", sizeof("abcd\r\n"));
													LPC_GPIO0->FIOSET |= (1<<5);
													LPC_GPIO0->FIOSET |= (1<<7);
													break;
												}
												
										}		
									
									}

								if(data==x){
											LPC_GPIO0->FIOCLR |= (1<<5);
											LPC_GPIO0->FIOCLR |= (1<<7);
											LPC_GPIO0->FIOCLR |= (1<<11); 
											LPC_GPIO0->FIOCLR |= (1<<4); 
											break;
								}
							
				//
				//					j=collision();
				//								if(j==1){
				//									agvmovement(flag,x);
				//									break;	
				//								}	
							}
				case 4:		/*Right*/
							LPC_GPIO0->FIOSET |= (1<<5);
							LPC_GPIO0->FIOSET |= (1<<15);
							LPC_GPIO0->FIOSET |= (1<<7);
							LPC_GPIO0->FIOSET |= (1<<4);
							while(data!=x){
//			GPIO_PinWrite(frwd,SET);
//			GPIO_PinWrite(enable_motor,SET);
		 
							if(!(LPC_GPIO0->FIOPIN & distance))
								{
									x++;
									UARTSend( 0, "read\r\n", sizeof("read\r\n"));
									delay_ms(50);
								}

								while((LPC_GPIO0->FIOPIN & detection)){
										if((LPC_GPIO0->FIOPIN & detection)){
												LPC_GPIO0->FIOCLR |= (1<<5);
												LPC_GPIO0->FIOCLR |= (1<<7);
													UARTSend( 0, "obstacle\r\n", sizeof("obstacle\r\n"));
										//	senddata6();
												while(!(LPC_GPIO0->FIOPIN & detection)){
													UARTSend( 0, "abcd\r\n", sizeof("abcd\r\n"));
													LPC_GPIO0->FIOSET |= (1<<5);
													LPC_GPIO0->FIOSET |= (1<<7);
													break;
												}
												
										}		
									
									}
								if(data==x){
											LPC_GPIO0->FIOCLR |= (1<<5);
											LPC_GPIO0->FIOCLR |= (1<<15); 
											LPC_GPIO0->FIOCLR |= (1<<7);
											LPC_GPIO0->FIOCLR |= (1<<4); 
											break;
								}
							
				//
				//					j=collision();
				//								if(j==1){
				//									agvmovement(flag,x);
				//									break;	
				//								}	
							}
				default : 				stop();
			}
			return;
}


void stop(){
	GPIO_PinWrite(frwd,LOW);
	GPIO_PinWrite(enable_backmotor,LOW);    
	GPIO_PinWrite(enable_frontmotor,LOW);
	GPIO_PinWrite(rvrs,LOW);   
	GPIO_PinWrite(LED1,LOW);   
	GPIO_PinWrite(LED2,LOW); 
	GPIO_PinWrite(right_pin,LOW); 
	GPIO_PinWrite(left_pin,LOW);    
	UARTSend( 0, "stop function\r\n", sizeof("stop function\r\n")); 
	return;
}

void forward(int a,int flag){
//	LPC_GPIO0->FIOCLR |= (1<<6);
//	LPC_GPIO0->FIOCLR |= (1<<7);
//	GPIO_PinWrite(frwd,LOW); 
//	GPIO_PinWrite(enable_motor,LOW); 
	stop(); 
	UARTSend( 0, "forward\r\n", sizeof("forward\r\n"));
	delay_ms(200);
//	LPC_GPIO0->FIOSET |= (1<<6);
//	LPC_GPIO0->FIOSET |= (1<<7);
	delay_ms(100); 
	destination(a,flag);
	UARTSend( 0, "stopped\r\n", sizeof("stopped\r\n"));	
	return;
}

void reverse(unsigned int a,int flag){
//	GPIO_PinWrite(rvrs,LOW); 
//	GPIO_PinWrite(enable_motor,LOW);
	stop();
	UARTSend( 0, "Reverse\r\n", sizeof("Reverse\r\n"));	
	delay_ms(100); 
	destination(a,flag);
	UARTSend( 0, "stopped\r\n", sizeof("stopped\r\n"));  
	return;
}

void left(unsigned int a,int flag){
	stop();
	UARTSend( 0, "LEFT\r\n", sizeof("LEFT\r\n"));	
	delay_ms(100); 
	destination(a,flag);
	UARTSend( 0, "stopped\r\n", sizeof("stopped\r\n"));  
	return;
}

void right(unsigned int a,int flag){
	stop();
	UARTSend( 0, "RIGHT\r\n", sizeof("RIGHT\r\n"));	
	delay_ms(100); 
	destination(a,flag);
	UARTSend( 0, "stopped\r\n", sizeof("stopped\r\n"));  
	return;
}





void ledon1(){
	stop();
	clear_sting(UART1Buffer,UART1Count);
	GPIO_PinWrite(LED1,HIGH);  
	return;
}


void ledon2(){
	stop();
	clear_sting(UART1Buffer,UART1Count);
	GPIO_PinWrite(LED2,HIGH);     // Make all the Port pin as low
	return;
}



void agvmovement(int flag,int data){
		switch(flag) {

					case 1  :
										forward(data,flag);
										return;
	
					case 2  :
										reverse(data,flag);
										return;
									
					case 3  :
										stop(data,flag);
										return;
									
					case 4  :
										left(data,flag);
										return;
									
					case 5  :
										right(data,flag);
										return;
									
					}		
}


void senddata1(){
   	unsigned short int i,j;
	unsigned char data_rcv[3];
	delay_ms(200);
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

	UART1Count=0; 
	UARTSend( 1, "AT+CIPSEND=0,7\r\n", sizeof("AT+CIPSEND=0,7\r\n"));
	delay_ms(800);


	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != '>');


	
	i--;
	for(j=0;j<1;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,">")) == 0)
			{
				UARTSend( 1, "FORWARD\r\n", sizeof("FORWARD\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}

void senddata2(){
   	unsigned short int i,j;
	unsigned char data_rcv[3];
	delay_ms(200);
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

	UART1Count=0; 
	UARTSend( 1, "AT+CIPSEND=0,7\r\n", sizeof("AT+CIPSEND=0,7\r\n"));
	delay_ms(800);


	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != '>');


	
	i--;
	for(j=0;j<1;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,">")) == 0)
			{
				UARTSend( 1, "REVERSE\r\n", sizeof("REVERSE\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}

void senddata3(){
   	unsigned short int i,j;
	unsigned char data_rcv[3];
	delay_ms(200);
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

	UART1Count=0; 
	UARTSend( 1, "AT+CIPSEND=0,4\r\n", sizeof("AT+CIPSEND=0,4\r\n"));
	delay_ms(800);


	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != '>');
	i--;
	for(j=0;j<1;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,">")) == 0)
			{
				UARTSend( 1, "STOP\r\n", sizeof("STOP\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}

void senddata4(){
   	unsigned short int i,j;
	unsigned char data_rcv[3];
	delay_ms(200);
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

	UART1Count=0; 
	UARTSend( 1, "AT+CIPSEND=0,5\r\n", sizeof("AT+CIPSEND=0,5\r\n"));
	delay_ms(800);


	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != '>');


	
	i--;
	for(j=0;j<1;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,">")) == 0)
			{
				UARTSend( 1, "RIGHT\r\n", sizeof("RIGHT\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}

void senddata5(){
   	unsigned short int i,j;
	unsigned char data_rcv[3];
	delay_ms(200);
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

	UART1Count=0; 
	UARTSend( 1, "AT+CIPSEND=0,7\r\n", sizeof("AT+CIPSEND=0,7\r\n"));
	delay_ms(800);


	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != '>');


	
	i--;
	for(j=0;j<1;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,">")) == 0)
			{
				UARTSend( 1, "LEFT\r\n", sizeof("LEFT\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}

void senddata6(){
   	unsigned short int i,j;
	unsigned char data_rcv[3];
	delay_ms(200);
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

	UART1Count=0; 
	UARTSend( 1, "AT+CIPSEND=0,7\r\n", sizeof("AT+CIPSEND=0,7\r\n"));
	delay_ms(800);


	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != '>');

								  
	
	i--;
	for(j=0;j<1;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,">")) == 0)
			{
				UARTSend( 1, "STOPPED\r\n", sizeof("STOPPED\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}


void getreply(){
	unsigned short int i,j,flag=3,data=0,k,l;
	unsigned char abc[]="+IPD,0,7:F-10";
	unsigned char data_rcv[2];
	unsigned char counter[1];
	int count,v;
	int result=0;

	
//	clear_sting(string_rec,sizeof(string_rec));

 	while (1) 
		{		
//				j=collision();
//								if(j==1){
//									agvmovement(flag,data);	
//								}						
				/* Loop forever */
			//	UARTSend( 0, (uint8_t *)UART1Buffer,UART1Count);
				delay_ms(300);
				i=0;
				while(abc[i++] != ':');
				i--;
				for(j=0;j<2;j++)
				{
	 			//	data_rcv[j]= UART1Buffer[i++];
					data_rcv[j]= abc[i++];
					UARTSend( 0, (uint8_t *)data_rcv,sizeof(data_rcv));
				}
			k=0;
			while(abc[k++] != '-');
		//		i--;
				for(l=0;l<2;l++)
				{
					counter[l]= abc[k++];
							
				}
					count = strlen(counter);					 
				for(v=0; v<count; v++){
					result = result * 10 + ( counter[v] - '0' );
					//	UARTSend( 0, (uint8_t *)counter,sizeof(counter))
				}
					if(result==10){
						UARTSend( 0, "\r\nresult =10",sizeof("\r\nresult =10"));
					}	
					
				if((tp_strcmp((uint8_t *)data_rcv,":F")) == 0)
				{
					flag=1;
					UARTSend( 0, "\r\ninside forward",sizeof("\r\ninside forward"));
				
					forward(result,flag);
					break;
					senddata1();
				}
				
				else if((tp_strcmp((uint8_t *)data_rcv,":B")) == 0)
				{
					flag=2;
					UARTSend( 0, "inside REVERSE",20);
					reverse(result,flag);
					UARTSend( 0, "\r\ninside REVERSE",20);
					break;
					senddata2();
				}
				else if((tp_strcmp((uint8_t *)data_rcv,":L")) == 0)
				{
					flag=3;
					UARTSend( 0, "inside LEFT",20);
					left(10,flag);
					senddata3();
				}
				else if((tp_strcmp((uint8_t *)data_rcv,":R")) == 0)
				{
					flag=4;
					UARTSend( 0, "inside Right",20);
					right(10,flag);
					senddata4();
				}
				
				else if((tp_strcmp((uint8_t *)data_rcv,":S")) == 0)
				{			   
					flag=5;
					UARTSend( 0, "inside Stop",20);
					stop();
					senddata5();
				}
			}
		 UARTSend( 0, "\r\ninside REVERSE",20);
	return;	
}

void mode(){
	unsigned short int i,j;
	unsigned char data_rcv[3];
 
//  	unsigned char s;
 // 	send_command("AT+CWMODE=3");
	UARTSend( 1, "AT+CWMODE=2", sizeof("AT+CWMODE=2"));	
  	delay_ms(100);
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != 'O');
//	data_rcv[0]='O';
//	data_rcv[1]='K';
//	
//	data_rcv[2]='\0';
	i--;
	for(j=0;j<2;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,"OK")) == 0)
			{
				UARTSend( 0, "SERVER", 7 );
				//	server();	
			}
		else
		{
			UARTSend( 0, "FAILED AS SERVER", 15 );
		}
   
	return;
}								  				  

void init_esp(){
	unsigned short int i,j;
	unsigned char data_rcv[3];
	UARTInit(0, 9600); /* uart0 */
  	UARTInit(1, 9600);	/* baud rate setting for esp */
   	UARTSend( 0, "wait", 4);
	UARTSend( 1, "ATE0\r\n", sizeof("ATE0\r\n"));	
 	delay_ms(100);
	UARTSend( 1, "AT+RST\r\n", sizeof("AT+RST\r\n"));
	delay_ms(500);
	clear_sting(UART1Buffer,UART1Count);
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	UARTSend( 1, "AT\r\n", sizeof("AT\r\n"));	
 	delay_ms(100); 
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;
	while(UART1Buffer[i++] != 'O');
	i--;
	for(j=0;j<2;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,"OK")) == 0)
		{
			UARTSend( 0, "Initialiize", 5 );	
		}
		else
		{
			UARTSend( 0, "Initialiize unsuccess", 9 );
		}
	return;
}

void mux(){
	unsigned short int i,j;
	unsigned char data_rcv[3];
	clear_sting(UART1Buffer,UART1Count);
	UARTSend( 1, "AT+CIPMUX=1\r\n", sizeof("AT+CIPMUX=1\r\n"));
	delay_ms(300);
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != 'O');
	i--;
	for(j=0;j<2;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,"OK")) == 0)
			{
				UARTSend( 0, "Mux completed", 13);
					server();	
			}
		else
		{
			mux();
		}
	return;	
}
void server(){
	unsigned short int i,j;
	unsigned char data_rcv[3];
	clear_sting(UART1Buffer,UART1Count);
	UARTSend( 1, "AT+CIPSERVER=1,139\r\n", sizeof("AT+CIPSERVER=1,139\r\n"));

 	delay_ms(100);
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );
	i=0;

	while(UART1Buffer[i++] != 'O');
	i--;
	for(j=0;j<2;j++)
	{
	 	data_rcv[j]= UART1Buffer[i++];
	}
	data_rcv[j]='\0';
	UARTSend( 0, (uint8_t *)data_rcv, sizeof(data_rcv) );
	if((tp_strcmp((uint8_t *)data_rcv,"OK")) == 0)
			{
				UARTSend( 0, "set as Server completed", 23 );
				getreply();
			}
		else
			{
				UARTSend( 0, "server unsuccess", 16 );
				mux();
			}
	 return;
}

void send_command(unsigned char *data){
	UARTSend( 1, data, sizeof(data) );
	return;	
}


void delay_ms(int a){
	int i,j;
	for(i = 0 ; i < a ; i++ )
	{
		for(j = 0 ; j < 10000 ; j++ ) ;
	}
	return ;
}




int main (void)
{
	int x; 
	SystemInit();
	initpins();
	
 	UARTInit(0, 9600);	/* baud rate setting */
	stop();
	getreply();     
//	GPIO_PinWrite(frwd,HIGH);      
//    GPIO_PinWrite(rvrs,HIGH);
//   	while(1){
//  	UARTSend( 0, "READ sensor\r\n", 15);
//	GPIO_PinWrite(frwd,HIGH);
//	delay_ms(2000);
////	GPIO_PinWrite(frwd,LOW);
//	stop();
//	delay_ms(2000);
//	GPIO_PinWrite(frwd,HIGH);
//	delay_ms(2000);
////	GPIO_PinWrite(frwd,LOW);
//	stop();
//	delay_ms(2000);
////	mode();
////	mux();
//			
//			//	x='a';
//				x=GPIO_PinRead(distance);
//				if(x==0){ 
//					UARTSend( 0, "1\r\n", 1);
//				}
//				else{
//						UARTSend( 0, "0\r\n", 1);
//				}
//		//		UARTSend( 0, x, 1);
//	
//
//	}												  
}

		




/*****************************************************************************
**                            End Of File
*****************************************************************************/
