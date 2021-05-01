# STUSB4500
USBC Power Delivery controller C library. Driver was tested on STM32. 

- STUSB4500 power delivery sink controller support up to 3 PDO
- User should define desire PDO2 & PDO3 in **stusb4500_cfg.h** file
- NOTE: PDO1 is fixed to 5V

## Example of usage

```
    // Init USBC PD controler
    stusb4500_init();

    // Get pointer to detail status data
    const stusb4500_usb_status_t * p_detail_status = stusb4500_get_status();

    // General status
    stusb4500_status_t general_status;

    while(1)
    {
        @every_xms
        {
            // USBC-PD controller handler
            general_status = stusb4500_hndl();

            // In case of error -> see detailed status
            if ( eSTUSB4500_OK != general_status )
            {
              // Check detailed status here
              // E.g.: p_detail_status -> CC1
            }
        }
    }
```
