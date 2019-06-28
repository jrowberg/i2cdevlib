I2CDev library rewritted to match esp-idf. DMP should work, only need is to setup 
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788);
and eventually change last value in components/MPU6050/MPU6050_6Axis_MotionApps20.h.
