This file decribes how to use TFA98XX drivers for MCU/EC platforms.

In case Calibration is needed main TFA START API must be with second parameter set to TRUE as follows :

Tfa9890_Error_t Tfa_generic_start(uint16_t slave_address, uint8_t calibrate = TRUE)

if Calibrate params is set to FALSE then 8 ohm will be hardcoded in the MTP at first boot up

Customer needs to define TFA98XX_PRINT (for debugging) and TFA98XX_SLEEP (sleeping execution)

Customer needs to replace TFA_I2C_WriteRead default with I2C WriteRead function 
