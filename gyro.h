#define ONLY_GZ 0
#define DISABL_NON_Z_G 0b00011100
#define MPU6050 0x68
#define MPU6050_PWR_MGMT_1 0x6B
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define GYRO_SCALE 131.0
#define GYRO_CONFIG 0x1b
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <cstdint>

class Gyro {
	int fd;

    public:
        Gyro(){
            fd=wiringPiI2CSetup(MPU6050);
        }

        Gyro(int id){fd = wiringPiI2CSetup(id);}

        void init(){
            write(MPU6050,0); 
            write(MPU6050_PWR_MGMT_1,3);    //PLL with Z axis gyroscope reference
            write(GYRO_CONFIG,16);  //1000deg/s Full Scale Range 
        }
		
		void write(int reg,int v){
            wiringPiI2CWriteReg8(this->fd,reg, v);    
        }
		
		uint8_t read8bit(int reg){
            return wiringPiI2CReadReg8(this->fd,reg);
        }
		
		int16_t connectHigh_Low(uint8_t high,uint8_t low){
            return (high<<8) | low;
        }

		int16_t read16bit(int reg,int reg2){
            return connectHigh_Low(read8bit(reg),read8bit(reg2));
        }

        float readGyroZ() {
            return read16bit(GYRO_ZOUT_H, GYRO_ZOUT_L);
        }


};


