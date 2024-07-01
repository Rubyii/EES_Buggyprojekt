#define GPIO_Trigger 23
#define GPIO_Echo 24
#define LED 7
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include "adafruit-motor-hat-cpp-library/source/adafruitmotorhat.h"
#include "adafruit-motor-hat-cpp-library/source/util.h"
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include "gyro.h"

AdafruitMotorHAT hat;
auto motor1{hat.getMotor(1)};
auto motor4 {hat.getMotor(4)};

void hdlr(int s){	 
    motor1->run(AdafruitDCMotor::kRelease);
	motor4->run(AdafruitDCMotor::kRelease);        
    exit(s);     
}

float entfernung(){

    digitalWrite(GPIO_Trigger, HIGH);
    std::this_thread::sleep_for(std::chrono::seconds(0.00001));
    digitalWrite(GPIO_Trigger, LOW);
 

    while(digitalRead(GPIO_Echo) == LOW){
        auto startTime = std::chrono::high_resolution_clock::now();
    }
 
    while(digitalRead(GPIO_Echo) == HIGH){
        auto endTime = std::chrono::high_resolution_clock::now();
    }

     std::chrono::duration<double, std::nano> Zeitdifferenz = endTime - startTime;
     Zeitdifferenz =  Zeitdifferenz.count() / 1000000000; // zu Sekunden konvertieren
	
    auto entfernung = (Zeitdifferenz * 34300) / 2; // 34300cm/sek schallgeschwindigkeit
 
    return entfernung;
}

void LedAnAus(bool zustand){
    if (zustand)
    {
        digitalWrite(LED, HIGH);
    }else{
        digitalWrite(LED, LOW);
    }
}

void rechts_drehen(Gyro gyro){
    motor1->setSpeed(500);
    motor4->setSpeed(0);

    while (true)
    {
        gyroZ_Winkelgeschwindigkeit = gyro.readGyroZ()/131.0;
        float zeitSekunden = 0.01;
        gierachse = gierachse + gyroZ_Winkelgeschwindigkeit*zeitSekunden;
        std::cout << "Gierachse: " << gierachse << std::endl;

        float distanz = entfernung(); // [cm]
        std::cout << "Distanz: " << distanz << std::endl;

        if (gierachse <=-90)
        {
            motor1->setSpeed(0);
            return;
        }
    }
}

void links_drehen(Gyro gyro){
    motor1->setSpeed(0);
    motor4->setSpeed(500);

    while (true)
    {
        gyroZ_Winkelgeschwindigkeit = gyro.readGyroZ()/131.0;
        float zeitSekunden = 0.01;
        gierachse = gierachse + gyroZ_Winkelgeschwindigkeit*zeitSekunden;
        std::cout << "Gierachse: " << gierachse << std::endl;

        float distanz = entfernung(); // [cm]
        std::cout << "Distanz: " << distanz << std::endl;

        if (gierachse >=90)
        {
            motor4->setSpeed(0);
            return;
        }
    }
}

void geradeaus_fahren(int sekunden, Gyro gyro){
   
    motor1->setSpeed(500);
    motor4->setSpeed(500);
    motor1->run(AdafruitDCMotor::kForward);
    motor4->run(AdafruitDCMotor::kForward);
    auto startTime = std::chrono::steady_clock::now();

    while (true) {
        gyroZ_Winkelgeschwindigkeit = gyro.readGyroZ()/131.0;
        float zeitSekunden = 0.01;
        gierachse = gierachse + gyroZ_Winkelgeschwindigkeit*zeitSekunden;
        std::cout << "Gierachse: " << gierachse << std::endl;

        float distanz = entfernung(); // [cm]
        std::cout << "Distanz: " << distanz << std::endl;

        if (distanz <= 20)
        {
            LedAnAus(true);
            ausweichRoutine(gyro);

        }else{
            LedAnAus(false);
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        if (elapsedTime >= sekunden) {
            stoppen();
            return;  
        }
    }
}

void rueckwaerts_fahren(int sekunden, Gyro gyro){
    motor1->setSpeed(500);
    motor4->setSpeed(500);
    motor1->run(AdafruitDCMotor::kBackward);
    motor4->run(AdafruitDCMotor::kBackward);
    auto startTime = std::chrono::steady_clock::now();

    while (true) {

        gyroZ_Winkelgeschwindigkeit = gyro.readGyroZ()/131.0;
        float zeitSekunden = 0.01;
        gierachse = gierachse + gyroZ_Winkelgeschwindigkeit*zeitSekunden;
        std::cout << "Gierachse: " << gierachse << std::endl;

        float distanz = entfernung(); // [cm]
        std::cout << "Distanz: " << distanz << std::endl;


        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        if (elapsedTime >= sekunden) {
            stoppen();
            return;  
        }
    }
}

void stoppen(){
    motor1->setSpeed(0);
    motor4->setSpeed(0);
    return;
}

void ausweichRoutine(Gyro gyro){
    LedAnAus(false);
    links_drehen(gyro);
    geradeaus_fahren(2, gyro);
    rechts_drehen(gyro);
    geradeaus_fahren(1, gyro);
    return;
}

Gyro gyro;
auto gyroZ_Winkelgeschwindigkeit;
auto gierachse;

int main(){   
	signal(SIGINT, hdlr);
    wiringPiSetup();
    pinMode(GPIO_Trigger, OUTPUT);
    pinMode(GPIO_Echo, INPUT);
    pinMode(LED, OUTPUT);

    motor1->setSpeed(500);
    motor4->setSpeed(500);
    motor1->run(AdafruitDCMotor::kForward);
    motor4->run(AdafruitDCMotor::kForward);

	gyro.init();

        while (1){
            gyroZ_Winkelgeschwindigkeit = gyro.readGyroZ()/131.0;
            float zeitSekunden = 0.01; //austesten
            gierachse = gierachse + gyroZ_Winkelgeschwindigkeit*zeitSekunden;
            std::cout << "Gierachse: " << gierachse << std::endl;

            float distanz = entfernung(); // [cm]
            std::cout << "Distanz: " << distanz << std::endl;

            geradeaus_fahren(2,gyro);

            std::this_thread::sleep_for(std::chrono::seconds(0.1));
        }
    return 0;
}

