#!/usr/bin/env python3
import string
import sys

stateWithName = dict(
    MENU=dict(
        msg=[
            'Daniel Shaver',
            'Simulator',
            '',
            'Press any button',
            'to play'
        ],
        stateAfterInput=[
            "START",
            "START",
            "START",
            "START",
            "START",
            "START",
        ],
        timeLimitFrames="60000",
        stateAfterTimeLimitExceeded="MENU",
    ),
    MISTAKE=dict(
        msg=[
            'Officer Langley:',
            "Don't!"
        ],
        stateAfterInput=[
            "BEING_SHOT",
            "BEING_SHOT",
            "BEING_SHOT",
            "BEING_SHOT",
            "BEING_SHOT",
            "BEING_SHOT",
        ],
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="BEING_SHOT",
    ),
    BEING_SHOT=dict(
        msg=[
            ''
        ],
        timeLimitFrames="0",
        stateAfterTimeLimitExceeded="ERROR",
    ),
    GAME_OVER=dict(
        msg=[
            ''
        ],
        timeLimitFrames="0",
        stateAfterTimeLimitExceeded="ERROR",
    ),
    START=dict(
        msg=[
            ''
        ],
        timeLimitFrames="0",
        stateAfterTimeLimitExceeded="ERROR",
    ),
    FIRST_COMMAND=dict(
        msg=[
            'Officer Brailsford:',
            'Stop! Stop!',
            'Hit the A button!',
            'Both of you!',
            'Press the A Button!',
            'Press the A Button!'
        ],
        stateAfterInput=[
            "WAIT",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
        ],
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),
    WAIT=dict(
        msg=[
            ''
        ],
        stateAfterInput=[
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
        ],
        timeLimitFrames="600",
        stateAfterTimeLimitExceeded="WHO_ELSE",
    ),
    WHO_ELSE=dict(
        msg=[
            'The first letter of',
            'the alphabet is B?'
        ],
        stateAfterInput=[
            "NOBODY_ELSE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
        ],
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),
    NOBODY_ELSE=dict(
        msg=[
            'The first letter of',
            'the alphabet is A?'
        ],
        stateAfterInput=[
            "POSITIVE",
            "POSITIVE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
        ],
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),
    POSITIVE=dict(
        msg=[
            'Are you positive?'
        ],
        stateAfterInput=[
            "FTC",
            "FTC",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
        ],
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),
    FTC=dict(
        msg=[
            'OK. Apparently, we',
            'have a failure for',
            'you to comprehend',
            'simple instructions.'
        ],
        stateAfterInput=[
            "AGAIN",
            "AGAIN",
            "AGAIN",
            "AGAIN",
            "AGAIN",
            "AGAIN",
        ],
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="AGAIN",
    ),
    AGAIN=dict(
        msg=[
            "I've got to go over",
            'some of them again.',
            'OK, can you both',
            'hear and understand',
            'me?'
        ],
        stateAfterInput=[
            "THREAT",
            "THREAT",
            "THREAT",
            "THREAT",
            "THREAT",
            "THREAT",
        ],
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),
    THREAT=dict(
        msg=[
            'If you make a mistake',
            'another mistake,',
            "there's a very severe",
            "possibility you're",
            'both going to get',
            'shot. Do you',
            'understand that?'
        ],
        stateAfterInput=[
            "QUESTION",
            "QUESTION",
            "QUESTION",
            "QUESTION",
            "QUESTION",
            "QUESTION",
        ],
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),
    QUESTION=dict(
        msg=[
            "[You try to ask",
            "what's going on]",
        ],
        stateAfterInput=[
            "SHUT_UP",
            "SHUT_UP",
            "SHUT_UP",
            "SHUT_UP",
            "SHUT_UP",
            "SHUT_UP",
        ],
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="SHUT_UP",
    ),
    ERROR=dict(
        msg=[
            "ERROR:",
            "You broke the game."
            "",
            "Please restart your",
            "Arduboy."
        ],
        stateAfterInput=[
            "ERROR",
            "ERROR",
            "ERROR",
            "ERROR",
            "ERROR",
            "ERROR",
        ],
        timeLimitFrames="INT_MAX",
        stateAfterTimeLimitExceeded="ERROR",
    ),
    SHUT_UP=dict(
        msg=[],
        stateAfterInput=[
            "ERROR",
            "ERROR",
            "ERROR",
            "ERROR",
            "ERROR",
            "ERROR",
        ],
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="ERROR",
    ),
)

def main():
    with open(
            "daniel-shaver-sim.ino.template"
        ) as f, open(
            "daniel-shaver-sim.ino", "w"
        ) as out:
        template = string.Template(f.read())
        out.write(
            template.substitute(
                dict(cpp_global=cppFromStates(stateWithName))))

def cppFromStates(stateWithName):
    namesAndStates = list(stateWithName.items())
    template = string.Template("""
$names_states

void printMsgOfState(State state) {
  char *msg = {};
  switch (state) {
$casesPrintMsgOfState
  }
  arduboy.print(msg);
  delete msg;
}

State stateAfterInput(State state, int inputIndex) {
  switch (state) {
$casesStateAfterInput
  default:
    return ERROR;
  }
}

int timeLimitFrames(State state) {
  switch (state) {
$casesTimeLimitFrames
  default:
    return INT_MAX;
  }
}

State stateAfterTimeLimitExceeded(State state) {
  switch (state) {
$casesStateAfterTimeLimitExceeded
  default: return ERROR;
  }
}
""".strip("\n"))
    return template.substitute(
        names_states="\n".join(
            "const State {} = {};".format(n, i)
                for i, (n, s) in enumerate(namesAndStates)),
        casesPrintMsgOfState="\n".join(
            caseFromState(n, s) for n, s in namesAndStates),
        casesStateAfterInput="\n".join(
            caseStateAfterInputOuter(n, s) for n, s in namesAndStates),
        casesTimeLimitFrames="\n".join(
            "  case {}: return {};".format(n, s["timeLimitFrames"])
                for n, s in namesAndStates),
        casesStateAfterTimeLimitExceeded="\n".join(
            "  case {}: return {};".format(n, s["stateAfterTimeLimitExceeded"])
                for n, s in namesAndStates))

def caseFromState(name, state):
    msg = "\n".join(state["msg"]) + "\0"
    return string.Template("""
  case $name:
    msg = new char[$len_msg]{$msg};
    break;
""".strip("\n")).substitute(
        name=name,
        len_msg=len(msg),
        msg=", ".join(r"'\x{:x}'".format(ord(c)) for c in msg))

def caseStateAfterInputOuter(name, state):
    if "stateAfterInput" not in state:
        return ""

    return string.Template("""
  case $name:
    switch (inputIndex) {
$cases
    default: return ERROR;
    }
""".strip("\n")).substitute(
        name=name,
        cases="\n".join(caseStateAfterInputInner(state, i) for i in range(6)))

def caseStateAfterInputInner(state, i):
    return string.Template("""
    case $i:
      return $stateAfterInput;
""".strip("\n")).substitute(i=i, stateAfterInput=state["stateAfterInput"][i])

if __name__ == "__main__":
    main()
