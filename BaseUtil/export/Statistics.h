#include <functional>
#include <atomic>

namespace BaseUtil {
    class Statistics {
    public:
        Statistics();
        ~Statistics();
        //ע��ص�����
        void RegisterWarningCB(const std::function<void()> &cb);
        void RegisterPeriodCB(const std::function<void(int last_counts, int curent_counts, int period)> &cb);

        //���ûص�����
        void SetPeriod(int usec);
        //��ʼ��ʱ
        void Start();
        //ֹͣ��ʱ��
        void Stop();

        //�������
        void Recount();
        //�ۼ�
        void Accumulate(int num = 1);

    private:
        const std::function<void()> &warning_cb;
        const std::function<void()> &period_cb;
        std::atomic_llong current_counts;
        std::atomic_llong last_counts;
    };
}