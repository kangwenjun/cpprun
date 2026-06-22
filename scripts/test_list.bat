
cd %~dp0
cd ..

python scripts/cpprun.py -s src/cJSON -b build/cJSON
python scripts/cpprun.py -s src/spdlog -b build/spdlog

:: cpprun::file
python scripts/cpprun.py -s src/file/file.hpp -b build/file
python scripts/cpprun.py -s src/file/file_name_cache.hpp -b build/file_name_cache

:: cpprun::dir
python scripts/cpprun.py -s src/dir/dir.hpp -b build/dir

:: wifi
python scripts/cpprun.py -s src/network/win_wifi_connect.cpp -b build/win_wifi_connect
python scripts/cpprun.py -s src/network/win_wifi_get_status.cpp -b build/win_wifi_get_status

python scripts/cpprun.py -s src/cv/auto_reset_event.hpp -b build/auto_reset_event
python scripts/cpprun.py -s src/cv/manual_reset_event.hpp -b build/manual_reset_event
python scripts/cpprun.py -s src/str/bits_to_hex.hpp -b build/bits_to_hex
python scripts/cpprun.py -s src/str/hex_to_bits.hpp -b build/hex_to_bits
python scripts/cpprun.py -s src/singleton.hpp -b build/singleton
python scripts/cpprun.py -s src/time/stopwatch.hpp -b build/stopwatch
python scripts/cpprun.py -s src/time/timestamp.hpp -b build/timestamp
python scripts/cpprun.py -s src/time/wait.hpp -b build/wait

python scripts/cpprun.py -s src/format/va_format.hpp -b build/format
python scripts/cpprun.py -s src/format/std_format.hpp -b build/std_format
python scripts/cpprun.py -s src/format/std_vformat.hpp -b build/std_vformat
python scripts/cpprun.py -s src/mutex/safe_data.hpp -b build/safe_data
python scripts/cpprun.py -s tests/bench_wait.cpp -b build/bench_wait

python scripts/cpprun.py -s src/charconv/from_chars_example.cpp -b build/charconv
python scripts/cpprun.py -s src/num/stoi.hpp -b build/stoi

:: boost.org
python scripts/cpprun.py -s src\boostorg\gpu.cpp -b build/boost_gpu

:: ub
python scripts/cpprun.py -s src\ub\too_few_parameters.cpp -b build/too_few_parameters
