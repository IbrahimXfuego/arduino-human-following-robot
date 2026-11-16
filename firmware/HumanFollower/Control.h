#pragma once
#include <stdint.h>
namespace follower {
struct Settings {
  int gap, deadband, maximumRange, minimumPwm, maximumPwm, gain;
};
struct Drive {
  int left, right;
};
// No reverse in following mode. Invalid/far readings stop even if an IR sees
// something.
inline Drive decide(int cm, bool leftSeen, bool rightSeen, const Settings &s) {
  if (cm <= 0 || cm > s.maximumRange || cm <= s.gap + s.deadband)
    return {0, 0};
  int pwm = s.minimumPwm + (cm - s.gap - s.deadband) * s.gain;
  if (pwm > s.maximumPwm)
    pwm = s.maximumPwm;
  if (leftSeen && !rightSeen)
    return {pwm / 3, pwm};
  if (rightSeen && !leftSeen)
    return {pwm, pwm / 3};
  return {pwm, pwm};
}
} // namespace follower
