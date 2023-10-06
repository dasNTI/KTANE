#include <LiquidCrystal.h>
#include <LedControl.h>
#include <SevSeg.h>
#include <DFRobotDFPlayerMini.h>
#include "SoftwareSerial.h"
void(* resetFunc) (void) = 0;

bool activeModules[6] { // wires, keypads, simonSays, password, labyrinth, morse
  false, false,
  false, false,
  false, false
};
bool solvedModules[6] {
  false, false,
  false, false,
  false, false
};
int moduleStatusLedPins[] {
    22, 23, 24, 25, 26, 27
};
int currentMistakes = 0;
bool gameActive = false;
bool gamePaused = false;
int gameTimer = 0;
int gameClockInterval = 1000;
int gameClock = 0;
int timerBeepingTimeout = 0;

SevSeg gameTimeDisplay;
bool gameTimeDisplayOn = true;
#define FirstMistakeLedPin A7
#define SecondMistakeLedPin A6

SoftwareSerial soundSerial(A9, A8); // RX, TX
DFRobotDFPlayerMini dfPlayer;

// Wires
const int wirePins[6] {7, 6, 4, 5, 3, 2};
bool initialWires[6] {
    false, false,
    false, false,
    false, false
};
int wireToBeChanged = 0;

// Keypads
const int keypadBtnPins[4] {49, 50, 53, 46};
const int keypadLedPins[4] {47, 52, 51, 48};
int keypadOrder[4] {3, 2, 1, 0};
int keypadCurrentIndex = 0;
bool keypadBtnPressed = false;

// simonSays
const int simonSaysBtnPins[4] {38, 42, 40, 44};
const int simonSaysLedPins[4] {45, 41, 43, 39};
int simonSaysBlinkSeq[5];
int simonSaysMistake0Seq[5];
int simonSaysMistake1Seq[5];
int simonSaysMistake2Seq[5];
int simonSaysSeqLength = 0;
int simonSaysIndex = 0;
int simonSaysUnlockedCount = 0;
int simonSaysClock = 0;
bool simonSaysTyping = false;
bool simonSaysBtnPressed = false;

// password
//const int passwordBtnPins[10] {23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
//const int passwordLcdPins[6] {33, 34, 35, 36, 37, 38};
//LiquidCrystal passwordLcd = LiquidCrystal(
//    passwordLcdPins[5],
//    passwordLcdPins[4],
//    passwordLcdPins[3],
//    passwordLcdPins[2],
//    passwordLcdPins[1],
//    passwordLcdPins[0]
//);
//char passwordChars[5][6] {
//    {'A', 'B', 'C', 'D', 'E', 'F'},
//    {'A', 'B', 'C', 'D', 'E', 'F'},
//    {'A', 'B', 'C', 'D', 'E', 'F'},
//    {'A', 'B', 'C', 'D', 'E', 'F'},
//    {'A', 'B', 'C', 'D', 'E', 'F'}
//};
//int passwordCharIndeces[5] {0, 0, 0, 0, 0};
//int passwordSolution[5] {0, 0, 0, 0, 0};
//bool passwordBtnPressed = false;
//#define passwordSubmitPin 3

