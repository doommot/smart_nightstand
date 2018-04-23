#include <util/delay.h>

#define default_mode 0 //starting mode
#define default_color 0

#define off 0//Чтобы выключить - analogWrite(white_pin, off), включить - analogWrite(white_pin, brightness)

#define low 10 //brightness - запомненная яркость, одно из этих значений, не off
#define medium 100
#define high 255

#define blue_pin 11
#define red_pin 10
#define green_pin 9
#define white_pin 6
/*------BUTTONS:
 * int1-color_button 3
 * int0-white_button 2
*/
#define command_flow 1000
#define command_white1 1001
#define command_white2 1002
#define command_white3 1003
#define command_off 1004
#define command_color 1005

int mode;
/*------MODES TABLE:
 * 0-off
 * 1-white
 * 2-color flow
 * 3-color hold
 * 4-white with gibernation
 *------РЕЖИМЫ:
 * 0-обе ленты выключены
 * 1-горит белая лента с регулировкой яркости при нажатии на белую кнопку
 * 2-горит РГБ лента с переливанием цвета
 * 3-горит РГБ лента
 * 4-горит белая лента
 */
int color;
int brightness;

void setup()
{
	mode=default_mode;
	color=default_color;
	brightness=high;
	pinMode(white_pin, OUTPUT); 
	pinMode(red_pin, OUTPUT);
	pinMode(green_pin, OUTPUT);
	pinMode(blue_pin, OUTPUT);
	analogWrite(white_pin, off);
	analogWrite(red_pin, off);
	analogWrite(green_pin, off);
	analogWrite(blue_pin, off);
	
	attachInterrupt(0, white_button, RISING);
	attachInterrupt(1, color_button, RISING);
	Serial.begin(9600);
}

void loop(){
	if (Serial.available()>1)
		bluetooth();
	if(mode==2)
	{
		if (color > 765)
			color = 0;
		else
			color++;
		RGB(color);
	}
	_delay_ms(40);
}

void white_button()
{
	if(mode==0)
	{
		mode=1;
		analogWrite(white_pin, brightness);
		RGB_off();
		_delay_ms(80);
		return;
	}
	if(mode==1)
	{
		if(brightness==high)
		{
			brightness=medium;
			analogWrite(white_pin, brightness);
			_delay_ms(80);
			return;
		}
		if(brightness==medium)
		{
			brightness=low;
			analogWrite(white_pin, brightness);
			_delay_ms(80);
			return;
		}
		if(brightness==low)
		{
			brightness=high;
			analogWrite(white_pin, off);
			mode=0;
			_delay_ms(80);
			return;
		}
	}
	if(mode==2||3)
	{
		mode=1;
		RGB_off();
		analogWrite(white_pin, brightness);
		_delay_ms(80);
		return;
	}
	if(mode==4)
	{
		mode=0;
		analogWrite(white_pin, off);
		RGB_off();
		_delay_ms(80);
		return;
	}
}

void color_button()
{
	if((mode==0)||(mode==1)||(mode==3))
	{
		mode=2;
		RGB(color);
		analogWrite(white_pin, off);
		_delay_ms(80);
		return;
	}
	if(mode==2)
	{
		mode=3;
		RGB(color);
		analogWrite(white_pin, off);
		_delay_ms(80);
		return;
	}
 /*
	if(mode==3)
	{
		mode=2;
		analogWrite(white_pin, off);
    _delay_ms(80);
    return;
	}
 */
}

void RGB_off()
{
	analogWrite(red_pin, off);
	analogWrite(green_pin, off);
	analogWrite(blue_pin, off);
}

void bluetooth()
{
	String input_string;
	String command;
	int command_int;
	while (Serial.available() > 0)
	{
		int input_char = Serial.read();
		if (isDigit(input_char))
		{
			// convert the incoming byte to a char and add it to the string:
			input_string += (char)input_char;
		}
		//if (input_char == '\n')
		//else command += (char)input_char;
	}

	command_int=input_string.toInt();

	switch (command_int)
	{
		case command_flow:
		{
			mode=2;
			analogWrite(white_pin, off);
			break;
		}
		case command_white1:
		{
			mode=1;
			analogWrite(white_pin, low);
			RGB_off();
			break;
		}
		case command_white2:
		{
			mode=1;
			RGB_off();
			analogWrite(white_pin, medium);
			break;
		}
		case command_white3:
		{
			mode=1;
			RGB_off();
			analogWrite(white_pin, high);
			break;
		}
		case command_off:
		{
			mode=0;
			analogWrite(white_pin, off);
			RGB_off();
			break;
		}
		case command_color:
		{
			mode=3;
			RGB(color);
			analogWrite(white_pin, off);
			break;
		}
		default:
		{
			analogWrite(white_pin, off);
			color = command_int;
			RGB(color);
			break;
		}
	}
	//command_int=0;
}

void RGB (int current_color)
{
	int redTone;
	int greenTone;
	int blueTone;
	if (current_color <= 255)
	{                  // zone 1
		redTone = 255 - current_color;           // red goes from on to off
		greenTone = current_color;               // green goes from off to on
		blueTone = 0;                    // blue is always off
	}
	else if (current_color <= 511)
	{              // zone 2
		redTone = 0;                      // red is always off
		greenTone = 255 - (current_color - 256);  // green on to off
		blueTone = (current_color - 256);         // blue off to on
	}
	else
	{                            // color >= 512 // zone 3
		redTone = (current_color - 512);            // red off to on
		greenTone = 0;                      // green is always off
		blueTone = 255 - (current_color - 512);     // blue on to off
	}
	analogWrite(red_pin, redTone);
	analogWrite(blue_pin, blueTone);
	analogWrite(green_pin, greenTone);
}
