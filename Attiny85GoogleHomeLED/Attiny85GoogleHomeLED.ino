// MAX4466 Microphone

#include <Adafruit_NeoPixel.h>
#include "Queue.h"
#include "Ledeffect.h"

#define DEBUG 0

#if(DEBUG)
#include "DigiKeyboard.h"
#endif


#define NEO_PIN 0
#define PIN_MIC_ANALOG A1

#define VOICE_CATCH_STDDIV  30
#define VOICE_PAUSE_TIME_THREASHOLD 200

#define DEALY 30
#define BUTTON_DEBOUNCE_PERIOD 20 //ms
#define NUM_PIXELS  24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN , NEO_GRB + NEO_KHZ800);

//Queue<uint16_t> queue(20);
Queue<uint16_t> queue(40);
Queue<uint16_t> stdQueue(40);

uint32_t Wheel(byte WheelPos);
void ledFadeOut();

//LEDeffect ledEffect(&strip);

volatile uint8_t wheelCountSpeedThreshold = 10;

void setup() {
  pinMode(PIN_MIC_ANALOG, INPUT);
  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  //  Serial.begin(9600);
}

volatile uint16_t wheelSpeedCount = 0;
volatile uint16_t wheelSpeedDuration = 10;
volatile long resetMaxStddivCount = 0;
volatile uint16_t ledUpdateCount = 0;

// Interrupt is called once a millisecond,
volatile long colorCount = 0;
volatile uint16_t lastAnalogValue = 0;
volatile uint16_t tempCount = 0;
volatile bool isSavingMode = false;
volatile uint32_t tempColor = 0;
volatile uint16_t innerCount = 0;
volatile bool isFirst = true;
volatile uint16_t stateCheckCount = 0;

volatile bool soundStateCheckFlag[2] = {false, false};
volatile bool firstDeadTime = true;

volatile double maxStdAverage = 0;
volatile uint16_t maxStdAverageCount = 0;
volatile uint16_t maxFrequentCount = 0;
volatile double decreaseGain = 0.0;

volatile double minStdAverage = 0;
volatile uint16_t minStdAverageCount = 0;
volatile uint16_t minFrequentCount = 0;

volatile uint16_t averCount = 0;
volatile uint16_t prevAverage = 0;
uint16_t tempVolumToNeopixel ;
volatile bool colorChangeFlag = false;

volatile double maxAverageTemp = 0;

void rotateColor() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    uint32_t tempColor = Wheel(((i * 256 / NUM_PIXELS) + colorCount) & 255);
    strip.setPixelColor(i, tempColor);
  }
}

uint8_t ledRotationCount = 0;
uint8_t ledRotationStep = 0;

void ledFunction() {
  colorCount += 1;
  // this is rainbow circle

  rotateColor();

  //  maxAverageTemp = max(maxAverageTemp, (double)stdQueue.average());
  //  tempVolumToNeopixel = map(maxAverageTemp, 0, queue.getMaxStddiv(), 0, 12);

  //  Serial.print(stdQueue.getMax());
  //  Serial.print(",");
  //  Serial.print(queue.average());
  //  Serial.print("\n");

  //spread from head to tail
#define MIN_AVER 40
#define MAX_AVER 180

  //tempVolumToNeopixel  = min(256, queue.average());
  tempVolumToNeopixel  = max(tempVolumToNeopixel,max(MIN_AVER, min(MAX_AVER , queue.average())));
 
  maxAverageTemp = max(maxAverageTemp, tempVolumToNeopixel);
  maxAverageTemp = map(maxAverageTemp , MIN_AVER, MAX_AVER , 0, 12);

  for (int i = NUM_PIXELS / 2; i > maxAverageTemp ; i--) {
    strip.setPixelColor(i, strip.Color(5, 5, 5));
    strip.setPixelColor(24 - i, strip.Color(5, 5, 5));
  }

  tempVolumToNeopixel -= 15;
  //maxAverageTemp--;
}


void taskUnit() {
  // 1ms 당 한 번씩 푸시를 하는 것이다. 즉, 샘플링은 1khz 라는거구나. 근데 이게 맞는건지 다시 검증하자.
  //틀렸다. 2.048ms 당 한 번씩 푸시한다.

  int micAnalogValue = analogRead(PIN_MIC_ANALOG);
  queue.push(abs(micAnalogValue - 512));
  stdQueue.push(queue.average());

  if (wheelSpeedCount ++ > 5) {
    wheelSpeedCount  = 0;
    if (!isSavingMode) {
      ledFunction();
    }
  }

  // 1000 사이클마다 체크 : 살아 있는지 죽었는지.
  // 1 1 : 살아있음
  // 0 0 : 죽었음 
  
  

  if (stateCheckCount ++ > 500 ) {
    stateCheckCount = 0;

    if (stdQueue.average() > 10) {
      soundStateCheckFlag[0] = true;
    } else {
      soundStateCheckFlag[0] = false;
    }

    if (soundStateCheckFlag[0] && soundStateCheckFlag[1]) {
      //살았음
      isSavingMode = false;
      firstDeadTime = true;

    } else if (soundStateCheckFlag[0] == false && soundStateCheckFlag[1] == false) {
      //죽었음

      isSavingMode = true;
      if (firstDeadTime) {
        ledFadeOut();
        firstDeadTime = false;
      }
    }
    soundStateCheckFlag[1] = soundStateCheckFlag[0];
  }


  
}



bool soundFlag = true;
long lastActivatedTime = 0;
uint16_t prevStd = 0;
uint16_t activationCount = 0;
uint16_t wakeCount = 0;
uint16_t lastThreshTime = 0;  ;

long lastTime = 0;
long currentTime = 0;
// ============================main ============================
void loop() {

  while (1) {
    // 10ms frequency
    currentTime = millis();
    if (currentTime - lastTime > 3) {
      lastTime = currentTime;
      taskUnit();
      strip.show();
    }
  }
}





//========== All about NEOPIXEL ===================================================================//


//255가지의 색을 나타내는 함수
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

  //  uint8_t w = (uint32_t)((tempColor >> 24) );
  uint8_t r = (uint32_t)((tempColor >> 16) );
  uint8_t g = (uint32_t)((tempColor >> 8) ) ;
  uint8_t b = (uint32_t)((tempColor >> 0 ));

  //  w = (w * brightness) >> 8;
  r = (r * brightness) >> 8;
  g = (g * brightness) >> 8;
  b = (b * brightness) >> 8;
  uint32_t changedColor =
    (((uint32_t)r << 16) ) |
    (((uint32_t)g  << 8) ) |
    (((uint32_t)b  << 0 ));
  return changedColor;
}

void ledFadeOut() {
  for (uint8_t i = 0 ; i < NUM_PIXELS; i ++) {
    strip.setPixelColor(i, 0);
    strip.show();
    delay(50);

  }
}
