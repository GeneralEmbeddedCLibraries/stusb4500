//////////////////////////////////////////////////////////////
// 
//	project:		STUSB4500 EVALUATION
//	date:			06.09.2020
//	
//	author:			Ziga Miklosic
//
//////////////////////////////////////////////////////////////

#ifndef _STUSB4500_REGDEF_H_
#define _STUSB4500_REGDEF_H_

//////////////////////////////////////////////////////////////
//	INCLUDES
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//	DEFINITIONS
//////////////////////////////////////////////////////////////

// Device I2C address
// NOTE: Based on pin definitions of DevADDR0 & DevADDR1
typedef enum
{
	eSTUSB4500_I2C_ADDR_0 = 0x28,
	eSTUSB4500_I2C_ADDR_1 = 0x29,
	eSTUSB4500_I2C_ADDR_2 = 0x2A,
	eSTUSB4500_I2C_ADDR_3 = 0x2B,
} stusb4500_i2c_addr_t;

// Register addresses
typedef enum
{
	eSTUSB4500_ADDR_ALERT_STATUS_1			= 0x0B,
	eSTUSB4500_ADDR_ALERT_STATUS_1_MASK		= 0x0C,
	eSTUSB4500_ADDR_PORT_STATUS_0			= 0x0D,
	eSTUSB4500_ADDR_PORT_STATUS_1			= 0x0E,
	eSTUSB4500_ADDR_CC_STATUS				= 0x11,
	eSTUSB4500_ADDR_CC_HW_FAULT_STATUS_0	= 0x12,
	eSTUSB4500_ADDR_CC_HW_FAULT_STATUS_1	= 0x13,
	eSTUSB4500_ADDR_PD_TYPEC_STATUS			= 0x14,
	eSTUSB4500_ADDR_TYPEC_STATUS			= 0x15,
	eSTUSB4500_ADDR_PRT_STATUS				= 0x16,
	eSTUSB4500_ADDR_PD_COMMAND_CTRL			= 0x1A,
	eSTUSB4500_ADDR_DEVICE_ID				= 0x2F,

	eSTUSB4500_ADDR_RX_HEADER_LOW			= 0x31,
	eSTUSB4500_ADDR_RX_HEADER_HIGH			= 0x32,

	eSTUSB4500_ADDR_RX_DATA_OBJ1_0			= 0x33,
	eSTUSB4500_ADDR_RX_DATA_OBJ1_1			= 0x34,
	eSTUSB4500_ADDR_RX_DATA_OBJ1_2			= 0x35,
	eSTUSB4500_ADDR_RX_DATA_OBJ1_3			= 0x36,

	eSTUSB4500_ADDR_RX_DATA_OBJ2_0			= 0x37,
	eSTUSB4500_ADDR_RX_DATA_OBJ2_1			= 0x38,
	eSTUSB4500_ADDR_RX_DATA_OBJ2_2			= 0x39,
	eSTUSB4500_ADDR_RX_DATA_OBJ2_3			= 0x3A,

	eSTUSB4500_ADDR_RX_DATA_OBJ3_0			= 0x3B,
	eSTUSB4500_ADDR_RX_DATA_OBJ3_1			= 0x3C,
	eSTUSB4500_ADDR_RX_DATA_OBJ3_2			= 0x3D,
	eSTUSB4500_ADDR_RX_DATA_OBJ3_3			= 0x3E,

	eSTUSB4500_ADDR_RX_DATA_OBJ4_0			= 0x3F,
	eSTUSB4500_ADDR_RX_DATA_OBJ4_1			= 0x40,
	eSTUSB4500_ADDR_RX_DATA_OBJ4_2			= 0x41,
	eSTUSB4500_ADDR_RX_DATA_OBJ4_3			= 0x42,

	eSTUSB4500_ADDR_RX_DATA_OBJ5_0			= 0x43,
	eSTUSB4500_ADDR_RX_DATA_OBJ5_1			= 0x44,
	eSTUSB4500_ADDR_RX_DATA_OBJ5_2			= 0x45,
	eSTUSB4500_ADDR_RX_DATA_OBJ5_3			= 0x46,

	eSTUSB4500_ADDR_RX_DATA_OBJ6_0			= 0x47,
	eSTUSB4500_ADDR_RX_DATA_OBJ6_1			= 0x48,
	eSTUSB4500_ADDR_RX_DATA_OBJ6_2			= 0x49,
	eSTUSB4500_ADDR_RX_DATA_OBJ6_3			= 0x4A,

	eSTUSB4500_ADDR_RX_DATA_OBJ7_0			= 0x4B,
	eSTUSB4500_ADDR_RX_DATA_OBJ7_1			= 0x4C,
	eSTUSB4500_ADDR_RX_DATA_OBJ7_2			= 0x4D,
	eSTUSB4500_ADDR_RX_DATA_OBJ7_3			= 0x4E,

	eSTUSB4500_ADDR_TX_HEADER_LOW			= 0x51,
	eSTUSB4500_ADDR_TX_HEADER_HIGH			= 0x52,

	eSTUSB4500_ADDR_DPM_PDO_NUM				= 0x70,

	eSTUSB4500_ADDR_DPM_SNK_PDO1_0 			= 0x85,
	eSTUSB4500_ADDR_DPM_SNK_PDO1_1 			= 0x86,
	eSTUSB4500_ADDR_DPM_SNK_PDO1_2 			= 0x87,
	eSTUSB4500_ADDR_DPM_SNK_PDO1_3 			= 0x88,

	eSTUSB4500_ADDR_DPM_SNK_PDO2_0 			= 0x89,
	eSTUSB4500_ADDR_DPM_SNK_PDO2_1 			= 0x8A,
	eSTUSB4500_ADDR_DPM_SNK_PDO2_2 			= 0x8B,
	eSTUSB4500_ADDR_DPM_SNK_PDO2_3 			= 0x8C,

	eSTUSB4500_ADDR_DPM_SNK_PDO3_0 			= 0x8D,
	eSTUSB4500_ADDR_DPM_SNK_PDO3_1 			= 0x8E,
	eSTUSB4500_ADDR_DPM_SNK_PDO3_2 			= 0x8F,
	eSTUSB4500_ADDR_DPM_SNK_PDO3_3 			= 0x90,

	eSTUSB4500_ADDR_RDO_REG_STATUS_0 		= 0x91,
	eSTUSB4500_ADDR_RDO_REG_STATUS_1 		= 0x92,
	eSTUSB4500_ADDR_RDO_REG_STATUS_2 		= 0x93,
	eSTUSB4500_ADDR_RDO_REG_STATUS_3 		= 0x94,
} stusb4500_addr_t;

