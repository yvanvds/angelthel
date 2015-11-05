﻿
// this is a comment that will be very long because we have to test the text offset.
// this is also a long comment to see what happens if we changes lines with offset.
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

