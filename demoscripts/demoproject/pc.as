Vec worldPos;
Str result;

void Update() {
   if(Ms.bp(0)) {
      if(World.getWorldPos(Ms.pos(), worldPos)) {
         if(MoveTo(worldPos)) {
            result = "";
         } else {
            result = "no path found";
         }
      }
   }
}

void Draw() {
   text(-0.7,0.7, result);
}
