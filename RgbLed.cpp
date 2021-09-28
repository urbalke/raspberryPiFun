#include <iostream>
#include <pigpio.h>
#include <string>
#include <thread>

#define HIGH 1
#define LOW 0

using GPIOPort = int;

void setup(GPIOPort colorRed, GPIOPort colorGreen, GPIOPort colorBlue)
{
    std::cout << "Raspberry Pi initialized!" << std::endl;
    if (gpioInitialise() < 0)
        exit(1);

    gpioSetMode(colorRed, PI_OUTPUT);
    gpioSetMode(colorGreen, PI_OUTPUT);
    gpioSetMode(colorBlue, PI_OUTPUT);
}

void shine(GPIOPort port, int value, int &shouldContinue, bool & isShining)
{
    while (shouldContinue)
    {
        if(isShining)
        {
            gpioWrite(port, HIGH);
            gpioDelay(255 - (255 - value));
            gpioWrite(port, LOW);
            gpioDelay(255 - value);
        }
    }
}

int main(void)
{
    GPIOPort colorRed = 17;
    GPIOPort colorGreen = 27;
    GPIOPort colorBlue = 22;
    bool isInMainLoop = true;
    bool isShining = false;
    int shineRed, shineGreen, shineBlue;
    int shouldContinue = 1;
    setup(colorRed, colorGreen, colorBlue);

    while (isInMainLoop)
    {
        std::cout << "Enter RGB:" << std::endl;
        std::cin >> shineRed >> shineGreen >> shineBlue;     
        
        std::thread Red(shine, colorRed, shineRed, std::ref(shouldContinue), std::ref(isShining));
        std::thread Green(shine, colorGreen, shineGreen, std::ref(shouldContinue), std::ref(isShining));
        std::thread Blue(shine, colorBlue, shineBlue, std::ref(shouldContinue), std::ref(isShining));

        if (shineRed == 0 and shineGreen == 0 and shineBlue == 0)
        {
            isInMainLoop = false;
            std::cout << "Ending" << std::endl;
            shouldContinue = false;
            try
            {
                Red.join();
                Green.join();
                Blue.join();
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                gpioWrite(colorRed, LOW);
                gpioWrite(colorGreen, LOW);
                gpioWrite(colorBlue, LOW);
            }
            gpioTerminate();
            return 1;
        }
        else
        {
            isShining = true;
            shouldContinue = 1;
        }

        std::cout << "Should continue?" << std::endl;
        std::cin >> shouldContinue;

        if (shouldContinue == 0)
        {
            try
            {
                Red.join();
                Green.join();
                Blue.join();
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                gpioWrite(colorRed, LOW);
                gpioWrite(colorGreen, LOW);
                gpioWrite(colorBlue, LOW);
            }
        }
        
    }

    gpioTerminate();
}
