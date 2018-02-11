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
        assert len(msg) <= 8, "msg longer than 8 lines"
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
            "You broke the game.",
            "",
            "Please restart your",
            "Arduboy.",
        ],
        stateNext="ERROR",
        timeLimitFrames="INT_MAX",
    ),

    MENU=state(
        msg=[
            'Daniel Shaver',
            'Simulator',
            '',
            '',
            '',
            '',
            '',
            'A: Play',
        ],
        stateAfterInput=[
            "PRE_I_SAID", # TODO: change back to START
        ] + (["MENU"] * 5),
        timeLimitFrames="INT_MAX",
        stateAfterTimeLimitExceeded="MENU",
    ),

    MISTAKE=state(
        msg=[
            "Don't!",
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
            "Stop! Stop! Get on",
            "the ground! Both of",
            "you! Lay down on the",
            "ground! Lay down on",
            "the ground!",
            "",
            "Down: Lie on the",
            "ground",
        ],
        stateAfterInput=[
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "MISTAKE",
            "WAIT",
            "MISTAKE",
        ],
        timeLimitFrames="90",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    WAIT=state(
        stateAfterInput=["WAIT_MISTAKE"] * 6,
        timeLimitFrames="600",
        stateAfterTimeLimitExceeded="WHO_ELSE",
    ),

    WAIT_MISTAKE=state(
        msg=[
            "Shut up! Did I ask",
            "you a question?",
            "",
            "",
            "",
            "",
            "",
            "A: \"No sir\"",
        ],
        stateNext="WAIT",
        timeLimitFrames="180",
    ),

    WHO_ELSE=state(
        msg=[
            "Who else is in the",
            "room?",
            "",
            "",
            "",
            "",
            "",
            "A: \"Nobody\"",
        ],
        stateAfterInput=["NOBODY_ELSE"] + (["MISTAKE"] * 5),
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    NOBODY_ELSE=state(
        msg=[
            "Nobody else is in the",
            "room?",
            "",
            "",
            "",
            "",
            "",
            "A: \"Nobody\"",
        ],
        stateAfterInput=["POSITIVE"] + (["MISTAKE"] * 5),
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    POSITIVE=state(
        msg=[
            "Are you positive?",
            "",
            "",
            "",
            "",
            "",
            "A: \"Nobody.",
            "Absolutely positive\"",
        ],
        stateAfterInput=["FTC"] + (["MISTAKE"] * 5),
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
        stateNext="AGAIN",
        timeLimitFrames="90",
    ),

    AGAIN=state(
        msg=[
            "I've got to go over",
            'some of them again.',
            'OK, can you both',
            'hear and understand',
            'me?'
            "",
            "",
            "A: \"Yes\"",
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
            'understand that?',
            "A: Ask a question",
        ],
        stateAfterInput=["SHUT_UP"] * 6,
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    SHUT_UP=state(
        msg=[
            "Shut up! I'm not here",
            "to be tactful or",
            "diplomatic with you.",
            "You listen, you obey",
            "",
            "",
            "",
            "A: Cross your arms",
        ],
        stateNext="DID_I",
        timeLimitFrames="180",
    ),

    DID_I=state(
        msg=[
            "For one thing, did I",
            "tell you to move,",
            "young man?",
            "",
            "",
            "",
            "",
            "A: Uncross your arms",
        ],
        stateNext="UNCROSS_ARMS",
        timeLimitFrames="180",
    ),

    UNCROSS_ARMS=state(
        msg=[
            "You:",
            "No sir no sir no sir",
        ],
        stateNext="PUT_BOTH_OF",
        timeLimitFrames="180",
    ),

    PUT_BOTH_OF=state(
        msg=[
            "Put both of your",
            "hands on top of your",
            "head and interlace",
            "your fingers. Take",
            "your feet and cross",
            "your left foot over",
            "your right foot",
        ],
        stateNext="PRETZEL",
        timeLimitFrames="180",
    ),

    PRETZEL=state(
        msg=[
            "[Press and hold A and",
            "B, and press Up and",
            "then Down to obey]",
        ],
    ),

    REPEAT_QUESTION=state(
        msg=[
            "Who else is in the",
            "room?",
            "",
            "",
            "",
            "",
            "",
            "A: \"Nobody\"",
        ],
        stateAfterInput=["DRUNK"] + (["MISTAKE"] * 5),
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    DRUNK=state(
        msg=[
            "Alright. Are you",
            "both drunk?",
            "",
            "",
            "",
            "",
            "",
            "A: \"No\"",
        ],
        stateAfterInput=["PROBLEMS_UNDERSTANDING"] + (["MISTAKE"] * 5),
        timeLimitFrames="180",
        stateAfterTimeLimitExceeded="MISTAKE",
    ),

    PROBLEMS_UNDERSTANDING=state(
        msg=[
            "Alright. So you're",
            "not going to have",
            "any problems",
            "understanding",
            "anything that I tell",
            "you, right?",
            "",
            "A: \"Correct\"",
        ],
        stateNext="CAN_I_GO",
        timeLimitFrames="180",
    ),

    CAN_I_GO=state(
        msg=[
            "Woman With You:",
            "Can I go to my room?",
        ],
        stateNext="YOU_ARE",
        timeLimitFrames="180",
    ),

    YOU_ARE=state(
        msg=[
            "No, you're not going",
            "to do anything but",
            "come towards us.",
        ],
        stateNext="YOUNG_MAN",
        timeLimitFrames="180",
    ),

    YOUNG_MAN=state(
        msg=[
            "Young man, you are",
            "not to move. You are",
            "to put your eyes",
            "down and look down at",
            "the carpet. You are",
            "to keep your fingers",
            "interlaced behind",
            "your head.",
        ],
        stateNext="KEEP_FEET_CROSSED",
        timeLimitFrames="180",
    ),

    KEEP_FEET_CROSSED=state(
        msg=[
            "You are to keep your",
            "feet crossed. If you",
            "move, we're going to",
            "consider that a",
            "threat, and we are",
            "going to deal with",
            "it, and you may not",
            "survive.",
        ],
        stateNext="DO_YOU_UNDERSTAND",
        timeLimitFrames="180",
    ),

    DO_YOU_UNDERSTAND=state(
        msg=[
            "Do you understand me?",
            "",
            "",
            "",
            "",
            "",
            "",
            "A: \"Yes sir\"",
        ],
        stateAfterInput=["YOUNG_LADY"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    YOUNG_LADY=state(
        msg=[
            "Young lady, shut up",
            "and listen. Alright.",
            "You are to keep your",
            "feet crossed. Take",
            "both of your hands",
            "and put them flat in",
            "front of you.",
        ],
        stateNext="YOUNG_LADY_2",
        timeLimitFrames="180",
    ),

    YOUNG_LADY_2=state(
        msg=[
            "You are to push",
            "yourself up to a",
            "kneeling position.",
            "Kneeling position.",
            "Now, put both your",
            "hands in the air and",
            "crawl towards us.",
        ],
        stateNext="PRE_STOP",
        timeLimitFrames="180",
    ),

    PRE_STOP=state(
        msg=[
            "[You uncross your",
            "legs without",
            "thinking]",
        ],
        stateNext="STOP",
        timeLimitFrames="90",
    ),

    STOP=state(
        msg=[
            "Stop!",
        ],
        stateNext="YOUNG_MAN_2",
        timeLimitFrames="90",
    ),

    YOUNG_MAN_2=state(
        msg=[
            "Young man, listen to",
            "my instructions and",
            "do not make a",
            "mistake! You are to",
            "keep your legs",
            "crossed! Do you",
            "understand me?",
            "A: \"Yes sir\"",
        ],
        stateAfterInput=["YOU_ARE_2"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    YOU_ARE_2=state(
        msg=[
            "You are to put both",
            "of your hands palms",
            "down straight out in",
            "front of you. Push",
            "yourself up to a",
            "kneeling position.",
        ],
        stateNext="PRE_I_SAID",
        timeLimitFrames="180",
    ),

    PRE_I_SAID=state(
        msg=[
            "[You uncross your",
            "legs again without",
            "thinking]",
        ],
        stateNext="I_SAID",
        timeLimitFrames="90",
    ),

    I_SAID=state(
        msg=[
            "I SAID KEEP YOUR LEGS",
            "CROSSED!",
            "",
            "",
            "",
            "",
            "",
            "A: \"I'm sorry\"",
        ],
        stateAfterInput=["I_DIDNT_SAY"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    I_DIDNT_SAY=state(
        msg=[
            "I didn't say this was",
            "a conversation-",
        ],
        stateNext="PRE_PUT_YOUR_HANDS_UP",
        timeLimitFrames="180"
    ),

    PRE_PUT_YOUR_HANDS_UP=state(
        msg=[
            "[You lower your hands",
            "a little]",
        ],
        stateNext="PUT_YOUR_HANDS_UP",
        timeLimitFrames="90"
    ),

    PUT_YOUR_HANDS_UP=state(
        msg=[
            "PUT YOUR HANDS UP IN",
            "THE AIR! HANDS UP IN",
            "THE AIR!",
        ],
        stateNext="YOU_DO_THAT",
        timeLimitFrames="90"
    ),

    YOU_DO_THAT=state(
        msg=[
            "You do that again",
            "we're shooting you!",
            "Do you understand?",
            "",
            "",
            "",
            "A: \"Please do not",
            "shoot me\"",
        ],
        stateAfterInput=["THEN_LISTEN"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    THEN_LISTEN=state(
        msg=[
            "Then listen to my",
            "instructions!",
            "",
            "",
            "",
            "",
            "",
            "A: \"I'm trying to-\"",
        ],
        stateAfterInput=["DONT_TALK"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    DONT_TALK=state(
        msg=[
            "DON'T TALK! LISTEN!",
        ],
        stateNext="HANDS_STRAIGHT",
        timeLimitFrames="90"
    ),

    HANDS_STRAIGHT=state(
        msg=[
            "Hands straight up in",
            "the air! Do not put",
            "your hands down for",
            "any reason! If you",
            "think you're going to",
            "fall, you'd better",
            "fall on your face.",
        ],
        stateNext="IF_YOUR",
        timeLimitFrames="180",
    ),

    IF_YOUR=state(
        msg=[
            "If your hands go back",
            "to the small of your",
            "back, we are going to",
            "shoot you! Do you",
            "understand me?",
            "",
            "",
            "A: \"Yes sir\"",
        ],
        stateAfterInput=["YOU_BEGIN"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    YOU_BEGIN=state(
        msg=[
            "[You begin sobbing]",
        ],
        stateNext="CRAWL",
        timeLimitFrames="180",
    ),

    CRAWL=state(
        msg=[
            "Crawl towards me!",
            "CRAWL TOWARDS ME!",
            "",
            "",
            "",
            "",
            "",
            "A: Begin crawling",
        ],
        stateAfterInput=["PRE_MISTAKE"] + (["MISTAKE"] * 5),
        stateAfterTimeLimitExceeded="MISTAKE",
        timeLimitFrames="180",
    ),

    PRE_MISTAKE=state(
        msg=[
            "[You reach down to",
            "pull up your pants",
            "without thinking]",
        ],
        stateNext="MISTAKE",
        timeLimitFrames="90",
    ),
)

def main():
    with open(
            "daniel-shaver-sim.ino.template",
        ) as f, open(
            "daniel-shaver-sim.ino", "w",
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
