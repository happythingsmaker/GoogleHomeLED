#include <Adafruit_NeoPixel.h>

// =============================LED Color effect =====================================================
class LEDeffect {
  private:

    boolean mHighActivated = false;
    int mHighSteps = 0;
    int mHighdMaxSteps = 150;
    int lastHighTime;


    boolean mFadeOutActivated = false;
    int mFadeOutSteps = 0;
    //꺼지는 속도를 조절한다.
    int mMaxFadeOutSteps = 300;
    Adafruit_NeoPixel* strip;

    int beatDuration = 0;
    int beatDurationCount = 0;
    int wheelCountSpeedThreshold = 10;
    int lastLoudTime = 0;

  public:

    LEDeffect(Adafruit_NeoPixel* argStrip) {
      strip = argStrip;
    }

    boolean isActivated() {
      return  mHighActivated;
    }

    void setWheelCountSpeedThreshold(int argThreshold) {
      wheelCountSpeedThreshold = argThreshold;
    }

    int getWheelCountSpeedThreshold() {
      return wheelCountSpeedThreshold;
    }


    void activeHigh() {

      if (!mHighActivated ) {
        mHighActivated = true;
        mHighSteps = 0;

        int currentTime = millis();
        const int rotationRatio = 3;
#define STEP_UNIT	(50)
        //const int stepUnit = 50;

        // 1초 미만으로 다시 오면 빠르게 돌아
        if (currentTime  - lastHighTime  < 1000) {
          setmHighdMaxSteps(STEP_UNIT * 4);
          wheelCountSpeedThreshold = 1;
        } else if (currentTime  - lastHighTime  < 2000) {
          setmHighdMaxSteps(STEP_UNIT * 5 );
          wheelCountSpeedThreshold = 1 + 3 * rotationRatio ;
        } else if (currentTime  - lastHighTime  < 3000) {
          setmHighdMaxSteps(STEP_UNIT * 6);
          wheelCountSpeedThreshold = 1 + 5 * rotationRatio ;
        } else if (currentTime  - lastHighTime  < 4000) {
          setmHighdMaxSteps(STEP_UNIT * 7);
          wheelCountSpeedThreshold = 1 + 7 * rotationRatio ;
        } else {
          setmHighdMaxSteps(STEP_UNIT * 8);
          wheelCountSpeedThreshold = 1 + 9 * rotationRatio ;

        }

        lastHighTime = currentTime;
      }
    }

    void setmHighdMaxSteps(int argMax) {
      mHighdMaxSteps = argMax;
    }

    void activeFadeOut() {
      if (!mFadeOutActivated ) {
        mFadeOutActivated  = true;
        mFadeOutSteps = 0;
      }
    }


    void routine() {

      if (mHighActivated) {
        //                mHighSteps ++;
        mHighSteps += 2;
        //밝아졌다가 어두워지도록 하는 것이다.
        if (mHighSteps  < mHighdMaxSteps / 2) {
          strip->setBrightness((long)(mHighSteps) * 230 / (mHighdMaxSteps / 2)  );
        } else {
          strip->setBrightness((long)(mHighdMaxSteps - mHighSteps) * 230 / (mHighdMaxSteps / 2)  + 10);
          //strip.setBrightness((mHighdMaxSteps  - mHighSteps / 10 * 9 ) * 255 / mHighdMaxSteps  );
        }

        if (mHighSteps >= mHighdMaxSteps) {
          mHighActivated = false;
        }
      }


      // == fade out action
      if (mFadeOutActivated) {
        mFadeOutSteps ++;

        //        strip.setBrightness((double)(mMaxFadeOutSteps - mFadeOutSteps) * 10 / mMaxFadeOutSteps );


        //strip->setPixelColor((long)mFadeOutSteps * strip->numPixels() / mMaxFadeOutSteps  , 0);
        strip->setPixelColor((long)mFadeOutSteps * 24 / mMaxFadeOutSteps  , 0);
        //        Serial.println((long)mFadeOutSteps * strip.numPixels() / mMaxFadeOutSteps);


        if (mFadeOutSteps >= mMaxFadeOutSteps) {
          mFadeOutActivated = false;
        }

      }
    }


};
