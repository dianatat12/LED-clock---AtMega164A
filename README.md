# LED-clock-AtMega164A
LED clock with 7-segment display, calendar and alarm using AtMega164A

## Hardware description
7 segment display
> We chosed the approach of having common cathode. In this type of display, all the cathode connections of the LED segments are connected together to logic 0 or ground. The separate segments are lightened by applying the logic 1 or HIGH signal through a current limiting resistor to forward bias the individual anode terminals a to g. We used it to display the digits for hours and minutes (each digit is for tens of hours, hours, tens of minutes, minutes, in this order, from left to right).
>We also used 2 common cathode single digit displays, for displaying the digits for the seconds.

Resistors
>As the voltage source of the microcontroller is greater than the voltage drop of the LEDs in the 7 – Segment Display, we needed ballast resistors to limit the current through the LEDs to prevent excess current that could have burnt them.
>A base resistor limits the current flowing into the base of the transistor to prevent it being damaged but it must also allow sufficient base current to flow to ensure that the transistor is fully saturated when switched on. 

Transistors
>By choosing a Quad Digit 7 Segment Display, we needed to understand how they work. Those devices have 4 digit made of 7 LEDs plus an extra LED for the decimal point next to each digit. That’s a total of 32 LEDs that ATMega should be able to turn on and off independently. But if we look at a display, it has only 12 pins. How it’s that possible? Well, the digits are multiplexed. We have four common pins, one for each digit. When one of this pin is active, you have control over a single digit using 8 pin interface (7 segments + decimal point). Digits are lit sequentially, in rapid succession and perceived as being always on. Four common signals and eight segment pins gives a total of 12 pins.

Push buttons
>We used buttons for setting the clock. By pressing on the first one, the digit which represents the hour is incremented. When the second button is pressed, the same as above for the minutes. Third and forth buttons are reserved for the alarm properties of the circuit.


## Software description
> Port A has analog inputs to the Analog to Digital Converter and they are 8-bit bi-directional input/output port if the A/D converter is not used.
We connected the A-G pins that correspond to the leds of the 7 segment(both 4x7 segment and single 7segment). These connections were made in series with a 1.6 KΩ resistors for each connection to protect the leds by limiting the current. 
The output pin is initialized with DDRA and we wrote its value using PORTA. 
```
// Port A initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRA=(1<<DDA7) | (1<<DDA6) | (1<<DDA5) | (1<<DDA4) | (1<<DDA3) | (1<<DDA2) | (1<<DDA1) | (1<<DDA0);

```
 >Port B are 8-bit bi-directional input/output port. We use it to initialize the push buttons that modify the minutes, hours and enters in the alarm mode.
 
```
// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=P Bit6=P Bit5=P Bit4=P Bit3=P Bit2=P Bit1=P Bit0=P 
PORTB=(1<<PORTB7) | (1<<PORTB6) | (1<<PORTB5) | (1<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);
```

>Port C are 8-bit bi-directional input/output port and they have JTAG interface ( Standard Test Access Port and Boundary-Scan). 
>We use port C to connect the base of the transistors in series with a 1kΩ resistor. Resistor limits the current flowing into the base of the transistor to prevent it being damaged, but it must also allow sufficient base current to flow to ensure that the transistor is fully saturated when switched on
```
// Port C initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRC=(1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
// State: Bit7=1 Bit6=1 Bit5=1 Bit4=1 Bit3=1 Bit2=1 Bit1=1 Bit0=1 
PORTC=(1<<PORTC7) | (1<<PORTC6) | (1<<PORTC5) | (1<<PORTC4) | (1<<PORTC3) | (1<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);
```

>Port D are 8-bit bi-directional input/output port. We use it to make the LED that was already on the board to blink when the alarm is set.
```
// Port D initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
```


> In order to make the LED blink in alarm mode, we are using the PORTD that has been initialized at the beginning.
```

if(alarm==1)//when alarm is enabled

{

PORTD |=(1<<6); //alarm led on

}

else

{

 PORTD&=~(1<<6);  //when alarm is enabled

}

      }
}```

>Interrupt is serviced by a C function called ISR (Interrupt Service  Routine).Timer1 output compare A is used to blink LED. 
```
//Interruption mechanism allows the microcontroller to respond to external events, or to events generated by chip peripherals.
interrupt [TIM0_COMPA] void timer0_compa_isr(void)```

>We initialized a timer that will refresh the digit after each count. This corresponds with the timer_0 defined in line 57.
```
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
```
