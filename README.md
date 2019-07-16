Brief:

Example firmware for ultraviolet lamp controller used for photoresist PCB creation.

It's schematics created via EasyEDA: https://easyeda.com/kalashnikov.alexander.b/exposer
You can order PCB for that device from there.

Build:

Just open the project in System Workbench for STM32 and build Release configuration. After that upload firmware via SWD (STLINK) or UART (Flash Loader Demonstrator) to MCU.

Features:

It acts as a countdown timer with memory, so the last successfull downcounting will be saved as a default exposition time.
It has a latch button, so if it is not pressed - the timer is paused and LED drivers are powered off. This feature can be used for a case opened checks during photoresist exposition if somebody will peek up into device.
