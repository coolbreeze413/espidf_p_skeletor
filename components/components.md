- arduino core -> components/arduino-esp32
- basic interactive VT100 shell (teraterm/putty) -> components/ntshell [arduino based]
- ssd1306 oled display support -> components/ssd1306 [arduino based]
- mqtt support -> components/async-mqtt-client [arduino based]
  this depends on AsyncTCP -> components/AsyncTCP
 
further additions needed:

- ota support
- coap support
- webserver using spiffs
- ftp server using spiffs
- light sleep/deep sleep and wakeup options
- date and time (sync via internet/NTP, maintain time using RTC)
- ble additions (central, peripheral)
- bt additions (spp, a2dp, avrcp, hfp)
- gsm/gprs support (AT Command based, Adafruit FONA lib) - call, sms, cellular data
- gps support
- lora support
- i2s mic/speaker support
- speech recognition using cmu sphinx ?
- bno055 support (imu)
- mpu 6050/9250 support (imu)
- rotary encoder support


[we need pure esp idf support as well, so that we can eliminate the arduino core if so required]
[shell, ssd1306, mqtt can definitely be esp idf components as of this point]




