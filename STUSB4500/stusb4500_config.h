//////////////////////////////////////////////////////////////
//
//	project:		ILI9488 EVALUATION
//	date:			10.09.2020
//
//	author:			Ziga Miklosic
//
//////////////////////////////////////////////////////////////

#ifndef _STUSB4500_CONFIG_H_
#define _STUSB4500_CONFIG_H_

//////////////////////////////////////////////////////////////
//	INCLUDES
//////////////////////////////////////////////////////////////
#include "stdint.h"
#include "stm32f7xx_hal.h"

// Debug communication port
#include "com_dbg.h"


//////////////////////////////////////////////////////////////
//	DEFINITIONS
//////////////////////////////////////////////////////////////

// Enable debug mode (0/1)
#define STUSB4500_DEBUG_EN				( 1 )


// **********************************************************
// 	I2C INTERFACE
// **********************************************************
// TODO: I2C interface should be provided in low level ->
//	should be configurable via config file


// **********************************************************
// 	GPIO PINS
// **********************************************************
// TODO: Jet to be implemented


// **********************************************************
// 	PDO DEFINITIONS
// **********************************************************

// Negotiate only 5V
// NOTE: This chooses always PDO1, PDO2 & PDO3 not used in that case
#define STUSB4500_NEGOTIATE_ONLY_5V_EN	( 0 )

// PDO2
#define STUSB4500_PDO_2_VOLTAGE_V		( 9.0f )	// [V]
#define STUSB4500_PDO_2_CURRENT_A		( 0.5f )	// [A]

// PDO3
#define STUSB4500_PDO_3_VOLTAGE_V		( 13.0 )	// [V]
#define STUSB4500_PDO_3_CURRENT_A		( 1.0f )	// [A]



// **********************************************************
// 	DEBUG COM PORT
// **********************************************************
#if ( STUSB4500_DEBUG_EN )
	#define STUSB4500_DBG_PRINT( ... )					( com_dbg_print( eCOM_DBG_CH_USBC_PD, (const char*) __VA_ARGS__ ))
	#define STUSB4500_ASSERT(x)							if ( 0 == x ) { STUSB4500_DBG_PRINT("ASSERT ERROR! file: %s, line: %d", __FILE__, __LINE__ ); while(1){} }
#else
	#define STUSB4500_DBG_PRINT( ... )					{ ; }
	#define STUSB4500_ASSERT(x)							{ ; }
#endif


//////////////////////////////////////////////////////////////
// END OF FILE
//////////////////////////////////////////////////////////////

#endif // _ILI9488_CONFIG_H_