// labyrinth
const int labyrinthBtnPins[4] {28, 30, 32, 34};
LedControl labyrinthMatrix = LedControl(31, 33, 35, 1);
const byte labyrinthMazes[9][8][8] {
    {
        {B0110, B0101, B0101, B0111, B0011, B0010, B0110, B0011},
        {B1100, B0101, B0011, B1010, B1100, B1011, B1010, B1000},
        {B0110, B0011, B1010, B1100, B0111, B1001, B1110, B0011},
        {B1010, B1010, B1110, B0011, B1110, B0101, B1001, B1010},
        {B1000, B1110, B1001, B1100, B1011, B0101, B0101, B1011},
        {B0110, B1101, B0101, B0011, B1010, B1100, B0011, B1010},
        {B1110, B0011, B0100, B1101, B1101, B0111, B1001, B1010},
        {B1000, B1100, B0101, B0101, B0001, B1100, B0101, B1001}
    },
    {
        {B0110, B0101, B0101, B0011, B0110, B0001, B0110, B0011},
        {B1100, B0011, B0100, B1011, B1010, B0110, B1001, B1010},
        {B0110, B1101, B0011, B1110, B1101, B1011, B0100, B1011},
        {B1100, B0011, B1010, B1110, B0011, B1110, B0101, B1001},
        {B0110, B1001, B1010, B1000, B1110, B1101, B0101, B0011},
        {B1010, B0110, B1101, B0101, B1011, B0110, B0011, B1010},
        {B1110, B1011, B0110, B0111, B1101, B1001, B1010, B1010},
        {B1000, B1100, B1001, B1000, B0100, B0101, B1101, B1001}
    },
    {
        {B0110, B0111, B0011, B0110, B0101, B0011, B0110, B0011},
        {B1010, B1010, B1100, B1101, B0011, B1100, B1001, B1010},
        {B1110, B1101, B0101, B0011, B1110, B0111, B0101, B1011},
        {B1100, B0011, B0110, B1001, B1000, B1100, B0011, B1010},
        {B0010, B1010, B1110, B0101, B0011, B0110, B1011, B1010},
        {B1010, B1100, B1101, B0011, B1010, B1010, B1010, B1010},
        {B1010, B0110, B0011, B1110, B1001, B1010, B1010, B1010},
        {B1100, B1001, B1100, B1001, B0100, B1001, B1100, B1001}
    },
    {
        {B0110, B0111, B0101, B0111, B0111, B0101, B0101, B0011},
        {B1010, B1100, B0011, B1000, B1110, B0011, B0110, B1001},
        {B1100, B0011, B1110, B0101, B1011, B1010, B1100, B0011},
        {B0010, B1110, B1011, B0110, B1001, B1110, B0011, B1010},
        {B1100, B1001, B1010, B1110, B0101, B1001, B1010, B1010},
        {B0110, B0011, B1100, B1111, B0011, B0110, B1001, B1010},
        {B1010, B1100, B0101, B1001, B1010, B1100, B0101, B1011},
        {B1100, B0101, B0101, B0001, B1100, B0101, B0101, B1001}
    },
    {
        {B0110, B0101, B0101, B0101, B0111, B0101, B0101, B0011},
        {B1110, B0111, B0011, B0110, B1011, B0110, B0111, B1001},
        {B1000, B1010, B1110, B1001, B1100, B1011, B1100, B0011},
        {B0110, B1001, B1110, B0101, B0011, B1000, B0110, B1011},
        {B1110, B0001, B1010, B0010, B1110, B0011, B1010, B1010},
        {B1110, B0101, B1101, B1011, B1010, B1110, B1001, B1010},
        {B1010, B0110, B0111, B1111, B1001, B1010, B0110, B1011},
        {B1100, B1001, B1000, B1100, B0001, B1100, B1001, B1000}
    },
    {
        {B0110, B0001, B0110, B0011, B0100, B0101, B0101, B0011},
        {B1100, B0111, B1011, B1100, B0111, B0101, B0101, B1011},
        {B0110, B1001, B1100, B0101, B1101, B0011, B0110, B1001},
        {B1110, B0111, B0111, B0101, B0101, B1011, B1100, B0011},
        {B1010, B1010, B1010, B0110, B0011, B1100, B0011, B1010},
        {B1000, B1010, B1110, B1011, B1010, B0110, B1001, B1010},
        {B0110, B1001, B1000, B1010, B1010, B1010, B0110, B1011},
        {B1100, B1001, B1000, B1100, B0001, B1100, B1001, B1000}
    },
    {
        {B0110, B0001, B0110, B0011, B0100, B0101, B0101, B0011},
        {B1100, B0111, B1011, B1100, B0111, B0101, B0101, B1011},
        {B0110, B1001, B1100, B0101, B1101, B0011, B0110, B1001},
        {B1110, B0111, B0111, B0101, B0101, B1011, B1100, B0011},
        {B1010, B1010, B1010, B0110, B0011, B1100, B0011, B1010},
        {B1000, B1010, B1110, B1011, B1010, B0110, B1001, B1010},
        {B0110, B1001, B1000, B1010, B1010, B1010, B0110, B1011},
        {B1100, B0101, B0101, B1001, B1100, B1101, B1001, B1000}
    },
    {
        {B0110, B0011, B0110, B0101, B0101, B0111, B0011, B0010},
        {B1010, B1100, B1001, B0110, B0011, B1010, B1000, B1010},
        {B1010, B0110, B0111, B1011, B1110, B1101, B0101, B1011},
        {B1110, B1001, B1010, B1010, B1010, B0110, B0011, B1010},
        {B1010, B0100, B1111, B1001, B1100, B1011, B1100, B1011},
        {B1100, B0101, B1111, B0101, B0101, B1101, B0111, B1011},
        {B0110, B0101, B1101, B0111, B0101, B0101, B1001, B1010},
        {B1100, B0101, B0001, B1100, B0101, B0101, B0101, B1001}
    },
    {
        {B0110, B0101, B0101, B0111, B0101, B0011, B0100, B0011},
        {B1110, B0111, B0011, B1110, B0001, B1110, B0101, B1001},
        {B1010, B1010, B1100, B1001, B0110, B1101, B0101, B0011},
        {B1010, B1100, B0101, B0011, B1010, B0110, B0101, B1001},
        {B1100, B0111, B0001, B1010, B1010, B1100, B0111, B0011},
        {B0100, B1101, B0011, B1110, B1101, B0111, B1001, B1010},
        {B0110, B0101, B1101, B1101, B0011, B1110, B0111, B1001},
        {B1100, B0101, B0101, B0001, B1000, B1000, B1100, B0001}
    }
};
int labyrinthPlayerX, labyrinthPlayerY, labyrinthGoalX, labyrinthGoalY;
int labyrinthCurrentMaze = 0;
int labyrinthClock = 0;
bool labyrinthBtnPressed = false;
bool labyrinthPlayerVisible = false;

