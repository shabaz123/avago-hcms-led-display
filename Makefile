
LIB_PATH = ./BBBIOlib/



led_display : main.c 
	gcc -o led_display main.c -L ${LIB_PATH} -lBBBio -lpthread


clean :
	rm -rf led_display

