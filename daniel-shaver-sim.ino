#include <Arduino.h>
#include <Arduboy2.h>
#include <ATMlib.h>
#include <limits.h>

Arduboy2 arduboy;
ATMsynth synth;
int numFrames = 1;
int framesSurvived = 0;

static const uint8_t PROGMEM gunshot[] = {
  // Number of tracks
  0x03,

  // Addresses of tracks
  0x00, 0x00,
  0x03, 0x00,
  0x06, 0x00,

  // Channels
  0x00,
  0x00,
  0x00,
  // Channel 3, noise
  0x02,

  // Track 0 (mute channel): 3 bytes
  0x40, 0,
  0x9F,

  // Track 1 (unused): 3 bytes
  0x9F,
  0x9F,
  0x9F,

  // Track 2
  // Set tempo to 50
  0x9D, 50,
  // Set volume to 200
  0x40, 200,
  // Start volume slide
  0x41, -16,
  // wait 8 ticks
  0x9F + 8,
  // Slow volume slide
  0x41, -8,
  // wait 128 ticks
  0x9F + 64,
  0x9F + 64,
  // stop channel
  0x9F,
};

typedef byte State;
const State MENU = 0;
const State MISTAKE = 1;
const State BEING_SHOT = 2;
const State GAME_OVER = 3;
const State START = 4;
const State FIRST_COMMAND = 5;
const State SHUT_UP = 16;

static const char *msg[] = {
  // 0: Menu
  "Daniel Shaver\n"
  "Simulator\n\n"
  "Press any button\n"
  "to play",

  // 1: Mistake
  "Officer Langley:\n"
  "Don't!",

  // 2: Being shot
  "",

  // 3: Game over
  "",

  // 4: Start
  "",

  // 5: First command
  "Officer Brailsford:\n"
  "Stop! Stop!\n"
  "Hit the A button!\n"
  "Both of you!\n"
  "Press the A Button!\n"
  "Press the A Button!",

  // 6: Wait for instructions
  "",

  // 7: Who else is in the room?
  "The first letter of\n"
  "the alphabet is B?",

  // 8: Nobody else is in the room?
  "The first letter of\n"
  "the alphabet is A?",

  // 9: Are you positive?
  "Are you positive?",

  // 10: Failure to comprehend instructions
  "OK. Apparently, we\n"
  "have a failure for\n"
  "you to comprehend\n"
  "simple instructions.",

  // 11: Go over again
  "I've got to go over\n"
  "some of them again.\n"
  "OK, can you both\n"
  "hear and understand\n"
  "me?",

  // 12: Threat
  "If you make a mistake\n"
  "another mistake,\n"
  "there's a very severe\n"
  "possibility you're\n"
  "both going to get\n"
  "shot. Do you\n"
  "understand that?",

  // 13: Make the player fail
  "",

};
static const State stateAfterInput[][6] = {
  // 0: Menu
  {
    START,
    START,
    START,
    START,
    START,
    START,
  },

  // 1: Mistake
  {
    BEING_SHOT,
    BEING_SHOT,
    BEING_SHOT,
    BEING_SHOT,
    BEING_SHOT,
    BEING_SHOT,
  },

  // 2: Being shot
  {},

  // 3: Game over
  {},

  // 4: Start
  {},

  // 5: First command
  {
    6,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
  },

  // 6: Wait for instructions
  {
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
  },

  // 7: Who else is in the room?
  {
    8,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
  },

  // 8: Nobody else is in the room?
  {
    9,
    9,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
  },

  // 9: Are you positive?
  {
    10,
    10,
    MISTAKE,
    MISTAKE,
    MISTAKE,
    MISTAKE,
  },

  // 10: Failure to comprehend instructions
  {
    11,
    11,
    11,
    11,
    11,
    11,
  },

  // 11: Go over again
  {
    12,
    12,
    12,
    12,
    12,
    12,
  },

  // 12: Threat
  {
    13,
    13,
    13,
    13,
    13,
    13,
  },

  // 13: Make the player fail
  {
    15,
    15,
    15,
    15,
    15,
    15,
  },

};
static const State timeLimitFrames[] = {
  // 0: Menu
  60000,

  // 1: Mistake
  90,

  // 2: Being shot
  0,

  // 3: Game over
  0,

  // 4: Start
  0,

  // 5: First command
  90,

  // 6: Wait for instructions
  7,

  // 7: Who else is in the room?
  180,

  // 8: Nobody else is in the room?
  180,

  // 9: Are you positive?
  180,

  // 10: Failure to comprehend instructions
  90,

  // 11: Go over again
  180,

  // 12: Threat
  180,

  // 13: Make the player fail
  INT_MAX,

};
static const State stateAfterTimeLimitExceeded[] = {
  // 0: Menu
  MENU,

  // 1: Mistake
  BEING_SHOT,

  // 2: Being shot
  0,

  // 3: Game over
  0,

  // 4: Start
  0,

  // 5: First command
  MISTAKE,

  // 6: Wait for instructions
  7,

  // 7: Who else is in the room?
  MISTAKE,

  // 8: Nobody else is in the room?
  MISTAKE,

  // 9: Are you positive?
  MISTAKE,

  // 10: Failure to comprehend instructions
  11,

  // 11: Go over again
  MISTAKE,

  // 12: Threat
  MISTAKE,

  // 13: Make the player fail
  17,

};


