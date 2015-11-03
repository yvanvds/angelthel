
// this is a comment

int counter = 0;
const Str S = "";
Flt posX = 1;
Flt posY = 0;

void Init() {
   counter = 0;
}

void Update() {
   if(Kb.bp(KB_SPACE)) counter++;
   
   posX -= Time.ad();
   if (posX < -1) {
      posX = 1;
      counter++;
   }
}

void Draw() {
   text(posX, posY, S + "Counter: " + counter);
}

void SetPosY(Flt value) {
   posY = value;
}

