#include<REGX52.h>
#define dataport P0 //port0 is set as lcd data port
int cms;
sbit trig=P3^5;//to initiallize port pin 3.5 as trigger pin
sbit rs=P2^0;//to initiallize port pin 2.0 as lcd rs pin
sbit rw=P2^1;//to initiallize port pin 2.1 as lcd rw pin
sbit e=P2^2;//to initiallize port pin 2.2 as lcd enable bin pin
sbit buz=P3^0;//to initiallize port pin 3.0 for buzzer output pin
sbit led=P3^7;//to initiallize port pin 3.7 for led output pin
sbit parkmode=P3^6;//to initiallize port pin 3.6 for parking mode switch 
sbit normalmode=P3^3;//to initiallize port pin 3.3 for normal mode switch
void delay(unsigned int msec)
{
  int i,j;
  for(i=0;i<msec;i++)
  for(j=0;j<1275;j++);
}
void lcd_cmd(unsigned char item) // Function to send command to LCD
{
  dataport = item;
  rs= 0;
  rw=0;
  e=1;
  delay(1);
  e=0;
  return;
}
void lcd_init(void){   //function to initialize lcd
lcd_cmd(0x38);
lcd_cmd(0x0c);
  delay(2);
lcd_cmd(0x01);
  delay(2);
lcd_cmd(0x81);
}
void lcd_data(unsigned char item) // Function to send data to LCD
{
  dataport = item;
  rs= 1;
  rw=0;
  e=1;
  delay(1);
  e=0;
  return;
}

void lcd_data_string(unsigned char *str) // Function to send string to LCD
{
  int i=0;
  while(str[i]!='\0')
   {
    lcd_data(str[i]);
    i++;
    delay(1);
   }
return;
}

void send_pulse(void) 
{
  TH0=0x00;TL0=0x00;
  trig=1;                  //Sending trigger pulse
  delay(5);                //Wait for about 10us
  trig=0;                  //Turn off trigger
}

unsigned int get_range(void)
{
  long int timer_val;
  send_pulse();
  while(!INT0);          //Waiting until echo pulse is detected
  while(INT0);           //Waiting until echo pulse changes its state
  timer_val=(TH0<<8)+TL0;//timer_val value is got by the timer 0
  lcd_cmd(0x81);
  lcd_data_string("DISTANCE:");
  lcd_cmd(0x8a);
  if(timer_val<38000)
   {
    cms=timer_val/59;  // formula to calculate the distance in cms
    if (cms!=0)
     {
			 int a=((cms/100)+48); //ASCII CONVERSION
				 int b=(((cms/10)%10)+48);//ASCII CONVERSION
			 int c=((cms%10)+48);//ASCII CONVERSION
      lcd_data(a);//to display the first digit on lcd
			 lcd_data(b);//to display the second digit on lcd
			 lcd_data(c);//to display the third digit on lcd
			 lcd_data_string(" CM");
			 
     }

   }
  else
  {
   lcd_cmd(0x06);
   lcd_data_string("Object out of range");
  }
  return cms;
}
void norm_code(void){
delay(200);
		 lcd_init();
lcd_data_string("NORMAL MODE");
		delay(200);
  while(1)
  {
   get_range();
   delay(2);
		}
}
void park_code(void){
lcd_init();
		lcd_data_string("PARKI");
		delay(200);
	 while(1)
  {
   get_range();
   delay(2);
		if(cms<=16){
		buz=1;
		led=1;
			}
	else{
	buz=0;
	led=0;
	}
  }
}
void main()
{
	parkmode=0xFFFF;//to intiallize switch 1 as input 
	normalmode=0xFFFF;//to intiallize switch 2 as input
	buz=0;
	led=0;
	lcd_init();
  lcd_data_string("SELECT THE MODE");
	delay(200);
	lcd_init();
	lcd_data_string("SW1=NORMAL MODE");
  delay(200);
	lcd_init();
	lcd_data_string("SW2=PARKING MODE");
	while((normalmode==1)&(parkmode==1));//used to wait untill one of either noraml mode sw1 or parking mode sw2 is pressed
  TMOD=0x09;//timer0 in 16 bit mode with gate enable
  TR0=1;//timer run enabled                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
  TH0=0x00;
  TL0=0x00;
  P3|=0x04;//setting pin P3.2
	if(normalmode==0){
		norm_code();//function call for normal switch
}
	else if(parkmode==0){
		park_code();//function call for parking code
	}
}
