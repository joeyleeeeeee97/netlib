#include "nocopyable.h"
#include <functional>
#include "Timestamp.h"
#include <memory>

#include "Atomic.h"
namespace netlib
{

typedef std::function<void()> TimerCallback;

class Timer : nocopyable{
 public:
  Timer(const TimerCallback& cb, Timestamp when, double interval)
    : callback_(cb),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0),
      sequence_(s_numCreated_.incrementAndGet())
      { }

  void run() const
  {
    callback_();
  }

  Timestamp expiration() const  { return expiration_; }
  bool repeat() const { return repeat_; }

  void restart(Timestamp now);
  int64_t sequence() const {
	return sequence_;
  }

 private:
  const TimerCallback callback_;
  Timestamp expiration_;
  const double interval_;
  const bool repeat_;
  const int64_t sequence_;
  static AtomicInt64 s_numCreated_;
};

}
