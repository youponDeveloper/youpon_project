copy %2\demo.bin %1\input.bin
copy %1\..\..\..\..\board\%3\kernel.bin %1\kernel.bin
cd /d %1 & add_crc.exe
del %1\input.bin
cd /d %1 & merge.exe
cd /d %1 & add_md5.exe
move %1\user_app.bin %2\
move %1\mico_ota.bin %2\
del %1\input.bin
del %1\kernel.bin
copy %1\..\..\..\..\platform\MCU\MW3xx\merge\* %2\
cd /d %2 & merge.exe
del %2\uart_wifi_bridge.bin
del %2\mw30x_uapsta_14.76.36.p84.bin
del %2\demo.bin
del %2\boot2.bin
del %2\merge.exe
del %2\config.ini