#include <LiquidCrystal.h>
#include <LedControl.h>
#include <SevSeg.h>
#include <DFRobotDFPlayerMini.h>
#include "SoftwareSerial.h"

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
int moduleStatusLedPins[6] {
    0, 1, 2, 3, 4, 5
};
int currentMistakes = 0;
bool gameActive = false;
bool gamePaused = false;
int gameTimer = 0;
int gameClockInterval = 1000;
int gameClock = 0;
bool timerBeeping = true;

SevSeg gameTimeDisplay;
bool gameTimeDisplayOn = true;
#define FirstMistakeLedPin 55
#define SecondMistakeLedPin 56

SoftwareSerial soundSerial(11,10); // RX, TX
DFRobotDFPlayerMini dfPlayer;

// Wires
const int wirePins[6] {1, 2, 3, 4, 5, 6};
bool initialWires[6] {
    false, false,
    false, false,
    false, false
};
int wireToBeChanged = 0;

// Keypads
const int keypadBtnPins[4] {7, 8, 9, 10};
const int keypadLedPins[4] {11, 12, 13, 14};
int keypadOrder[4] {0, 1, 2, 3};
int keypadCurrentIndex = 0;
bool keypadBtnPressed = false;

// simonSays
const int simonSaysBtnPins[4] {15, 16, 17, 18};
const int simonSaysLedPins[4] {19, 20, 21, 22};
int * simonSaysBlinkSeq;
int * simonSaysMistake0Seq;
int * simonSaysMistake1Seq;
int * simonSaysMistake2Seq;
int simonSaysIndex = 0;
int simonSaysUnlockedCount = 0;
int simonSaysClock = 0;
bool simonSaysTyping = false;
bool simonSaysBtnPressed = false;

// password
const int passwordBtnPins[10] {23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
const int passwordLcdPins[6] {33, 34, 35, 36, 37, 38};
LiquidCrystal passwordLcd = LiquidCrystal(
    passwordLcdPins[5],
    passwordLcdPins[4],
    passwordLcdPins[3],
    passwordLcdPins[2],
    passwordLcdPins[1],
    passwordLcdPins[0]
);
char passwordChars[5][6] {
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'A', 'B', 'C', 'D', 'E', 'F'}
};
int passwordCharIndeces[5] {0, 0, 0, 0, 0};
int passwordSolution[5] {0, 0, 0, 0, 0};
bool passwordBtnPressed = false;
#define passwordSubmitPin 3

// labyrinth
const int labyrinthBtnPins[4] {39, 40, 41, 42};
LedControl labyrinthMatrix = LedControl(43, 44, 45, 1);
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
char * morseSeq = "";
int morseSeqLength = 0;
int morseIndex = 0;
int morseSolution = 0;
int morseClock = 0;
bool morseLampStatus = false;
#define morseLedPin 46
#define morsePotPin A0
#define morseBtnPin 47

