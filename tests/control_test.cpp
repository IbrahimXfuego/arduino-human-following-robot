#include "../firmware/HumanFollower/Control.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
using namespace follower;
int main() {
  Settings s{25, 5, 100, 105, 165, 3};
  for (int bad : {0, -1, 101, 200}) {
    auto d = decide(bad, true, false, s);
    assert(d.left == 0 && d.right == 0);
  }
  for (int cm = 1; cm <= 30; cm++) {
    auto d = decide(cm, false, false, s);
    assert(d.left == 0 && d.right == 0);
  }
  auto near = decide(31, false, false, s), far = decide(90, false, false, s);
  assert(near.left == near.right && near.left > 0);
  assert(far.left >= near.left && far.left <= 165);
  auto left = decide(40, true, false, s), right = decide(40, false, true, s);
  assert(left.left < left.right && right.left > right.right);
  auto both = decide(40, true, true, s);
  assert(both.left == both.right);
  for (int cm = 0; cm < 200; cm++) {
    auto d = decide(cm, true, false, s);
    assert(d.left >= 0 && d.right >= 0 && d.left <= 165 && d.right <= 165);
  }
  puts("Control checks passed: invalid/far stops, too-close behaviour, "
       "deadband, speed limits and steering.");
}
