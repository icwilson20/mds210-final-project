 //variables for twinkle twinkle little star
note_t melody[] = {
  NOTE_C, NOTE_G, NOTE_G, NOTE_A, NOTE_G, NOTE_MAX, NOTE_B, NOTE_C
};

int octave[] = {
  4, 3, 3, 3, 3, 3, 3, 4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

   //variables for all notes used
double f4 = 349.23;
double c5 = 523.25;
double b4 = 493.88;
double bflat4 = 466.16;
double aflat4 = 415.3;
double dflat5 = 554.37;
double eflat5 = 622.25;
double e5 = 659.25;
double f3 = 174.61;
double aflat3 = 233.08;
double dflat4 = 277.18;
double c4 = 261.63;
double g4 = 392.0;
double g5 = 783.99;
double fSharp4 = 369.99;
double f5 = 698.46;
double c6 = 1046.50;

//variables for all rhythms

double q = 1000/3;
double dq = q*1.5;
double e = q/2;
double s = e/2;
double dE = s*3;
double h = 2*q;
double dH = h + q;
double special = h + e;
double w = h+h;

//arrays for first measure;
double rhythmns1[] = {dq, e, s, s, s, s, e, e};
//notes for first measure
double notes1[] = {f4, c5, b4, c5, b4, c5, b4, c5};
//arrays for second and third and fourth measures
double r2[] = {q, dq,e,e,e, q, q, q, q, e, e, e, e, q, q};
//notes for second and third and fourth measure
double n2[] = {aflat4, f4, f4, aflat4, c5, dflat5, aflat4, dflat5, eflat5, c5, dflat5,c5, dflat5, c5, c6};  
//first verse (now listen closely...) next two measures
double r3[] = {q, q, q, q, q, q, e, e, q};
double n3[] = {f3, aflat3, c4, aflat3, f3, aflat3, dflat4, c4, aflat3};
//still first verse (heres a little lesson in trickery...) next two measures
double r4[]= {e, e, e, e, e, e, e, e, dE, dE, h, e};
double n4[] = {g4, aflat4, g4, f4, g4, aflat4, g4, f4, aflat4, f4, c5, c5};
//still first verse notes array and rhythmns arrays next three measures
double r5[] = {e, e, e, e, q, q, dE, dE, h, q, e, e, e, e, e, e, e,e};
double n5[] = {dflat5, dflat5, dflat5, dflat5, dflat5, eflat5, c5, dflat5, c5, g4, aflat4, aflat4, g4, f4, g4, aflat4, g4, f4};
//next three measures
double r6[] = {dE, dE, h, e, e, e, e, e, e, e, e, e, dE, dE, special+e};
double n6[] = {aflat4, f4, c5, c5, dflat5, dflat5, dflat5, dflat5, c5, dflat5, eflat5, dflat5, c5, dflat5, c5};
//next four measures
double r7[] = {e, e, e, e, h, e, dE, dE, h, e, e, e, e, e, q, q, dH, q};
double n7[] = {f4, bflat4, bflat4, aflat4, bflat4, bflat4, c5, c5, c5, c5, dflat5, dflat5, dflat5, dflat5, dflat5, dflat5, eflat5, dflat5};
double r8[] = {q, q, q};
double n8[] = {aflat4, g4, fSharp4};
double r9[] = {e, e, e, e, h, e, e, e, e, q, q, w};
double n9[] = {c5, dflat5, c5, dflat5, c5, c5, dflat5, c5, dflat5, c5, f5, f5 };
