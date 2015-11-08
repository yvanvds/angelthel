#include "../test1.as"
#include "../testb/test2.as"
#include "testc/test3.as"
const Str S;
void main(){
    int testa=1;
    int testb=2;
   int a=compare1(testa,testb);
   
   if(a==-1)ScreenMessage(S+testa+" is less than "+testb);
    if(a==1)ScreenMessage(S+testa+" is greater than "+testb);
    if(a==0)ScreenMessage(S+testa+" is equal to "+testb);
   text(0.0f,0.0f,"Test");
}
