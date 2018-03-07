// MAX4466 Microphone

#include <Adafruit_NeoPixel.h>
#include "Queue.h"

#define NEO_PIN 0
#define PIN_MIC_ANALOG A1
#define NUM_PIXELS  24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

Queue<int16_t> queue(40);
Queue<int16_t> stdQueue(40);

uint16_t wheelSpeedCount = 0;
uint32_t colorCount = 0;
uint8_t isSavingMode = false;
uint16_t stateCheckCount = 0;
uint8_t soundStateCheckFlag[2] = { false, false };
uint8_t firstDeadTime = true;

void setup() {
	pinMode(PIN_MIC_ANALOG, INPUT);
	strip.begin();
}

void rotateColor() {
// this is rainbow circle
	colorCount += 1;
	for (uint8_t i = 0; i < NUM_PIXELS; i++) {
		uint32_t tempColor = Wheel(((i * 256 / NUM_PIXELS) + colorCount) & 255);
		strip.setPixelColor(i, tempColor);
	}
}

uint16_t tempVolumToNeopixel;
void ledFunction() {

	rotateColor();

	const uint8_t MIN_AVER = 40;
	const uint8_t MAX_AVER = 180;

	tempVolumToNeopixel = max(tempVolumToNeopixel, max(MIN_AVER, min(MAX_AVER, queue.average())));
	uint8_t lowLightCount = map(tempVolumToNeopixel, MIN_AVER, MAX_AVER, 0, 12);

	for (int i = NUM_PIXELS / 2; i > lowLightCount; i--) {
		const uint8_t LOW_LIGHT = 5;
		strip.setPixelColor(i, strip.Color(LOW_LIGHT, LOW_LIGHT, LOW_LIGHT));
		strip.setPixelColor(24 - i, strip.Color(LOW_LIGHT, LOW_LIGHT, LOW_LIGHT));
	}

	tempVolumToNeopixel -= 5;
}

void checkSleep() {

	// in order to check whether it is silent, check it twice time.


	//if (stdQueue.average() > 10) {

	if (queue.average() > 10) {
		soundStateCheckFlag[0] = true;
	} else {
		soundStateCheckFlag[0] = false;
	}

	// if it is loud and previous state was loud,
	if (soundStateCheckFlag[0] && soundStateCheckFlag[1]) {
		// turn off the saving mode
		isSavingMode = false;
		// clear flag for "fade out"
		firstDeadTime = true;

	} else if (soundStateCheckFlag[0] == false && soundStateCheckFlag[1] == false) {
		// if prev and current is silent, fade out.

		isSavingMode = true;
		if (firstDeadTime) {
			ledFadeOut();
			firstDeadTime = false;
		}
	}

	//save current state to another variable
	soundStateCheckFlag[1] = soundStateCheckFlag[0];

}


void taskUnit() {

	int16_t micAnalogValue = analogRead(PIN_MIC_ANALOG);
	queue.push(abs(micAnalogValue - 512));
	stdQueue.push(queue.average());

	// once every 10 times
	if (wheelSpeedCount++ > 10) {
		wheelSpeedCount = 0;
		if (!isSavingMode) {
			ledFunction();
		}
	}

	// once every 1500 times
	if (stateCheckCount++ > 1500) {
		stateCheckCount = 0;
		checkSleep();
	}

}

// Main ==========================================================================================//
void loop() {
	long lastTime = 0;
	long currentTime = 0;
	currentTime = millis();
	if (currentTime - lastTime > 2) {
		lastTime = currentTime;
		taskUnit();
		strip.setBrightness(127);
		strip.show();
	}
}




// All about NEOPIXEL ===========================================================================//
uint32_t Wheel(byte WheelPos) {
	if (WheelPos < 85) {
		return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	} else if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	} else {
		WheelPos -= 170;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}

uint32_t setOneBrightness(uint32_t tempColor, uint8_t brightness) {
	uint8_t r = (uint32_t)((tempColor >> 16));
	uint8_t g = (uint32_t)((tempColor >> 8));
	uint8_t b = (uint32_t)((tempColor >> 0));
	r = (r * brightness) >> 8;
	g = (g * brightness) >> 8;
	b = (b * brightness) >> 8;
	uint32_t changedColor = (((uint32_t) r << 16)) | (((uint32_t) g << 8)) | (((uint32_t) b << 0));
	return changedColor;
}

void ledFadeOut() {
	for (uint8_t i = 0; i < NUM_PIXELS; i++) {
		strip.setPixelColor(i, 0);
		strip.show();
		delay(50);
	}
}
