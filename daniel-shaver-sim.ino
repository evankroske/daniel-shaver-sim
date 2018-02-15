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
  3,

  // Addresses of tracks
  0, 0,
  3, 0,
  7, 0,

  // Channels
  0,
  0,
  0,
  1,

  // Track 0 (mute channel)
  0x40, 0,
  0x9F,

  // Track 1 (unused)
  0xFD, 4, 2,
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
  // stop channel
  0xFE,
};

typedef byte State;

const State ERROR = 0;
const State MENU = 1;
const State MISTAKE = 2;
const State BEING_SHOT = 3;
const State GAME_OVER = 4;
const State START = 5;
const State FIRST_COMMAND = 6;
const State WAIT = 7;
const State WAIT_MISTAKE = 8;
const State WHO_ELSE = 9;
const State NOBODY_ELSE = 10;
const State POSITIVE = 11;
const State FTC = 12;
const State AGAIN = 13;
const State THREAT = 14;
const State SHUT_UP = 15;
const State DID_I = 16;
const State UNCROSS_ARMS = 17;
const State PUT_BOTH_OF = 18;
const State PRETZEL = 19;
const State REPEAT_QUESTION = 20;
const State DRUNK = 21;
const State PROBLEMS_UNDERSTANDING = 22;
const State CAN_I_GO = 23;
const State YOU_ARE = 24;
const State YOUNG_MAN = 25;
const State KEEP_FEET_CROSSED = 26;
const State DO_YOU_UNDERSTAND = 27;
const State YOUNG_LADY = 28;
const State YOUNG_LADY_2 = 29;
const State PRE_STOP = 30;
const State STOP = 31;
const State YOUNG_MAN_2 = 32;
const State YOU_ARE_2 = 33;
const State PRE_I_SAID = 34;
const State I_SAID = 35;
const State I_DIDNT_SAY = 36;
const State PRE_PUT_YOUR_HANDS_UP = 37;
const State PUT_YOUR_HANDS_UP = 38;
const State YOU_DO_THAT = 39;
const State THEN_LISTEN = 40;
const State DONT_TALK = 41;
const State HANDS_STRAIGHT = 42;
const State IF_YOUR = 43;
const State YOU_BEGIN = 44;
const State CRAWL = 45;
const State PRE_MISTAKE = 46;

