/**
******************************************************************************
* @file    st1615touch.c 
* @author  hehongxing
* @version V1.0.0
* @date    20-Sep-2016
* @brief   ST1615 user controller operation
******************************************************************************
* @attention
******************************************************************************
*/ 
#include "MICO.h"
#include "st1615touch.h"
#include "user_config.h"

#define st1615_log(M, ...) custom_log("ST1615", M, ##__VA_ARGS__)

/************** I2C/SPI buffer length ******/
#define	TP_BUFFER_LEN  3

//static u8 suspend_state = 0;
/* I2C device */
static mico_i2c_device_t st1615_i2c_device = {
  (mico_i2c_t)FT6X06_I2C_DEVICE, ST1615_ID, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

OSStatus st1615_touchpanel_I2C_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	OSStatus err = kNoErr;
	mico_i2c_message_t tp_i2c_msg = {NULL, NULL, 0, 0, 0, false};

	uint8_t array[TP_BUFFER_LEN]={0};
	uint8_t stringpos;
	array[0] = reg_addr;
	for (stringpos = 0; stringpos < cnt; stringpos++) {
	      array[stringpos + 1] = *(reg_data + stringpos);
	}

	err = MicoI2cBuildTxMessage(&tp_i2c_msg, array, cnt + 1, 3);
	require_noerr( err, exit );
	err = MicoI2cTransfer(&st1615_i2c_device, &tp_i2c_msg, 1);
	require_noerr( err, exit );

	exit:  
	return err;
}

OSStatus st1615_touchpanel_I2C_bus_read(uint8_t *reg_data, uint8_t cnt)
{
	OSStatus err = kNoErr;
	mico_i2c_message_t tp_i2c_msg = {NULL, NULL, 0, 0, 0, false};

	err = MicoI2cBuildRxMessage(&tp_i2c_msg, reg_data, cnt, 3);
	require_noerr( err, exit );
	err = MicoI2cTransfer(&st1615_i2c_device, &tp_i2c_msg, 1);
	require_noerr( err, exit );

	exit:
	return err;
}

OSStatus st1615_touchpanel_Write_RegData(uint8_t reg_addr, uint8_t reg_data)
{
	OSStatus err = kNoErr;
	err = st1615_touchpanel_I2C_bus_write(/*0x80|*/reg_addr, &reg_data, 1);
	return err;
}

OSStatus st1615_touchpanel_Read_RegData(uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
	OSStatus err = kNoErr;
	err = st1615_touchpanel_I2C_bus_write(/*0xA0|*/reg_addr, NULL, 0);
	err = st1615_touchpanel_I2C_bus_read(reg_data, count);
	return err;  
}

OSStatus st1615_touchpanel_Clear_intrtrupt( void )
{
	OSStatus err = kNoErr;
	//err = st1615_touchpanel_I2C_bus_write(0x80|CLIT_ADDR, NULL, 0);
	return err;
}

void st1615_touchpanel_enable()
{
  
}

 void st1615_tp_data_readout(uint8_t *valid, uint8_t *Xpos, uint8_t *Ypos)
 {
	uint8_t buf[32];
	stx_report_data_t *pdata;

	memset(buf, 0, sizeof(buf));
	if (kNoErr != st1615_touchpanel_Read_RegData(FINGERS, buf, 6))
	{
		pdata = (stx_report_data_t *) buf;

		*valid = pdata->xyz_data[0].valid;

		*Xpos = pdata->xyz_data[0].x_h << 8 | pdata->xyz_data[0].x_l;
		*Ypos = pdata->xyz_data[0].y_h << 8 | pdata->xyz_data[0].y_l;
	}
 }
OSStatus st1615_touchpanel_data_readout(ST_CTTouchStatus *pPointInfo)
{
	OSStatus err = kNoErr;
	int  i;
	uint8_t buf[USER_NUMBER_VAL_32]={USER_NUMBER_VAL_0};
	stx_report_data_t *pdata;
	
	if(NULL == pPointInfo)
	{
		st1615_log("pTouchStatus is NULL.\n");
		return err;
	}

	if (kNoErr != st1615_touchpanel_Read_RegData(FINGERS, buf, USER_NUMBER_VAL_12)) // Read Coordinates from default Reg. address 0x10.
		goto err;

	pdata = (stx_report_data_t *) buf;
	pPointInfo->M_key = USER_NUMBER_VAL_0;

	for(i = USER_NUMBER_VAL_0; i < ST_MAX_TOUCH_POINTS; i++)
	{
		if(pdata->xyz_data[i].valid == USER_NUMBER_VAL_1)
		{
			pPointInfo->M_key++;
		}
	}

	pPointInfo->M_updown = pdata->xyz_data[USER_NUMBER_VAL_0].valid;
	pPointInfo->M_pStCTPoint[USER_NUMBER_VAL_0].M_Xpos = pdata->xyz_data[USER_NUMBER_VAL_0].x_h << USER_NUMBER_VAL_8 | pdata->xyz_data[USER_NUMBER_VAL_0].x_l;
	pPointInfo->M_pStCTPoint[USER_NUMBER_VAL_0].M_Ypos = pdata->xyz_data[USER_NUMBER_VAL_0].y_h << USER_NUMBER_VAL_8 | pdata->xyz_data[USER_NUMBER_VAL_0].y_l;
	pPointInfo->M_nPoint++;

	return err;
err:
	st1615_log("read  coordinates failed.\n");
	return err;

}

