#ifndef AD5940Main_hpp
#define AD5940Main_hpp

#define APPBUFF_SIZE 1024

#include "SqrWaveVoltammetry.h"
#include <Vector.h>

class AD5940_SQV{
    public:

    AD5940_SQV();
    ~AD5940_SQV();
    //-------------//
    bool Initialize();
    bool StartSequence();
    bool CheckForResult();
    void OutputResult();

    //-------------//
    protected:

    bool addOutputs(float *pData, uint32_t DataCount);
    bool processCurrents();
    bool printVector(Vector<float>& a_vector);
    //-------------//
    Vector<float> _all_meas;
    Vector<float> _pos_meas;
    Vector<float> _neg_meas;
    Vector<float> _dif_meas;

    float storage_0[APPBUFF_SIZE];
    float storage_1[APPBUFF_SIZE];
    float storage_2[APPBUFF_SIZE];
    float storage_3[APPBUFF_SIZE];

};


#endif