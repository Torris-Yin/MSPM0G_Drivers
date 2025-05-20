# Hints by DCZYewen

- AD9954(Software SPI) 基于康威科技模块厂家例程移植到MSPM0平台。
- M0HELPER 一个工具简化了一些操作，做了一些封装

Ti is providing too old version of CMSIS-DSP, incase of you will be using the new features, I ported the CMSIS-DSP-1.10 to MSPM0 platform.
Using it you should add the library to linker path and add the Include folder to include path, then you are ready to go.

AD9954 Driver implemented software SPI tuning, but not all of the AD9954 features. Use the library as simple as just define 11 pins on sysconfig.

Helper classes is implemented under C++, it provided OO model of ADC12 provided MCS51 like IO operations and some other stuffs.

