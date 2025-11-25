#include "ad5940.h"
#include "SqrWaveVoltammetry.h"
#include "AD5940Main.hpp"
#include <LibPrintf.h> 


/////////////////////////////////////////////////////


AD5940_SQV the_sqv;

void setup() {

  Serial.begin(115200);
  delay(1000);
  printf("Hello\n");

  // void AD5940_Main_Setup(void);
  // AD5940_Main_Setup();

  the_sqv.Initialize();
  the_sqv.StartSequence();

  while (the_sqv.CheckForResult() == 0){
    delay(1000);
  }
  the_sqv.OutputResult(); 
  
  printf("Program Finished \n");

}

void loop() {

  delay(1000);

}

