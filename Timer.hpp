#if !defined(HPP_timer)
#define HPP_timer

struct Timer {
  double old_time, time, frame_time;

  Timer();
  void update();
};

#endif // HPP_timer
