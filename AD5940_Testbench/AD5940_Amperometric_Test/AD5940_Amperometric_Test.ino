#include "ad5940.h"
#include "Amperometric.h"
#include <LibPrintf.h> 


/////////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);
  delay(1000);
  printf("Hello\n");

  void AD5940_Main_Setup(void);
  AD5940_MCUResourceInit(NULL);
  AD5940_Main_Setup();

}

void loop() {
  bool AD5940_Measure_Measure_Step(void);
  
  AD5940_Measure_Measure_Step();
  // printf("loop\n");
  delay(1000);
}
