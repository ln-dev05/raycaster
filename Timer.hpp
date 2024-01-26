#if !defined(HPP_timer)
#define HPP_timer

struct Timer {
  double old_time, time, frame_time, frame_rate;

  Timer();
  void update(unsigned long tick);
};

#endif // HPP_timer
