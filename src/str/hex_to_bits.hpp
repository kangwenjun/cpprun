// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

/*
 * Convert hex string to bit string.
 *
 * Examples:
 *  - "5"   -> "101" (trim leading zeros)
 *  - "0F"  -> "00001111" (full 4*n bits)
 *  - "D6B" -> "110101101011"
 */
#include <cstring>

namespace cpprun::str
{

constexpr const char* _lookup[] = 
{
    "0000","0001","0010","0011",
    "0100","0101","0110","0111",
    "1000","1001","1010","1011",
    "1100","1101","1110","1111"
};

constexpr unsigned char _hexmap[] = 
{
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    0,1,2,3,4,5,6,7,8,9,255,255,255,255,255,255,
    255,10,11,12,13,14,15,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,10,11,12,13,14,15,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

inline bool hex_to_bits(const char *src, int len, char *output)
{
	if (src == nullptr || output == nullptr || len <= 0)
	{
		if (output != nullptr)
			output[0] = '\0';
		return false;
	}

	int out = 0;
	for (int i = 0; i < len; ++i)
	{
		unsigned char uc = static_cast<unsigned char>(src[i]);
		unsigned char v = _hexmap[uc];
		if (v == 255)
        {
            output[0] = '\0';
            return false;
        }

		memcpy(output + out, _lookup[v], 4);
		out += 4;
	}

	output[out] = '\0';
    return true;
}

} // namespace cpprun

#ifdef __MAIN__
#include <iostream>

int main()
{
    using namespace cpprun::str;
    
	char bits[256] = {0};

	hex_to_bits("5", 1, bits); // expect "0101"
    if (memcmp(bits, "0101", 4) != 0)
    {
        std::cerr << "Test failed for input '5'. Expected '0101', got '" << bits << "'." << std::endl;
        return __LINE__;
    }
	std::cout << "5 -> " << bits << std::endl;

	hex_to_bits("0F", 2, bits); // expect "00001111"
    if (memcmp(bits, "00001111", 8) != 0)
    {
        std::cerr << "Test failed for input '0F'. Expected '00001111', got '" << bits << "'." << std::endl;
        return __LINE__;
    }
	std::cout << "0F -> " << bits << std::endl;

	hex_to_bits("D6B", 3, bits); // expect "110101101011"
    if (memcmp(bits, "110101101011", 12) != 0)
    {
        std::cerr << "Test failed for input 'D6B'. Expected '110101101011', got '" << bits << "'." << std::endl;
        return __LINE__;
    }
	std::cout << "D6B -> " << bits << std::endl;

    // static unsigned char hexmap[256];
    // static bool hexmap_inited = false;
    // if (!hexmap_inited)
    // {
    //     for (int j = 0; j < 256; ++j) hexmap[j] = 0xFF;
    //     for (unsigned char ch = '0'; ch <= '9'; ++ch) hexmap[ch] = ch - '0';
    //     for (unsigned char ch = 'a'; ch <= 'f'; ++ch) hexmap[ch] = 10 + (ch - 'a');
    //     for (unsigned char ch = 'A'; ch <= 'F'; ++ch) hexmap[ch] = 10 + (ch - 'A');
    //     hexmap_inited = true;
    // }

    // for (int i = 0; i < 256; ++i)
    // {        
    //     std::cout << static_cast<int>(hexmap[i]) << ",";
    //     if ((i + 1) % 16 == 0)
    //         std::cout << std::endl;
    // }

	return 0;
}

#endif // __MAIN__

