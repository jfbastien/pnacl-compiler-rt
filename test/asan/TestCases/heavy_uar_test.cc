// RUN: export ASAN_OPTIONS=detect_stack_use_after_return=1
// RUN: %clangxx_asan -O0 %s -o %t && not %run %t 2>&1 | FileCheck %s
// RUN: %clangxx_asan -O2 %s -o %t && not %run %t 2>&1 | FileCheck %s
// XFAIL: arm-linux-gnueabi

// FIXME: Fix this test under GCC.
// REQUIRES: Clang

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

__attribute__((noinline))
char *pretend_to_do_something(char *x) {
  __asm__ __volatile__("" : : "r" (x) : "memory");
  return x;
}

__attribute__((noinline))
char *LeakStack() {
  char x[1024];
  memset(x, 0, sizeof(x));
  return pretend_to_do_something(x);
}

template<size_t kFrameSize>
__attribute__((noinline))
void RecursiveFunctionWithStackFrame(int depth) {
  if (depth <= 0) return;
  char x[kFrameSize];
  x[0] = depth;
  pretend_to_do_something(x);
  RecursiveFunctionWithStackFrame<kFrameSize>(depth - 1);
}

int main(int argc, char **argv) {
  int n_iter = argc >= 2 ? atoi(argv[1]) : 1000;
  int depth  = argc >= 3 ? atoi(argv[2]) : 500;
  for (int i = 0; i < n_iter; i++) {
    RecursiveFunctionWithStackFrame<10>(depth);
    RecursiveFunctionWithStackFrame<100>(depth);
    RecursiveFunctionWithStackFrame<500>(depth);
    RecursiveFunctionWithStackFrame<1024>(depth);
    RecursiveFunctionWithStackFrame<2000>(depth);
    RecursiveFunctionWithStackFrame<5000>(depth);
    RecursiveFunctionWithStackFrame<10000>(depth);
  }
  char *stale_stack = LeakStack();
  RecursiveFunctionWithStackFrame<1024>(10);
  stale_stack[100]++;
  // CHECK: ERROR: AddressSanitizer: stack-use-after-return on address
  // CHECK: is located in stack of thread T0 at offset {{116|132}} in frame
  // CHECK:  in LeakStack(){{.*}}heavy_uar_test.cc:
  // CHECK: [{{16|32}}, {{1040|1056}}) 'x'
  return 0;
}