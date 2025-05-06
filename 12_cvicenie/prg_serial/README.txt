Below you can find steps on how to communicate with NUCLEO-F446RE using named pipes!


Steps:

	1) First build project for recieving messages and create flashing sequence by NUCLEO.
	   -- This file can be found in this repository under name "mbed-os-blink_control.NUCLEO_F446RE.bin" --


	2) Compile prg_serial.c and prg_control.c using 

		clang -std=c99 -pedantic -Wall -Werror -O1 -g -fsanitize=address \
     			 prg_control.c prg_serial.c -o prg_control 
	

	3) While NUCLEO is connected to MAC find it's USB name using

		ls /dev/{tty,cu}.usbmodem*


	4) Build prg_comp.c using 

		./prg_control /dev/cu.usbmodem*

	   where * represents number at the end of the response to command in step 3)
			

	5) Flash NUCLEO with file mentioned in step 1)


	6) Green LED should flash 5 times, signeling that NUCLEO has written 'i' to buffer and is 
	   waiting for input


	7) Use following four commands to communicate with ocnnected NUCLEO:

		1 - nastaví periodu blikání LED na 50 ms
		2 - nastaví periodu blikání LED na 100 ms
		3 - nastaví periodu blikání LED na 200 ms
		4 - nastaví periodu blikání LED na 500 ms
		5 - nastaví periodu blikání LED na 1000 ms.
		0 - zastaví blikání, nechá LED ve stavu v jakém zrovna je.
