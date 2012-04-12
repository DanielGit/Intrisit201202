REM StSvnRev http://192.168.1.116/svn/NOAHOS _noahos.bin 0x1ec
v2combine v2.bin 0x400 0x1f0 _noahos.bin logo.bin
call version 0xF710000A 0x1f8 v2.bin NOAH_APP_ND800
copy /b/y v2.bin ..\..\system.bin
copy /b/y v2.bin ..\..\ND800.bin