void setup() {
    for (int i = 0; i < 6; i++) pinMode(wirePins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(keypadBtnPins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(keypadLedPins[i], OUTPUT);
    for (int i = 0; i < 4; i++) pinMode(simonSaysBtnPins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(simonSaysLedPins[i], OUTPUT);
    for (int i = 0; i < 10; i++) pinMode(passwordBtnPins[i], INPUT_PULLUP);
    for (int i = 0; i < 4; i++) pinMode(labyrinthBtnPins[i], INPUT_PULLUP);
    pinMode(morseBtnPin, INPUT_PULLUP);
    pinMode(morsePotPin, INPUT);
    pinMode(morseLedPin, OUTPUT);

    labyrinthMatrix.shutdown(0, false);
    labyrinthMatrix.setIntensity(0, 2);
    labyrinthMatrix.clearDisplay(0);
    
    passwordLcd.begin(2, 16);
    pinMode(passwordSubmitPin, INPUT_PULLUP);

    pinMode(FirstMistakeLedPin, OUTPUT);
    pinMode(SecondMistakeLedPin, OUTPUT);
    byte displayPins1[4] {A5, A2, A1, 13};
    byte displayPins2[8] {A4, A0, 11, 9, 8, A3, 12, 10};
    gameTimeDisplay.begin(COMMON_CATHODE, 4, displayPins1, displayPins2);
    gameTimeDisplay.setBrightness(90);


    if (!dfPlayer.begin(soundSerial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("df kacke"));
        while(true){
            delay(0);
        }
    }
    for (int i = 0; i < 6; i++) pinMode(moduleStatusLedPins[i], OUTPUT);

    Serial.begin(9600);
}

void handleSerial() {
    char command = Serial.read();

    if (command == 'D') {
        String serialNr = Serial.readStringUntil(';');
        String url = Serial.readStringUntil('\n');
        passwordLcd.clear();
        passwordLcd.setCursor(0, 0);
        passwordLcd.print("Seriennr.:" + serialNr);
        passwordLcd.setCursor(0, 1);
        passwordLcd.print(url);

        return;
    }
    if (command == 'A') {
        gameActive = false;
        for (int i = 0; i < 6; i++) {
            solvedModules[i] = false;
            activeModules[i] = false;
        }

        return;
    }

    if (command != 'S') return;

    int time = Serial.readStringUntil('_').toInt() * 30;
    gameClock = 0;
    gameClockInterval = 1000;
    currentMistakes = 0;

    while (Serial.available()) {
        char module = Serial.read();
        Serial.println(module);

        switch (module) {
            case 'W':
                wireToBeChanged = Serial.readStringUntil('_').toInt();
                Serial.print("Wires Lösung: ");
                Serial.println(wireToBeChanged);
                for (int i = 0; i < 6; i++) {
                    initialWires[i] = digitalRead(wirePins[i]);
                }
            break;

            case 'K':
                for (int i = 0; i < 4; i++) {
                    int next = ((String) Serial.read()).toInt();
                    Serial.print("Next Keypad: ");
                    Serial.println(next);
                    keypadOrder[i] = next;
                }
                keypadCurrentIndex = 0;
            break;

            case 'S': {
                String blinkSeq = Serial.readStringUntil(',');
                String mistake0Seq = Serial.readStringUntil(',');
                String mistake1Seq = Serial.readStringUntil(',');
                String mistake2Seq = Serial.readStringUntil('_');
                int length = blinkSeq.length();

                char * blinkSeqC;
                char * mistake0SeqC;
                char * mistake1SeqC;
                char * mistake2SeqC;
                blinkSeq.toCharArray(blinkSeqC, length);
                mistake0Seq.toCharArray(mistake0SeqC, length);
                mistake1Seq.toCharArray(mistake1SeqC, length);
                mistake2Seq.toCharArray(mistake2SeqC, length);

                for (int i = 0; i < length; i++) {
                    simonSaysBlinkSeq[i] = ((String) blinkSeqC[i]).toInt();
                    simonSaysMistake0Seq[i] = ((String) mistake0SeqC[i]).toInt();
                    simonSaysMistake1Seq[i] = ((String) mistake1SeqC[i]).toInt();
                    simonSaysMistake2Seq[i] = ((String) mistake2SeqC[i]).toInt();
                }

                Serial.println(simonSaysBlinkSeq[0]);
                Serial.println(simonSaysMistake0Seq[0]);
                Serial.println(simonSaysMistake1Seq[0]);
                Serial.println(simonSaysMistake2Seq[0]);
                simonSaysIndex = 0;
            }

            case 'P':
                for (int i = 0; i < 5; i++) {
                    passwordSolution[i] = ((String) Serial.read()).toInt();
                }
                Serial.read();
                Serial.println(passwordSolution[0]);

                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 6; j++) {
                        char opt = Serial.read();
                        passwordChars[i][j] = opt;
                    }
                    Serial.read();
                }
                Serial.read();
                Serial.println(passwordChars[0][0]);
            break;

            case 'L':
                labyrinthCurrentMaze = ((String) Serial.read()).toInt();
                Serial.read();
                labyrinthPlayerX = ((String) Serial.read()).toInt();
                labyrinthPlayerY = ((String) Serial.read()).toInt();
                Serial.read();
                labyrinthGoalX = ((String) Serial.read()).toInt();
                labyrinthGoalY = ((String) Serial.read()).toInt();
                Serial.read();
                Serial.println(labyrinthCurrentMaze);
            break;

            case 'M': {
                morseSolution = Serial.readStringUntil(',').toInt();
                String seq = Serial.readStringUntil('_');
                seq.toCharArray(morseSeq, seq.length());
                morseIndex = 0;
                Serial.println(morseSeq);
            }

            default:
                Serial.read();
            break;
        }
    }

    timerBeeping = false;
    for (int i = 0; i < 4; i++) {
        gameTimer = 4 - i;
        updateClock(true);
        delay(500);
        updateClock(false);
        delay(500);
    }
    gameActive = true;
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
        gameTimeDisplay.setNumber(seconds + minutes * 100, 4);
        return;
    }


}
void increaseMistakes() {
    dfPlayer.play(3);
    currentMistakes++;
    gameClockInterval -= 250;
    if (currentMistakes == 1) digitalWrite(FirstMistakeLedPin, HIGH);
    if (currentMistakes == 2) digitalWrite(SecondMistakeLedPin, HIGH);
    if (currentMistakes == 3) gamePaused = true;
}

