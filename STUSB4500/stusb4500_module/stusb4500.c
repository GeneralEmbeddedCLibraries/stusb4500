//////////////////////////////////////////////////////////////
// 
//	project:		STUSB4500 EVALUATION
//	date:			05.09.2020
//	
//	author:			Ziga Miklosic
//
//////////////////////////////////////////////////////////////


// TODO: 	1. check gpio alarms/attach, power1/2 status


//////////////////////////////////////////////////////////////
//	INCLUDES
//////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"
#include "string.h"

#include "stusb4500.h"
#include "stusb4500_config.h"
#include "stusb4500_low_if.h"

// Low level
#include "i2c.h"


//////////////////////////////////////////////////////////////
//	DEFINITIONS
//////////////////////////////////////////////////////////////

// Convert voltage physical to raw
#define STUSB4500_CONVERT_VOLTAGE_TO_RAW			((float32_t) ( 1.0 / 0.05 )) 	// [ quant / V ]

// Convert voltage raw to physical
#define STUSB4500_CONVERT_VOLTAGE_TO_V				((float32_t) ( 0.05 )) 			// [ V / quant ]

// Convert voltage physical to raw
#define STUSB4500_CONVERT_CURRENT_TO_RAW			((float32_t) ( 1.0 / 0.01 )) 	// [ quant / A ]

// Convert voltage raw to physical
#define STUSB4500_CONVERT_CURRENT_TO_A				((float32_t) ( 0.01 )) 			// [ A / quant ]


// Raw PDO
typedef union
{
	uint32_t U;
	int32_t I;
	struct
	{
		uint32_t current 			: 10;	// [9:0]
		uint32_t voltage 			: 10;	// [19:10]
		uint32_t reserved 			: 3;	// [22:20]
		uint32_t fast_role 			: 2;	// [24:23]
		uint32_t dual_role_data		: 1;	// [25]
		uint32_t USB_com_en			: 1;	// [26]
		uint32_t unconstrained_pwr	: 1;	// [27]
		uint32_t high_capability	: 1;	// [28]
		uint32_t dual_role_pwr		: 1;	// [29]
		uint32_t type				: 2;	// [31:30]
	} B;
} stusb4500_pdo_raw_t;

// Raw RDO
typedef union
{
	uint32_t U;
	int32_t I;
	struct
	{
		uint32_t current_max 			: 10;	// [9:0]
		uint32_t current_operating		: 10;	// [19:10]
		uint32_t reserved 				: 3;	// [22:20]
		uint32_t extend_msg_support 	: 1;	// [23]
		uint32_t no_usb_suspend			: 1;	// [24]
		uint32_t usb_com_capable		: 1;	// [25]
		uint32_t capability_mismatched	: 1;	// [26]
		uint32_t give_back_flag			: 1;	// [27]
		uint32_t object_position		: 3;	// [30:28]
		uint32_t reserved2				: 1;	// [31]
	} B;
} stusb4500_rdo_raw_t;


//////////////////////////////////////////////////////////////
//	VARIABLES
//////////////////////////////////////////////////////////////

// Device PDO
static stusb4500_pdo_t g_PDO[ eSTUSB4500_PDO_NUM_OF ] =
{
		// PDO 1 Description - NOTE: Fixed DO NOT CHANGE!
		{
			.num = eSTUSB4500_PDO_1,

			.voltage					= 5.0f,
			.current 					= 1.5f,

			.flags.USB_com_enable 		= 0,
			.flags.dual_role_data		= 0,
			.flags.dual_role_power		= 0,
			.flags.unconstrained_power	= 0,
			.flags.high_capability		= 1,

			.fast_role_swap				= eSTUSB4500_ROLE_SWAP_NOT_SUPPORTED,
			.type 						= eSTUSB4500_PDO_TYPE_FIXED_SUPPLY,
		},

		// PDO 2 Description - NOTE: Fixed DO NOT CHANGE!
		{
			.num = eSTUSB4500_PDO_2,

			.voltage					= STUSB4500_PDO_2_VOLTAGE_V,
			.current 					= STUSB4500_PDO_2_CURRENT_A,

			.flags.USB_com_enable 		= 0,
			.flags.dual_role_data		= 0,
			.flags.dual_role_power		= 0,
			.flags.unconstrained_power	= 0,
			.flags.high_capability		= 0,

			.fast_role_swap				= eSTUSB4500_ROLE_SWAP_NOT_SUPPORTED,
			.type 						= eSTUSB4500_PDO_TYPE_FIXED_SUPPLY,
		},

		// PDO 3 Description - NOTE: Fixed DO NOT CHANGE!
		{
			.num = eSTUSB4500_PDO_3,

			.voltage					= STUSB4500_PDO_3_VOLTAGE_V,
			.current 					= STUSB4500_PDO_3_CURRENT_A,

			.flags.USB_com_enable 		= 0,
			.flags.dual_role_data		= 0,
			.flags.dual_role_power		= 0,
			.flags.unconstrained_power	= 0,
			.flags.high_capability		= 0,

			.fast_role_swap				= eSTUSB4500_ROLE_SWAP_NOT_SUPPORTED,
			.type 						= eSTUSB4500_PDO_TYPE_FIXED_SUPPLY,
		}
};

