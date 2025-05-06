Below you can find steps on how to communicate with NUCLEO-F446RE using named pipes!


Steps:

	1) First build project for recieving messages and create flashing sequence by NUCLEO.
	   -- This file can be found in this repository under name "mbed-os-blink_control.NUCLEO_F446RE.bin" --
	   -- Source code for board is stored in file "main.cpp"

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

		s - rozsvícení LED. Nucleo pošle zpět na seriový port znak a (ack).
		e - zhasnutí LED. Nucleo pošle zpět na seriový port znak a (ack).
		f - změní stav LED. Nucleo pošle zpět na seriový port znak a (ack). 
		q - quit the program. 
