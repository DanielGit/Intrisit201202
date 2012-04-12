combin BootLoader.bin 0x4000 Boot.bin Loader.bin
combin _v2Bios.bin 0x80000 BootLoader.bin _noahos.bin
v2combine V2Bios.bin 0x1000 0x401f0 _v2Bios.bin BiosApp.bin
Del /Q _v2Bios.bin
Del /Q BootLoader.bin
call BiosVer 0xF710000A 0x401f8 V2Bios.bin NOAH_ND800
copy /b/y V2Bios.bin ..\..\bios.bin
copy /b/y V2Bios.bin ..\..\ND800bios.bin