// Device status
static stusb4500_usb_status_t g_stusb4500_status;

// Initialization flag
static bool gb_is_init = false;


//////////////////////////////////////////////////////////////
// FUNCTIONS PROTOTYPES
//////////////////////////////////////////////////////////////
static stusb4500_status_t 	stusb4500_reinit					(void);
static stusb4500_status_t 	stusb4500_soft_reset				(void);
static stusb4500_status_t 	stusb4500_ping_device				(void);
static stusb4500_status_t 	stusb4500_clear_interrupts			(void);
static stusb4500_status_t 	stusb4500_set_interrupt_mask		(void);
static stusb4500_status_t	stusb4500_refresh_status			(void);
static stusb4500_status_t 	stusb4500_write_device_pdo			(const stusb4500_pdo_t * p_pdo);
static stusb4500_status_t 	stusb4500_read_device_pdo			(stusb4500_pdo_t * p_pdo);
static void 				stusb4500_assemble_raw_pdo_frame	(stusb4500_pdo_raw_t * const p_pdo_raw, const stusb4500_pdo_t * const p_pdo);
static void 				stusb4500_parse_raw_pdo_frame		(stusb4500_pdo_t * const p_pdo, const stusb4500_pdo_raw_t * const p_pdo_raw);
static void 				stusb4500_parse_raw_rdo_frame		(stusb4500_usb_status_t * const p_usb_status,
																const stusb4500_rdo_raw_t * const p_rdo_raw,
																const stusb4500_PORT_STATUS_1_t * const p_port_stat,
																const stusb4500_CC_STATUS_t * const p_cc_stat);