static void st1615_getRevision(void)
{
	uint8_t buffer[4] = {0};
	st1615_touchpanel_Read_RegData(FIRMWARE_VERSION, buffer, 4);
	
	st1615_log("FW revision (hex) = %x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
}

int st1615_getStatus(void)
{
	uint8_t data[8] = {0};
	
	if(kNoErr != st1615_touchpanel_Read_RegData(STATUS_REG, data,1))
	{
		st1615_log("read status fail  %02x \n", data[0]);
		return -1;
	}
	st1615_log("status: %02x",data[0]);
	return data[0];
}

int st1615_setSmartWakeupSta(void)
{
	uint8_t data[8] = {0};
	uint8_t wakeup = 0x00;
	
	if(kNoErr != st1615_touchpanel_Read_RegData(0xF0, data,1))
	{
		st1615_log("read SMART WAKE UP fail  %02x \n", data[0]);
		return -1;
	}
	st1615_log("smart wake up statu: %02x",data[0]);


	if(kNoErr != st1615_touchpanel_I2C_bus_write(MISC_CONTROL, &wakeup,1))
	{
		st1615_log("write SMART WAKE UP fail  %02x \n", data[0]);
		return -1;
	}

	if(kNoErr != st1615_touchpanel_Read_RegData(0xF0, data,1))
	{
		st1615_log("2 read SMART WAKE UP fail  %02x \n", data[0]);
		return -1;
	}
	st1615_log("2 smart wake up statu: %02x",data[0]);

	
	return data[0];
}



void  st1615_setPowerDown()
{
	uint8_t suspend_command = 0x02;
	st1615_log("ST_SET_POWER_DOWN start\n");
		
	if(kNoErr != st1615_touchpanel_I2C_bus_write(DEVICE_CONTROL_REG, &suspend_command,1) )
		st1615_log("ST_SET_POWER_DOWN fail with write command\n");
	else
	{
		mico_thread_msleep(100);
		if(st1615_getStatus() == 0)
			st1615_log("ST_SET_POWER_DOWN ok\n");
		else
			st1615_log("ST_SET_POWER_DOWN fail with check status fail\n");
		
	}
}

void st1615_swk_set_swk_enable()
{
	
	int ret = 0;
	unsigned char buffer[2] = {0};
	ret = st1615_touchpanel_Read_RegData(MISC_CONTROL, buffer, 1);
	if(ret == 0)
	{
		buffer[1] = buffer[0] | 0x80;
		st1615_touchpanel_I2C_bus_write(MISC_CONTROL, buffer+1, 1);		
		mico_thread_msleep(500);
	}
}

int st1615_ts_set_powerdown_bit(int value)
{
	int ret = 0;
	uint8_t buffer[2];

	st1615_log("%s, value = %d\n", __FUNCTION__, value);
	ret = st1615_touchpanel_Read_RegData(DEVICE_CONTROL_REG, buffer, 1);
	if (ret < 0){
		st1615_log("read device control status error (%d)\n", ret);
		return ret;
	}else{
		st1615_log("dev status = %d \n", buffer[0]);
	}

	if(value == 0)
		buffer[1] = buffer[0] & 0xfd;
	else
		buffer[1] = buffer[0] | 0x2;

	ret = st1615_touchpanel_I2C_bus_write(DEVICE_CONTROL_REG, buffer+1, 1);
	if (ret < 0){
		st1615_log("write power down error (%d)\n", ret);
		return ret;
	}

	return 0;
}

int st1615_ts_suspend()
{
	int ret;
	st1615_log("%s\n", __FUNCTION__);
	st1615_swk_set_swk_enable();
	//suspend_state = 1;
	ret = st1615_ts_set_powerdown_bit(1);

	return ret;
}

int st1615_ts_resume()
{
	int ret;

	st1615_log("%s\n", __FUNCTION__);
	ret = st1615_ts_set_powerdown_bit(0);
	//suspend_state = 0;
	
	return ret;
}

OSStatus st1615_touchpanel_sensor_init(void)
{
	OSStatus err = kNoErr;
	int status;

	//st1615_log("i2c [port=%d]", st1615_i2c_device.port, st1615_i2c_device.address);
	MicoI2cFinalize(&st1615_i2c_device); 

	/*int st1615 sensor i2c device*/
	err = MicoI2cInitialize(&st1615_i2c_device);
	require_noerr_action( err, exit, st1615_log("FT6X06_ERROR: MicoI2cInitialize err = %d.", err) );

#if 0
	if( false == MicoI2cProbeDevice(&st1615_i2c_device, 5) ){
		st1615_log("FT6X06_ERROR: no i2c device found!");
		err = kNotFoundErr;
		goto exit;
	}
#endif

	st1615_log("i2c 2[port=%d]", st1615_i2c_device.port);

	if(status = st1615_getStatus() == -1)
	{
		st1615_log("get status=%d failed !", status);
		return err;
	}

	st1615_getRevision();
	//st1615_setSmartWakeupSta();
	
	exit:
	return err;
}

OSStatus st1615_touchpanel_sensor_deinit(void)
{
	OSStatus err = kUnknownErr;

	err = MicoI2cFinalize(&st1615_i2c_device);
	require_noerr_action( err, exit, st1615_log("st1615_touchpanel_ERROR: MicoI2cFinalize err = %d.", err));

	exit:
	return err;
}


