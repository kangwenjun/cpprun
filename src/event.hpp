#pragma once

#include <mutex>
#include <condition_variable>
#include <cstdint>

class MyEventEx
{
public:

    /*
        返回：
            true        等到事件
            false       没有等到事件，超时
    */
    template <class _Rep, class _Period>
    bool wait_for(const std::chrono::duration<_Rep, _Period> &duration)
    {
        std::unique_lock<std::mutex> lker(m_mtx);
        // 捕获当前 generation，只有当 generation 变化或有单次信号时才返回
        std::uint64_t gen = m_generation;
        bool ret = m_cv.wait_for(lker, duration, [this, gen] { return m_generation != gen || m_one_shots > 0; });
        if (!ret)
            return false;

        if (m_one_shots > 0)
        {
            --m_one_shots;
            return true;
        }

        // generation 已变化，表示 notify_all 在等待期间发生
        return true;
    }
       
    void notify_one()
    {
        {
            std::lock_guard<std::mutex> lk(m_mtx);
            ++m_one_shots;
        }
        m_cv.notify_one();
    }

    /* 事件设置 */
    void notify_all()
    {
        {
            std::lock_guard<std::mutex> lk(m_mtx);
            ++m_generation; // 增加 generation，使当前正在等待的线程通过
        }
        m_cv.notify_all();
    }

private:

    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::uint64_t m_generation{ 0 };
    std::size_t m_one_shots{ 0 };
};
