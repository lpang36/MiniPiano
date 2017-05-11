int notePins[] = {2,3,4,5,6,7,8,9};
int tones[] = {1915,1700,1519,1432,1275,1136,1014,956};
int recordedNotes[100] = {};
int recordedDurations[100] = {};
//int arraySize = 1;
int count = 0;
int piezoPin = 10;
int playbackPin = 11;
int recordPin = 12;
boolean inRecord = false;
boolean inPlayback = false;
int lastButtonStates[] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};
int buttonStates[] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};
unsigned long lastDebounceTimes[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long debounceDelay = 100;
int lastEvent = 0;

/*
void playNote (int pitch,int duration);
int playNoteFromInput (int i);
void record();
void playback();
*/

void setup () {
        Serial.begin(9600); 
	for (int i = 0; i<8; i++) {
		pinMode(notePins[i],INPUT);
	}
	pinMode(piezoPin,OUTPUT);
	pinMode(playbackPin,INPUT_PULLUP);
	pinMode(recordPin,INPUT_PULLUP);
}

int debounce (int pin) {
  return digitalRead(pin);
}

/*
int debounce (int pin) {
	int value = digitalRead(pin);
	if (value != lastButtonStates[pin]) {
		lastDebounceTimes[pin] = millis();
	}
	if ((millis()-lastDebounceTimes[pin])>debounceDelay) {
		if (value!=lastButtonStates[pin]) {
			lastButtonStates[pin] = value;
		}
		if (lastButtonStates[pin] == HIGH) {
			return HIGH;
		}
	}
	return LOW;
}
*/
void playNote (int pitch, int duration) {
        Serial.print(pitch);
                                Serial.print(" ");
                                Serial.print(duration);
                                Serial.print("\n");
        if (pitch==-1) {
                delay(duration);
                return;
        }
	pinMode(notePins[pitch],OUTPUT);
	digitalWrite(notePins[pitch],HIGH);
        int note = tones[pitch];
	for (long i = 0; i<duration*1000L; i+=note*2) {
		digitalWrite(piezoPin,HIGH);
		delayMicroseconds(note);
		digitalWrite(piezoPin,LOW);
		delayMicroseconds(note);
		if (debounce(playbackPin)==LOW) {
			inPlayback = false;
			digitalWrite(notePins[pitch],LOW);
			pinMode(notePins[pitch],INPUT);
			return;
		}
	}
	digitalWrite(notePins[pitch],LOW);
	pinMode(notePins[pitch],INPUT);
}

void playNoteFromInput (int i) {
	int duration = 0;
	pinMode(notePins[i],OUTPUT);
	digitalWrite(notePins[i],HIGH);
        pinMode(notePins[i],INPUT);
        //while (true) {
	while (debounce(notePins[i]) == HIGH) {
                //Serial.print(tones[i]);
		int pitch = tones[i];
		digitalWrite(piezoPin,HIGH);
		delayMicroseconds(pitch);
		digitalWrite(piezoPin,LOW);
		delayMicroseconds(pitch);
		//duration+=2*pitch;
	}
        pinMode(notePins[i],OUTPUT);
	digitalWrite(notePins[i],LOW);
        pinMode(notePins[i],INPUT);
	//return duration;
}

void record () {
	//arraySize = 1;
	count = 0;
        //int recordedNotes[100];
        //int recordedDurations[100];
	//recordedNotes = int[1];
	//recordedDurations = int[1];
        while (debounce(recordPin)==LOW) {
          Serial.print("limbo\n");
        }
        lastEvent = millis();
        //        Serial.print(lastEvent);
	while (inRecord) {
		int duration = -1;
		int i;
		for (i = 0; i<8; i++) {
			int value = debounce(notePins[i]);
			if (value == HIGH) {
                                //Serial.print(millis());
                                //Serial.print(lastEvent);
                                int duration = millis()-lastEvent;
                                recordedNotes[count] = -1;
                                recordedDurations[count] = duration;
                                //Serial.print(duration);
                                //Serial.print("\n");
                                count++;
                                lastEvent = millis();
				playNoteFromInput(i);
                                duration = millis()-lastEvent;
                                lastEvent = millis();
    		        	recordedNotes[count] = i;
    	        		recordedDurations[count] = duration;
    		        	count++;
			}			
		}
		if (debounce(recordPin)==LOW) {
			inRecord = false;
		}
	}
}

void playback () {
        while (debounce(playbackPin)==LOW) {
                Serial.print("limbo\n");
        }
	for (int i = 0; i<count; i++) {
		playNote (recordedNotes[i],recordedDurations[i]);
		if (!inPlayback) {
			return;
		}
	}
}

void loop () {
        //Serial.print(debounce(recordPin));
	for (int i = 0; i<8; i++) {
		int value = debounce(notePins[i]);
		if (value == HIGH) {
			playNoteFromInput(i);
		}			
	}
	int value = debounce(recordPin);
	if (value == LOW) {
                Serial.print("recording\n");
		inRecord = true;
		record();
                while (debounce(recordPin)==LOW) {
                        Serial.print("end limbo\n");
                }
	}
	value = debounce(playbackPin);
	if (value == LOW) {
                Serial.print("playing\n");
		inPlayback = true;
		playback();
                Serial.print("end check\n");
                while (debounce(playbackPin)==LOW) {
                        Serial.print("end limbo\n");
                }
	}
}
