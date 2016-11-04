# nxp-cortex
Projects for NXP Microcontrollers based upon the Cortex-M

11u35 - Sockpuppet implementations for the NXP11Uxx.
 initially implemented on the Embedded artists
 LPC11U35 quickstart board.

11u35/minimal - a launcher-based minimal implementation.

Cabling
------------------------------
FTDI 232-RL: 1/Gnd    11U35  Ground
FTDI 232-RL: 4/TX/Ora 11U35/18
FTDI 232-RL: 5/RX/Yel 11U35/19

Running MPE Umbilical Forth on the Mac

---- Start up the compiler
xArmCortexDevOSX include nxp11uxx-umbilical.ctl

---- boot the device in mass storage mode
Reset it while depressing the DFU button.

---- Switch to another window and make+install
---- the new binary
srec_cat @packageit.srecs
cp firmware.bin /Volumes/CRP\ DISABLD/firmware.bin 
sudo diskutil eject /Volumes/CRP\ DISABLD/

