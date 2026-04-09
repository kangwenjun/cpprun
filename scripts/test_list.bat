
cd %~dp0
cd ..

python scripts\cpprun.py -s src\auto_reset_event.hpp -b build/auto_reset_event
python scripts\cpprun.py -s src\manual_reset_event.hpp -b build/manual_reset_event
python scripts\cpprun.py -s src\bits_to_hex.hpp -b build/bits_to_hex
python scripts\cpprun.py -s src\hex_to_bits.hpp -b build/hex_to_bits
python scripts\cpprun.py -s src\singleton.hpp -b build/singleton
python scripts\cpprun.py -s src\stopwatch.hpp -b build/stopwatch
python scripts\cpprun.py -s src\timestamp.hpp -b build/timestamp
python scripts\cpprun.py -s src\wait.hpp -b build/wait
python scripts\cpprun.py -s src\file.hpp -b build/file


python scripts\cpprun.py -s tests\bench_wait.cpp -b build/bench_wait