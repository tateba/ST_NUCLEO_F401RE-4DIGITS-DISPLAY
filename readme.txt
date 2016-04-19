*****************************************************************************
** 4 Digits Display controlled by the Nucleo  Board						   **
*****************************************************************************

Description
	The program read the year, month, day, date, hours, minutes and seconds
	to an RTC Module and prints it to the 4 digits display.
	The on board user switch is used to change the data printed.

RTOS
	The RTOS use is ChibiOS/RT, Thanks to Giovanni.

TARGET
	The program runs on an ST NUCLEOF401RE board from ST-Microelectronics.

Build Procedure
    1- Change the ChibiOS path according to your directories on the Makefile.
    2- Type make to compile the program

Flash the Board
	To program the board you can use one of the following:
    1- By using the st-flash command: st-flash write build/ch.bin 0x8000000
    2- By using my command, I add a makefile rules to easy program the board.
	the command is then: make program

	TODO: Comment the connexion of the board and external components and 
	modules.

Enjoy, comment.....
