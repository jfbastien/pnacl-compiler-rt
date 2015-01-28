/*===----- atomic64.c - Support functions for 64-bit atomic operations.-----===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 *===-----------------------------------------------------------------------===
 *
 *  atomic64.c defines a set of functions for performing atomic accesses on
 *  64-bit memory locations. It also implements spinlock synchronization
 *  operations.
 *
 *===-----------------------------------------------------------------------===
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * PNaCl only needs these on MIPS, since it lacks hardware 64-bit atomics,
 * unlike x86 and ARM.
 */
#if defined(PNACL_mips32)

#pragma redefine_extname __nacl_sync_fetch_and_add_8 __sync_fetch_and_add_8
#pragma redefine_extname __nacl_sync_fetch_and_sub_8 __sync_fetch_and_sub_8
#pragma redefine_extname __nacl_sync_fetch_and_and_8 __sync_fetch_and_and_8
#pragma redefine_extname __nacl_sync_fetch_and_or_8  __sync_fetch_and_or_8
#pragma redefine_extname __nacl_sync_fetch_and_xor_8 __sync_fetch_and_xor_8

#pragma redefine_extname __nacl_sync_add_and_fetch_8 __sync_add_and_fetch_8
#pragma redefine_extname __nacl_sync_sub_and_fetch_8 __sync_sub_and_fetch_8
#pragma redefine_extname __nacl_sync_and_and_fetch_8 __sync_and_and_fetch_8
#pragma redefine_extname __nacl_sync_or_and_fetch_8  __sync_or_and_fetch_8
#pragma redefine_extname __nacl_sync_xor_and_fetch_8 __sync_xor_and_fetch_8

#pragma redefine_extname __nacl_sync_bool_compare_and_swap_8 \
                         __sync_bool_compare_and_swap_8
#pragma redefine_extname __nacl_sync_val_compare_and_swap_8 \
                         __sync_val_compare_and_swap_8
#pragma redefine_extname __nacl_sync_lock_test_and_set_8 \
                         __sync_lock_test_and_set_8
#pragma redefine_extname __nacl_sync_lock_release_8 __sync_lock_release_8

static void __spin_lock(volatile int *lock) {
  while (__sync_lock_test_and_set(lock, 1))
    while (*lock) {}
}

static void __spin_unlock(volatile int *lock) {
  __sync_lock_release(lock);
}

/*
 * Make sure the lock is on its own cache line to prevent false sharing.
 * Put it inside a struct that is aligned and padded to the typical MIPS
 * cacheline which is 32 bytes.
 */
static struct {
  int lock;
  char pad[32 - sizeof(int)];
} __attribute__((aligned (32))) lock = { 0 };


uint64_t __nacl_sync_fetch_and_add_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  *ptr = ret + val;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_fetch_and_sub_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  *ptr = ret - val;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_fetch_and_and_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  *ptr = ret & val;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_fetch_and_or_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  *ptr = ret | val;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_fetch_and_xor_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  *ptr = ret ^ val;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_add_and_fetch_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr + val;
  *ptr = ret;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_sub_and_fetch_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr - val;
  *ptr = ret;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_and_and_fetch_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr & val;
  *ptr = ret;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_or_and_fetch_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr | val;
  *ptr = ret;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_xor_and_fetch_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr ^ val;
  *ptr = ret;

  __spin_unlock(&lock.lock);

  return ret;
}

bool __nacl_sync_bool_compare_and_swap_8(volatile uint64_t *ptr,
                                         uint64_t oldval, uint64_t newval) {
  bool ret = false;

  __spin_lock(&lock.lock);

  if (*ptr == oldval) {
    *ptr = newval;
    ret = true;
  }

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_val_compare_and_swap_8(volatile uint64_t *ptr,
                                            uint64_t oldval, uint64_t newval) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  if (ret == oldval)
    *ptr = newval;

  __spin_unlock(&lock.lock);

  return ret;
}

uint64_t __nacl_sync_lock_test_and_set_8(volatile uint64_t *ptr, uint64_t val) {
  uint64_t ret;

  __spin_lock(&lock.lock);

  ret = *ptr;
  *ptr = val;

  __spin_unlock(&lock.lock);

  return ret;
}

void __nacl_sync_lock_release_8(volatile uint64_t *ptr) {
  __spin_lock(&lock.lock);

  *ptr = 0;

  __spin_unlock(&lock.lock);
}

#endif
