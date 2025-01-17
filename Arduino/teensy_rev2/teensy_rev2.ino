#include <OctoWS2811.h>

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define BLACK  0x000000

const int ledsPerStrip = 170;
const int bytesPerColor = 3;
const int headerSize = 4;
const int maxRows = 8;
const int numLEDs = ledsPerStrip * maxRows;
const int maxPayloadSize = (bytesPerColor * numLEDs); 
const int maxBufSize = maxPayloadSize + headerSize;
const int baudRate = 921600;
const unsigned long resetTime = 10000;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

byte ACK = 0x13;

byte* pkt_buf[2];

byte inputBuffer[maxBufSize];

/*

0 - 0xFFFF
1 - state
2 - payload size
3 - payload size
4 - payload
...
N - payload 

*/

byte outputBuffer[2];
int serialBufIdx;
int serialBufLen;
int * bufPtr;
bool serialPktEnd;
unsigned long startTime;
bool serialReset;


void setOff();
void setWall(byte grid[]);
void setWallPtr(int * bufPtr);
void showWall();
void bootScreen();
void resetScreen();
void serialInterupt();

void setup()
{
	leds.begin();
	leds.show();
	Serial.begin(baudRate);
	serialBufIdx = 0;
	serialBufLen = -1;
	serialPktEnd = false;
	startTime = millis();
	serialReset = false;

	memset(inputBuffer, 0, maxBufSize);

	bootScreen();
}

void loop()
{
	if(serialPktEnd)
	{
		serialPktEnd = false;
		serialBufIdx = 0;

		setWall(inputBuffer);
		//setWallPtr(inputBuffer);
	}

	serialInterupt();
}

void serialEvent()
{
	if(serialPktEnd) { return; }
	if(!Serial)
	{
		Serial.begin(baudRate); 
	}

	while(Serial.available())
	{
		startTime = millis();
		inputBuffer[serialBufIdx++] = Serial.read();

		if(serialBufIdx >= maxBufSize)
		{
			serialPktEnd = true;
		}
	}
}

void setOff()
{
	for(int i = 0; i < numLEDs; i++)
	{
		leds.setPixel(i, BLACK);
	}

	leds.show();
}

void setWallPtr(int * bufPtr)
{
	int * ptr = bufPtr;

	int r, g, b;

	for int(i = 0; i < numLEDs; i++)
	{
		if(maxPaylodSize > (ptr - bufPtr)) { return; }

		r = *ptr;
		g = *(ptr+1);
		b = *(ptr+2);

		leds

		ptr += 3;
		leds.setPixel(ledIdx, r | g | b);		
	}

	showWall();
}

void setWall(byte grid[])
{
	int r, g, b;
	int ledIdx = 0;

	for(int i = 4; i < maxBufSize; ledIdx++)
	{
		r = grid[i++] << 16;
		g = grid[i++] << 8;
		b = grid[i++];

		leds.setPixel(ledIdx, r | g | b);
	}

	showWall();
}

void showWall() { leds.show(); }

void bootScreen()
{
	for(int i = 0; i < numLEDs; i++)
	{
		leds.setPixel(i, RED);	
	}

	leds.show();
	delay(1000);

	for(int i = 0; i < numLEDs; i++)
	{
		leds.setPixel(i, YELLOW);	
	}

	leds.show();
	delay(1000);
	
	for(int i = 0; i < numLEDs; i++)
	{
		leds.setPixel(i, GREEN);	
	}

	leds.show();
	delay(1000);
}

void resetScreen()
{
	for(int i = 0; i < numLEDs; i++)
	{
		leds.setPixel(i, BLUE);	
	}

	leds.show();
}

void serialInterupt()
{
	if(abs(millis() - startTime) >= resetTime)
	{
		Serial.end();
		resetScreen();
		memset(inputBuffer, 0, maxBufSize);
		serialBufIdx = 0;
	}
}