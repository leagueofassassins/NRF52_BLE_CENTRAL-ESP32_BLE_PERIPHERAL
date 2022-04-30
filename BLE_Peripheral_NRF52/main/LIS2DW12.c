/* @file LIS2DW12.c
   @brief a simple library to initialize and read 
          raw X,Y and Z axes of LIS2DW12 accelerometer device  
   @author bheesma-10
*/


#include "LIS2DW12.h"
#include <stdio.h>

/*pin definitions*/
#define I2C_MASTER_SDA_IO   GPIO_NUM_21       
#define I2C_MASTER_SCL_IO   GPIO_NUM_22
#define I2C_MASTER_FREQ_HZ  400000              //400khz    


/*enum variable for error codes*/
esp_err_t error_code;

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */
static i2c_port_t i2c_port = I2C_NUM_0;


/*
**********************************************************************************************
@brief:initializing i2c bus parameters
@return:parameter configuration status
@param:None
**********************************************************************************************
*/
int i2c_init(void){

    i2c_config_t conf = {
     .mode = I2C_MODE_MASTER,
     .sda_io_num = I2C_MASTER_SDA_IO,         // select GPIO specific to your project
     .sda_pullup_en = GPIO_PULLUP_ENABLE,
     .scl_io_num = I2C_MASTER_SCL_IO,         // select GPIO specific to your project
     .scl_pullup_en = GPIO_PULLUP_ENABLE,
     .master.clk_speed = I2C_MASTER_FREQ_HZ,  // select frequency specific to your project
     // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */

    };

    return(i2c_param_config(i2c_port,&conf));
}

/*
**********************************************************************************************
@brief:read registers of LIS2DW12
@return:register value 
@param:address of the register to be read
**********************************************************************************************
*/
uint8_t i2c_read_reg(uint8_t register_address){

    uint8_t reg_value;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();     //Create a command link 
	i2c_master_start(cmd);   //start bit
	i2c_master_write_byte(cmd,(LIS2DW12_ADDRESS<<1) | WRITE_BIT,ACK_CHECK_EN);   //write slave address and ensure to receive ACK
	i2c_master_write_byte(cmd,register_address,ACK_CHECK_EN);   //write register address and ensure to receive ACK

	/*repeated start*/ 
	i2c_master_start(cmd); 

	i2c_master_write_byte(cmd,(LIS2DW12_ADDRESS<<1) | READ_BIT,ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &reg_value, ACK_CHECK_EN);
	i2c_master_stop(cmd);    //stop bit
	/*Trigger the execution of the command link by I2C controller
			Once the execution is triggered, the command link cannot be modified.*/
	error_code = i2c_master_cmd_begin(i2c_port,cmd,200/portTICK_RATE_MS);  //timeout of 200ms
	/*After the commands are transmitted, release the resources used by the command link*/
	i2c_cmd_link_delete(cmd);

    return reg_value;
}

/*
**********************************************************************************************
@brief:write registers of LIS2DW12
@return:None
@param:register address to write, data to be written
**********************************************************************************************
*/
void i2c_write_reg(uint8_t register_address,uint8_t data){

	
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();     //Create a command link 
	i2c_master_start(cmd);   //start bit
	i2c_master_write_byte(cmd,(LIS2DW12_ADDRESS<<1) | WRITE_BIT,ACK_CHECK_EN);   //write slave address and ensure to receive ACK
	i2c_master_write_byte(cmd,register_address,ACK_CHECK_EN);   //write register address and ensure to receive ACK  
	i2c_master_write_byte(cmd,data,ACK_CHECK_EN);   //write data and ensure to receive ACK  
	i2c_master_stop(cmd);    //stop bit

	/*Trigger the execution of the command link by I2C controller
			Once the execution is triggered, the command link cannot be modified.*/
	error_code = i2c_master_cmd_begin(i2c_port,cmd,500/portTICK_RATE_MS);  //timeout of 500ms
	/*After the commands are transmitted, release the resources used by the command link*/
	i2c_cmd_link_delete(cmd);
}



