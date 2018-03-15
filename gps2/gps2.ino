#include <LiquidCrystal.h>
#include <TinyGPS.h>

TinyGPS gps;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);							//LCD driver pins

char computer;

int c;

int led = 13;

long lat, lng;
unsigned long fix_age, time, date, speed, course;
unsigned long chars;
unsigned short sentences, failed_checksum;
//int year;
//byte month, day, hour, minute, second, hundredths;

int DEG;
int MIN1;
int MIN2;

void LAT() {											//Latitude state
	DEG = lat / 1000000;
	MIN1 = (lat / 10000) % 100;
	MIN2 = lat % 10000;

	lcd.setCursor(0, 0);						// set the LCD cursor	 position
	lcd.print("LAT:");
	lcd.print(DEG);
	lcd.write(0xDF);
	lcd.print(MIN1);
	lcd.print(".");
	lcd.print(MIN2);
	lcd.print("'	 ");
}
void LON() {											 //Longitude state
	DEG = lng / 1000000;
	MIN1 = (lng / 10000) % 100;
	MIN2 = lng % 10000;

	lcd.setCursor(0, 1);						 // set the LCD cursor	 position
	lcd.print("LON:");
	lcd.print(DEG);
	lcd.write(0xDF);
	lcd.print(MIN1);
	lcd.print(".");
	lcd.print(MIN2);
	lcd.print("'	 ");
}

void setup()
{
	Serial.begin(115200);
	Serial1.begin(38400);						//Set the GPS baud rate.

	pinMode(led, OUTPUT);

	lcd.begin(16, 2);							 // start the library
	lcd.setCursor(0, 0);						// set the LCD cursor	 position
	lcd.print("GPS test");					// print a simple message on the LCD
	delay(5000);

	lat = 0;
	lng = 0;
}

void loop()
{
	computer = 0;
	
	while (Serial1.available())
	{
		digitalWrite(led, HIGH);
		c = Serial1.read();
		if (gps.encode(c))												// Check the GPS data
		{
			// process new gps info here
		}

	}
	digitalWrite(led, LOW);
	gps.get_position(&lat, &lng, &fix_age);		 // retrieves +/- lat/long in 100000ths of a degree

	LAT();
	LON();

	if (Serial.available())
	{
		computer = Serial.read();

		if (computer)
		{
			Serial.write((byte*) &lng, sizeof(long));
			Serial.write((byte*) &lat, sizeof(long));
		}
	}
}
