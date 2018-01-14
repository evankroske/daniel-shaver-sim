#!/usr/bin/env python3
import string
import sys

def state(
        msg=None,
        stateAfterInput=None,
        timeLimitFrames=None,
        stateAfterTimeLimitExceeded=None,
        stateNext=None):
    result = {}
    if msg is not None:
        result["msg"] = msg
    if timeLimitFrames is not None:
        result["timeLimitFrames"] = timeLimitFrames

    if stateNext is not None:
        result["stateAfterInput"] = [stateNext] * 6
        result["stateAfterTimeLimitExceeded"] = stateNext
        return result

    if stateAfterInput is not None:
        result["stateAfterInput"] = stateAfterInput
    if stateAfterTimeLimitExceeded is not None:
        result["stateAfterTimeLimitExceeded"] = stateAfterTimeLimitExceeded
    return result

stateWithName = dict(
    ERROR=state(
        msg=[
            "ERROR:",
            "You broke the game."
            "",
            "Please restart your",
            "Arduboy."
        ],
        stateNext="ERROR",
        timeLimitFrames="INT_MAX",
    ),

    MENU=state(
        msg=[
            'Daniel Shaver',
            'Simulator',
            '',
            'Press any button',
            'to play'
        ],
        stateAfterInput=[
            "PRETZEL", # TODO: change back to START
            "START",
            "START",
            "START",
            "START",
            "START",
        ],
        timeLimitFrames="60000",
        stateAfterTimeLimitExceeded="MENU",
    ),

    MISTAKE=state(
        msg=[
            'Officer Langley:',
            "Don't!"
        ],
        stateAfterInput=["BEING_SHOT"] * 6,
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="BEING_SHOT",
    ),

    BEING_SHOT=state(),

    GAME_OVER=state(),

    START=state(),

    FIRST_COMMAND=state(
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

    WAIT=state(
        stateAfterInput=["MISTAKE"] * 6,
        timeLimitFrames="600",
        stateAfterTimeLimitExceeded="WHO_ELSE",
    ),

    WHO_ELSE=state(
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

    NOBODY_ELSE=state(
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

    POSITIVE=state(
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

    FTC=state(
        msg=[
            'OK. Apparently, we',
            'have a failure for',
            'you to comprehend',
            'simple instructions.'
        ],
        stateAfterInput=["AGAIN"] * 6,
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="AGAIN",
    ),

    AGAIN=state(
        msg=[
            "I've got to go over",
            'some of them again.',
            'OK, can you both',
            'hear and understand',
            'me?'
        ],
        stateAfterInput=["THREAT"] * 6,
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    THREAT=state(
        msg=[
            'If you make a mistake',
            'another mistake,',
            "there's a very severe",
            "possibility you're",
            'both going to get',
            'shot. Do you',
            'understand that?'
        ],
        stateAfterInput=["QUESTION"] * 6,
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    QUESTION=state(
        msg=[
            "[You try to ask",
            "what's going on]",
        ],
        stateNext="SHUT_UP",
        timeLimitFrames="90",
    ),

    SHUT_UP=state(
        msg=[
            "Shut up! I'm not here",
            "to be tactful or",
            "diplomatic with you.",
            "You listen, you obey",
        ],
        stateNext="CROSS_ARMS",
        timeLimitFrames="180",
    ),

    CROSS_ARMS=state(
        msg=[
            "[You cross your arms",
            "unconciously]",
        ],
        stateNext="DID_I",
        timeLimitFrames="180",
    ),

    DID_I=state(
        msg=[
            "For one thing, did I",
            "tell you to move,",
            "young man?",
        ],
        stateNext="UNCROSS_ARMS",
        timeLimitFrames="180",
    ),

    UNCROSS_ARMS=state(
        msg=[
            "[You put your hands",
            "back out]",
            "No sir no sir no sir",
        ],
        stateNext="PRETZEL",
        timeLimitFrames="180",
    ),

    PRETZEL=state(
        msg=[
            "Press and hold A and",
            "B, and press up and",
            "then down.",
        ],
    ),

    REPEAT_QUESTION=state(
        msg=[
            "The first letter of",
            "the alphabet is B?",
        ],
        stateAfterInput=["DRUNK"] + (["MISTAKE"] * 5)
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    DRUNK=state(),
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
    template = string.Template(r"""
$names_states

void printMsgOfState(State state) {
  char *msg = {};
  switch (state) {
$casesPrintMsgOfState
  default:
    msg = new char[1]{'\x0'};
    break;
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
    return 0;
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
            caseFromState(n, s)
                for n, s in namesAndStates
                if "msg" in s),
        casesStateAfterInput="\n".join(
            caseStateAfterInputOuter(n, s)
                for n, s in namesAndStates
                if "stateAfterInput" in s),
        casesTimeLimitFrames="\n".join(
            "  case {}: return {};".format(n, s["timeLimitFrames"])
                for n, s in namesAndStates
                if "timeLimitFrames" in s),
        casesStateAfterTimeLimitExceeded="\n".join(
            "  case {}: return {};".format(n, s["stateAfterTimeLimitExceeded"])
                for n, s in namesAndStates
                if "stateAfterTimeLimitExceeded" in s))

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
