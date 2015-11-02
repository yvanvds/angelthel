
// this is a comment

int counter = 0;
const Str S = "";
Flt pos = 1;

void Init() {
   counter = 0;
}

void Update() {
   if(Kb.bp(KB_SPACE)) counter++;
   
   pos -= Time.ad();
   if (pos < -1) {
      pos = 1;
      counter++;
   }
}

void Draw() {
   text(pos, 0, S + "Counter: " + counter);
}

