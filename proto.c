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
void agvmovement(int flag);
unsigned char string_rec[20];
unsigned char machine_num[50];
#define LED1      P2_2
#define LED2      P2_3

#define frwd1	  P0_6
#define frwd2	  P0_7

#define rvrs1	  P0_8
#define rvrs2	  P0_9

#define obstacle  P0_8
#define magnetic1 P0_9
#define magnetic2 P0_10
#define magnetic3 P0_11

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
	/*outputs*/
	GPIO_PinFunction(LED1,PINSEL_FUNC_0); // Configure Pin for Gpio
	GPIO_PinDirection(LED1,OUTPUT);        /* Configure the pins as Output to blink the Leds*/
   	GPIO_PinFunction(LED2,PINSEL_FUNC_0); // Configure Pin for Gpio
    GPIO_PinDirection(LED2,OUTPUT);
	GPIO_PinFunction(frwd1,PINSEL_FUNC_0);
	GPIO_PinDirection(frwd1,OUTPUT);  
	GPIO_PinFunction(frwd2,PINSEL_FUNC_0);
	GPIO_PinDirection(frwd2,OUTPUT);        
	GPIO_PinFunction(rvrs1,PINSEL_FUNC_0);
    GPIO_PinDirection(rvrs1,OUTPUT);        
	GPIO_PinFunction(rvrs2,PINSEL_FUNC_0);
    GPIO_PinDirection(rvrs2,OUTPUT);
								   

	GPIO_PinWrite(LED1,LOW);     
  	GPIO_PinWrite(LED2,LOW);     
	GPIO_PinWrite(frwd1,LOW);      
    GPIO_PinWrite(rvrs1,LOW);
	     
	GPIO_PinWrite(frwd2,LOW);      
    GPIO_PinWrite(rvrs2,LOW);
	/*inputs*/
	GPIO_PinFunction(obstacle,PINSEL_FUNC_0); 
	GPIO_PinDirection(obstacle,INPUT); 
	GPIO_PinFunction(magnetic1,PINSEL_FUNC_0); 
	GPIO_PinDirection(magnetic1,INPUT);	
	GPIO_PinFunction(magnetic2,PINSEL_FUNC_0); 
	GPIO_PinDirection(magnetic2,INPUT);
	GPIO_PinFunction(magnetic3,PINSEL_FUNC_0); 
	GPIO_PinDirection(magnetic3,INPUT);
}

void stop(){
	GPIO_PinWrite(frwd1,LOW);    
	GPIO_PinWrite(rvrs1,LOW); 
	
	GPIO_PinWrite(frwd2,LOW);    
	GPIO_PinWrite(rvrs2,LOW);   
	GPIO_PinWrite(LED1,LOW);   
	GPIO_PinWrite(LED2,LOW);     
	return;
}

void forward(){
	stop();
	UARTSend( 0, "forward\r\n", sizeof("forward\r\n"));
	GPIO_PinWrite(frwd1,HIGH);     // Make all the Port pin as high
	GPIO_PinWrite(frwd2,HIGH);     // Make all the Port pin as high	
	return;
}

void reverse(){
	stop();
	UARTSend( 0, "Reverse\r\n", sizeof("Reverse\r\n"));	
	GPIO_PinWrite(rvrs1,HIGH); 
	GPIO_PinWrite(rvrs2,HIGH);  
	return;
}

void right_turn(){
	GPIO_PinWrite(frwd2,LOW);
	return;
}
void left_turn(){
	GPIO_PinWrite(frwd1,LOW);
	return;
}
int collision(){
	int i;
	i=GPIO_PinRead(obstacle);
	if(i==1){
		stop();
		senddata6();
			while(i!=0){
			i=GPIO_PinRead(obstacle); 
			if(i==0){
			return 1;
			}
		}
	}
	return 0;
}

