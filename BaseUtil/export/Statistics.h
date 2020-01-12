#include <functional>
#include <atomic>

namespace BaseUtil {
    class Statistics {
    public:
        Statistics();
        ~Statistics();
        //注册回调函数
        void RegisterWarningCB(const std::function<void()> &cb);
        void RegisterPeriodCB(const std::function<void(int last_counts, int curent_counts, int period)> &cb);

        //设置回调周期
        void SetPeriod(int usec);
        //开始计时
        void Start();
        //停止计时；
        void Stop();

        //清空重来
        void Recount();
        //累加
        void Accumulate(int num = 1);

    private:
        const std::function<void()> &warning_cb;
        const std::function<void()> &period_cb;
        std::atomic_llong current_counts;
        std::atomic_llong last_counts;
    };
}