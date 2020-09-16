//////////////////////////////////////////////////////////////
// 
//	project:		STUSB4500 EVALUATION
//	date:			05.09.2020
//	
//	author:			Ziga Miklosic
//
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
//	INCLUDES
//////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"

#include "stusb4500_low_if.h"
#include "stusb4500_config.h"


#include "i2c.h"


//////////////////////////////////////////////////////////////
//	DEFINITIONS
//////////////////////////////////////////////////////////////

// Pointer functions to I2C
typedef i2c_status_t (*pf_stusb4500_read_t)(const uint8_t dev_addr, const uint8_t reg_addr, uint8_t * p_rx_data, const uint32_t size);
typedef i2c_status_t (*pf_stusb4500_write_t)(const uint8_t dev_addr, const uint8_t reg_addr, uint8_t * p_tx_data, const uint32_t size);


//////////////////////////////////////////////////////////////
//	VARIABLES
//////////////////////////////////////////////////////////////

// Read/Write pointer function
static pf_stusb4500_read_t 	gpf_i2c_read = NULL;
static pf_stusb4500_write_t gpf_i2c_write = NULL;

//////////////////////////////////////////////////////////////
// FUNCTIONS PROTOTYPES
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// FUNCTIONS 
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/*
*			Initialize low level interface
*
*	param:		none
*	return:		status - Status of initialization
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_low_if_init(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;

	// Set up interface
	// NOTE: User should provide i2c functions
	gpf_i2c_read = &i2c_read;
	gpf_i2c_write = &i2c_write;

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Write to register
*
*	param:		addr 		- Register address
*	param:		p_tx_data	- Pointer to transmit data
*	param:		size		- Size of transmit data
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_low_if_write_register(const stusb4500_addr_t addr, const uint8_t * p_tx_data, const uint32_t size)
{
	stusb4500_status_t status = eSTUSB4500_OK;

	if ( NULL != gpf_i2c_write )
	{
		if ( eI2C_OK != gpf_i2c_write( eSTUSB4500_I2C_ADDR_0, addr,(uint8_t*) p_tx_data, size ))
		{
			status = eSTUSB4500_ERROR;
		}
	}
	else
	{
		status = eSTUSB4500_ERROR;

		STUSB4500_DBG_PRINT( "Low level write function not defined!" );
		STUSB4500_ASSERT( 0 );
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Read from register
*
*	param:		addr 		- Register address
*	param:		p_rx_data	- Pointer to receive data
*	param:		size		- Size of received data
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_low_if_read_register(const stusb4500_addr_t addr, uint8_t * p_rx_data, const uint32_t size)
{
	stusb4500_status_t status = eSTUSB4500_OK;

	if ( NULL != gpf_i2c_read )
	{
		if ( eI2C_OK != gpf_i2c_read( eSTUSB4500_I2C_ADDR_0, addr, p_rx_data, size ))
		{
			status = eSTUSB4500_ERROR;
		}
	}
	else
	{
		status = eSTUSB4500_ERROR;

		STUSB4500_DBG_PRINT( "Low level read function not defined!" );
		STUSB4500_ASSERT( 0 );
	}

	return status;
}



//////////////////////////////////////////////////////////////
// END OF FILE
//////////////////////////////////////////////////////////////