//////////////////////////////////////////////////////////////
// FUNCTIONS 
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/*
*			Initialization of STUSB4500
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_init(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	stusb4500_pdo_t read_PDO[3];
	uint8_t i;

	if ( false == gb_is_init )
	{
		// Low level init
		// TODO: Here check if I2C is init...

		if ( eSTUSB4500_OK != stusb4500_low_if_init())
		{
			status = eSTUSB4500_ERROR;
		}
		else
		{
			gb_is_init = true;

			// Ping device
			status |= stusb4500_ping_device();

			// Clear all interrupts
			status |= stusb4500_clear_interrupts();

			// Set interrupt mask
			status |= stusb4500_set_interrupt_mask();
		}

		// Configure PDOs
		status |= stusb4500_write_device_pdo((stusb4500_pdo_t*) &g_PDO );

		// Validate written PDOs
		status |= stusb4500_read_device_pdo((stusb4500_pdo_t*) &read_PDO );

		for ( i = 0; i < eSTUSB4500_PDO_NUM_OF; i++ )
		{
			if ( 0 != memcmp( &g_PDO[i].voltage, &read_PDO[i].voltage, sizeof( float32_t )))
			{
				status = eSTUSB4500_ERROR;
				break;
			}
		}

		// Issue re-negotiation with the source
		status |= stusb4500_soft_reset();

		// Check status
		if ( eSTUSB4500_OK != status )
		{
			STUSB4500_DBG_PRINT( "Module initialization failed!" );
			STUSB4500_ASSERT( 0 );
		}
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Re-Initialization of STUSB4500
*
*		This function is used in case if PD source device is not
*		present at initialization time, therefore negotiation cannot
*		occur. Calling this function triggers re-negotiation based
*		on preconfigurable PDOs.
*
*		For detection of cable check status.attached flag!
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_reinit(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	stusb4500_pdo_t read_PDO[3];
	uint8_t i;

	// Check if init
	if ( true == gb_is_init )
	{
		// Ping device
		status |= stusb4500_ping_device();

		// Clear all interrupts
		status |= stusb4500_clear_interrupts();

		// Set interrupt mask
		status |= stusb4500_set_interrupt_mask();

		// Configure PDOs
		status |= stusb4500_write_device_pdo((stusb4500_pdo_t*) &g_PDO );

		// Validate written PDOs
		status |= stusb4500_read_device_pdo((stusb4500_pdo_t*) &read_PDO );

		for ( i = 0; i < eSTUSB4500_PDO_NUM_OF; i++ )
		{
			// Check that voltages matches
			if ( 0 != memcmp( &g_PDO[i].voltage, &read_PDO[i].voltage, sizeof( float32_t )))
			{
				status = eSTUSB4500_ERROR;
				break;
			}
		}

		// Issue re-negotiation with the source
		status |= stusb4500_soft_reset();
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			STUSB4500 handler
*
*		Here status of device is refreshed and re-negotiated
*		with power source device at cable plug-in.
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_hndl(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	static stusb4500_attach_t attached_prev = eSTUSB4500_NOT_ATTACHED;

	// Refresh status
	status = stusb4500_refresh_status();

	// Reinitialize after plug in cable
	if ( eSTUSB4500_OK == status )
	{
		if 	( 	( eSTUSB4500_ATTACHED == g_stusb4500_status.attached )
			&& 	( eSTUSB4500_NOT_ATTACHED == attached_prev ))
		{
			stusb4500_reinit();
		}

		// Store previous
		attached_prev = g_stusb4500_status.attached;
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Read device ID
*
*	param:		p_id 		- Pointer to read ID
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_read_device_id	(uint8_t * const p_id)
{
	stusb4500_status_t status = eSTUSB4500_OK;

	if ( true == gb_is_init )
	{
		status = stusb4500_low_if_read_register( eSTUSB4500_ADDR_DEVICE_ID, p_id, 1UL );
	}
	else
	{
		status = eSTUSB4500_ERROR;

		STUSB4500_DBG_PRINT( "Module not initialized!" );
		STUSB4500_ASSERT( 0 );
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Ping device
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_ping_device(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	uint8_t dev_id;

	if ( eSTUSB4500_OK == stusb4500_read_device_id( &dev_id ))
	{
		if 	(	( eSTUSB4500_ID_0 == dev_id )
			||	( eSTUSB4500_ID_1 == dev_id ))
		{
			status = eSTUSB4500_OK;
		}
		else
		{
			status = eSTUSB4500_ERROR;
		}
	}
	else
	{
		status = eSTUSB4500_ERROR;
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Clear device interrupts.
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_clear_interrupts(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	uint8_t dummy[10];

	status |= stusb4500_low_if_read_register( eSTUSB4500_ADDR_PORT_STATUS_0, (uint8_t*) &dummy, 10UL );

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Set up alarm interrupt mask
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_set_interrupt_mask(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	stusb4500_ALERT_STATUS_1_MASK_t alert_msk_reg;

	// Unmask alarms
	alert_msk_reg.U = 0;
	alert_msk_reg.B.CC_FAULT_STATUS_AL_MASK = 1;
	alert_msk_reg.B.PORT_STATUS_AL_MASK = 1;
	alert_msk_reg.B.PRT_STATUS_AL_MASK = 1;
	alert_msk_reg.B.TYPEC_MONITORING_STATUS_MASK = 1;

	// Write
	status = stusb4500_low_if_write_register( eSTUSB4500_ADDR_ALERT_STATUS_1_MASK, &alert_msk_reg.U, 1UL );

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Perform soft reset of device.
*
*		NOTE: This triggers new PD contract negotiations.
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
stusb4500_status_t stusb4500_soft_reset(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;

	status |= stusb4500_low_if_write_register( eSTUSB4500_ADDR_TX_HEADER_LOW, (const uint8_t*) "\x0D", 1UL );
	status |= stusb4500_low_if_write_register( eSTUSB4500_ADDR_PD_COMMAND_CTRL, (const uint8_t*) "\x26", 1UL );

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Read device PDOs
*
*	param:		p_pdo		- Pointer to PDO data structure
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_read_device_pdo(stusb4500_pdo_t * p_pdo)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	stusb4500_pdo_raw_t pdo_raw;
	uint8_t i;

	for ( i = 0; i < eSTUSB4500_PDO_NUM_OF; i++ )
	{
		// Read raw PDO
		status |= stusb4500_low_if_read_register(( eSTUSB4500_ADDR_DPM_SNK_PDO1_0 + ( 4U * i )), (uint8_t*) &pdo_raw.U, 4UL );

		// Parse PDO
		if ( eSTUSB4500_OK == status )
		{
			stusb4500_parse_raw_pdo_frame( p_pdo, &pdo_raw );
		}

		// Next PDO
		p_pdo++;
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Write device PDOs
*
*	param:		p_pdo		- Pointer to PDO data structure
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_write_device_pdo(const stusb4500_pdo_t * p_pdo)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	stusb4500_pdo_raw_t pdo_raw;
	uint8_t i;

	for ( i = 0; i < eSTUSB4500_PDO_NUM_OF; i++ )
	{
		// Assemble pdo
		stusb4500_assemble_raw_pdo_frame( &pdo_raw, p_pdo );

		// Write PDO
		status |= stusb4500_low_if_write_register(( eSTUSB4500_ADDR_DPM_SNK_PDO1_0 + ( 4U * i )), (uint8_t*) &pdo_raw.U, 4UL );

		// Next PDO
		p_pdo++;
	}

	#if ( STUSB4500_NEGOTIATE_ONLY_5V_EN )

		// Set only PDO1
		status |= stusb4500_low_if_write_register( eSTUSB4500_ADDR_DPM_PDO_NUM, (const uint8_t*) "\x01", 1UL );
	#else

		// Check for all three PDO match
		status |= stusb4500_low_if_write_register( eSTUSB4500_ADDR_DPM_PDO_NUM, (const uint8_t*) "\x03", 1UL );
	#endif

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Assemble raw pdo frame
*
*	param:		p_pdo_raw	- Pointer to raw PDO data
*	param:		p_pdo		- Pointer to PDO data
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static void stusb4500_assemble_raw_pdo_frame(stusb4500_pdo_raw_t * const p_pdo_raw, const stusb4500_pdo_t * const p_pdo)
{
	p_pdo_raw -> U = 0;

	p_pdo_raw -> B.current 				= ( STUSB4500_CONVERT_CURRENT_TO_RAW * p_pdo -> current );
	p_pdo_raw -> B.voltage 				= ( STUSB4500_CONVERT_VOLTAGE_TO_RAW * p_pdo -> voltage );

	p_pdo_raw -> B.USB_com_en 			= p_pdo -> flags.USB_com_enable;
	p_pdo_raw -> B.dual_role_data		= p_pdo -> flags.dual_role_data;
	p_pdo_raw -> B.dual_role_pwr		= p_pdo -> flags.dual_role_power;
	p_pdo_raw -> B.unconstrained_pwr 	= p_pdo -> flags.unconstrained_power;
	p_pdo_raw -> B.high_capability		= p_pdo -> flags.high_capability;

	p_pdo_raw -> B.fast_role			= p_pdo -> fast_role_swap;
	p_pdo_raw -> B.type					= p_pdo -> type;

}


//////////////////////////////////////////////////////////////
/*
*			Parse raw pdo frame
*
*	param:		p_pdo		- Pointer to PDO data
*	param:		p_pdo_raw	- Pointer to raw PDO data
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static void stusb4500_parse_raw_pdo_frame(stusb4500_pdo_t * const p_pdo, const stusb4500_pdo_raw_t * const p_pdo_raw)
{
	p_pdo -> current 					= ( STUSB4500_CONVERT_CURRENT_TO_A  * p_pdo_raw -> B.current );
	p_pdo -> voltage 					= ( STUSB4500_CONVERT_VOLTAGE_TO_V * p_pdo_raw -> B.voltage );

	p_pdo -> flags.USB_com_enable		= p_pdo_raw -> B.USB_com_en;
	p_pdo -> flags.dual_role_data		= p_pdo_raw -> B.dual_role_data;
	p_pdo -> flags.dual_role_power		= p_pdo_raw -> B.dual_role_pwr;
	p_pdo -> flags.unconstrained_power	= p_pdo_raw -> B.unconstrained_pwr;
	p_pdo -> flags.high_capability		= p_pdo_raw -> B.high_capability;

	p_pdo -> fast_role_swap 			= p_pdo_raw -> B.fast_role;
}


//////////////////////////////////////////////////////////////
/*
*			Get status of device
*
*	param:		none
*	return:		p_status - Pointer to status data
*/
//////////////////////////////////////////////////////////////
const stusb4500_usb_status_t * stusb4500_get_status(void)
{
	stusb4500_usb_status_t * p_status;

	if ( true == gb_is_init )
	{
		p_status = &g_stusb4500_status;
	}
	else
	{
		p_status = NULL;

		STUSB4500_DBG_PRINT( "Module not initialized!" );
		STUSB4500_ASSERT( 0 );
	}

	return (const stusb4500_usb_status_t*) p_status;
}


