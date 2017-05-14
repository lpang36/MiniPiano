int notePins[] = {2,3,4,5,6,7,8,9}; //pins used to play notes
int tones[] = {1915,1700,1519,1432,1275,1136,1014,956}; //tones of each note
int recordedNotes[100] = {}; //array with recorded tones
int recordedDurations[100] = {}; //array with recorded durations
int count = 0; //number of recorded notes
int piezoPin = 10; //pin for piezo
int playbackPin = 11; //button for playback
int recordPin = 12; //button for record
boolean inRecord = false; //is the program in record mode
boolean inPlayback = false; //is the program in playback mode
int lastEvent = 0; //time of previous note played

void setup () {
	//initialize all pins
	for (int i = 0; i<8; i++) {
		pinMode(notePins[i],INPUT);
	}
	pinMode(piezoPin,OUTPUT);
	pinMode(playbackPin,INPUT_PULLUP);
	pinMode(recordPin,INPUT_PULLUP);
}

//play note during playback
void playNote (int pitch, int duration) {
	//during a rest, play nothing
    if (pitch==-1) {
        delay(duration);
        return;
    }
    //turn on led for duration of note
	pinMode(notePins[pitch],OUTPUT);
	digitalWrite(notePins[pitch],HIGH); 
    int note = tones[pitch];
    //play given note on piezo
	for (long i = 0; i<duration*1000L; i+=note*2) {
		digitalWrite(piezoPin,HIGH);
		delayMicroseconds(note);
		digitalWrite(piezoPin,LOW);
		delayMicroseconds(note);
		//exit note playing when playback button is pressed again
		if (digitalRead(playbackPin)==LOW) {
			inPlayback = false;
			//turn off led
			digitalWrite(notePins[pitch],LOW);
			pinMode(notePins[pitch],INPUT);
			return;
		}
	}
	//turn off led
	digitalWrite(notePins[pitch],LOW);
	pinMode(notePins[pitch],INPUT);
}

//play note in real time from user input
void playNoteFromInput (int i) {
	//turn on led for duration of note
	pinMode(notePins[i],OUTPUT);
	digitalWrite(notePins[i],HIGH);
    pinMode(notePins[i],INPUT);
    //continue to play note as long as button is pressed
	while (digitalRead(notePins[i]) == HIGH) {
		int pitch = tones[i];
		digitalWrite(piezoPin,HIGH);
		delayMicroseconds(pitch);
		digitalWrite(piezoPin,LOW);
		delayMicroseconds(pitch);
	}
	//turn off led
    pinMode(notePins[i],OUTPUT);
	digitalWrite(notePins[i],LOW);
    pinMode(notePins[i],INPUT);
}

//record user input
void record () {
	count = 0;
	//dummy loop to prevent holding button issue
    while (digitalRead(recordPin)==LOW) {
    }
    lastEvent = millis(); //keep track of start of first note or rest
	while (inRecord) {
		//loop through each button to test for input
		for (int i = 0; i<8; i++) {
			int value = digitalRead(notePins[i]);
			//if button is pressed
			if (value == HIGH) {
				//end previous rest period and input into array
                int duration = millis()-lastEvent;
                recordedNotes[count] = -1;
                recordedDurations[count] = duration;
                count++;
                lastEvent = millis();
                //play note inputted by user
				playNoteFromInput(i);
				//end previous note and input into array
                duration = millis()-lastEvent;
                lastEvent = millis();
    		    recordedNotes[count] = i;
    	        recordedDurations[count] = duration;
    		    count++;
			}			
		}
		//exit record mode if record button is pressed again
		if (digitalRead(recordPin)==LOW) {
			inRecord = false;
		}
	}
}

//play back stored notes
void playback () {
	//dummy loop to prevent button holding issue
    while (digitalRead(playbackPin)==LOW) {
    }
	for (int i = 0; i<count; i++) {
		//play stored note
		playNote (recordedNotes[i],recordedDurations[i]);
		//exit playback mode if playback button is pressed again
		if (!inPlayback) {
			return;
		}
	}
}

void loop () {
	//loop through each button to test for input
	for (int i = 0; i<8; i++) {
		int value = digitalRead(notePins[i]);
		//if button is pressed
		if (value == HIGH) {
			//play note inputted by user
			playNoteFromInput(i);
		}			
	}
	int value = digitalRead(recordPin);
	//if record button is pressed enter record mode
	if (value == LOW) {
		inRecord = true;
		record();
		//dummy loop to prevent button holding issue
        while (digitalRead(recordPin)==LOW) {
        }
	}
	value = digitalRead(playbackPin);
	//if playback button is pressed enter playback mode
	if (value == LOW) {
		inPlayback = true;
		playback();
		//dummy loop to prevent button holding issue
        while (digitalRead(playbackPin)==LOW) {
        }
	}
}
