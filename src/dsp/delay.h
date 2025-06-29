#pragma once

namespace dsp
{

template<typename T>
class delay
{
public:

    delay() = delete;
    delay(const T initRet = 0) : z(initRet) {}

    T ins(const T sample)
    {
        auto ret = z;
        z = sample;
        return ret;
    }

    T get() { return z; }

private:
    T z;
};

}
