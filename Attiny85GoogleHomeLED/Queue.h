template<class T>
class Queue {

private:
	T* data;
	uint32_t mSize;
	uint16_t iterator = 1;
	T maxStddivValue;
	T minStddivValue;
	int16_t mStddiv;
	int32_t tempSum;
	T tempMinMax;

public:
	Queue(uint8_t pSize) {

		data = (T*) malloc(pSize * sizeof(T));
		mSize = pSize;
	}

	void push(T input) {

		if (iterator == mSize) {
			iterator = 0;
		}

		data[iterator] = input;
		iterator++;
	}

	int32_t sum() {

		tempSum = 0;

		for (uint8_t i = 0; i < mSize; i++) {
			tempSum += data[i];
		}

		return tempSum;
	}

	T average() {

		return (int32_t)(sum() / mSize);
	}

	T stddiv() {

		uint32_t tempSum = average();
		uint32_t tempDiffSumPower = 0;

		for (uint8_t i = 0; i < mSize; i++) {
			tempDiffSumPower += ((tempSum - data[i]) * (tempSum - data[i]));
		}

		T tempStddiv = sqrt(tempDiffSumPower / mSize);

		maxStddivValue = max(maxStddivValue, tempStddiv);
		minStddivValue = min(minStddivValue, tempStddiv);
		mStddiv = tempStddiv;
		return tempStddiv;

	}

	T getStddiv() {

		return mStddiv;
	}

	T getMin() {

		tempMinMax = 9999;
		for (uint32_t i = 0; i < mSize; i++) {
			tempMinMax = min(tempMinMax, data[i]);
		}
		return tempMinMax;
	}

	T getMax() {

		tempMinMax = 0;
		for (uint8_t i = 0; i < mSize; i++) {
			tempMinMax = max(tempMinMax, data[i]);
		}
		return tempMinMax;
	}

	void printQueue() {

		Serial.print("mSize : ");
		Serial.println(mSize);
		Serial.print("iterator : ");
		Serial.println(iterator);
		for (uint32_t i = 0; i < mSize; i++) {
			Serial.print("[");
			Serial.print(data[i]);
			Serial.print("]");
		}
		Serial.println();
	}
};
