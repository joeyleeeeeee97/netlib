#include "Timer.h"

using namespace netlib;

void Timer::restart(Timestamp now)
{
  if (repeat_)
  {
    expiration_ = addTime(now, interval_);
  }
  else
  {
    expiration_ = Timestamp::invalid();
  }
}


AtomicInt64 Timer::s_numCreated_;




                                                                                  
