

#if defined(__MAIN__)
#include <format>
#include <iostream>

// {:[填充][对齐][符号][#][0][宽度][.精度][L][类型]}
int main()
{
    std::cout << std::format("{}{}{}", "^", "_", "^") << std::endl;
    std::cout << std::format("{0}{1}{0}", "^", "_") << std::endl;

    // `<         `
    std::cout << std::format("`{:<10}`", "<") << std::endl;

    // `         >`
    std::cout << std::format("`{:>10}`", ">") << std::endl;

    // `    ^     `
    std::cout << std::format("`{:^10}`", "^") << std::endl;

    // `<*********`
    std::cout << std::format("`{:*<10}`", "<") << std::endl;

    // `*********>`
    std::cout << std::format("`{:*>10}`", ">") << std::endl;

    // `****^*****`
    std::cout << std::format("`{:*^10}`", "^") << std::endl;

    // `+42/-42`
    std::cout << std::format("{:+d}/{:+d}", 42, -42) << std::endl;

    // `42/-42`
    std::cout << std::format("{:-d}/{:-d}", 42, -42) << std::endl;

    // ` 42/-42`
    std::cout << std::format("{: d}/{: d}", 42, -42) << std::endl;

    // `101010/0b101010`
    std::cout << std::format("{:b}/{:#b}", 42, 42) << std::endl;

    // `52/052`
    std::cout << std::format("{:o}/{:#o}", 42, 42) << std::endl;

    // `42/42`
    std::cout << std::format("{:d}/{:#d}", 42, 42) << std::endl;

    // `2a/0x2a`
    std::cout << std::format("{:x}/{:#x}", 42, 42) << std::endl;

    // `2A/0X2A`
    std::cout << std::format("{:X}/{:#X}", 42, 42) << std::endl;

    // `         42`
    std::cout << std::format("{:10}", 42) << std::endl;
    std::cout << std::format("{:{}}", 42, 10) << std::endl;

    // `3.14`
    std::cout << std::format("{:.2f}", 3.1415926) << std::endl;
    std::cout << std::format("{:.{}f}", 3.1415926, 2) << std::endl;

    // `3.14e+00/3.14E+00`
    std::cout << std::format("{:.2e}/{:.2E}", 3.1415926, 3.1415926) << std::endl;

    // `3.14159/3.14159`
    std::cout << std::format("{:g}/{:G}", 3.1415926, 3.1415926) << std::endl;

    // `1.921fb4d12d84ap+1/1.921FB4D12D84AP+1`
    std::cout << std::format("{:a}/{:A}", 3.1415926, 3.1415926) << std::endl;

    // eg: `0x7ff74c5ddc40`
    std::cout << std::format("{:p}", static_cast<const void *>(&main)) << std::endl;

    // `{}`
    std::cout << std::format("{{}}") << std::endl;

    // `{42}`
    std::cout << std::format("{{{}}}", 42) << std::endl;

    // `00042`
    std::cout << std::format("{:05d}", 42) << std::endl; 

    // `example`
    std::cout << std::format("{:s}", "example") << std::endl;

    // 255/255/11111111/377/ff
    std::cout << std::format("{}/{:d}/{:b}/{:o}/{:x}", 255, 255, 255, 255, 255) << std::endl;

    // 1/true
    std::cout << std::format("{:d}/{}", true, true) << std::endl;

    // 0/false
    std::cout << std::format("{:d}/{}", false, false) << std::endl;

    // `     right`
    int w = 10;
    std::cout << std::format("{:>{}}", "right", w) << std::endl; 


    return 0;
}

#endif // __MAIN__