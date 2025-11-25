/*!
 *****************************************************************************
 @file:    AD5940Main.c
 @author:  $Author: nxu2 $
 @brief:   Used to control specific application and process data.
 @version: $Revision: 766 $
 @date:    $Date: 2017-08-21 14:09:35 +0100 (Mon, 21 Aug 2017) $
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.

*****************************************************************************/
#include "AD5940Main.hpp"


#define APPBUFF_SIZE 1024
uint32_t AppBuff[APPBUFF_SIZE];
float LFOSCFreq;    /* Measured LFOSC frequency */

void MeasureLFOC(void){
  LFOSCMeasure_Type LfoscMeasure;
  /* Measure LFOSC frequency */
  /**@note Calibrate LFOSC using system clock. The system clock accuracy decides measurement accuracy. Use XTAL to get better result. */
  LfoscMeasure.CalDuration = 1000.0;  /* 1000ms used for calibration. */
  LfoscMeasure.CalSeqAddr = 0;        /* Put sequence commands from start address of SRAM */
  LfoscMeasure.SystemClkFreq = 16000000.0f; /* 16MHz in this firmware. */
  AD5940_LFOSCMeasure(&LfoscMeasure, &LFOSCFreq);
  printf("LFOSC Freq:%f\n", LFOSCFreq);
  return;
}

void AD5940RampStructInit(void)
{
  AppSWVCfg_Type *pRampCfg;
  
  AppSWVGetCfg(&pRampCfg);
  /* Step1: configure general parmaters */
  pRampCfg->SeqStartAddr = 0x10;                /* leave 16 commands for LFOSC calibration.  */
  pRampCfg->MaxSeqLen = 1024-0x10;              /* 4kB/4 = 1024  */
  pRampCfg->RcalVal = 10000.0;                  /* 10kOhm RCAL */
  pRampCfg->ADCRefVolt = 1.820f;               /* The real ADC reference voltage. Measure it from capacitor C12 with DMM. */
  pRampCfg->FifoThresh = 1;                   /* Maximum value is 2kB/4-1 = 512-1. Set it to higher value to save power. */
  pRampCfg->SysClkFreq = 16000000.0f;           /* System clock is 16MHz by default */
  pRampCfg->LFOSCClkFreq = LFOSCFreq;           /* LFOSC frequency */
	pRampCfg->AdcPgaGain = ADCPGA_1P5;
	pRampCfg->ADCSinc3Osr = ADCSINC3OSR_4;
  
	/* Step 2:Configure square wave signal parameters */
  pRampCfg->RampStartVolt = -600.0f;          /* Measurement starts at 0V*/
  pRampCfg->RampPeakVolt = 0.0f;     		      /* Measurement finishes at -0.4V */
  pRampCfg->VzeroStart = 2200.0f; //   1300.0f;             /* Vzero is voltage on SE0 pin: 1.3V */
  pRampCfg->VzeroPeak = 400.0f;  //     1300.0f;              /* Vzero is voltage on SE0 pin: 1.3V */
  pRampCfg->Frequency = 300;                  /* Frequency of square wave in Hz */
  pRampCfg->SqrWvAmplitude = 25;              /* Amplitude of square wave in mV */
  pRampCfg->SqrWvRampIncrement = 5;           /* Increment in mV*/
  pRampCfg->SampleDelay = 0.2f;               /* Time between update DAC and ADC sample. Unit is ms and must be < (1/Frequency)/2 - 0.2*/
  pRampCfg->LPTIARtiaSel = LPTIARTIA_1K;      /* Maximum current decides RTIA value */
	pRampCfg->bRampOneDir = bTRUE;//bFALSE;			/* Only measure ramp in one direction */
}


//------------------------------------//
AD5940_SQV::AD5940_SQV(){
  _all_meas.setStorage(storage_0);
  _pos_meas.setStorage(storage_1);
  _neg_meas.setStorage(storage_2);
  _dif_meas.setStorage(storage_3);
}
AD5940_SQV::~AD5940_SQV(){}

//---//
bool AD5940_SQV::Initialize(){
  AD5940_MCUResourceInit(NULL);
  AD5940PlatformCfg();
  MeasureLFOC();
  AD5940RampStructInit();

  AppSWVInit(AppBuff, APPBUFF_SIZE);    /* Initialize RAMP application. Provide a buffer, which is used to store sequencer commands */
	
	AD5940_Delay10us(100000);		/* Add a delay to allow sensor reach equilibrium befor starting the measurement */

  printf("Finished Initialization \n");
  return 0;
}

bool AD5940_SQV::StartSequence(){
  AppSWVCtrl(APPCTRL_START, 0);          /* Control IMP measurement to start. Second parameter has no meaning with this command. */
  printf("Sent Command \n");
  return 0;
}

bool AD5940_SQV::CheckForResult(){
  uint32_t temp; 
  printf("Check Responce \n");
  if(AD5940_GetMCUIntFlag())
    {
      AD5940_ClrMCUIntFlag();
      temp = APPBUFF_SIZE;
      AppSWVISR(AppBuff, &temp);
      addOutputs((float*)AppBuff, temp);

      return 1;
    }
  return 0;
}

void AD5940_SQV::OutputResult(){
  printf("Outputting Responce\n");

  processCurrents();

  printf("%d %d %d %d \n",_all_meas.size(),_pos_meas.size(),_neg_meas.size(), _dif_meas.size() );

  printf("all measurements: ");
  printVector(_all_meas);
  printf("\n");



  printf("positive current: ");
  printVector(_pos_meas);
  printf("\n");

  printf("negative current: ");
  printVector(_neg_meas);
  printf("\n");
  
  printf("differential current: ");
  printVector(_dif_meas);
  printf("\n");

  return;
}

//---//
bool AD5940_SQV::addOutputs(float *pData, uint32_t DataCount){
  printf("Adding Result\n");

  for(int i=0;i<DataCount;i++){
    // printf("%.3f ", pData[i]);
    _all_meas.push_back(pData[i]);
  }

  printf("Result Added\n");
  return 0;
}

bool AD5940_SQV::printVector(Vector<float>& a_vector){
  for (int i = 0; i < a_vector.size(); i++){
    printf("%.3f ",a_vector.at(i));
  }
  return 0;
}

bool AD5940_SQV::processCurrents(){
  // for the length of the all measuremnts
    // add the first to the pos second to neg
    // add difference to diff

  for (int i = 0; i<_all_meas.size(); i+=2){
    _pos_meas.push_back(_all_meas.at(i));
    _neg_meas.push_back(_all_meas.at(i+1));
    _dif_meas.push_back(_all_meas.at(i) - _all_meas.at(i+1));
  }

  return 0;
}