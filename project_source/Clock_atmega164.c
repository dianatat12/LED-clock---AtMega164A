#include <mega164a.h> //initalization for all the function of atmega164
#define F_CPU 8000000UL // F_CPU to 8000000 runs the code  at 8MHz instead of the 1MHZ,the clock speed 
#include <delay.h> // library that contains a delay function
#include<stdbool.h> //used to create boolean variables; used for pins initialization

void DELAY_ms(unsigned int ms_count); //generates approximate delay of 1ms for each count;if 1000 is passed as the argument then it generates delay of apprx 1000ms(1sec)
void DELAY_us(unsigned int us_count);//generates approximate delay of 1us for each count;max of 255 sec delay can be generated using this function
    
volatile unsigned char seconds=0, minutes=0, hours=0, alarm_hr=0, alarm_min=0,alarm_sec=0,alarm=0;//declare a EEPROM variable
//we use volatile because variables may be unexpectedly changed, so that the compiler will not assume the value of the variable;  I/O locations
char a1=0,a2=0,toggle=0;
    int current_digit;
    void init_timer0(void);
	void init_timer1(void);    
char hexvalue[10]= {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};//port hex declaration of 0-9 numbers
//this array stores binary bit pattern that will be send to PORTD(output pin)

void main(void)
{

// Crystal Oscillator division factor: 1
#pragma optsize-
//in order to run the code at 8MHz we set CLKPR to divide by 1 in code
CLKPR=(1<<CLKPCE);//enable change of CLKPS bits
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif


// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRA=(1<<DDA7) | (1<<DDA6) | (1<<DDA5) | (1<<DDA4) | (1<<DDA3) | (1<<DDA2) | (1<<DDA1) | (1<<DDA0);
// State: Bit7=1 Bit6=1 Bit5=1 Bit4=1 Bit3=1 Bit2=1 Bit1=1 Bit0=1 
PORTA=(1<<PORTA7) | (1<<PORTA6) | (1<<PORTA5) | (1<<PORTA4) | (1<<PORTA3) | (1<<PORTA2) | (1<<PORTA1) | (1<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=P Bit6=P Bit5=P Bit4=P Bit3=P Bit2=P Bit1=P Bit0=P 
PORTB=(1<<PORTB7) | (1<<PORTB6) | (1<<PORTB5) | (1<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);

// Port C initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRC=(1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
// State: Bit7=1 Bit6=1 Bit5=1 Bit4=1 Bit3=1 Bit2=1 Bit1=1 Bit0=1 
PORTC=(1<<PORTC7) | (1<<PORTC6) | (1<<PORTC5) | (1<<PORTC4) | (1<<PORTC3) | (1<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);


// Port D initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

init_timer0(); //timer0 initialization for digit refresh
init_timer1(); 
// Globally enable interrupts
#asm("sei")
while (1)
//with this function we initialize the buttons and by pressing them, we can setup the minutes and hours

      { 
      
      
   if(PINB & (1<<2))//it checks if PINB has value 1 on bit 1 and 2, it will return true for the if function
 {
 a1=0;//flag that checks if something happends or not
 }
//PORTB is initialized with 1 on every bit
//PINB reads the values of every port in PORTB
 if((a1==0) && (!(PINB & (1<<2)))) //check if the buttons changed their state
 {
 
  toggle=!toggle;//another flag
  a1=1;//flag becomes 1
 
 }
     
      if(toggle==0) {
      if(!(PINB & (1<<0)))//check if the button on the pin 0 has changed into an output state
      {
       minutes++;
       delay_ms(500);
       if(minutes>=60)
       {
       minutes=0; //when there are 60 minutes, the hour digit will be turned to 1 and the minutes will start counting again from 0
       hours++;     
       }     
      }  
      if(!(PINB & (1<<1)))//check if the button on the pin 1 has changed into an output state
      {
       hours++;
       delay_ms(500);
       if(hours>23)
       {
       hours=0;      
       }     
      }
       
      }
    
    else
    
    {    
       if(!(PINB & (1<<0)))
      {
       alarm_min++;
       delay_ms(500);
       if(alarm_min>=60)
       {
       alarm_min=0; 
       alarm_hr++;     
       }     
      }  
      if(!(PINB & (1<<1)))
      {
       alarm_hr++;
       delay_ms(500);
       if(alarm_hr>23)
       {
       alarm_hr=0;      
       }     
      }   

    }  

 if(PINB & (1<<3))  
 {
 a2=0;//alarm flag
 }

 if((a2==0) && (!(PINB & (1<<3))))
 {
 
  alarm=!alarm; 
  a2=1;
 
 }


if(alarm==1)//when alarm is enabled

{

PORTD |=(1<<6); //alarm led on

}

else

{

 PORTD&=~(1<<6);  //when alarm is enabled

}

      }
}



void init_timer0(void)//this timer0 for digit refresh
{
// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 31.250 kHz
// Mode: CTC top=OCR0A
// OC0A output: Disconnected
// OC0B output: Disconnected
// Timer Period: 0.8 ms
// Control Register B for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (1<<WGM01) | (0<<WGM00);//wgmo1 are responsabile for wave generation
TCCR0B=(0<<WGM02) | (1<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;//TC0 counter value
OCR0A=0xFF;//output compare register A
OCR0B=0x00;//output compare register B
// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (1<<OCIE0A) | (0<<TOIE0);
}

 
void init_timer1(void) 
{
// Timer 1 initialization
// Clock source: System Clock
// Clock value: 31.250 kHz
// Mode: CTC top=OCR1A
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 1 s
// Timer1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x7A; 
OCR1AL=0x12; 
OCR1BH=0x00;
OCR1BL=0x00;
// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (1<<OCIE1A) | (0<<TOIE1);
}

// Timer 0 output compare A interrupt service routine
//Interruption mechanism allows the microcontroller to respond to external events, or to events generated by chip peripherals.
interrupt [TIM0_COMPA] void timer0_compa_isr(void)
{
   
     PORTA=0x00;
     PORTC=0x00;
     DELAY_us(50); 
     
	 //port A da tensiune pe pinii declarati in array-ul hexvalue de pe pozitia restului impartirii secundelor la 10
     if(toggle==0) 
     {
    
        switch (current_digit)
    {
        case 1:   
        PORTA = hexvalue[seconds%10];
        //DigitTo7SegEncoder(seconds%10,commonn);
        PORTC=0b00000001;//da tensiune pe pinul C0
        break;
        
        case 2: 
        PORTA = hexvalue[(seconds/10)%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00000010;//da tensiune pe pinul C1
        break; 
        case 3: 
        PORTA = hexvalue[minutes%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00000100;//da tensiune pe pinul C2
        break;
        case 4: 
        PORTA = hexvalue[(minutes/10)%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00001000;//da tensiune pe pinul C3
        break;  
          case 5: 
        PORTA = hexvalue[hours%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00010000;//da tensiune pe pinul C4
        break;
        case 6: 
        PORTA = hexvalue[(hours/10)%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00100000;//da tensiune pe pinul C5
        break;  
        
    }
    current_digit = (current_digit % 7) + 1; 
    }
   

 
    else
     {
    
       switch (current_digit)
    {
        case 1:   
        PORTA = hexvalue[alarm_sec%10];
        //DigitTo7SegEncoder(seconds%10,commonn);
         PORTC=0b00000001;
        break;
        
        case 2: 
        PORTA = hexvalue[(alarm_sec/10)%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00000010;
        break; 
        case 3: 
        PORTA = hexvalue[alarm_min%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
         PORTC=0b00000100;
        break;
        case 4: 
        PORTA = hexvalue[(alarm_min/10)%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
       PORTC=0b00001000;
        break;  
          case 5: 
        PORTA = hexvalue[alarm_hr%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
          PORTC=0b00010000;
        break;
        case 6: 
        PORTA = hexvalue[(alarm_hr/10)%10];
        //DigitTo7SegEncoder((seconds/10)%10,commonn);
        PORTC=0b00100000;
        break;  
        
    }
    current_digit = (current_digit % 7) + 1;  
    
    
    
    }
    
}


void DELAY_ms(unsigned int ms_count)//function declarated in the beggining; delay between counts
{    
    unsigned int i,j;
    for(i=0; i<ms_count; i++)
    
    {
        for(j=0;j<1000; j++);
    }
}
void DELAY_us(unsigned int us_count)
{
    unsigned int i1;
    for(i1=0; i1<us_count; i1++);
    

}


 // Timer1 overflow interrupt service routine
interrupt [TIM1_COMPA] void timer1_ovf_isr(void)//refresh the ports to change seconds to minutes, minutes to hours
{

  seconds++;
  
if(seconds>=60)
{
minutes++;
seconds=0;        
  
}
if(minutes>=60)
{
  hours++;
  minutes=0;
  
}
if(hours>23)
{
  hours=0;
  //minutes=0;
  //seconds=0;
  
}


}