// morse
String morseSeq = "";
int morseSeqLength = 0;
int morseIndex = 0;
int morseSolution = 0;
int morseClock = 0;
bool morseLampStatus = false;
bool morseBtnPressed = false;
#define morseLedPin 36
#define morsePotPin A10
#define morseBtnPin 37


String urlParts[4];
bool displayInfoIdle = false;
int displayInfoIdleIndex = 0;


void setup() {
    Serial.begin(9600);

    for (int i = 0; i < 6; i++) pinMode(moduleStatusLedPins[i], OUTPUT);
    for (int i = 0; i < 6; i++) pinMode(wirePins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(keypadBtnPins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(keypadLedPins[i], OUTPUT);
    for (int i = 0; i < 4; i++) pinMode(simonSaysBtnPins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(simonSaysLedPins[i], OUTPUT);
    //for (int i = 0; i < 10; i++) pinMode(passwordBtnPins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(labyrinthBtnPins[i], INPUT_PULLUP);
    pinMode(morseBtnPin, INPUT_PULLUP);
    pinMode(morsePotPin, INPUT);
    pinMode(morseLedPin, OUTPUT);

    labyrinthMatrix.shutdown(0, false);
    labyrinthMatrix.setIntensity(0, 2);
    //labyrinthMatrix.clearDisplay(0);
    labyrinthMatrix.setRow(0, 1, B01010101);
    
    //passwordLcd.begin(2, 16);
    //pinMode(passwordSubmitPin, INPUT_PULLUP);

    pinMode(FirstMistakeLedPin, OUTPUT);
    pinMode(SecondMistakeLedPin, OUTPUT);
    byte displayPins1[4] {A5, A2, A1, 13};
    byte displayPins2[8] {A4, A0, 11, 9, 8, A3, 12, 10};
    gameTimeDisplay.begin(COMMON_CATHODE, 4, displayPins1, displayPins2, false, false);
    gameTimeDisplay.setBrightness(80);
    gameTimeDisplay.setNumber(1234, 2);

    soundSerial.begin(9600);
  	wait(1000);
    if (!dfPlayer.begin(soundSerial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("df kacke"));
        while(true){
            delay(0);
        }
    }
    dfPlayer.volume(1);
    dfPlayer.play(5);

}

int convert(char c) {
  //if (c == '0') return 0;
  //if (c == '1') return 1;
  //if (c == '2') return 2;
  //if (c == '3') return 3;
  //return 4;
  return ((String) c).toInt();
}
String readSerialUntil(char terminator) {
  char * str;
  while (Serial.peek() != terminator) {
    str += Serial.read();
  }
  Serial.read();
}
void wait(int milliSeconds) {
    for (int i = 0; i < milliSeconds; i += 2) {
        delay(2);
        gameTimeDisplay.refreshDisplay();
    };
    return;
}

void handleSerial() {
    char command = Serial.read();
    wait(1000);

    if (command == 'I') {
        urlParts[0] = Serial.readStringUntil('.');
        urlParts[1] = Serial.readStringUntil('.');
        urlParts[2] = Serial.readStringUntil('.');
        urlParts[3] = Serial.readStringUntil(';');
        displayInfoIdle = true;
        displayInfoIdleIndex = 0;
    }
    if (command == 'B') {
        displayInfoIdle = false;
        gameTimeDisplay.blank();
    }

    if (command == 'R') {
        gameActive = false;
        gamePaused = false;
        for (int i = 0; i < 6; i++) digitalWrite(moduleStatusLedPins[i], LOW);
        for (int i = 0; i < 4; i++) digitalWrite(keypadLedPins[i], LOW);
        for (int i = 0; i < 4; i++) digitalWrite(simonSaysLedPins[i], LOW);
        //for (int i = 0; i < 10; i++) pinMode(passwordBtnPins[i], INPUT_PULLUP);
        digitalWrite(morseLedPin, LOW);

        labyrinthMatrix.clearDisplay(0);
        
        //passwordLcd.begin(2, 16);
        //pinMode(passwordSubmitPin, INPUT_PULLUP);

        digitalWrite(FirstMistakeLedPin, LOW);
        digitalWrite(SecondMistakeLedPin, LOW);
        gameTimeDisplay.blank();
    }

    if (command != 'G') return;

    int time = Serial.readStringUntil('_').toInt() * 30;
    gameClock = 0;
    gameClockInterval = 1000;
    currentMistakes = 0;
    Serial.print("T: ");
    Serial.println(time);

    gameTimeDisplay.setBrightness(1);
    gameTimeDisplay.setNumber(0000, 3);

    for (int i = 0; i < 6; i++) solvedModules[i] = false;

    while (Serial.available()) {
        char module = Serial.read();
        Serial.println(module);

        switch (module) {
            case 'W':
                wireToBeChanged = Serial.readStringUntil('_').toInt();
                wait(10);
                Serial.println(wireToBeChanged);
                for (int i = 0; i < 6; i++) {
                    initialWires[i] = digitalRead(wirePins[i]);
                }
                activeModules[0] = true;
                solvedModules[0] = false;
            break;

            case 'K': {
                String seq = Serial.readStringUntil('_');

                for (int i = 0; i < 4; i++) {
                    int next = ((String) seq.charAt(i)).toInt();
                    wait(10);
                    keypadOrder[i] = next;
                }
                Serial.println(keypadOrder[0]);
                Serial.println(keypadOrder[1]);

                keypadCurrentIndex = 0;
                activeModules[1] = true;
                solvedModules[1] = false;
            }

            case 'S': {
                simonSaysUnlockedCount = 0;

                String blinkSeq = Serial.readStringUntil(',');
                wait(10);
                String mistake0Seq = Serial.readStringUntil(',');
                wait(10);
                String mistake1Seq = Serial.readStringUntil(',');
                wait(10);
                String mistake2Seq = Serial.readStringUntil('_');
                wait(10);
                int length = blinkSeq.length();
                simonSaysSeqLength = length;
                Serial.println(blinkSeq);

                for (int i = 0; i < length; i++) {
                    simonSaysBlinkSeq[i] = ((String) blinkSeq.charAt(i)).toInt();
                    simonSaysMistake0Seq[i] = ((String) mistake0Seq.charAt(i)).toInt();
                    simonSaysMistake1Seq[i] = ((String) mistake1Seq.charAt(i)).toInt();
                    simonSaysMistake2Seq[i] = ((String) mistake2Seq.charAt(i)).toInt();
                }

                for (int i = 0; i < length; i++) {
                  Serial.println(simonSaysBlinkSeq[i]);
                }

                //Serial.println(simonSaysMistake0Seq[0]);
                //Serial.println(simonSaysMistake1Seq[0]);
                //Serial.println(simonSaysMistake2Seq[0]);
                simonSaysIndex = 0;
                activeModules[2] = true;
                solvedModules[2] = false;
            }

            case 'P':
                //for (int i = 0; i < 5; i++) {
                //    passwordSolution[i] = ((String) Serial.read()).toInt();
                //}
                //Serial.read();
                //Serial.println(passwordSolution[0]);
//
                //for (int i = 0; i < 5; i++) {
                //    for (int j = 0; j < 6; j++) {
                //        char opt = Serial.read();
                //        passwordChars[i][j] = opt;
                //    }
                //    Serial.read();
                //}
                //Serial.read();
                //Serial.println(passwordChars[0][0]);
                //activeModules[3] = true;
                //solvedModules[3] = false;
            break;

            case 'L':
                char c;
                c = Serial.read();
                labyrinthCurrentMaze = ((String) c).toInt();
                Serial.println(labyrinthCurrentMaze);
                Serial.read();
                wait(10);
                c = Serial.read();
                labyrinthPlayerX = ((String) c).toInt();
                c = Serial.read();
                labyrinthPlayerY = ((String) c).toInt();
                Serial.read();
                wait(10);
                c = Serial.read();
                labyrinthGoalX = ((String) c).toInt();
                c = Serial.read();
                labyrinthGoalY = ((String) c).toInt();
                Serial.read();
                wait(10);
                Serial.println(labyrinthCurrentMaze);
                activeModules[4] = true;
                solvedModules[4] = false;
            break;

            case 'M': {
                morseIndex = 0;
                morseClock = 0;
                morseLampStatus = false;
                morseBtnPressed = false;

                morseSolution = Serial.readStringUntil(',').toInt();
                wait(10);
                String seq = Serial.readStringUntil('_');
                wait(10);
                morseSeq = seq;
                morseSeqLength = seq.length();
                morseIndex = 0;
                Serial.println(morseSolution);
                Serial.println(morseSeq);
                activeModules[5] = true;
                solvedModules[5] = false;
            }

            default:
                Serial.read();
            break;
        }

        wait(1000);
    }

    wait(2000);
    Serial.println("test");

    timerBeepingTimeout = 2;
    for (int i = 0; i < 4; i++) {
        gameTimer = 4 - i;
        updateClock(true);
        wait(500);
        updateClock(false);
        wait(500);
    }
    gameTimer = time;
    gameActive = true;
    dfPlayer.play(5);
}
void updateClock(bool on) {
    if (on) {
        gameTimeDisplay.setBrightness(90);
    } else {
        gameTimeDisplay.setBrightness(0);
    }

    int minutes = floor(gameTimer / 60);
    int seconds = gameTimer % 60;

    if (minutes != 0) {
        gameTimeDisplay.setNumber(seconds + minutes * 100, 2);
        gameTimeDisplay.refreshDisplay();
        return;
    }

    gameTimeDisplay.setNumber(seconds, 2);
    gameTimeDisplay.refreshDisplay();
}
void increaseMistakes() {
    timerBeepingTimeout = 3;
    dfPlayer.play(3);
    currentMistakes++;
    gameClockInterval -= 250;
    if (currentMistakes == 1) digitalWrite(FirstMistakeLedPin, HIGH);
    if (currentMistakes == 2) digitalWrite(SecondMistakeLedPin, HIGH);
    if (currentMistakes == 3) gamePaused = true;
}
void checkIfDone() {
    for (int i = 0; i < 6; i++) {
        if (solvedModules[i] != activeModules[i]) return;
    }
    gamePaused = true;
    dfPlayer.play(6);
}

void handleWires() {
    for (int i = 0; i < 6; i++) {
        if ((bool) digitalRead(wirePins[i]) == initialWires[i]) continue;
        if (i == wireToBeChanged) {
            solvedModules[0] = true;
            digitalWrite(moduleStatusLedPins[0], HIGH);
            checkIfDone();
            return;
        }

        increaseMistakes();
        initialWires[i] = digitalRead(wirePins[i]);
        return;
    }
}
void handleKeypads() {
    Serial.println(keypadOrder[keypadCurrentIndex]);
    if (keypadBtnPressed) {
        for (int i = 0; i < 4; i++) {
            if (digitalRead(keypadBtnPins[i]) == LOW) return; 
        }
        keypadBtnPressed = false;
    }

    for (int i = 0; i < 4; i++) {
        if (digitalRead(keypadBtnPins[i]) != LOW) continue;
        keypadBtnPressed = true;

        if (i == keypadOrder[keypadCurrentIndex]) {
            digitalWrite(keypadLedPins[i], HIGH);
            keypadCurrentIndex++;
        }else {
            keypadCurrentIndex = 0;
            for (int i = 0; i < 4; i++) digitalWrite(keypadLedPins[i], LOW);
            increaseMistakes();
        }
        return;
    }
}
void handleSimonSays() {
    simonSaysClock += 50;

    if (!simonSaysTyping) {
        if (simonSaysIndex == simonSaysUnlockedCount + 1) {
            if (simonSaysClock > 1250) {
                simonSaysClock = 0;
                simonSaysIndex = 0;
            }
        }else {
            if (simonSaysClock < 500) {
                digitalWrite(simonSaysLedPins[simonSaysBlinkSeq[simonSaysIndex]], HIGH);
            } else {
                digitalWrite(simonSaysLedPins[simonSaysBlinkSeq[simonSaysIndex]], LOW);
                if (simonSaysClock >= 1250) {
                    simonSaysClock = 0;
                    simonSaysIndex++;
                }
            }
        }
    }

    if (simonSaysTyping) {
        if (simonSaysClock == 400) {
            for (int i = 0; i < 4; i++) {
                digitalWrite(simonSaysLedPins[i], LOW);
            }
        }

        if (simonSaysClock > 3000) {
            simonSaysClock = 0;
            simonSaysIndex = 0;
            simonSaysTyping = false;
        }
    }

    if (simonSaysBtnPressed) {
        for (int i = 0; i < 4; i++) {
            if (digitalRead(simonSaysBtnPins[i]) == LOW) return;
        }
        simonSaysBtnPressed = false;
    }

    for (int i = 0; i < 4; i++) {
        if (simonSaysBtnPressed) break;
        if (digitalRead(simonSaysBtnPins[i]) == LOW) {
            Serial.println(i);
            if (!simonSaysTyping) simonSaysIndex = 0;
            simonSaysTyping = true;
            simonSaysClock = 0;
            simonSaysBtnPressed = true;
            digitalWrite(simonSaysLedPins[i], HIGH);

            int seq[5] {0, 0, 0, 0, 0};
            for (int j = 0; j < 5; j++) {
                if (currentMistakes == 0) seq[j] = simonSaysMistake0Seq[j];
                if (currentMistakes == 1) seq[j] = simonSaysMistake1Seq[j];
                if (currentMistakes == 2) seq[j] = simonSaysMistake2Seq[j];
            }

            if (i != seq[simonSaysIndex]) {
                increaseMistakes();
                if (simonSaysIndex != 0) digitalWrite(simonSaysBtnPins[simonSaysIndex - 1], LOW);
                simonSaysUnlockedCount = 1;
                simonSaysIndex = 2;
                simonSaysClock = -1500;
            } else {
                if (simonSaysIndex != 0) {
                    digitalWrite(simonSaysLedPins[
                        seq[simonSaysIndex - 1]
                    ], LOW);
                    wait(25);
                }

                digitalWrite(simonSaysBtnPins[i], HIGH);
                simonSaysIndex++;
                if (simonSaysIndex == simonSaysUnlockedCount + 1) {
                    simonSaysClock = -1000;
                    simonSaysUnlockedCount++;
                    simonSaysIndex++;

                    if (simonSaysUnlockedCount == simonSaysSeqLength) {
                        solvedModules[2] = true;
                        checkIfDone();
                        digitalWrite(moduleStatusLedPins[2], HIGH);
                        for (int i = 0; i < 4; i++) {
                            digitalWrite(simonSaysLedPins[i], LOW);
                        }
                    }
                }
            }
        }
    }
}
void handlePassword() {
    //if (passwordBtnPressed) {
    //    for (int i = 0; i < 10; i++) {
    //        if (digitalRead(passwordBtnPins[i]) == LOW) return;
    //    }
    //    if (digitalRead(passwordSubmitPin) == LOW) return;
    //    passwordBtnPressed = false;
    //}
//
    //for (int i = 0; i < 5; i++) {
    //    int up = digitalRead(passwordBtnPins[i]);
    //    int down = digitalRead(passwordBtnPins[i + 5]);
//
    //    if (up == LOW) {
    //        passwordCharIndeces[i]--;
    //        if (passwordCharIndeces[i] < 0) passwordCharIndeces[i] = 5;
//
    //        char* text = "";
    //        for (int j = 0; j < 5; j++) {
    //            text += passwordChars[j][passwordCharIndeces[j]];
    //        }
    //        passwordLcd.clear();
    //        passwordLcd.setCursor(0, 0);
    //        passwordLcd.println(text);
//
    //        passwordBtnPressed = true;
    //        return;
    //    } else if (down == LOW) {
    //        passwordCharIndeces[i]++;
    //        if (passwordCharIndeces[i] > 5) passwordCharIndeces[i] = 0;
//
    //        char* text = "";
    //        for (int j = 0; j < 5; j++) {
    //            text += passwordChars[j][passwordCharIndeces[j]];
    //        }
    //        passwordLcd.clear();
    //        passwordLcd.setCursor(0, 0);
    //        passwordLcd.println(text);
//
    //        passwordBtnPressed = true;
    //        return;
    //    }
//
    //    if (digitalRead(passwordSubmitPin) == LOW) {
    //        passwordBtnPressed = true;
    //        for (int j = 0; j < 5; j++) {
    //            if (passwordCharIndeces[j] != passwordSolution[j]) {
    //                increaseMistakes();
    //                return;
    //            }
    //        }
//
    //        solvedModules[3] = true;
    //        digitalWrite(moduleStatusLedPins[3], HIGH);
    //    }
    //}
}
void handleLabyrinth() {
    labyrinthClock += 50;
    if (labyrinthClock == 500) {
        labyrinthClock = 0;
        labyrinthPlayerVisible = !labyrinthPlayerVisible;
    }

    if (digitalRead(labyrinthBtnPins[0]) == LOW) {
        if (labyrinthBtnPressed) return;

        byte testByte = labyrinthMazes[labyrinthCurrentMaze][labyrinthPlayerY][labyrinthPlayerX];
        if (testByte & B1000 == 0x0) {
            increaseMistakes();
            return;
        }

        if (labyrinthPlayerY > 0) labyrinthPlayerY -= 1;
        labyrinthBtnPressed = true;
    } else if (digitalRead(labyrinthBtnPins[2]) == LOW) {
        if (labyrinthBtnPressed) return;

        byte testByte = labyrinthMazes[labyrinthCurrentMaze][labyrinthPlayerY][labyrinthPlayerX];
        if (testByte & B0010 == 0x0) {
            increaseMistakes();
            return;
        }

        if (labyrinthPlayerY < 7) labyrinthPlayerY += 1;
        labyrinthBtnPressed = true;
    } else if (digitalRead(labyrinthBtnPins[3]) == LOW) {
        if (labyrinthBtnPressed) return;

        byte testByte = labyrinthMazes[labyrinthCurrentMaze][labyrinthPlayerY][labyrinthPlayerX];
        if (testByte & B0001 == 0x0) {
            increaseMistakes();
            return;
        }

        if (labyrinthPlayerX > 0) labyrinthPlayerX -= 1;
        labyrinthBtnPressed = true;
    } else if (digitalRead(labyrinthBtnPins[1]) == LOW) {
        if (labyrinthBtnPressed) return;

        byte testByte = labyrinthMazes[labyrinthCurrentMaze][labyrinthPlayerY][labyrinthPlayerX];
        if (testByte & B0100 == 0x0) {
            increaseMistakes();
            return;
        }

        if (labyrinthPlayerX < 7) labyrinthPlayerX += 1;
        labyrinthBtnPressed = true;
    } else {
        labyrinthBtnPressed = false;
    }

    int goalXConversion = pow(2, 7 - labyrinthGoalX);
    int playerXConversion = 128;
    for (int i = 0; i < labyrinthPlayerX; i++) playerXConversion /= 2;

    for (int row = 0; row < 8; row++) {
        int dots = 0;
        if (row == labyrinthGoalY && !(labyrinthPlayerX == labyrinthGoalX && labyrinthPlayerY == labyrinthGoalY)) dots += goalXConversion;
        if (row == labyrinthPlayerY && labyrinthPlayerVisible) dots += playerXConversion;
        labyrinthMatrix.setRow(0, row, (byte) dots);
    }

    if (labyrinthPlayerX == labyrinthGoalX && labyrinthPlayerY == labyrinthGoalY) {
        solvedModules[4] = true;
        digitalWrite(moduleStatusLedPins[4], HIGH);
    }
}
void handleMorse() {
    morseClock += 50;

    if (morseBtnPressed) {
        if (digitalRead(morseBtnPin) != LOW) morseBtnPressed = false;
    }

    if (digitalRead(morseBtnPin) == LOW && !morseBtnPressed) {
        int v = analogRead(morsePotPin);
        float freq = (float) v / 1023 * 14;
        freq = ceil(freq - freq / 14);
        if (13 - freq == morseSolution) {
            solvedModules[5] = true;
            digitalWrite(moduleStatusLedPins[5], HIGH);
            digitalWrite(morseLedPin, LOW);
            checkIfDone();
        }else {
            increaseMistakes();
            morseBtnPressed = true;
        }
    }

    if (morseIndex == morseSeqLength) {
        if (morseClock >= 2500) {
            morseClock = 0;
            morseIndex = 0;
        }
        return;
    }

    char c = morseSeq.charAt(morseIndex);
    if (c == '-') {
        if (morseClock >= 1250) {
            morseClock = 0;
            morseIndex++;
        }
        return;
    }

    int blinkLength = 0;
    if (c == '0') blinkLength = 350;
    if (c == '1') blinkLength = 650;

    if (morseClock < blinkLength) {
        digitalWrite(morseLedPin, HIGH);
    }else {
        digitalWrite(morseLedPin, LOW);
        if (morseClock > blinkLength + 250) {
            morseClock = 0;
            morseIndex++;
        }
    } 
}

void loop() {
    if (Serial.available()) handleSerial();
    gameTimeDisplay.refreshDisplay();

    if (displayInfoIdle) {
        switch (displayInfoIdleIndex) {
            case 0:
                gameTimeDisplay.setChars("IP");
                break;
            case 1:
                gameTimeDisplay.setChars(urlParts[0].c_str());
                break;
            case 2:
                gameTimeDisplay.setChars(urlParts[1].c_str());
                break;
            case 3:
                gameTimeDisplay.setChars(urlParts[2].c_str());
                break;
            case 4:
                gameTimeDisplay.setChars(urlParts[3].c_str());
                displayInfoIdleIndex = -1;
                break;
        }
        displayInfoIdleIndex++;
        wait(2000);
    }

    if (!gameActive) return;
    gameClock += 50;
    if (gameClock == gameClockInterval) {
        gameClock = 0;
        gameTimer--;
        if (gameTimer == 0) gamePaused = true;

        if (timerBeepingTimeout == 0) {
            //dfPlayer.play(1);
        } else {
            timerBeepingTimeout--;
        }
    }
    updateClock(true);

    if (gamePaused) {
        if (currentMistakes == 3 || gameTimer == 0) {
            dfPlayer.play(4);
            for (int i = 0; i < 8; i++) {
                updateClock(gameTimeDisplayOn);
                gameTimeDisplayOn = !gameTimeDisplayOn;
                if (Serial.available()) handleSerial();
                wait(500);
            }
            resetFunc();
        }else {
            dfPlayer.play(6);
            while (gamePaused) {
                updateClock(gameTimeDisplayOn);
                gameTimeDisplayOn = !gameTimeDisplayOn;
                if (Serial.available()) handleSerial();
                wait(500);
            }
        }
        return;
    }

    if (activeModules[0] && !solvedModules[0]) handleWires();
    if (activeModules[1] && !solvedModules[1]) handleKeypads();
    if (activeModules[2] && !solvedModules[2]) handleSimonSays();
    if (activeModules[3] && !solvedModules[3]) handlePassword();
    if (activeModules[4] && !solvedModules[4]) handleLabyrinth();
    if (activeModules[5] && !solvedModules[5]) handleMorse();

    wait(50);
}
