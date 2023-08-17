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
int currentMistakes = 0;
bool gameActive = false;
bool gameWon = false;
int gameTimer = 0;
int gameClock = 0;

SevSeg gameTimeDisplay;
bool gameTimeDisplayOn = true;

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
const int keypadOrder[4] {0, 1, 2, 3};
const int keypadCurrentIndex = 0;

// simonSays
const int simonSaysBtnPins[4] {15, 16, 17, 18};
const int simonSaysLedPins[4] {19, 20, 21, 22};
const int simonSaysBlinkSeq[4] = {0, 1, 2, 3};
const int simonSaysMistake0Seq[4] = {0, 1, 2, 3};
const int simonSaysMistake1Seq[4] = {0, 1, 2, 3};
const int simonSaysMistake2Seq[4] = {0, 1, 2, 3};

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
int playerX, playerY, goalX, goalY;
int currentMaze = 0;

// morse
char morseSeq[] = "";
int morseSeqLength = 0;
int morseIndex = 0;
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

    byte displayPins1[4] {A1, A2, A3, A4};
    byte displayPins2[8] {A5, A6, A7, A8, A9, A10, A11, A12};
    gameTimeDisplay.begin(COMMON_CATHODE, 4, displayPins1, displayPins2);
    gameTimeDisplay.setBrightness(90);

    if (!dfPlayer.begin(soundSerial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("df kacke"));
        while(true){
            delay(0);
        }
    }

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

    }


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

void handleWires() {

}
void handleKeypads() {

}
void handleSimonSays() {

}
void handlePassword() {

}
void handleLabyrinth() {

}
void handleMorse() {

}

void loop() {
    if (Serial.available()) handleSerial();

    if (!gameActive) return;
    gameClock += 50;
    if (gameClock == 1000) {
        gameClock = 0;
        gameTimer--;
    }
    updateClock(true);

    if (gameWon) {
        while (gameWon) {
            updateClock(gameTimeDisplayOn);
            gameTimeDisplayOn = !gameTimeDisplayOn;
            if (Serial.available()) handleSerial();
            delay(500);
        }
        return;
    }

    void * functions {
        handleWires,
        handleKeypads,
        handleSimonSays,
        handlePassword,
        handleLabyrinth,
        handleMorse
    };
    for (int i = 0; i < 6; i++) {
        if (activeModules[i] && !solvedModules[i]) functions[i]();
    }

    delay(50);
}