/*
  // 15: Daniel asks a question
  "You tried to ask a\n"
  "question",
  {
    SHUT_UP,
    SHUT_UP,
    SHUT_UP,
    SHUT_UP,
    SHUT_UP,
    SHUT_UP,
  },
  90,
  SHUT_UP,

  // 16: Shut up
  {
    "Shut up! I'm not here\n"
    "to be tactful or\n"
    "diplomatic with you.\n"
    "You listen, you obey",
    {
      14,
      14,
      14,
      14,
      14,
      14,
    },
    180,
    14,
  },
*/

uint8_t inputWithId[] = {
  A_BUTTON,
  B_BUTTON,
  UP_BUTTON,
  RIGHT_BUTTON,
  DOWN_BUTTON,
  LEFT_BUTTON,
};

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.audio.on();

  synth.play(gunshot);
}

State state = MENU;
int framesSinceLastState = 0;

void loop() {
  if (!(arduboy.nextFrame())) return;

  if (numFrames) {
    arduboy.clear();
    arduboy.print("Loading");
    if (numFrames > 260) {
      synth.playPause();
      numFrames = 0;
    } else {
      numFrames++;
    }
    arduboy.display();
    return;
  }

  arduboy.clear();
  arduboy.pollButtons();

  switch (state) {
  case BEING_SHOT:
    synth.playPause();
    framesSinceLastState = 0;
    state = GAME_OVER;
    break;

  case GAME_OVER:
    arduboy.print(
      "Officer Brailsford\n"
      "shot you five times\n"
      "with his AR-15.\n"
      "You are dead.\n\n"
      "You survived for\n");
    arduboy.print(framesSurvived / 60);
    arduboy.print(" seconds.");
    break;

  case START:
    framesSurvived = 0;
    framesSinceLastState = 0;
    state = FIRST_COMMAND;
    break;

  default:
    if (framesSinceLastState > timeLimitFrames[state]) {
      framesSinceLastState = 0;
      state = stateAfterTimeLimitExceeded[state];
    }
    arduboy.print(msg[state]);
    for (int i = 0; i < 6; i++) {
      if (arduboy.justPressed(inputWithId[i])) {
        framesSinceLastState = 0;
        state = stateAfterInput[state][i];
        break;
      }
    }
    break;
  }

  if (state != GAME_OVER) {
    framesSurvived++;
  }
  framesSinceLastState++;
  arduboy.display();
}
