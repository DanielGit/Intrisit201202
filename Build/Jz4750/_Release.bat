REM StSvnRev http://192.168.1.116/svn/NOAHOS _noahos.bin 0x1ec
v2combine v2.bin 0x400 0x1f0 _noahos.bin logo.bin
call version 0xC5000000 0x1f8 v2.bin NOAH_APP_NC500
copy /b/y v2.bin ..\..\system.bin
copy /b/y v2.bin ..\..\nc500.bin
