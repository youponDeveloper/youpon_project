/**
******************************************************************************
* @file    sn3236ctrl.c 
* @author  hehongxing
* @version V1.0.0
* @date    20-Sep-2016
* @brief   sn3236 user controller operation
******************************************************************************
* @attention
*
******************************************************************************
*/ 
#include "MICO.h"
#include "sn3236ctrl2.h"
#include "sn3236ctrl.h"

#define sn3236_2_log(M, ...) custom_log("SN3236", M, ##__VA_ARGS__)

/************** I2C/SPI buffer length ******/
#define	SN3236_2_BUFFER_LEN 		40

/* I2C device */
mico_i2c_device_t ledctrl_2_i2c_device = {
  (mico_i2c_t)SN3236_I2C_DEVICE, SN3236_2_ID, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

OSStatus SN3236_2_I2C_probe()
{
  OSStatus err = kNoErr;
  mico_i2c_message_t apds_i2c_msg = {NULL, NULL, 0, 0, 0, false};

  uint8_t array[SN3236_2_BUFFER_LEN]={0};
 // uint8_t stringpos;
  array[0] = 0x00;

  err = MicoI2cBuildTxMessage(&apds_i2c_msg, array, 1, 3);
  require_noerr( err, exit );
  err = MicoI2cTransfer(&ledctrl_2_i2c_device, &apds_i2c_msg, 1);
  require_noerr( err, exit );
  
exit:  
  return err;
}


OSStatus SN3236_2_I2C_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
  OSStatus err = kNoErr;
  mico_i2c_message_t apds_i2c_msg = {NULL, NULL, 0, 0, 0, false};

  uint8_t array[SN3236_2_BUFFER_LEN]={0};
  uint8_t stringpos;
  array[0] = reg_addr;
  for (stringpos = 0; stringpos < cnt; stringpos++) {
          array[stringpos + 1] = *(reg_data + stringpos);
  }

  err = MicoI2cBuildTxMessage(&apds_i2c_msg, array, cnt + 1, 3);
  require_noerr( err, exit );
  err = MicoI2cTransfer(&ledctrl_2_i2c_device, &apds_i2c_msg, 1);
  require_noerr( err, exit );
  
exit:  
  return err;
}

OSStatus SN3236_2_I2C_bus_read(uint8_t *reg_data, uint8_t cnt)
{
  OSStatus err = kNoErr;
  mico_i2c_message_t apds_i2c_msg = {NULL, NULL, 0, 0, 0, false};

  err = MicoI2cBuildRxMessage(&apds_i2c_msg, reg_data, cnt, 3);
  require_noerr( err, exit );
  err = MicoI2cTransfer(&ledctrl_2_i2c_device, &apds_i2c_msg, 1);
  require_noerr( err, exit );

exit:
  return err;
}

OSStatus SN3236_2_Write_RegData(uint8_t reg_addr, uint8_t reg_data)
{
  OSStatus err = kNoErr;
  err = SN3236_2_I2C_bus_write(/*0x80|*/reg_addr, &reg_data, 1);
  
  return err;
}

OSStatus SN3236_2_Read_RegData(uint8_t reg_addr, uint8_t *reg_data)
{
	  OSStatus err = kNoErr;
	  err = SN3236_2_I2C_bus_write(/*0xA0|*/reg_addr, NULL, 0);
	  err = SN3236_2_I2C_bus_read(reg_data, 1);
	  return err;  
}

OSStatus sn3236_2_sensor_init(void)
{
  OSStatus err = kNoErr;
  uint8_t device_id=0;
 // bool ret = 0;

   sn3236_2_log("i2c [port=%d]", ledctrl_2_i2c_device.port);
  MicoI2cFinalize(&ledctrl_2_i2c_device); 

  /*int sn3236 sensor i2c device*/
  err = MicoI2cInitialize(&ledctrl_2_i2c_device);
  require_noerr_action( err, exit, sn3236_2_log("SN3236_ERROR: MicoI2cInitialize err = %d.", err) );
  
#if 0
  if( false == MicoI2cProbeDevice(&ledctrl_2_i2c_device, 5) ){
    sn3236_2_log("SN3236_ERROR: no i2c device found!");
  //  err = kNotFoundErr;
  // MicoI2cProbeDevice(&ledctrl_2_i2c_device, 5);
    //goto exit;
  }
#endif

  sn3236_2_log("i2c 2[port=%d]", ledctrl_2_i2c_device.port);

  sn3236_2_log("i2c 3 [device_id=%d]", device_id);
  
exit:
  return err;
}

OSStatus sn3236_2_sensor_deinit(void)
{
  OSStatus err = kUnknownErr;
  
  err = MicoI2cFinalize(&ledctrl_2_i2c_device);
  require_noerr_action( err, exit, sn3236_2_log("SN3236_ERROR: MicoI2cFinalize err = %d.", err));
  
exit:
  return err;
}


