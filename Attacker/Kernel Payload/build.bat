@echo off
cmake -Bbuild -GNinja -DCMAKE_MAKE_PROGRAM="%~dp0bin\ninja.exe"
cmake --build build --clean-first