void printMsgOfState(State state) {
  char *msg = {};
  switch (state) {
  case ERROR:
    msg = new char[57]{'\x45', '\x52', '\x52', '\x4f', '\x52', '\x3a', '\xa', '\x59', '\x6f', '\x75', '\x20', '\x62', '\x72', '\x6f', '\x6b', '\x65', '\x20', '\x74', '\x68', '\x65', '\x20', '\x67', '\x61', '\x6d', '\x65', '\x2e', '\xa', '\xa', '\x50', '\x6c', '\x65', '\x61', '\x73', '\x65', '\x20', '\x72', '\x65', '\x73', '\x74', '\x61', '\x72', '\x74', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x41', '\x72', '\x64', '\x75', '\x62', '\x6f', '\x79', '\x2e', '\x0'};
    break;
  case MENU:
    msg = new char[37]{'\x44', '\x61', '\x6e', '\x69', '\x65', '\x6c', '\x20', '\x53', '\x68', '\x61', '\x76', '\x65', '\x72', '\xa', '\x53', '\x69', '\x6d', '\x75', '\x6c', '\x61', '\x74', '\x6f', '\x72', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x50', '\x6c', '\x61', '\x79', '\x0'};
    break;
  case MISTAKE:
    msg = new char[7]{'\x44', '\x6f', '\x6e', '\x27', '\x74', '\x21', '\x0'};
    break;
  case FIRST_COMMAND:
    msg = new char[117]{'\x53', '\x74', '\x6f', '\x70', '\x21', '\x20', '\x53', '\x74', '\x6f', '\x70', '\x21', '\x20', '\x47', '\x65', '\x74', '\x20', '\x6f', '\x6e', '\xa', '\x74', '\x68', '\x65', '\x20', '\x67', '\x72', '\x6f', '\x75', '\x6e', '\x64', '\x21', '\x20', '\x42', '\x6f', '\x74', '\x68', '\x20', '\x6f', '\x66', '\xa', '\x79', '\x6f', '\x75', '\x21', '\x20', '\x4c', '\x61', '\x79', '\x20', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x6f', '\x6e', '\x20', '\x74', '\x68', '\x65', '\xa', '\x67', '\x72', '\x6f', '\x75', '\x6e', '\x64', '\x21', '\x20', '\x4c', '\x61', '\x79', '\x20', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x6f', '\x6e', '\xa', '\x74', '\x68', '\x65', '\x20', '\x67', '\x72', '\x6f', '\x75', '\x6e', '\x64', '\x21', '\xa', '\xa', '\x44', '\x6f', '\x77', '\x6e', '\x3a', '\x20', '\x4c', '\x69', '\x65', '\x20', '\x6f', '\x6e', '\x20', '\x74', '\x68', '\x65', '\xa', '\x67', '\x72', '\x6f', '\x75', '\x6e', '\x64', '\x0'};
    break;
  case WAIT_MISTAKE:
    msg = new char[52]{'\x53', '\x68', '\x75', '\x74', '\x20', '\x75', '\x70', '\x21', '\x20', '\x44', '\x69', '\x64', '\x20', '\x49', '\x20', '\x61', '\x73', '\x6b', '\xa', '\x79', '\x6f', '\x75', '\x20', '\x61', '\x20', '\x71', '\x75', '\x65', '\x73', '\x74', '\x69', '\x6f', '\x6e', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x42', '\x3a', '\x20', '\x22', '\x4e', '\x6f', '\x20', '\x73', '\x69', '\x72', '\x22', '\x0'};
    break;
  case WHO_ELSE:
    msg = new char[42]{'\x57', '\x68', '\x6f', '\x20', '\x65', '\x6c', '\x73', '\x65', '\x20', '\x69', '\x73', '\x20', '\x69', '\x6e', '\x20', '\x74', '\x68', '\x65', '\xa', '\x72', '\x6f', '\x6f', '\x6d', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x42', '\x3a', '\x20', '\x22', '\x4e', '\x6f', '\x62', '\x6f', '\x64', '\x79', '\x22', '\x0'};
    break;
  case NOBODY_ELSE:
    msg = new char[45]{'\x4e', '\x6f', '\x62', '\x6f', '\x64', '\x79', '\x20', '\x65', '\x6c', '\x73', '\x65', '\x20', '\x69', '\x73', '\x20', '\x69', '\x6e', '\x20', '\x74', '\x68', '\x65', '\xa', '\x72', '\x6f', '\x6f', '\x6d', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x42', '\x3a', '\x20', '\x22', '\x4e', '\x6f', '\x62', '\x6f', '\x64', '\x79', '\x22', '\x0'};
    break;
  case POSITIVE:
    msg = new char[56]{'\x41', '\x72', '\x65', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x70', '\x6f', '\x73', '\x69', '\x74', '\x69', '\x76', '\x65', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x42', '\x3a', '\x20', '\x22', '\x4e', '\x6f', '\x62', '\x6f', '\x64', '\x79', '\x2e', '\xa', '\x41', '\x62', '\x73', '\x6f', '\x6c', '\x75', '\x74', '\x65', '\x6c', '\x79', '\x20', '\x70', '\x6f', '\x73', '\x69', '\x74', '\x69', '\x76', '\x65', '\x22', '\x0'};
    break;
  case FTC:
    msg = new char[77]{'\x4f', '\x4b', '\x2e', '\x20', '\x41', '\x70', '\x70', '\x61', '\x72', '\x65', '\x6e', '\x74', '\x6c', '\x79', '\x2c', '\x20', '\x77', '\x65', '\xa', '\x68', '\x61', '\x76', '\x65', '\x20', '\x61', '\x20', '\x66', '\x61', '\x69', '\x6c', '\x75', '\x72', '\x65', '\x20', '\x66', '\x6f', '\x72', '\xa', '\x79', '\x6f', '\x75', '\x20', '\x74', '\x6f', '\x20', '\x63', '\x6f', '\x6d', '\x70', '\x72', '\x65', '\x68', '\x65', '\x6e', '\x64', '\xa', '\x73', '\x69', '\x6d', '\x70', '\x6c', '\x65', '\x20', '\x69', '\x6e', '\x73', '\x74', '\x72', '\x75', '\x63', '\x74', '\x69', '\x6f', '\x6e', '\x73', '\x2e', '\x0'};
    break;
  case AGAIN:
    msg = new char[91]{'\x49', '\x27', '\x76', '\x65', '\x20', '\x67', '\x6f', '\x74', '\x20', '\x74', '\x6f', '\x20', '\x67', '\x6f', '\x20', '\x6f', '\x76', '\x65', '\x72', '\xa', '\x73', '\x6f', '\x6d', '\x65', '\x20', '\x6f', '\x66', '\x20', '\x74', '\x68', '\x65', '\x6d', '\x20', '\x61', '\x67', '\x61', '\x69', '\x6e', '\x2e', '\xa', '\x4f', '\x4b', '\x2c', '\x20', '\x63', '\x61', '\x6e', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x62', '\x6f', '\x74', '\x68', '\xa', '\x68', '\x65', '\x61', '\x72', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\xa', '\x6d', '\x65', '\x3f', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x59', '\x65', '\x73', '\x22', '\x0'};
    break;
  case THREAT:
    msg = new char[146]{'\x49', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x6d', '\x61', '\x6b', '\x65', '\x20', '\x61', '\x20', '\x6d', '\x69', '\x73', '\x74', '\x61', '\x6b', '\x65', '\xa', '\x61', '\x6e', '\x6f', '\x74', '\x68', '\x65', '\x72', '\x20', '\x6d', '\x69', '\x73', '\x74', '\x61', '\x6b', '\x65', '\x2c', '\xa', '\x74', '\x68', '\x65', '\x72', '\x65', '\x27', '\x73', '\x20', '\x61', '\x20', '\x76', '\x65', '\x72', '\x79', '\x20', '\x73', '\x65', '\x76', '\x65', '\x72', '\x65', '\xa', '\x70', '\x6f', '\x73', '\x73', '\x69', '\x62', '\x69', '\x6c', '\x69', '\x74', '\x79', '\x20', '\x79', '\x6f', '\x75', '\x27', '\x72', '\x65', '\xa', '\x62', '\x6f', '\x74', '\x68', '\x20', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\x20', '\x67', '\x65', '\x74', '\xa', '\x73', '\x68', '\x6f', '\x74', '\x2e', '\x20', '\x44', '\x6f', '\x20', '\x79', '\x6f', '\x75', '\xa', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\x20', '\x74', '\x68', '\x61', '\x74', '\x3f', '\xa', '\x41', '\x3a', '\x20', '\x41', '\x73', '\x6b', '\x20', '\x61', '\x20', '\x71', '\x75', '\x65', '\x73', '\x74', '\x69', '\x6f', '\x6e', '\x0'};
    break;
  case SHUT_UP:
    msg = new char[103]{'\x53', '\x68', '\x75', '\x74', '\x20', '\x75', '\x70', '\x21', '\x20', '\x49', '\x27', '\x6d', '\x20', '\x6e', '\x6f', '\x74', '\x20', '\x68', '\x65', '\x72', '\x65', '\xa', '\x74', '\x6f', '\x20', '\x62', '\x65', '\x20', '\x74', '\x61', '\x63', '\x74', '\x66', '\x75', '\x6c', '\x20', '\x6f', '\x72', '\xa', '\x64', '\x69', '\x70', '\x6c', '\x6f', '\x6d', '\x61', '\x74', '\x69', '\x63', '\x20', '\x77', '\x69', '\x74', '\x68', '\x20', '\x79', '\x6f', '\x75', '\x2e', '\xa', '\x59', '\x6f', '\x75', '\x20', '\x6c', '\x69', '\x73', '\x74', '\x65', '\x6e', '\x2c', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x6f', '\x62', '\x65', '\x79', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x43', '\x72', '\x6f', '\x73', '\x73', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x61', '\x72', '\x6d', '\x73', '\x0'};
    break;
  case DID_I:
    msg = new char[75]{'\x46', '\x6f', '\x72', '\x20', '\x6f', '\x6e', '\x65', '\x20', '\x74', '\x68', '\x69', '\x6e', '\x67', '\x2c', '\x20', '\x64', '\x69', '\x64', '\x20', '\x49', '\xa', '\x74', '\x65', '\x6c', '\x6c', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x74', '\x6f', '\x20', '\x6d', '\x6f', '\x76', '\x65', '\x2c', '\xa', '\x79', '\x6f', '\x75', '\x6e', '\x67', '\x20', '\x6d', '\x61', '\x6e', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x55', '\x6e', '\x63', '\x72', '\x6f', '\x73', '\x73', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x61', '\x72', '\x6d', '\x73', '\x0'};
    break;
  case UNCROSS_ARMS:
    msg = new char[26]{'\x59', '\x6f', '\x75', '\x3a', '\xa', '\x4e', '\x6f', '\x20', '\x73', '\x69', '\x72', '\x20', '\x6e', '\x6f', '\x20', '\x73', '\x69', '\x72', '\x20', '\x6e', '\x6f', '\x20', '\x73', '\x69', '\x72', '\x0'};
    break;
  case PUT_BOTH_OF:
    msg = new char[132]{'\x50', '\x75', '\x74', '\x20', '\x62', '\x6f', '\x74', '\x68', '\x20', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x68', '\x61', '\x6e', '\x64', '\x73', '\x20', '\x6f', '\x6e', '\x20', '\x74', '\x6f', '\x70', '\x20', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x68', '\x65', '\x61', '\x64', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x69', '\x6e', '\x74', '\x65', '\x72', '\x6c', '\x61', '\x63', '\x65', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x66', '\x69', '\x6e', '\x67', '\x65', '\x72', '\x73', '\x2e', '\x20', '\x54', '\x61', '\x6b', '\x65', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x66', '\x65', '\x65', '\x74', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x63', '\x72', '\x6f', '\x73', '\x73', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x6c', '\x65', '\x66', '\x74', '\x20', '\x66', '\x6f', '\x6f', '\x74', '\x20', '\x6f', '\x76', '\x65', '\x72', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x72', '\x69', '\x67', '\x68', '\x74', '\x20', '\x66', '\x6f', '\x6f', '\x74', '\x0'};
    break;
  case PRETZEL:
    msg = new char[61]{'\x5b', '\x50', '\x72', '\x65', '\x73', '\x73', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x68', '\x6f', '\x6c', '\x64', '\x20', '\x41', '\x20', '\x61', '\x6e', '\x64', '\xa', '\x42', '\x2c', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x70', '\x72', '\x65', '\x73', '\x73', '\x20', '\x55', '\x70', '\x20', '\x61', '\x6e', '\x64', '\xa', '\x74', '\x68', '\x65', '\x6e', '\x20', '\x44', '\x6f', '\x77', '\x6e', '\x20', '\x74', '\x6f', '\x20', '\x6f', '\x62', '\x65', '\x79', '\x5d', '\x0'};
    break;
  case REPEAT_QUESTION:
    msg = new char[42]{'\x57', '\x68', '\x6f', '\x20', '\x65', '\x6c', '\x73', '\x65', '\x20', '\x69', '\x73', '\x20', '\x69', '\x6e', '\x20', '\x74', '\x68', '\x65', '\xa', '\x72', '\x6f', '\x6f', '\x6d', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x42', '\x3a', '\x20', '\x22', '\x4e', '\x6f', '\x62', '\x6f', '\x64', '\x79', '\x22', '\x0'};
    break;
  case DRUNK:
    msg = new char[42]{'\x41', '\x6c', '\x72', '\x69', '\x67', '\x68', '\x74', '\x2e', '\x20', '\x41', '\x72', '\x65', '\x20', '\x79', '\x6f', '\x75', '\xa', '\x62', '\x6f', '\x74', '\x68', '\x20', '\x64', '\x72', '\x75', '\x6e', '\x6b', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x42', '\x3a', '\x20', '\x22', '\x4e', '\x6f', '\x22', '\x0'};
    break;
  case PROBLEMS_UNDERSTANDING:
    msg = new char[111]{'\x41', '\x6c', '\x72', '\x69', '\x67', '\x68', '\x74', '\x2e', '\x20', '\x53', '\x6f', '\x20', '\x79', '\x6f', '\x75', '\x27', '\x72', '\x65', '\xa', '\x6e', '\x6f', '\x74', '\x20', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\x20', '\x68', '\x61', '\x76', '\x65', '\xa', '\x61', '\x6e', '\x79', '\x20', '\x70', '\x72', '\x6f', '\x62', '\x6c', '\x65', '\x6d', '\x73', '\xa', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\x69', '\x6e', '\x67', '\xa', '\x61', '\x6e', '\x79', '\x74', '\x68', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x68', '\x61', '\x74', '\x20', '\x49', '\x20', '\x74', '\x65', '\x6c', '\x6c', '\xa', '\x79', '\x6f', '\x75', '\x2c', '\x20', '\x72', '\x69', '\x67', '\x68', '\x74', '\x3f', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x43', '\x6f', '\x72', '\x72', '\x65', '\x63', '\x74', '\x22', '\x0'};
    break;
  case CAN_I_GO:
    msg = new char[37]{'\x57', '\x6f', '\x6d', '\x61', '\x6e', '\x20', '\x57', '\x69', '\x74', '\x68', '\x20', '\x59', '\x6f', '\x75', '\x3a', '\xa', '\x43', '\x61', '\x6e', '\x20', '\x49', '\x20', '\x67', '\x6f', '\x20', '\x74', '\x6f', '\x20', '\x6d', '\x79', '\x20', '\x72', '\x6f', '\x6f', '\x6d', '\x3f', '\x0'};
    break;
  case YOU_ARE:
    msg = new char[57]{'\x4e', '\x6f', '\x2c', '\x20', '\x79', '\x6f', '\x75', '\x27', '\x72', '\x65', '\x20', '\x6e', '\x6f', '\x74', '\x20', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\xa', '\x74', '\x6f', '\x20', '\x64', '\x6f', '\x20', '\x61', '\x6e', '\x79', '\x74', '\x68', '\x69', '\x6e', '\x67', '\x20', '\x62', '\x75', '\x74', '\xa', '\x63', '\x6f', '\x6d', '\x65', '\x20', '\x74', '\x6f', '\x77', '\x61', '\x72', '\x64', '\x73', '\x20', '\x75', '\x73', '\x2e', '\x0'};
    break;
  case YOUNG_MAN:
    msg = new char[149]{'\x59', '\x6f', '\x75', '\x6e', '\x67', '\x20', '\x6d', '\x61', '\x6e', '\x2c', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\xa', '\x6e', '\x6f', '\x74', '\x20', '\x74', '\x6f', '\x20', '\x6d', '\x6f', '\x76', '\x65', '\x2e', '\x20', '\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\xa', '\x74', '\x6f', '\x20', '\x70', '\x75', '\x74', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x65', '\x79', '\x65', '\x73', '\xa', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x6c', '\x6f', '\x6f', '\x6b', '\x20', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x61', '\x74', '\xa', '\x74', '\x68', '\x65', '\x20', '\x63', '\x61', '\x72', '\x70', '\x65', '\x74', '\x2e', '\x20', '\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\xa', '\x74', '\x6f', '\x20', '\x6b', '\x65', '\x65', '\x70', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x66', '\x69', '\x6e', '\x67', '\x65', '\x72', '\x73', '\xa', '\x69', '\x6e', '\x74', '\x65', '\x72', '\x6c', '\x61', '\x63', '\x65', '\x64', '\x20', '\x62', '\x65', '\x68', '\x69', '\x6e', '\x64', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x68', '\x65', '\x61', '\x64', '\x2e', '\x0'};
    break;
  case KEEP_FEET_CROSSED:
    msg = new char[146]{'\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\x20', '\x74', '\x6f', '\x20', '\x6b', '\x65', '\x65', '\x70', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x66', '\x65', '\x65', '\x74', '\x20', '\x63', '\x72', '\x6f', '\x73', '\x73', '\x65', '\x64', '\x2e', '\x20', '\x49', '\x66', '\x20', '\x79', '\x6f', '\x75', '\xa', '\x6d', '\x6f', '\x76', '\x65', '\x2c', '\x20', '\x77', '\x65', '\x27', '\x72', '\x65', '\x20', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\xa', '\x63', '\x6f', '\x6e', '\x73', '\x69', '\x64', '\x65', '\x72', '\x20', '\x74', '\x68', '\x61', '\x74', '\x20', '\x61', '\xa', '\x74', '\x68', '\x72', '\x65', '\x61', '\x74', '\x2c', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x77', '\x65', '\x20', '\x61', '\x72', '\x65', '\xa', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\x20', '\x64', '\x65', '\x61', '\x6c', '\x20', '\x77', '\x69', '\x74', '\x68', '\xa', '\x69', '\x74', '\x2c', '\x20', '\x61', '\x6e', '\x64', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x6d', '\x61', '\x79', '\x20', '\x6e', '\x6f', '\x74', '\xa', '\x73', '\x75', '\x72', '\x76', '\x69', '\x76', '\x65', '\x2e', '\x0'};
    break;
  case DO_YOU_UNDERSTAND:
    msg = new char[41]{'\x44', '\x6f', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\x20', '\x6d', '\x65', '\x3f', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x59', '\x65', '\x73', '\x20', '\x73', '\x69', '\x72', '\x22', '\x0'};
    break;
  case YOUNG_LADY:
    msg = new char[135]{'\x59', '\x6f', '\x75', '\x6e', '\x67', '\x20', '\x6c', '\x61', '\x64', '\x79', '\x2c', '\x20', '\x73', '\x68', '\x75', '\x74', '\x20', '\x75', '\x70', '\xa', '\x61', '\x6e', '\x64', '\x20', '\x6c', '\x69', '\x73', '\x74', '\x65', '\x6e', '\x2e', '\x20', '\x41', '\x6c', '\x72', '\x69', '\x67', '\x68', '\x74', '\x2e', '\xa', '\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\x20', '\x74', '\x6f', '\x20', '\x6b', '\x65', '\x65', '\x70', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x66', '\x65', '\x65', '\x74', '\x20', '\x63', '\x72', '\x6f', '\x73', '\x73', '\x65', '\x64', '\x2e', '\x20', '\x54', '\x61', '\x6b', '\x65', '\xa', '\x62', '\x6f', '\x74', '\x68', '\x20', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x68', '\x61', '\x6e', '\x64', '\x73', '\xa', '\x61', '\x6e', '\x64', '\x20', '\x70', '\x75', '\x74', '\x20', '\x74', '\x68', '\x65', '\x6d', '\x20', '\x66', '\x6c', '\x61', '\x74', '\x20', '\x69', '\x6e', '\xa', '\x66', '\x72', '\x6f', '\x6e', '\x74', '\x20', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x2e', '\x0'};
    break;
  case YOUNG_LADY_2:
    msg = new char[129]{'\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\x20', '\x74', '\x6f', '\x20', '\x70', '\x75', '\x73', '\x68', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x73', '\x65', '\x6c', '\x66', '\x20', '\x75', '\x70', '\x20', '\x74', '\x6f', '\x20', '\x61', '\xa', '\x6b', '\x6e', '\x65', '\x65', '\x6c', '\x69', '\x6e', '\x67', '\x20', '\x70', '\x6f', '\x73', '\x69', '\x74', '\x69', '\x6f', '\x6e', '\x2e', '\xa', '\x4b', '\x6e', '\x65', '\x65', '\x6c', '\x69', '\x6e', '\x67', '\x20', '\x70', '\x6f', '\x73', '\x69', '\x74', '\x69', '\x6f', '\x6e', '\x2e', '\xa', '\x4e', '\x6f', '\x77', '\x2c', '\x20', '\x70', '\x75', '\x74', '\x20', '\x62', '\x6f', '\x74', '\x68', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x68', '\x61', '\x6e', '\x64', '\x73', '\x20', '\x69', '\x6e', '\x20', '\x74', '\x68', '\x65', '\x20', '\x61', '\x69', '\x72', '\x20', '\x61', '\x6e', '\x64', '\xa', '\x63', '\x72', '\x61', '\x77', '\x6c', '\x20', '\x74', '\x6f', '\x77', '\x61', '\x72', '\x64', '\x73', '\x20', '\x75', '\x73', '\x2e', '\x0'};
    break;
  case PRE_STOP:
    msg = new char[41]{'\x5b', '\x59', '\x6f', '\x75', '\x20', '\x75', '\x6e', '\x63', '\x72', '\x6f', '\x73', '\x73', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x6c', '\x65', '\x67', '\x73', '\x20', '\x77', '\x69', '\x74', '\x68', '\x6f', '\x75', '\x74', '\xa', '\x74', '\x68', '\x69', '\x6e', '\x6b', '\x69', '\x6e', '\x67', '\x5d', '\x0'};
    break;
  case STOP:
    msg = new char[6]{'\x53', '\x74', '\x6f', '\x70', '\x21', '\x0'};
    break;
  case YOUNG_MAN_2:
    msg = new char[134]{'\x59', '\x6f', '\x75', '\x6e', '\x67', '\x20', '\x6d', '\x61', '\x6e', '\x2c', '\x20', '\x6c', '\x69', '\x73', '\x74', '\x65', '\x6e', '\x20', '\x74', '\x6f', '\xa', '\x6d', '\x79', '\x20', '\x69', '\x6e', '\x73', '\x74', '\x72', '\x75', '\x63', '\x74', '\x69', '\x6f', '\x6e', '\x73', '\x20', '\x61', '\x6e', '\x64', '\xa', '\x64', '\x6f', '\x20', '\x6e', '\x6f', '\x74', '\x20', '\x6d', '\x61', '\x6b', '\x65', '\x20', '\x61', '\xa', '\x6d', '\x69', '\x73', '\x74', '\x61', '\x6b', '\x65', '\x21', '\x20', '\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\x20', '\x74', '\x6f', '\xa', '\x6b', '\x65', '\x65', '\x70', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x6c', '\x65', '\x67', '\x73', '\xa', '\x63', '\x72', '\x6f', '\x73', '\x73', '\x65', '\x64', '\x21', '\x20', '\x44', '\x6f', '\x20', '\x79', '\x6f', '\x75', '\xa', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\x20', '\x6d', '\x65', '\x3f', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x59', '\x65', '\x73', '\x20', '\x73', '\x69', '\x72', '\x22', '\x0'};
    break;
  case YOU_ARE_2:
    msg = new char[116]{'\x59', '\x6f', '\x75', '\x20', '\x61', '\x72', '\x65', '\x20', '\x74', '\x6f', '\x20', '\x70', '\x75', '\x74', '\x20', '\x62', '\x6f', '\x74', '\x68', '\xa', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x68', '\x61', '\x6e', '\x64', '\x73', '\x20', '\x70', '\x61', '\x6c', '\x6d', '\x73', '\xa', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x73', '\x74', '\x72', '\x61', '\x69', '\x67', '\x68', '\x74', '\x20', '\x6f', '\x75', '\x74', '\x20', '\x69', '\x6e', '\xa', '\x66', '\x72', '\x6f', '\x6e', '\x74', '\x20', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x2e', '\x20', '\x50', '\x75', '\x73', '\x68', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x73', '\x65', '\x6c', '\x66', '\x20', '\x75', '\x70', '\x20', '\x74', '\x6f', '\x20', '\x61', '\xa', '\x6b', '\x6e', '\x65', '\x65', '\x6c', '\x69', '\x6e', '\x67', '\x20', '\x70', '\x6f', '\x73', '\x69', '\x74', '\x69', '\x6f', '\x6e', '\x2e', '\x0'};
    break;
  case PRE_I_SAID:
    msg = new char[47]{'\x5b', '\x59', '\x6f', '\x75', '\x20', '\x75', '\x6e', '\x63', '\x72', '\x6f', '\x73', '\x73', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x6c', '\x65', '\x67', '\x73', '\x20', '\x61', '\x67', '\x61', '\x69', '\x6e', '\x20', '\x77', '\x69', '\x74', '\x68', '\x6f', '\x75', '\x74', '\xa', '\x74', '\x68', '\x69', '\x6e', '\x6b', '\x69', '\x6e', '\x67', '\x5d', '\x0'};
    break;
  case I_SAID:
    msg = new char[51]{'\x49', '\x20', '\x53', '\x41', '\x49', '\x44', '\x20', '\x4b', '\x45', '\x45', '\x50', '\x20', '\x59', '\x4f', '\x55', '\x52', '\x20', '\x4c', '\x45', '\x47', '\x53', '\xa', '\x43', '\x52', '\x4f', '\x53', '\x53', '\x45', '\x44', '\x21', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x49', '\x27', '\x6d', '\x20', '\x73', '\x6f', '\x72', '\x72', '\x79', '\x22', '\x0'};
    break;
  case I_DIDNT_SAY:
    msg = new char[38]{'\x49', '\x20', '\x64', '\x69', '\x64', '\x6e', '\x27', '\x74', '\x20', '\x73', '\x61', '\x79', '\x20', '\x74', '\x68', '\x69', '\x73', '\x20', '\x77', '\x61', '\x73', '\xa', '\x61', '\x20', '\x63', '\x6f', '\x6e', '\x76', '\x65', '\x72', '\x73', '\x61', '\x74', '\x69', '\x6f', '\x6e', '\x2d', '\x0'};
    break;
  case PRE_PUT_YOUR_HANDS_UP:
    msg = new char[32]{'\x5b', '\x59', '\x6f', '\x75', '\x20', '\x6c', '\x6f', '\x77', '\x65', '\x72', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x68', '\x61', '\x6e', '\x64', '\x73', '\xa', '\x61', '\x20', '\x6c', '\x69', '\x74', '\x74', '\x6c', '\x65', '\x5d', '\x0'};
    break;
  case PUT_YOUR_HANDS_UP:
    msg = new char[51]{'\x50', '\x55', '\x54', '\x20', '\x59', '\x4f', '\x55', '\x52', '\x20', '\x48', '\x41', '\x4e', '\x44', '\x53', '\x20', '\x55', '\x50', '\x20', '\x49', '\x4e', '\xa', '\x54', '\x48', '\x45', '\x20', '\x41', '\x49', '\x52', '\x21', '\x20', '\x48', '\x41', '\x4e', '\x44', '\x53', '\x20', '\x55', '\x50', '\x20', '\x49', '\x4e', '\xa', '\x54', '\x48', '\x45', '\x20', '\x41', '\x49', '\x52', '\x21', '\x0'};
    break;
  case YOU_DO_THAT:
    msg = new char[88]{'\x59', '\x6f', '\x75', '\x20', '\x64', '\x6f', '\x20', '\x74', '\x68', '\x61', '\x74', '\x20', '\x61', '\x67', '\x61', '\x69', '\x6e', '\xa', '\x77', '\x65', '\x27', '\x72', '\x65', '\x20', '\x73', '\x68', '\x6f', '\x6f', '\x74', '\x69', '\x6e', '\x67', '\x20', '\x79', '\x6f', '\x75', '\x21', '\xa', '\x44', '\x6f', '\x20', '\x79', '\x6f', '\x75', '\x20', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\x3f', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x50', '\x6c', '\x65', '\x61', '\x73', '\x65', '\x20', '\x64', '\x6f', '\x20', '\x6e', '\x6f', '\x74', '\xa', '\x73', '\x68', '\x6f', '\x6f', '\x74', '\x20', '\x6d', '\x65', '\x22', '\x0'};
    break;
  case THEN_LISTEN:
    msg = new char[57]{'\x54', '\x68', '\x65', '\x6e', '\x20', '\x6c', '\x69', '\x73', '\x74', '\x65', '\x6e', '\x20', '\x74', '\x6f', '\x20', '\x6d', '\x79', '\xa', '\x69', '\x6e', '\x73', '\x74', '\x72', '\x75', '\x63', '\x74', '\x69', '\x6f', '\x6e', '\x73', '\x21', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x49', '\x27', '\x6d', '\x20', '\x74', '\x72', '\x79', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\x2d', '\x22', '\x0'};
    break;
  case DONT_TALK:
    msg = new char[20]{'\x44', '\x4f', '\x4e', '\x27', '\x54', '\x20', '\x54', '\x41', '\x4c', '\x4b', '\x21', '\x20', '\x4c', '\x49', '\x53', '\x54', '\x45', '\x4e', '\x21', '\x0'};
    break;
  case HANDS_STRAIGHT:
    msg = new char[140]{'\x48', '\x61', '\x6e', '\x64', '\x73', '\x20', '\x73', '\x74', '\x72', '\x61', '\x69', '\x67', '\x68', '\x74', '\x20', '\x75', '\x70', '\x20', '\x69', '\x6e', '\xa', '\x74', '\x68', '\x65', '\x20', '\x61', '\x69', '\x72', '\x21', '\x20', '\x44', '\x6f', '\x20', '\x6e', '\x6f', '\x74', '\x20', '\x70', '\x75', '\x74', '\xa', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x68', '\x61', '\x6e', '\x64', '\x73', '\x20', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x66', '\x6f', '\x72', '\xa', '\x61', '\x6e', '\x79', '\x20', '\x72', '\x65', '\x61', '\x73', '\x6f', '\x6e', '\x21', '\x20', '\x49', '\x66', '\x20', '\x79', '\x6f', '\x75', '\xa', '\x74', '\x68', '\x69', '\x6e', '\x6b', '\x20', '\x79', '\x6f', '\x75', '\x27', '\x72', '\x65', '\x20', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\xa', '\x66', '\x61', '\x6c', '\x6c', '\x2c', '\x20', '\x79', '\x6f', '\x75', '\x27', '\x64', '\x20', '\x62', '\x65', '\x74', '\x74', '\x65', '\x72', '\xa', '\x66', '\x61', '\x6c', '\x6c', '\x20', '\x6f', '\x6e', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x66', '\x61', '\x63', '\x65', '\x2e', '\x0'};
    break;
  case IF_YOUR:
    msg = new char[113]{'\x49', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x68', '\x61', '\x6e', '\x64', '\x73', '\x20', '\x67', '\x6f', '\x20', '\x62', '\x61', '\x63', '\x6b', '\xa', '\x74', '\x6f', '\x20', '\x74', '\x68', '\x65', '\x20', '\x73', '\x6d', '\x61', '\x6c', '\x6c', '\x20', '\x6f', '\x66', '\x20', '\x79', '\x6f', '\x75', '\x72', '\xa', '\x62', '\x61', '\x63', '\x6b', '\x2c', '\x20', '\x77', '\x65', '\x20', '\x61', '\x72', '\x65', '\x20', '\x67', '\x6f', '\x69', '\x6e', '\x67', '\x20', '\x74', '\x6f', '\xa', '\x73', '\x68', '\x6f', '\x6f', '\x74', '\x20', '\x79', '\x6f', '\x75', '\x21', '\x20', '\x44', '\x6f', '\x20', '\x79', '\x6f', '\x75', '\xa', '\x75', '\x6e', '\x64', '\x65', '\x72', '\x73', '\x74', '\x61', '\x6e', '\x64', '\x20', '\x6d', '\x65', '\x3f', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x22', '\x59', '\x65', '\x73', '\x20', '\x73', '\x69', '\x72', '\x22', '\x0'};
    break;
  case YOU_BEGIN:
    msg = new char[20]{'\x5b', '\x59', '\x6f', '\x75', '\x20', '\x62', '\x65', '\x67', '\x69', '\x6e', '\x20', '\x73', '\x6f', '\x62', '\x62', '\x69', '\x6e', '\x67', '\x5d', '\x0'};
    break;
  case CRAWL:
    msg = new char[59]{'\x43', '\x72', '\x61', '\x77', '\x6c', '\x20', '\x74', '\x6f', '\x77', '\x61', '\x72', '\x64', '\x73', '\x20', '\x6d', '\x65', '\x21', '\xa', '\x43', '\x52', '\x41', '\x57', '\x4c', '\x20', '\x54', '\x4f', '\x57', '\x41', '\x52', '\x44', '\x53', '\x20', '\x4d', '\x45', '\x21', '\xa', '\xa', '\xa', '\xa', '\xa', '\xa', '\x41', '\x3a', '\x20', '\x42', '\x65', '\x67', '\x69', '\x6e', '\x20', '\x63', '\x72', '\x61', '\x77', '\x6c', '\x69', '\x6e', '\x67', '\x0'};
    break;
  case PRE_MISTAKE:
    msg = new char[56]{'\x5b', '\x59', '\x6f', '\x75', '\x20', '\x72', '\x65', '\x61', '\x63', '\x68', '\x20', '\x64', '\x6f', '\x77', '\x6e', '\x20', '\x74', '\x6f', '\xa', '\x70', '\x75', '\x6c', '\x6c', '\x20', '\x75', '\x70', '\x20', '\x79', '\x6f', '\x75', '\x72', '\x20', '\x70', '\x61', '\x6e', '\x74', '\x73', '\xa', '\x77', '\x69', '\x74', '\x68', '\x6f', '\x75', '\x74', '\x20', '\x74', '\x68', '\x69', '\x6e', '\x6b', '\x69', '\x6e', '\x67', '\x5d', '\x0'};
    break;
  default:
    msg = new char[1]{'\x0'};
    break;
  }
  arduboy.print(msg);
  delete msg;
}

State stateAfterInput(State state, int inputIndex) {
  switch (state) {
  case ERROR:
    switch (inputIndex) {
    case 0:
      return ERROR;
    case 1:
      return ERROR;
    case 2:
      return ERROR;
    case 3:
      return ERROR;
    case 4:
      return ERROR;
    case 5:
      return ERROR;
    default: return ERROR;
    }
  case MENU:
    switch (inputIndex) {
    case 0:
      return BEING_SHOT;
    case 1:
      return MENU;
    case 2:
      return MENU;
    case 3:
      return MENU;
    case 4:
      return MENU;
    case 5:
      return MENU;
    default: return ERROR;
    }
  case MISTAKE:
    switch (inputIndex) {
    case 0:
      return BEING_SHOT;
    case 1:
      return BEING_SHOT;
    case 2:
      return BEING_SHOT;
    case 3:
      return BEING_SHOT;
    case 4:
      return BEING_SHOT;
    case 5:
      return BEING_SHOT;
    default: return ERROR;
    }
  case FIRST_COMMAND:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return WAIT;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case WAIT:
    switch (inputIndex) {
    case 0:
      return WAIT_MISTAKE;
    case 1:
      return WAIT_MISTAKE;
    case 2:
      return WAIT_MISTAKE;
    case 3:
      return WAIT_MISTAKE;
    case 4:
      return WAIT_MISTAKE;
    case 5:
      return WAIT_MISTAKE;
    default: return ERROR;
    }
  case WAIT_MISTAKE:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return WAIT;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case WHO_ELSE:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return NOBODY_ELSE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case NOBODY_ELSE:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return POSITIVE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case POSITIVE:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return FTC;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case FTC:
    switch (inputIndex) {
    case 0:
      return AGAIN;
    case 1:
      return AGAIN;
    case 2:
      return AGAIN;
    case 3:
      return AGAIN;
    case 4:
      return AGAIN;
    case 5:
      return AGAIN;
    default: return ERROR;
    }
  case AGAIN:
    switch (inputIndex) {
    case 0:
      return THREAT;
    case 1:
      return THREAT;
    case 2:
      return THREAT;
    case 3:
      return THREAT;
    case 4:
      return THREAT;
    case 5:
      return THREAT;
    default: return ERROR;
    }
  case THREAT:
    switch (inputIndex) {
    case 0:
      return SHUT_UP;
    case 1:
      return SHUT_UP;
    case 2:
      return SHUT_UP;
    case 3:
      return SHUT_UP;
    case 4:
      return SHUT_UP;
    case 5:
      return SHUT_UP;
    default: return ERROR;
    }
  case SHUT_UP:
    switch (inputIndex) {
    case 0:
      return DID_I;
    case 1:
      return DID_I;
    case 2:
      return DID_I;
    case 3:
      return DID_I;
    case 4:
      return DID_I;
    case 5:
      return DID_I;
    default: return ERROR;
    }
  case DID_I:
    switch (inputIndex) {
    case 0:
      return UNCROSS_ARMS;
    case 1:
      return UNCROSS_ARMS;
    case 2:
      return UNCROSS_ARMS;
    case 3:
      return UNCROSS_ARMS;
    case 4:
      return UNCROSS_ARMS;
    case 5:
      return UNCROSS_ARMS;
    default: return ERROR;
    }
  case UNCROSS_ARMS:
    switch (inputIndex) {
    case 0:
      return PUT_BOTH_OF;
    case 1:
      return PUT_BOTH_OF;
    case 2:
      return PUT_BOTH_OF;
    case 3:
      return PUT_BOTH_OF;
    case 4:
      return PUT_BOTH_OF;
    case 5:
      return PUT_BOTH_OF;
    default: return ERROR;
    }
  case PUT_BOTH_OF:
    switch (inputIndex) {
    case 0:
      return PRETZEL;
    case 1:
      return PRETZEL;
    case 2:
      return PRETZEL;
    case 3:
      return PRETZEL;
    case 4:
      return PRETZEL;
    case 5:
      return PRETZEL;
    default: return ERROR;
    }
  case REPEAT_QUESTION:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return DRUNK;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case DRUNK:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return PROBLEMS_UNDERSTANDING;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case PROBLEMS_UNDERSTANDING:
    switch (inputIndex) {
    case 0:
      return CAN_I_GO;
    case 1:
      return CAN_I_GO;
    case 2:
      return CAN_I_GO;
    case 3:
      return CAN_I_GO;
    case 4:
      return CAN_I_GO;
    case 5:
      return CAN_I_GO;
    default: return ERROR;
    }
  case CAN_I_GO:
    switch (inputIndex) {
    case 0:
      return YOU_ARE;
    case 1:
      return YOU_ARE;
    case 2:
      return YOU_ARE;
    case 3:
      return YOU_ARE;
    case 4:
      return YOU_ARE;
    case 5:
      return YOU_ARE;
    default: return ERROR;
    }
  case YOU_ARE:
    switch (inputIndex) {
    case 0:
      return YOUNG_MAN;
    case 1:
      return YOUNG_MAN;
    case 2:
      return YOUNG_MAN;
    case 3:
      return YOUNG_MAN;
    case 4:
      return YOUNG_MAN;
    case 5:
      return YOUNG_MAN;
    default: return ERROR;
    }
  case YOUNG_MAN:
    switch (inputIndex) {
    case 0:
      return KEEP_FEET_CROSSED;
    case 1:
      return KEEP_FEET_CROSSED;
    case 2:
      return KEEP_FEET_CROSSED;
    case 3:
      return KEEP_FEET_CROSSED;
    case 4:
      return KEEP_FEET_CROSSED;
    case 5:
      return KEEP_FEET_CROSSED;
    default: return ERROR;
    }
  case KEEP_FEET_CROSSED:
    switch (inputIndex) {
    case 0:
      return DO_YOU_UNDERSTAND;
    case 1:
      return DO_YOU_UNDERSTAND;
    case 2:
      return DO_YOU_UNDERSTAND;
    case 3:
      return DO_YOU_UNDERSTAND;
    case 4:
      return DO_YOU_UNDERSTAND;
    case 5:
      return DO_YOU_UNDERSTAND;
    default: return ERROR;
    }
  case DO_YOU_UNDERSTAND:
    switch (inputIndex) {
    case 0:
      return YOUNG_LADY;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case YOUNG_LADY:
    switch (inputIndex) {
    case 0:
      return YOUNG_LADY_2;
    case 1:
      return YOUNG_LADY_2;
    case 2:
      return YOUNG_LADY_2;
    case 3:
      return YOUNG_LADY_2;
    case 4:
      return YOUNG_LADY_2;
    case 5:
      return YOUNG_LADY_2;
    default: return ERROR;
    }
  case YOUNG_LADY_2:
    switch (inputIndex) {
    case 0:
      return PRE_STOP;
    case 1:
      return PRE_STOP;
    case 2:
      return PRE_STOP;
    case 3:
      return PRE_STOP;
    case 4:
      return PRE_STOP;
    case 5:
      return PRE_STOP;
    default: return ERROR;
    }
  case PRE_STOP:
    switch (inputIndex) {
    case 0:
      return STOP;
    case 1:
      return STOP;
    case 2:
      return STOP;
    case 3:
      return STOP;
    case 4:
      return STOP;
    case 5:
      return STOP;
    default: return ERROR;
    }
  case STOP:
    switch (inputIndex) {
    case 0:
      return YOUNG_MAN_2;
    case 1:
      return YOUNG_MAN_2;
    case 2:
      return YOUNG_MAN_2;
    case 3:
      return YOUNG_MAN_2;
    case 4:
      return YOUNG_MAN_2;
    case 5:
      return YOUNG_MAN_2;
    default: return ERROR;
    }
  case YOUNG_MAN_2:
    switch (inputIndex) {
    case 0:
      return YOU_ARE_2;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case YOU_ARE_2:
    switch (inputIndex) {
    case 0:
      return PRE_I_SAID;
    case 1:
      return PRE_I_SAID;
    case 2:
      return PRE_I_SAID;
    case 3:
      return PRE_I_SAID;
    case 4:
      return PRE_I_SAID;
    case 5:
      return PRE_I_SAID;
    default: return ERROR;
    }
  case PRE_I_SAID:
    switch (inputIndex) {
    case 0:
      return I_SAID;
    case 1:
      return I_SAID;
    case 2:
      return I_SAID;
    case 3:
      return I_SAID;
    case 4:
      return I_SAID;
    case 5:
      return I_SAID;
    default: return ERROR;
    }
  case I_SAID:
    switch (inputIndex) {
    case 0:
      return I_DIDNT_SAY;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case I_DIDNT_SAY:
    switch (inputIndex) {
    case 0:
      return PRE_PUT_YOUR_HANDS_UP;
    case 1:
      return PRE_PUT_YOUR_HANDS_UP;
    case 2:
      return PRE_PUT_YOUR_HANDS_UP;
    case 3:
      return PRE_PUT_YOUR_HANDS_UP;
    case 4:
      return PRE_PUT_YOUR_HANDS_UP;
    case 5:
      return PRE_PUT_YOUR_HANDS_UP;
    default: return ERROR;
    }
  case PRE_PUT_YOUR_HANDS_UP:
    switch (inputIndex) {
    case 0:
      return PUT_YOUR_HANDS_UP;
    case 1:
      return PUT_YOUR_HANDS_UP;
    case 2:
      return PUT_YOUR_HANDS_UP;
    case 3:
      return PUT_YOUR_HANDS_UP;
    case 4:
      return PUT_YOUR_HANDS_UP;
    case 5:
      return PUT_YOUR_HANDS_UP;
    default: return ERROR;
    }
  case PUT_YOUR_HANDS_UP:
    switch (inputIndex) {
    case 0:
      return YOU_DO_THAT;
    case 1:
      return YOU_DO_THAT;
    case 2:
      return YOU_DO_THAT;
    case 3:
      return YOU_DO_THAT;
    case 4:
      return YOU_DO_THAT;
    case 5:
      return YOU_DO_THAT;
    default: return ERROR;
    }
  case YOU_DO_THAT:
    switch (inputIndex) {
    case 0:
      return THEN_LISTEN;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case THEN_LISTEN:
    switch (inputIndex) {
    case 0:
      return DONT_TALK;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case DONT_TALK:
    switch (inputIndex) {
    case 0:
      return HANDS_STRAIGHT;
    case 1:
      return HANDS_STRAIGHT;
    case 2:
      return HANDS_STRAIGHT;
    case 3:
      return HANDS_STRAIGHT;
    case 4:
      return HANDS_STRAIGHT;
    case 5:
      return HANDS_STRAIGHT;
    default: return ERROR;
    }
  case HANDS_STRAIGHT:
    switch (inputIndex) {
    case 0:
      return IF_YOUR;
    case 1:
      return IF_YOUR;
    case 2:
      return IF_YOUR;
    case 3:
      return IF_YOUR;
    case 4:
      return IF_YOUR;
    case 5:
      return IF_YOUR;
    default: return ERROR;
    }
  case IF_YOUR:
    switch (inputIndex) {
    case 0:
      return YOU_BEGIN;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case YOU_BEGIN:
    switch (inputIndex) {
    case 0:
      return CRAWL;
    case 1:
      return CRAWL;
    case 2:
      return CRAWL;
    case 3:
      return CRAWL;
    case 4:
      return CRAWL;
    case 5:
      return CRAWL;
    default: return ERROR;
    }
  case CRAWL:
    switch (inputIndex) {
    case 0:
      return PRE_MISTAKE;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  case PRE_MISTAKE:
    switch (inputIndex) {
    case 0:
      return MISTAKE;
    case 1:
      return MISTAKE;
    case 2:
      return MISTAKE;
    case 3:
      return MISTAKE;
    case 4:
      return MISTAKE;
    case 5:
      return MISTAKE;
    default: return ERROR;
    }
  default:
    return ERROR;
  }
}

int timeLimitFrames(State state) {
  switch (state) {
  case ERROR: return INT_MAX;
  case MENU: return INT_MAX;
  case MISTAKE: return 90;
  case FIRST_COMMAND: return 90;
  case WAIT: return 600;
  case WAIT_MISTAKE: return 180;
  case WHO_ELSE: return 180;
  case NOBODY_ELSE: return 180;
  case POSITIVE: return 180;
  case FTC: return 90;
  case AGAIN: return 180;
  case THREAT: return 180;
  case SHUT_UP: return 180;
  case DID_I: return 180;
  case UNCROSS_ARMS: return 180;
  case PUT_BOTH_OF: return 180;
  case REPEAT_QUESTION: return 180;
  case DRUNK: return 180;
  case PROBLEMS_UNDERSTANDING: return 180;
  case CAN_I_GO: return 180;
  case YOU_ARE: return 180;
  case YOUNG_MAN: return 180;
  case KEEP_FEET_CROSSED: return 180;
  case DO_YOU_UNDERSTAND: return 180;
  case YOUNG_LADY: return 180;
  case YOUNG_LADY_2: return 180;
  case PRE_STOP: return 90;
  case STOP: return 90;
  case YOUNG_MAN_2: return 180;
  case YOU_ARE_2: return 180;
  case PRE_I_SAID: return 90;
  case I_SAID: return 180;
  case I_DIDNT_SAY: return 180;
  case PRE_PUT_YOUR_HANDS_UP: return 90;
  case PUT_YOUR_HANDS_UP: return 90;
  case YOU_DO_THAT: return 180;
  case THEN_LISTEN: return 180;
  case DONT_TALK: return 90;
  case HANDS_STRAIGHT: return 180;
  case IF_YOUR: return 180;
  case YOU_BEGIN: return 180;
  case CRAWL: return 180;
  case PRE_MISTAKE: return 90;
  default:
    return 0;
  }
}

State stateAfterTimeLimitExceeded(State state) {
  switch (state) {
  case ERROR: return ERROR;
  case MENU: return MENU;
  case MISTAKE: return BEING_SHOT;
  case FIRST_COMMAND: return MISTAKE;
  case WAIT: return WHO_ELSE;
  case WAIT_MISTAKE: return MISTAKE;
  case WHO_ELSE: return MISTAKE;
  case NOBODY_ELSE: return MISTAKE;
  case POSITIVE: return MISTAKE;
  case FTC: return AGAIN;
  case AGAIN: return MISTAKE;
  case THREAT: return MISTAKE;
  case SHUT_UP: return DID_I;
  case DID_I: return UNCROSS_ARMS;
  case UNCROSS_ARMS: return PUT_BOTH_OF;
  case PUT_BOTH_OF: return PRETZEL;
  case REPEAT_QUESTION: return MISTAKE;
  case DRUNK: return MISTAKE;
  case PROBLEMS_UNDERSTANDING: return CAN_I_GO;
  case CAN_I_GO: return YOU_ARE;
  case YOU_ARE: return YOUNG_MAN;
  case YOUNG_MAN: return KEEP_FEET_CROSSED;
  case KEEP_FEET_CROSSED: return DO_YOU_UNDERSTAND;
  case DO_YOU_UNDERSTAND: return MISTAKE;
  case YOUNG_LADY: return YOUNG_LADY_2;
  case YOUNG_LADY_2: return PRE_STOP;
  case PRE_STOP: return STOP;
  case STOP: return YOUNG_MAN_2;
  case YOUNG_MAN_2: return MISTAKE;
  case YOU_ARE_2: return PRE_I_SAID;
  case PRE_I_SAID: return I_SAID;
  case I_SAID: return MISTAKE;
  case I_DIDNT_SAY: return PRE_PUT_YOUR_HANDS_UP;
  case PRE_PUT_YOUR_HANDS_UP: return PUT_YOUR_HANDS_UP;
  case PUT_YOUR_HANDS_UP: return YOU_DO_THAT;
  case YOU_DO_THAT: return MISTAKE;
  case THEN_LISTEN: return MISTAKE;
  case DONT_TALK: return HANDS_STRAIGHT;
  case HANDS_STRAIGHT: return IF_YOUR;
  case IF_YOUR: return MISTAKE;
  case YOU_BEGIN: return CRAWL;
  case CRAWL: return MISTAKE;
  case PRE_MISTAKE: return MISTAKE;
  default: return ERROR;
  }
}

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

  static byte extraState = 0;

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

  case PRETZEL:
    if (framesSinceLastState > 300) {
      extraState = 0x0;
      framesSinceLastState = 0;
      state = MISTAKE;
      break;
    }
    printMsgOfState(state);
    if (arduboy.pressed(A_BUTTON | B_BUTTON)) {
      extraState |= 0x1;
    }
    if (arduboy.justPressed(UP_BUTTON)) {
      if ((extraState & 0x2) != 0) {
        extraState = 0x0;
        framesSinceLastState = 0;
        state = MISTAKE;
        break;
      }
      extraState |= 0x2;
    }
    if (arduboy.justPressed(DOWN_BUTTON)) {
      if ((extraState & 0x4) != 0 || (extraState & 0x2) == 0) {
        extraState = 0x0;
        framesSinceLastState = 0;
        state = MISTAKE;
        break;
      }
      extraState |= 0x4;
    }
    if (extraState == 0x7) {
      extraState = 0x0;
      framesSinceLastState = 0;
      state = REPEAT_QUESTION;
    }
    break;

  default:
    if (framesSinceLastState > timeLimitFrames(state)) {
      framesSinceLastState = 0;
      state = stateAfterTimeLimitExceeded(state);
    }
    printMsgOfState(state);
    for (int i = 0; i < 6; i++) {
      if (arduboy.justPressed(inputWithId[i])) {
        framesSinceLastState = 0;
        state = stateAfterInput(state, i);
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