void handleWires() {
    for (int i = 0; i < 6; i++) {
        if ((bool) digitalRead(wirePins[i]) == initialWires[i]) continue;
        if (i == wireToBeChanged) {
            solvedModules[0] = true;
            digitalWrite(moduleStatusLedPins[0], HIGH);
            return;
        }

        increaseMistakes();
        initialWires[i] = digitalRead(wirePins[i]);
        return;
    }
}
void handleKeypads() {
    if (keypadBtnPressed) {
        for (int i = 0; i < 4; i++) {
            if (digitalRead(keypadBtnPins[i]) == LOW) return; 
        }
        keypadBtnPressed = false;
    }

    for (int i = 0; i < 4; i++) {
        if (digitalRead(keypadBtnPins[i]) != LOW) continue;
        keypadBtnPressed = true;

        if (keypadOrder[keypadCurrentIndex] == i) {
            digitalWrite(keypadLedPins[i], HIGH);
            keypadCurrentIndex++;
        }else {
            keypadCurrentIndex = 0;
            for (int i = 0; i < 4; i++) digitalWrite(keypadLedPins[i], LOW);
            increaseMistakes();
        	return;
        }
    }
}
void handleSimonSays() {
    simonSaysClock += 50;

    if (!simonSaysTyping) {
        if (simonSaysIndex == simonSaysUnlockedCount) {
            if (simonSaysClock > 1500) {
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
        if (simonSaysClock == 750) {
            for (int i = 0; i < 4; i++) {
                digitalWrite(simonSaysBtnPins[i], LOW);
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
            if (!simonSaysTyping) simonSaysIndex = 0;
            simonSaysTyping = true;
            simonSaysClock = 0;

            int * seq;
            if (currentMistakes == 0) seq = simonSaysMistake0Seq;
            if (currentMistakes == 1) seq = simonSaysMistake1Seq;
            if (currentMistakes == 2) seq = simonSaysMistake2Seq;

            if (i != simonSaysMistake0Seq[simonSaysIndex]) {
                increaseMistakes();
                if (simonSaysIndex != 0) digitalWrite(simonSaysBtnPins[simonSaysIndex - 1], LOW);
                simonSaysUnlockedCount = 1;
                simonSaysIndex = 1;
                simonSaysClock = -1500;
            } else {
                if (simonSaysIndex != 0) {
                    digitalWrite(simonSaysLedPins[
                        simonSaysMistake0Seq[simonSaysIndex - 1]
                    ], LOW);
                    delay(25);
                }

                digitalWrite(simonSaysBtnPins[i], HIGH);
                simonSaysIndex++;
                if (simonSaysIndex == simonSaysUnlockedCount) {
                    simonSaysClock = -1000;
                    simonSaysUnlockedCount++;
                    simonSaysIndex++;

                    if (simonSaysUnlockedCount == sizeof(simonSaysBlinkSeq) / sizeof(simonSaysBlinkSeq[0])) {
                        solvedModules[2] = true;
                        digitalWrite(moduleStatusLedPins[2], HIGH);
                    }
                }
            }
        }
    }
}
void handlePassword() {
    if (passwordBtnPressed) {
        for (int i = 0; i < 10; i++) {
            if (digitalRead(passwordBtnPins[i]) == LOW) return;
        }
        if (digitalRead(passwordSubmitPin) == LOW) return;
        passwordBtnPressed = false;
    }

    for (int i = 0; i < 5; i++) {
        int up = digitalRead(passwordBtnPins[i]);
        int down = digitalRead(passwordBtnPins[i + 5]);

        if (up == LOW) {
            passwordCharIndeces[i]--;
            if (passwordCharIndeces[i] < 0) passwordCharIndeces[i] = 5;

            char* text = "";
            for (int j = 0; j < 5; j++) {
                text += passwordChars[j][passwordCharIndeces[j]];
            }
            passwordLcd.clear();
            passwordLcd.setCursor(0, 0);
            passwordLcd.println(text);

            passwordBtnPressed = true;
            return;
        } else if (down == LOW) {
            passwordCharIndeces[i]++;
            if (passwordCharIndeces[i] > 5) passwordCharIndeces[i] = 0;

            char* text = "";
            for (int j = 0; j < 5; j++) {
                text += passwordChars[j][passwordCharIndeces[j]];
            }
            passwordLcd.clear();
            passwordLcd.setCursor(0, 0);
            passwordLcd.println(text);

            passwordBtnPressed = true;
            return;
        }

        if (digitalRead(passwordSubmitPin) == LOW) {
            passwordBtnPressed = true;
            for (int j = 0; j < 5; j++) {
                if (passwordCharIndeces[j] != passwordSolution[j]) {
                    increaseMistakes();
                    return;
                }
            }

            solvedModules[3] = true;
            digitalWrite(moduleStatusLedPins[3], HIGH);
        }
    }
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
    morseClock++;

    if (digitalRead(morseBtnPin) == LOW) {
        int v = analogRead(morsePotPin);
        int freq = round(v / 16);
        if (freq == morseSolution) {
            solvedModules[5] = true;
            digitalWrite(moduleStatusLedPins[5], HIGH);
        }
    }

    if (morseIndex == morseSeqLength) {
        if (morseClock >= 1500) {
            morseClock = 0;
            morseIndex = 0;
        }
        return;
    }

    char c = morseSeq[morseIndex];
    if (c == '-') {
        if (morseClock >= 850) {
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

    static unsigned int timer = millis();
    if (millis() - timer < 50) return;
    timer = millis();

    gameTimeDisplay.refreshDisplay();

    static int timer = millis();
    if (millis() - timer < 50) return;
    timer = millis();

    if (!gameActive) return;
    gameClock += 50;
    if (gameClock == gameClockInterval) {
        gameClock = 0;
        gameTimer--;
        if (timerBeeping) dfPlayer.play(1);
        if (!timerBeeping) {
            dfPlayer.play(5);
            timerBeeping = true;
        }
    }
    updateClock(true);

    if (gamePaused) {
        if (currentMistakes == 3 || gameTimer == 0) {
            dfPlayer.play(4);
        }else {
            dfPlayer.play(6);
        }

        while (gamePaused) {
            updateClock(gameTimeDisplayOn);
            gameTimeDisplayOn = !gameTimeDisplayOn;
            if (Serial.available()) handleSerial();
            delay(500);
        }
        return;
    }

    if (activeModules[0] && !solvedModules[0]) handleWires();
    if (activeModules[1] && !solvedModules[1]) handleKeypads();
    if (activeModules[2] && !solvedModules[2]) handleSimonSays();
    if (activeModules[3] && !solvedModules[3]) handlePassword();
    if (activeModules[4] && !solvedModules[4]) handleLabyrinth();
    if (activeModules[5] && !solvedModules[5]) handleMorse();
}