int main() {
   int x = 7;
   float y = 0.7;
   if(x < 1) return false;
   //comment here
   Memc<Str> strings;
   for(int i = 0; i < strings.elms(); i++) { 
      strings[i] += i;
      if(i > 100) break;
   }
}
























