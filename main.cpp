#include "mainwindow.h"
#include <QApplication>

#include <iostream>
#include <stdint.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include  "GPIO.h"
#include "mcp3424.h"

using namespace std;

void checkRS_Port(){

    int handle;
    struct termios term;
    int USB[10] = {0,1,2,3,4,5,6,7,8,9};

    for(int numberUSB = 0 ; numberUSB < 10 ; numberUSB++){
        char path[DIRECTION_MAX];
        snprintf(path, DIRECTION_MAX, "/dev/ttyUSB%d", USB[numberUSB]);
        //open device
        handle = open( path, O_RDWR | O_NOCTTY | O_NONBLOCK );

        if( handle < 0 ){
            printf("Can't open ttyUSB%d  \n",USB[numberUSB]);
            continue;
        }
        //get device struct
        if( tcgetattr( handle, &term ) != 0 ){
                close( handle );
                printf("Can't tcgetattr ttyUSB%d \n",USB[numberUSB]);
                continue;
        }

        term.c_cflag &= ~CSTOPB;          // One bite of stop
        term.c_cflag &= ~CSIZE;
        term.c_cflag |= CS8;                //  Mask for 8bite data
        term.c_iflag &= ~IXON ;           //software flow control out off
        term.c_cflag &= ~IXOFF;                // software flow contron IN off

        cfsetispeed(&term,B9600);
        cfsetospeed(&term,B9600);

        // cleean buffor
        int buf = 50;
        while(buf){
            char temp[10] = {' ', ' ' , ' ',' ', ' ' , ' ' ,' ' , ' ' , ' ' , ' '  };
            read(handle , temp , sizeof(char) *10 );     // check , that buffor is empty
            buf--;

           if (temp[1] == ' ' && temp[9] == ' ' )
             break;

        }

        int myflag = 1;

        for (int i = 0 ; i<10 ; i++ ){
            char myChar[7] = { 'H', 'e' , 'l', 'l' , 'o' ,' ' , '!' };
            int NumberOfBytesToRead =sizeof(char)*7;
            char temp[7]= {' '  , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' };

            write( handle, myChar, NumberOfBytesToRead);
            usleep(50000);
            read(handle , temp , NumberOfBytesToRead);


            for ( int i = 0 ; i<7 ; i++ ){

                char x = (char)temp[i] ;

                if (myChar[i] != x ){
                    myflag = 0;
                }
            }
        }

        if ( myflag == 0 )
            printf("Problem z ttyUSB%d \n",USB[numberUSB]);
        else
            printf("Port ttyUSB%d dziala ok  \n", USB[numberUSB]);


        close(handle);
    }


}

int main(int argc, char *argv[])
{
    GPIO gpio;
    mcp3424 mcp3424;

    QApplication a(argc, argv);
    MainWindow w;
    w.showFullScreen();

    return a.exec();
}
