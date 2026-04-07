namespace cpprun
{

template <typename T>
class singleton
{
public:
    static T &instance()
    {
        static T s;
        return s;
    }
};

} // namespace cpprun

#ifdef __MAIN__
#include <iostream>


int main()
{
    using namespace cpprun;

    {
        auto &s1 = singleton<int>::instance();
        auto &s2 = singleton<int>::instance();
        if (&s1 != &s2) 
        {
            std::cerr << "Test failed for singleton<int>. Expected both instances to be the same." << std::endl;
            return __LINE__;
        }

        s1 = 42;
        if (s2 != 42) 
        {
            std::cerr << "Test failed for singleton<int>. Expected value to be 42, got " << s2 << "." << std::endl;
            return __LINE__;
        }
    }

    {
        class MyClass : public singleton<MyClass>
        {
        public:
            MyClass() : value(0) {}
            int value;
        };

        auto &s1 = MyClass::instance();
        auto &s2 = MyClass::instance();
        if (&s1 != &s2) 
        {
            std::cerr << "Test failed for singleton<MyClass>. Expected both instances to be the same." << std::endl;
            return __LINE__;
        }

        s1.value = 123;
        if (s2.value != 123) 
        {
            std::cerr << "Test failed for singleton<MyClass>. Expected value to be 123, got " << s2.value << "." << std::endl;
            return __LINE__;
        }
    }

    return 0;
}

#endif // __MAIN__