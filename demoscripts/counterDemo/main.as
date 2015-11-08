int counter = 0;
const Str S = "";
Flt posX = 1;
Flt posY = 0;
Edge2 line1, line2;

void Init() {
   counter = 0;
   line1.set(-0.7, -0.1, 0.7, -0.1);
   line2.set(-0.7,  0.1, 0.7,  0.1);
}

void Update() {
   if(Kb.bp(KB_SPACE)) counter++;
   if(Ms.bp(0)) counter = 0;
   posX -= Time.ad();
   if (posX < -1) {
      posX = 1;
      counter++;
   }
}

void Draw() {
   text(posX, posY, S + "Counter: " + counter);
   line1.draw(RED  , WHITE);
   line2.draw(WHITE, BLUE );
}

void SetPosY(Flt value) {
   posY = value;
}


