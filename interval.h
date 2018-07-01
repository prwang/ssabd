//
// Created by prwang on 7/1/2018.
//

#ifndef SSABD_INTERVAL_H
#define SSABD_INTERVAL_H
#include <cassert>
enum INTV_FLAGS {
  I_NORM = 0,
  I_NINFT = 1,
  I_PINFT = 2,
  I_NPINFT = 3,
  I_NUL = 4,
};
const int BADNUM = 2147483647; //also float NAN
struct interval {
  bool is_float;
  int flags;
  union {
    struct { float fL, fR; };
    struct { int iL, iR; };
  };

  // [-∞,+∞]
  interval(bool _is_float) { flags = I_NPINFT; is_float = _is_float; iL = iR = BADNUM; }

  // [_, +∞]
  interval(bool, float r) { is_float = true; iL = BADNUM; fR = r; flags = I_NINFT; }
  interval(bool, int r) { is_float = false; iL = BADNUM; iR = r; flags = I_NINFT; }

  // [-∞,_]
  interval(float l, bool) { is_float = true; iR = BADNUM; fL = l; flags = I_PINFT; }
  interval(int l, bool) { is_float = false; iR = BADNUM; iL = l; flags = I_PINFT; }

  interval(float l, float r) {
    assert(l <= r); is_float = true;  flags = I_NORM, fL = l, fR = r; }
  interval(int l, int r) {
    assert(l <= r); is_float = false;  flags = I_NORM, iL = l, iR = r; }

};
#endif //SSABD_INTERVAL_H
