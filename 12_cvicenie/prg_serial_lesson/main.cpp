#include "mbed.h"
 
DigitalOut myled(LED1);
BufferedSerial serial (USBTX, USBRX, 115200);
 
void blink(uint32_t i, Kernel::Clock::duration_u32 t = 50ms){
    for(; i > 0; i--){
        myled = true;
        ThisThread::sleep_for(t);
        myled = false;
        ThisThread::sleep_for(t);
    }
}
 
int main() {
    serial.set_blocking(true);
    const char init_msg = 'i';
    serial.write(&init_msg, 1);
    blink(5);
    while(true){
        char received_value;
        ssize_t received_in_fact = serial.read(&received_value, 1);
        char acknowledge_msg = 'a';
        if(received_in_fact == 1){
            switch(received_value){
                case 's': myled = true; break;
                case 'e': myled = false; break;
                case 'f': myled = !myled; break;
                default: acknowledge_msg = '!';
            }
        }else{
            acknowledge_msg = '*';
        }
        serial.write(&acknowledge_msg, 1);
    }
}