// Device ID
// NOTE: 	This value is taken from ST library.
//			Cannot find in any document.
typedef enum
{
	eSTUSB4500_ID_0		= 0x25,
	eSTUSB4500_ID_1		= 0x21,
} stusb4500_dev_id_t;

// Alert status mask bits
typedef struct
{
	uint8_t reserved 						: 1;
	uint8_t PRT_STATUS_AL_MASK				: 1;
	uint8_t reserved1						: 2;
	uint8_t CC_FAULT_STATUS_AL_MASK			: 1;
	uint8_t TYPEC_MONITORING_STATUS_MASK	: 1;
	uint8_t PORT_STATUS_AL_MASK				: 1;
	uint8_t reserved2						: 1;
} stusb4500_ALERT_STATUS_1_MASK_bits_t;

// Alert status mask
typedef union
{
	uint8_t U;
	int8_t	I;
	stusb4500_ALERT_STATUS_1_MASK_bits_t B;
} stusb4500_ALERT_STATUS_1_MASK_t;

// Port status 0 bits
typedef struct
{
	uint8_t ATTACH_TRANS 	: 1;
	uint8_t reserved		: 7;
} stusb4500_PORT_STATUS_0_bits_t;

// Port status 0
typedef union
{
	uint8_t U;
	int8_t I;
	stusb4500_PORT_STATUS_0_bits_t B;
} stusb4500_PORT_STATUS_0_t;

// Port status 1 bits
typedef struct
{
	uint8_t ATTACH 			: 1;
	uint8_t DATA_MODE 		: 1;
	uint8_t POWER_MODE		: 1;
	uint8_t reserved 		: 1;
	uint8_t ATTACHED_DEVICE	: 1;
} stusb4500_PORT_STATUS_1_bits_t;

// Port status 1
typedef union
{
	uint8_t U;
	int8_t I;
	stusb4500_PORT_STATUS_1_bits_t B;
} stusb4500_PORT_STATUS_1_t;

// CC status bits
typedef struct
{
	uint8_t CC1_STATE				: 2;
	uint8_t CC2_STATE				: 2;
	uint8_t CONNECT_RESULT			: 1;
	uint8_t LOCKING_4_CONNECTION	: 1;
	uint8_t reserved				: 2;
} stusb4500_CC_STATUS_bits_t;

// CC status
typedef union
{
	uint8_t U;
	int8_t I;
	stusb4500_CC_STATUS_bits_t B;
} stusb4500_CC_STATUS_t;


//////////////////////////////////////////////////////////////
//	VARIABLES
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// END OF FILE
//////////////////////////////////////////////////////////////

#endif // _STUSB4500_REGDEF_H_
