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
    ),
    MISTAKE=dict(
        msg=[
            'Officer Langley:',
            "Don't!"
        ],
    ),
    BEING_SHOT=dict(
        msg=[
            ''
        ],
    ),
    GAME_OVER=dict(
        msg=[
            ''
        ],
    ),
    START=dict(
        msg=[
            ''
        ],
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
    ),
    WAIT=dict(
        msg=[
            ''
        ],
    ),
    WHO_ELSE=dict(
        msg=[
            'The first letter of',
            'the alphabet is B?'
        ],
    ),
    NOBODY_ELSE=dict(
        msg=[
            'The first letter of',
            'the alphabet is A?'
        ],
    ),
    POSITIVE=dict(
        msg=[
            'Are you positive?'
        ],
    ),
    FTC=dict(
        msg=[
            'OK. Apparently, we',
            'have a failure for',
            'you to comprehend',
            'simple instructions.'
        ],
    ),
    AGAIN=dict(
        msg=[
            "I've got to go over",
            'some of them again.',
            'OK, can you both',
            'hear and understand',
            'me?'
        ],
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
    ),
    QUESTION=dict(
        msg=[
            ''
        ],
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
$cases
  }
  arduboy.print(msg);
  delete msg;
}
""".strip("\n"))
    return template.substitute(
        cases="\n".join(caseFromState(n, s) for n, s in namesAndStates),
        names_states="\n".join(
            "const State {} = {};".format(n, i)
                for i, (n, s) in enumerate(namesAndStates)))

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

if __name__ == "__main__":
    main()