int magnetic_line(){
	int i,j,k,a;
	i=GPIO_PinRead(magnetic1);	//left magnetic sensor
	j=GPIO_PinRead(magnetic2);	//middle magnetic sensor
	k=GPIO_PinRead(magnetic3);	//right magnetic sensor

	if(j==1){
		senddata6();
			while(j!=0){
			a=collision();
								if(a==1){
									magnetic_line();	
								}
			i=GPIO_PinRead(magnetic1); 
			j=GPIO_PinRead(magnetic2);
			k=GPIO_PinRead(magnetic3);
			
				if(i==0 | j==0 | k==0){
//					flag=1;
					stop();
					return 1;
				}
				else if(i==0 | j==0 | k==1){
//					flag=2;
					right_turn();
					delay_ms(1000);
				}
				else if(i==0 | j==1 | k==0){
//					flag=3;
					forward();
				}
				else if(i==0 | j==1 | k==1){
//					flag=4;
					right_turn();
					delay_ms(500);
					forward();
				}
				else if(i==1 | j==0 | k==0){
//					flag=5;
					left_turn();
					delay_ms(1000);
				}
				else if(i==1 | j==0 | k==1){
//					flag=6;
					stop();
					return 2;
				}
				else if(i==1 | j==1 | k==0){
//					flag=7;
					left_turn();
					delay_ms(500);
					forward();
				}
				else if(i==1 | j==1 | k==1){
//					flag=8;
					stop();
					return 3;
				}
			}
	}
	else if(i==1){
		senddata6();
			while(i!=0){
			a=collision();
								if(a==1){
									magnetic_line();	
								}
			i=GPIO_PinRead(magnetic1); 
			j=GPIO_PinRead(magnetic2);
			k=GPIO_PinRead(magnetic3);
			
				if(i==0 | j==0 | k==0){
//					flag=1;
					stop();
					return 1;
				}
				else if(i==0 | j==0 | k==1){
//					flag=2;
					right_turn();
					delay_ms(1000);
				}
				else if(i==0 | j==1 | k==0){
//					flag=3;
					forward();
				}
				else if(i==0 | j==1 | k==1){
//					flag=4;
					right_turn();
					delay_ms(500);
					forward();
				}
				else if(i==1 | j==0 | k==0){
//					flag=5;
					left_turn();
					delay_ms(1000);
				}
				else if(i==1 | j==0 | k==1){
//					flag=6;
					stop();
					return 2;
				}
				else if(i==1 | j==1 | k==0){
//					flag=7;
					left_turn();
					delay_ms(500);
					forward();
				}
				else if(i==1 | j==1 | k==1){
//					flag=8;
					stop();
					return 3;
				}
			}
		}
	else if(k==1){
		senddata6();
		while(k!=0){
		a=collision();
								if(a==1){
									magnetic_line();	
								}
			i=GPIO_PinRead(magnetic1); 
			j=GPIO_PinRead(magnetic2);
			k=GPIO_PinRead(magnetic3);
			
				if(i==0 | j==0 | k==0){
//					flag=1;
					stop();
					return 1;
				}
				else if(i==0 | j==0 | k==1){
//					flag=2;
					right_turn();
					delay_ms(1000);
				}
				else if(i==0 | j==1 | k==0){
//					flag=3;
					forward();
				}
				else if(i==0 | j==1 | k==1){
	//				flag=4;
					right_turn();
					delay_ms(500);
					forward();
				}
				else if(i==1 | j==0 | k==0){
	//				flag=5;
					left_turn();
					delay_ms(1000);
				}
				else if(i==1 | j==0 | k==1){
	//				flag=6;
					stop();
					return 2;
				}
				else if(i==1 | j==1 | k==0){
//					flag=7;
					left_turn();
					delay_ms(500);
					forward();
				}
				else if(i==1 | j==1 | k==1){
	//				flag=8;
					stop();
					return 3;
				}
			}
		}
		return 0;
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



void agvmovement(int flag){
		switch(flag) {

					case 1  :
										stop();
										return;
	
					case 2  :
										reverse();
										return;
									
					case 3  :
										stop();
										return;
									
					case 4  :
										ledon1();
										return;
									
					case 5  :
										ledon2();
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
				UARTSend( 1, "STOPPED\r\n", sizeof("COLLISION\r\n"));
			}
		else
		{
			UARTSend( 0, "DATA NOT SENT to client", 23 );
		}
	return;
}


void option(){
	stop();
	return;
}

void turning(){
	int i,j,k;
	i=GPIO_PinRead(magnetic1);	//left magnetic sensor
	j=GPIO_PinRead(magnetic2);	//middle magnetic sensor
	k=GPIO_PinRead(magnetic3);	//right magnetic sensor
	
	 if(k==1){
		senddata6();
		while(k!=0){
			i=GPIO_PinRead(magnetic1); 
			j=GPIO_PinRead(magnetic2);
			k=GPIO_PinRead(magnetic3);
				 if(i==0 | j==0 | k==1){
					 magnetic_line();
				}
			
			}
		}
}


void getreply(){
	unsigned short int i,j,flag=3,a;
	unsigned char data_rcv[7];
	clear_sting(UART1Buffer,UART1Count);
	clear_sting(string_rec,sizeof(string_rec));

 	while (1) 
		{		
				j=collision();
								if(j==1){
									agvmovement(flag);	
								}						
				/* Loop forever */
				clear_sting(UART1Buffer,UART1Count);
				delay_ms(300);
				i=0;
				while(UART1Buffer[i++] != 'S');
				i--;
				for(j=0;j<7;j++)
				{
	 				data_rcv[j]= UART1Buffer[i++];
				}
				if((tp_strcmp((uint8_t *)data_rcv,"START:1")) == 0)
				{
					senddata1();
					a=magnetic_line();
					switch(a)  {
						case 1: a=magnetic_line();
						case 2: 
								option();
								right_turn();
								turning();
						case 3:
								option();		  
					}
					flag=1;
				}
				else if((tp_strcmp((uint8_t *)data_rcv,"START:2")) == 0)
				{
					stop();
					senddata2();
					flag=2;
				}
//				else if((tp_strcmp((uint8_t *)data_rcv,"START:3")) == 0)
//				{
//					stop();
//					senddata3();
//					flag=3;
//				}
//				else if((tp_strcmp((uint8_t *)data_rcv,"START:4")) == 0)
//				{
//					ledon1(); 
//					senddata4();
//					flag=4;
//				}
//				
//				else if((tp_strcmp((uint8_t *)data_rcv,"START:5")) == 0)
//				{			   
//					ledon2(); 	
//					senddata5();
//					flag=5;
//				}
			}
	
		
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
	SystemInit();
	initpins();

	init_esp();

  	UARTSend( 0, "echo completed", 15);
	mode();
	mux();
	UARTSend( 0, (uint8_t *)UART1Buffer, UART1Count );												  
}

		




/*****************************************************************************
**                            End Of File
*****************************************************************************/
