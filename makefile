test:
	.\build\Debug\emulator.exe examples\test.hex

testL:
	.\build\Debug\emulator.exe examples\test.hex -L test_log.log

run:
	.\build\Release\emulator.exe examples\test.hex
runL:
	.\build\Release\emulator.exe examples\test.hex -L test_log.log