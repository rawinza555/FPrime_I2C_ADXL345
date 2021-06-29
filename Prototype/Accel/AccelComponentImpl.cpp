// ======================================================================
// \title  AccelComponentImpl.cpp
// \author rpk
// \brief  cpp file for Accel component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Prototype/Accel/AccelComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Prototype {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  AccelComponentImpl ::
    AccelComponentImpl(
        const char *const compName
    ) : AccelComponentBase(compName)
  {

  }

  void AccelComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    AccelComponentBase::init(instance);
  }
  //set up BW rate and power control based on python code in https://www.engineersgarage.com/adxl345-accelerometer-raspberry-pi-i2c/
  void AccelComponentImpl :: preamble(void)
  {
      Fw::Buffer BwRateSetup;
      Fw::Buffer PwrSetup;
      unsigned char BwConfig[2];
      unsigned char PwrConfig[2];

      BwConfig[0] = 0x2C; //BW register
      BwConfig[1] = 0x0B; //BW set value
      PwrConfig[0] = 0x2D; //Power register
      PwrConfig[1] = 0x08; //Power value

      BwRateSetup.setSize(2);
      BwRateSetup.setData(BwConfig);

      PwrSetup.setSize(2);
      PwrSetup.setData(PwrConfig);

      this->I2cWrite_out(0,addr,BwRateSetup);
      this->I2cWrite_out(0,addr,PwrSetup);
  }


  AccelComponentImpl ::
    ~AccelComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void AccelComponentImpl ::
    SchedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    //get accelerometer data to x y and z based on python code https://www.engineersgarage.com/adxl345-accelerometer-raspberry-pi-i2c/
    Fw::Buffer accelRecv;
    Fw::Buffer accelWrite;
    unsigned char accelData[6];
    unsigned char accelDataReg[] = {0x32,0x33,0x34,0x35,0x36,0x37};
    char* pointer;
    char data_temp;

    accelRecv.setSize(1);
    accelWrite.setSize(1);

    for (NATIVE_INT_TYPE buffer = 0; buffer < sizeof(accelDataReg); buffer++) {
          accelWrite.setData((U8*)accelDataReg[buffer]);
          this->I2cWriteRead_out(0,addr,accelWrite,accelRecv);
          pointer = reinterpret_cast<char*>(accelRecv.getData());
          sscanf(pointer,"%c",data_temp);
          accelData[buffer] = data_temp;

      }
    NATIVE_INT_TYPE x = accelData[1]<<8 | accelData[0];
    NATIVE_INT_TYPE y = accelData[3]<<8 | accelData[2];
    NATIVE_INT_TYPE z = accelData[5]<<8 | accelData[4];

    x = x * 0.004*9.80665;
    y = y * 0.004*9.80665;
    z = z * 0.004*9.80665;
    this->tlmWrite_Accel_X(x);
    this->tlmWrite_Accel_Y(y);
    this->tlmWrite_Accel_Z(z);
  }

} // end namespace Prototype
