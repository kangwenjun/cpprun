#pragma once

#include <format>
#include <string>
#include <string_view>
#include <cstring>
#include <tuple>
#include <utility>
#include <type_traits>
#include <iostream>




#if defined(__MAIN__)
#include <iostream>
#include <vector>
#include <string>

int main()
{
	try {
		std::string aw = "world";
		int bw = 123;
		std::string fmt = "Hello {}, number={}";
		auto fa2 = std::make_format_args(aw, bw);
		std::string s2 = std::vformat(fmt, fa2);
		std::cout << "vformat with make_format_args direct: " << s2 << std::endl;
	}
	catch (const std::format_error &e) {
		std::cerr << "vformat with make_format_args direct error: " << e.what() << std::endl;
	}

	return 0;
}
#endif

