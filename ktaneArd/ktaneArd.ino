#include <LiquidCrystal.h>
#include <LedControl.h>

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
    }
}
int playerX, playerY, goalX, goalY;
int currentMaze = 0;

void setup() {

}

void loop() {

}
