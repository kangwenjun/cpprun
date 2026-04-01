/*
1. 101 -> 5
2. 1101 -> D
3. 110101101011 -> D6B
*/

constexpr const char _hex_digits_upper[] = "0123456789ABCDEF";
constexpr const char _hex_digits_lower[] = "0123456789abcdef";

inline void bits_to_hex(const char *src, int len, char *output, bool uppercase = false)
{
    if (src == nullptr || output == nullptr || len <= 0)
    {
        if (output != nullptr)
            output[0] = '\0';
        return;
    }

    int out = 0;
    const char *p = src;
    const int pad = (4 - (len % 4)) % 4;
    const char *hex_digits = uppercase ? _hex_digits_upper : _hex_digits_lower;
    if (pad) 
    {
        int x = 0;
        for (int i = 0; i < 4 - pad && len > 0; ++i) 
        {
            x = (x << 1) | (*p++ == '1');
            --len;
        }

        output[out++] = hex_digits[x];
    }

    while (len >= 4) 
    {
        len -= 4;
        int v = ((*p++ == '1') << 3) | ((*p++ == '1') << 2) | ((*p++ == '1') << 1) | ((*p++ == '1'));
        output[out++] = hex_digits[v];
    }

    output[out] = '\0';
}

#ifdef __MAIN__
#include <iostream>


int main()
{
    char hex_output[16] = {0};

    bits_to_hex("101", 3, hex_output, true); // 5
    std::cout << "101" << " -> " << hex_output << std::endl;

    bits_to_hex("1101", 4, hex_output, true); // D
    std::cout << "1101" << " -> " << hex_output << std::endl;

    bits_to_hex("110101101011", 12, hex_output, true); // D6B
    std::cout << "110101101011" << " -> " << hex_output << std::endl;

    return 0;
}

#endif // __MAIN__