/*
**********************************************************************************************
@brief:initialize LIS2DW12
@return:None
@param:esp log character tag
**********************************************************************************************
*/
void lis2dw12_init(const char* TAG){

    error_code = i2c_init();
	if(error_code == 0){
		ESP_LOGI(TAG,"I2C initialized...................");
	}

	ESP_ERROR_CHECK(i2c_driver_install(i2c_port,I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));

    /*Device ID*/
    uint8_t who_am_i = i2c_read_reg(WHO_AM_I);
    if(who_am_i == 0x44){
    	ESP_LOGI(TAG,"LIS2DW12 validated................");
    }
    else{
    	ESP_LOGI(TAG,"who am i value: %x",who_am_i);
    }

    ESP_LOGI(TAG,"WHO_AM_I:%x",who_am_i);

    /*Auto-increment Register Address during multi-byte operation*/
    if((i2c_read_reg(CTRL2) & IF_ADD_INC_MSK)==0){
    	i2c_write_reg(CTRL2,IF_ADD_INC_MSK);
    }

    /*Block Data Unit*/
    if((i2c_read_reg(CTRL2) & BDU_MSK)==0){
    	i2c_write_reg(CTRL2,BDU_MSK);
    }

    ESP_LOGI(TAG,"CTRL2:%x",i2c_read_reg(CTRL2));

    /*FiFo mode selection*/
    if((i2c_read_reg(FIFO_CTRL) & FMODE_MSK)==0){
    	i2c_write_reg(FIFO_CTRL,FMODE_STREAM);
    }

    ESP_LOGI(TAG,"FIFO_CTRL:%x",i2c_read_reg(FIFO_CTRL));

    /*Power mode selection*/
    if(i2c_read_reg(CTRL1)==0){
       i2c_write_reg(CTRL1,HIGH_PERFORMANCE_MODE);   //set mode as HIGH_PERFORMANCE_MODE	
       i2c_write_reg(CTRL1,LP_MODE2);                //Low-Power Mode 2 (14-bit resolution)
    }


    /*Output Data Rate selection*/
    i2c_write_reg(CTRL1,LOW_POWER_MODE_12_5HZ);

    ESP_LOGI(TAG,"CTRL1:%x",i2c_read_reg(CTRL1));

    /*Single data conversion on demand mode selection*/
    if(i2c_read_reg(CTRL3)==0){
    	i2c_write_reg(CTRL3,SLP_MODE_SEL_MSK);       
    	i2c_write_reg(CTRL3,SLP_MODE1_MSK);
    }

    ESP_LOGI(TAG,"CTRL3:%x",i2c_read_reg(CTRL3));

    /*Full scale selection*/
    i2c_write_reg(CTRL6,FS_4G);

}

/*
**********************************************************************************************
@brief:function to get raw value of x-axes of LIS2DW12
@return:raw x-axes value 
@param:None
**********************************************************************************************
*/
uint8_t get_x_axes_data(void){

	raw_X_axesvalue.X_OUT[0] = i2c_read_reg(OUT_X_L);
	raw_X_axesvalue.X_OUT[1] = i2c_read_reg(OUT_X_H);

	return (raw_X_axesvalue.x_axes_raw);

}

/*
**********************************************************************************************
@brief:function to get raw value of y-axes of LIS2DW12
@return:raw y-axes value 
@param:None
**********************************************************************************************
*/
uint8_t get_y_axes_data(void){

	raw_Y_axesvalue.Y_OUT[0] = i2c_read_reg(OUT_Y_L);
	raw_Y_axesvalue.Y_OUT[1] = i2c_read_reg(OUT_Y_H);

	return (raw_Y_axesvalue.y_axes_raw);

}

/*
**********************************************************************************************
@brief:function to get raw value of z-axes of LIS2DW12
@return:raw z-axes value 
@param:None
**********************************************************************************************
*/
uint8_t get_z_axes_data(void){

	raw_Z_axesvalue.Z_OUT[0] = i2c_read_reg(OUT_Z_L);
	raw_Z_axesvalue.Z_OUT[1] = i2c_read_reg(OUT_Z_H);

	return (raw_Z_axesvalue.z_axes_raw);

}
