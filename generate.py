#!/usr/bin/env python3
import string
import sys

stateWithName = dict(
    menu=dict(
        msg=[
            'Daniel Shaver',
            'Simulator',
            '',
            'Press any button',
            'to play'
        ],
    ),
    B=dict(
        msg=[
            'Officer Langley:',
            "Don't!"
        ],
    ),
    C=dict(
        msg=[
            ''
        ],
    ),
    D=dict(
        msg=[
            ''
        ],
    ),
    E=dict(
        msg=[
            ''
        ],
    ),
    F=dict(
        msg=[
            'Officer Brailsford:',
            'Stop! Stop!',
            'Hit the A button!',
            'Both of you!',
            'Press the A Button!',
            'Press the A Button!'
        ],
    ),
    G=dict(
        msg=[
            ''
        ],
    ),
    H=dict(
        msg=[
            'The first letter of',
            'the alphabet is B?'
        ],
    ),
    I=dict(
        msg=[
            'The first letter of',
            'the alphabet is A?'
        ],
    ),
    J=dict(
        msg=[
            'Are you positive?'
        ],
    ),
    K=dict(
        msg=[
            'OK. Apparently, we',
            'have a failure for',
            'you to comprehend',
            'simple instructions.'
        ],
    ),
    L=dict(
        msg=[
            "I've got to go over",
            'some of them again.',
            'OK, can you both',
            'hear and understand',
            'me?'
        ],
    ),
    M=dict(
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
    N=dict(
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
    return """
void printMsgOfState(State state) {
    char *msg = new char[4]{'D', 'a', 'n'};
    arduboy.print(msg);
    delete msg;
}
""".strip()

if __name__ == "__main__":
    main()