//////////////////////////////////////////////////////////////
/*
*			Read status from device
*
*	param:		none
*	return:		status 		- Status of operation
*/
//////////////////////////////////////////////////////////////
static stusb4500_status_t stusb4500_refresh_status(void)
{
	stusb4500_status_t status = eSTUSB4500_OK;
	stusb4500_PORT_STATUS_1_t port_status1;
	stusb4500_CC_STATUS_t cc_status;
	stusb4500_rdo_raw_t rdo_raw;

	// Check if init
	if ( true == gb_is_init )
	{
		// Read USB-C connection status
		status |= stusb4500_low_if_read_register( eSTUSB4500_ADDR_PORT_STATUS_1, &port_status1.U, 1UL );
		status |= stusb4500_low_if_read_register( eSTUSB4500_ADDR_CC_STATUS, &cc_status.U, 1UL );

		// Read USB PD status
		status |= stusb4500_low_if_read_register( eSTUSB4500_ADDR_RDO_REG_STATUS_0, (uint8_t*) &rdo_raw.U, 4UL );

		// Parse read status
		stusb4500_parse_raw_rdo_frame( &g_stusb4500_status, &rdo_raw, &port_status1, &cc_status );
	}
	else
	{
		status = eSTUSB4500_ERROR;

		STUSB4500_DBG_PRINT( "Module not initialized!" );
		STUSB4500_ASSERT( 0 );
	}

	return status;
}


