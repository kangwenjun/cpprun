#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>


namespace cpprun::format {

inline size_t va_format_size(const char *format, ...)
{
    if (!format) return 0;

    va_list ap;
    va_start(ap, format);

#if defined(_MSC_VER)
    int required = _vscprintf(format, ap);
#else
    int required = vsnprintf(NULL, 0, format, ap);
#endif

    va_end(ap);
    return required >= 0 ? (size_t)required : 0;
}

inline size_t va_format_to(char *buffer, size_t size, const char *format, ...)
{
    if (!buffer || !format || !size) 
    {
        if (buffer && size) buffer[0] = '\0';
        return 0;
    }

    va_list ap;
    va_start(ap, format);
    int required = vsnprintf(buffer, size, format, ap);
    va_end(ap);

    if (required < 0) 
    {
        buffer[0] = '\0';
        return 0;
    }
    
    if ((size_t)required < size) 
    {
        buffer[required] = '\0';
        return (size_t)required;
    }
    
    size_t length = size - 1;  
    buffer[length] = '\0';
    return length;
}

} // namespace cpprun::format

#if defined(__MAIN__)
#include <iostream>
#include <vector>

int main()
{
    using namespace cpprun::format;

    const char *fmt = "Hello %s, number=%d";
    size_t needed = va_format_size(fmt, "world", 123);
    std::vector<char> buf(needed + 1);
    size_t written = va_format_to(buf.data(), buf.size(), fmt, "world", 123);

    std::cout << "formatted size=" << needed << ", written=" << written << '\n';
    std::cout << buf.data() << std::endl;
    return 0;
}
#endif