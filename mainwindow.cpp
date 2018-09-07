#include "mainwindow.h"
#include "ui_mainwindow.h"

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


GPIO gpio;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->setUrl(QUrl(QStringLiteral("http://www.inwebit.pl/")));
    ui->widget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    checkRS_Port();
    ui->label_3->setText("OK");
}



void MainWindow::on_pushButton_clicked()
{
    int selectedGpio;

    if (ui->radioButton->isChecked())
        selectedGpio = 27;
    else if (ui->radioButton_2->isChecked())
        selectedGpio = 1;
    else if (ui->radioButton_3->isChecked())
        selectedGpio = 12;
    else if (ui->radioButton_4->isChecked())
        selectedGpio = 26;
    else
        return;

    std::cout << "selected gpio: " << selectedGpio <<std::endl;

    gpio.gpioOn(selectedGpio);

    if (ui->radioButton->isChecked())
        ui->lcdNumber->display("1");
    else if (ui->radioButton_2->isChecked())
        ui->lcdNumber_2->display("1");
    else if (ui->radioButton_3->isChecked())
        ui->lcdNumber_3->display("1");
    else if (ui->radioButton_4->isChecked())
        ui->lcdNumber_4->display("1");
}

void MainWindow::on_pushButton_2_clicked()
{
    int selectedGpio;

    if (ui->radioButton->isChecked())
        selectedGpio = 27;
    else if (ui->radioButton_2->isChecked())
        selectedGpio = 1;
    else if (ui->radioButton_3->isChecked())
        selectedGpio = 12;
    else if (ui->radioButton_4->isChecked())
        selectedGpio = 26;
    else
        return;


    gpio.gpioOff(selectedGpio);

    if (ui->radioButton->isChecked())
        ui->lcdNumber->display("0");
    else if (ui->radioButton_2->isChecked())
        ui->lcdNumber_2->display("0");
    else if (ui->radioButton_3->isChecked())
        ui->lcdNumber_3->display("0");
    else if (ui->radioButton_4->isChecked())
        ui->lcdNumber_4->display("0");
}