//////////////////////////////////////////////////////////////
/*
*			Parse raw rdo frame
*
*	param:		p_usb_status	- Pointer to usb data
*	param:		p_rdo_raw		- Pointer to raw RDO data
*	param:		p_port_stat		- Pointer to port status register
*	param:		p_cc_stat		- Pointer to CC register
*	return:		status 			- Status of operation
*/
//////////////////////////////////////////////////////////////
static void stusb4500_parse_raw_rdo_frame(stusb4500_usb_status_t * const p_usb_status, const stusb4500_rdo_raw_t * const p_rdo_raw, const stusb4500_PORT_STATUS_1_t * const p_port_stat, const stusb4500_CC_STATUS_t * const p_cc_stat)
{
	// Current capabilities
	p_usb_status -> RDO.current.maximum 	= ( STUSB4500_CONVERT_CURRENT_TO_A * p_rdo_raw -> B.current_max );
	p_usb_status -> RDO.current.operating 	= ( STUSB4500_CONVERT_CURRENT_TO_A * p_rdo_raw -> B.current_operating );

	// Object position
	if ( p_rdo_raw -> B.object_position > 0 )
	{
		p_usb_status -> RDO.mode = eSTUSB4500_RDO_MODE_USB_PD;
		p_usb_status -> RDO.src_pdo = p_rdo_raw -> B.object_position;
	}
	else
	{
		p_usb_status -> RDO.mode = eSTUSB4500_RDO_MODE_USB_C;
	}

	// Attached
	p_usb_status -> attached = p_port_stat -> B.ATTACH ;

	// CC state
	p_usb_status -> CC1 = p_cc_stat -> B.CC1_STATE;
	p_usb_status -> CC2 = p_cc_stat -> B.CC2_STATE;
}


//////////////////////////////////////////////////////////////
// END OF FILE
//////////////////////////////////////////////////////////////

