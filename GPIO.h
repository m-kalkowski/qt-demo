#ifndef GPIO_H
#define GPIO_H

using namespace std;
class GPIO
{
    public:
        GPIO();
        virtual ~GPIO();
        void gpioOn(int pin);
        void gpioOff(int pin);
        int GPIOWrite(int, int);
        int GPIODirection(int,int);
        int GPIOUnexport(int);
        int GPIOExport(int);
};

#endif // GPIO_H
