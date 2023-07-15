#include <reg52.h>
 
#define uchar unsigned char
#define uint  unsigned int
 
sbit LB=P0^0;   //   out2
sbit LF=P0^1;   //   out1
sbit RB=P0^2;   //   out4
sbit RF=P0^3;   //   out3
sbit DJ = P0^5;
sbit TRIG=P2^1;
sbit ECHO=P2^0;
sbit ENA=P1^0;
sbit ENB=P1^1;
int value;
char counter=1;
char jiaodu=3,PWMA,PWMB,flag=0; 

void UsartConfiguration()//串口波特率
{   
    SCON=0X50;  //设定串口工作方式
    TMOD=0X21;          
    PCON=0X00;      
    TH1=0Xfd;           
    TL1=0Xfd; //波特率9600
    TR1= 1;                 
    ES = 1;        
    EA = 1;         
}
void T0_Start()//
{   
    EA=1;
	  ET0=1;
	  TMOD=0X21;  
    TH0=0xFE;
    TL0=0x33;  
    TR0=1; 	
}
void T2_Start()//?????
{   
    EA=1;
	  ET2=1;
	  RCAP2H=0x00;
	  RCAP2L=0x00;
    TH2=RCAP2H;
    TL2=RCAP2L;   
    TR2=0;  
}
 void stop1()
{   
	  PWMA=0;
	  PWMB=0;
    LF=0;
    LB=0;
    RF=0;
    RB=0;
}
 
void forward1()
{
		PWMA=15;
	  PWMB=15;
    LF=1;
    LB=0;
    RF=1;
    RB=0;     
}
 
void back1()
{
	  PWMA=15;
	  PWMB=15;
    LF=0;
    LB=1;
    RF=0;
    RB=1;   
}
 
void left1()
{
	  PWMA=15;
	  PWMB=15;
    LF=0;  
    LB=1;
    RF=1;
    RB=0;
}
 
void right1()
{
		PWMA=15;
	  PWMB=15;
    LF=1;
    LB=0;
    RF=0;
    RB=1;
     
}
 void stop2()
{   
		PWMA=0;
	  PWMB=0;
    LF=0;
    LB=0;
    RF=0;
    RB=0;
}
 
void forward2()
{
		PWMA=40;
	  PWMB=40;
    LF=1;
    LB=0;
    RF=1;
    RB=0;     
}
 
void back2()
{
		PWMA=40;
	  PWMB=40;
    LF=0;
    LB=1;
    RF=0;
    RB=1;   
}
 
void left2()
{
		PWMA=40;
	  PWMB=40;
    LF=0;
    LB=1;
    RF=1;
    RB=0;
}
 
void right2()
{
		PWMA=40;
	  PWMB=40;
    LF=1;
    LB=0;
    RF=0;
    RB=1;
     
}
void delay_10us(int ten_us)
{
	while(ten_us--);	
}
void delay_ms(int ms)
{
	int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}
void ultrasonicwave_init(void){
	TRIG=0;
	ECHO=1;
	T2_Start();
}
float ultrasonicwave_measure(void){
	float ftemp;
		TRIG=1;
		delay_10us(2);
		TRIG=0;
		while(ECHO==0){;}
		TR2=1;
		while(ECHO==1){;}
		TR2=0;		
		ftemp=TH2*256+TL2;	
		TH2=0X00;
		TL2=0X00;
		delay_ms(30);
	return(float)(ftemp*0.00034/2)*1000;
}
void main()
{
    T0_Start();
	  jiaodu=3;
    UsartConfiguration(); 
    ultrasonicwave_init();  
    while(1){
		value=ultrasonicwave_measure()/10;
		if(flag==5){
		if(jiaodu==3 && value > 35){ 
			forward1();
			value=ultrasonicwave_measure()/10;
		}
		else if(jiaodu==3 && value<=35){
			stop1();
			jiaodu=2;
			delay_ms(200);
			value=ultrasonicwave_measure()/10;		
		}
		if(jiaodu == 2 && value >35){
			jiaodu =3;
			delay_ms(200);
			left1();
			delay_ms(500);
			stop1();
      value=ultrasonicwave_measure()/10;
	  }
		else if(jiaodu == 2 && value <= 35)
	  {
			jiaodu = 4;
			delay_ms(200);
      value=ultrasonicwave_measure()/10;
	  }
		if(jiaodu == 4 && value >35)
		{
			jiaodu =3;
			delay_ms(200);
			right1();
			delay_ms(500);
			stop1();
			value=ultrasonicwave_measure()/10;
		}
			else if(jiaodu == 4 && value <=35)
		{
			jiaodu =3;
			delay_ms(200);
			back1();
			delay_ms(500);
			right1();
			delay_ms(500);
			stop1();
			value=ultrasonicwave_measure()/10;
		}
	}
      else if(flag<=4){
        switch(flag)
        {
            case (1):  
								forward2();break;
            case (2):
								back2();break;
            case (3):
                left2();break;
            case (4):
                right2();break;
            case (0):
                stop2();break;
            			
        }			

			}		
		
		} 
}      
void Tmr0_isr() interrupt 1 //T1中断用来扫描数码管和计500ms测距
{  
	//TF2=0;       //软件清零
	TH0=0xFE;							//500us中断
	TL0=0x33;
	counter++;//中断次数，即：多少个0.5ms
  if(counter <= jiaodu) DJ = 1;//高电平占空比由变量jiaodu控制，jiaodu=1时高电平0.5ms,jiaodu=5时高电平2.5ms
	if(counter > jiaodu && counter <= 40) DJ = 0;//低电平占空比，jiaodu=1时低电平19.5ms，jiaodu=5时低电平17.5ms
	if(counter <= PWMA) ENA = 1;
	if(counter > PWMA && counter <= 40) ENA = 0;
	if(counter <= PWMB) ENB = 1;
	if(counter > PWMB && counter <= 40) ENB = 0;
	if(counter > 40) counter = 0;//脉宽20ms
}  
void Com_Int(void) interrupt 4
{
    uchar receive_data; 
    EA = 0;
    if(RI == 1) 
    { 
        RI=0; 
        LF=0;
        RF=0;
        LB=0;
        RB=0;
        receive_data = SBUF;
        switch(receive_data)
        {
            case ('1'):  
								flag=1;break;
            case ('2'):
                flag=2;break;
            case ('3'):
                flag=3;break;
            case ('4'):
                flag=4;break;
            case ('0'):
                flag=0;break;
						case ('5'):
                flag=5;break;
            			
        }           
    }       
        EA = 1;
}
    
