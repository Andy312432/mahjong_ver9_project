#include <atomic>
typedef class Spin_Lock {
private:
    int number;
    std::atomic_bool latch;
public:
    void add() {
        lock();
        number++;
        unlock();
    }
    void lock() {
        bool unlatched = false;
        while (!latch.compare_exchange_weak(unlatched, true, std::memory_order_acquire)) {
            unlatched = false;
        }
    }
    void unlock() {
        latch.store(false, std::memory_order_release);
    }
}SpinLock;

static inline SpinLock tableIDLock;