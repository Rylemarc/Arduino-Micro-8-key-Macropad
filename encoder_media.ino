#include "HID-Project.h"

#define MEDIA_VOLUME_UP 0xE9
#define MEDIA_VOLUME_DOWN 0xEA
#define MEDIA_PLAY_PAUSE	0xCD
#define MEDIA_NEXT	0xB5
#define MEDIA_PREVIOUS	0xB6

#define CLK 2  // Rotary Encoder CLK (A)
#define DT 3   // Rotary Encoder DT (B)
#define SW 4   // Encoder button

// For macropad
const int keys[9] = {5,6,7,8,9,10,14,15,16}; // Key pins

const int maxDelay = 1000; // Maximum allowed delay between presses
unsigned long first_pressed_millis = 0;
int pressCount = 0;
bool multiplePressCheck = false;

volatile int encoderPos = 0; // Position counter
int lastStateCLK;

// layout count
int modes = 0;

void setup() {

  // Initialise rotary encoder
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);

  // Initialise key switches
  for (int i = 0; i < 9; i++) {
      pinMode(keys[i], INPUT_PULLUP);
  }
  
  Serial.begin(9600);
  
  lastStateCLK = digitalRead(CLK);  // Read initial state of CLK

  // Attach interrupts to handle rotary changes
  attachInterrupt(digitalPinToInterrupt(CLK), updateEncoder, CHANGE);

  Consumer.begin();
}

void loop() {

  // Check button press
  buttonPress();

  // Check multiple button presses
  if (pressCount > 0) {
    first_pressed_millis = millis();
    while (millis() - first_pressed_millis < maxDelay) {
      // Check if button is pressed again
      buttonPress();
    }

    // Check how many times has the encoder been pressed
    switch (pressCount) {
      // If no button pressed after
      case 1:
        Serial.println("Single press");
        Consumer.write(MEDIA_PLAY_PAUSE);
        multiplePressCheck = true;
        break;

      // If button is pressed twice
      case 2:
        Serial.println("Double press");
        Consumer.write(MEDIA_NEXT);
        multiplePressCheck = true;
        break;

      // If button is pressed three times
      case 3:
        Serial.println("Triple press");
        Consumer.write(MEDIA_PREVIOUS);
        multiplePressCheck = true;
        break;
    }

    // Reset the press count to zero
    pressCount = 0;
  }

  // Print encoder position if changed
  static int lastPos = 0;
  if (encoderPos != lastPos) {
      Serial.print("Encoder Position: ");
      Serial.println(encoderPos);
      lastPos = encoderPos;
  }


  // Check for key presses
  for (int i = 0; i < 9; i++) {
    if (digitalRead(keys[i]) == LOW) {  
      // Check for the current mode
      if (modes == 0) {
        layout1(i);
      }
      else if (modes == 1) {
        layout2(i);
      }
      else {
        layout3(i);
      }
      delay(250);
    }
  }
}

// void buttonPress() {
//   // Check button press
//     if (digitalRead(SW) == LOW) {
        // pressCount++; 
        // delay(250); // Debounce delay
//     }
// }

void buttonPress() {
  static unsigned long pressStartTime = 0;
  static bool buttonHeld = false;

  if (digitalRead(SW) == LOW) {
    if (pressStartTime == 0) {
      pressStartTime = millis(); // Record when the button was first pressed
    }

    // If button is held for more than 3 seconds
    if (millis() - pressStartTime > 3000 && !buttonHeld) {
      Serial.println("Mode change");
      modes = (modes + 1) % 3;

      switch (modes) {
        case 0: Keyboard.write(KEYPAD_ADD); break;       // Numpad +
        case 1: Keyboard.write(KEYPAD_SUBTRACT); break;  // Numpad -
        case 2: Keyboard.write(KEYPAD_MULTIPLY); break;  // Numpad *
      }

      buttonHeld = true; // Prevent multiple triggers
    }
    else {
      pressCount++; 
      delay(250); // Debounce delay
    }
  } else {
    pressStartTime = 0; // Reset when button is released
    buttonHeld = false;
  }
}


void updateEncoder() {
    int stateCLK = digitalRead(CLK);
    int stateDT = digitalRead(DT);
    
    if (stateCLK != lastStateCLK) { // Only process when state changes
        if (stateDT != stateCLK) {
          Consumer.write(MEDIA_VOLUME_UP);
          if (encoderPos < 100) { // Check if the encoder is below 100
            encoderPos++;  // Clockwise
            encoderPos++;  // Clockwise
          }
        } 
        else {
          Consumer.write(MEDIA_VOLUME_DOWN);
            if (encoderPos > 0) { // Check if the encoder is above 100
              encoderPos--;  // Counterclockwise
              encoderPos--;  // Counterclockwise
            }
        }
    }
    lastStateCLK = stateCLK;
}

void layout1(int key) {
  switch (key) {
    case 0: Keyboard.write(KEY_F13); break;
    case 1: Keyboard.write(KEY_F14); break;
    case 2: Keyboard.write(KEY_F15); break;
    case 3: Keyboard.write(KEY_F16); break;
    case 4: Keyboard.write(KEY_F17); break;
    case 5: Keyboard.write(KEY_F18); break;
    case 6: Keyboard.write(KEY_F19); break;
    case 7: Keyboard.write(KEY_F20); break;
    case 8: Keyboard.write(KEY_F21); break;
  }
}

void layout2(int key) {
  switch (key) {
    case 0: Keyboard.write(KEYPAD_0); break;
    case 1: Keyboard.write(KEYPAD_1); break;
    case 2: Keyboard.write(KEYPAD_2); break;
    case 3: Keyboard.write(KEYPAD_3); break;
    case 4: Keyboard.write(KEYPAD_4); break;
    case 5: Keyboard.write(KEYPAD_5); break;
    case 6: Keyboard.write(KEYPAD_6); break;
    case 7: Keyboard.write(KEYPAD_7); break;
    case 8: Keyboard.write(KEYPAD_8); break;
  }
}

void layout3(int key) {
  switch (key) {
    case 0: Keyboard.write(KEY_F22); break;
    case 1: Keyboard.write(KEY_F23); break;
    case 2: Keyboard.write(KEY_F24); break;
    case 3: Keyboard.write(KEYPAD_9); break;
    case 4: Keyboard.write(KEYPAD_DOT); break;
    case 5: Keyboard.write(KEYPAD_PIPE); break;
    case 6: Keyboard.write(KEYPAD_AMPERSAND); break;
    case 7: Keyboard.write(KEYPAD_DOUBLEAMPERSAND); break;
    case 8: Keyboard.write(KEYPAD_POUND_SIGN); break;
  }
}

