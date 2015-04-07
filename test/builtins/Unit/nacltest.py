#!/usr/bin/env python
# Copyright (c) 2015 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This script runs the tests for compiler_rt. It is meant to run on a
# fully-constructed toolchain (as opposed to being run during the compiler-rt
# build process) because the tests need libc/libnacl to link.

import os
import shutil
import subprocess
import sys
import tempfile

def find_run_py():
  """ Find run.py somewhere in one of the parents of the current directory."""
  dir = os.path.abspath(os.path.dirname(__file__))
  while os.path.dirname(dir) != dir:
    run_py = os.path.join(dir, 'run.py')
    if os.path.isfile(run_py):
      return run_py
    dir = os.path.dirname(dir)
  print 'Could not find run.py'
  sys.exit(1)

def parse_args(argv):
  cc = argv[1]
  tests = argv[2:]
  return cc, tests

def get_tests(functions, test_dir):
  """ Find the test files for the given functions.

      Given a list of functions to test, find each corresponding compiler-rt
      unit test, if it exists. The functions may be specified as bare function
      names (with no underscores), or source files of the same name.
  """
  tests = []
  for function in functions:
    # If the argument is specified as a source file, strip the suffix.
    if function.endswith('.c'):
      function = function[:-2]
    test_src = os.path.join(test_dir, '%s_test.c' % function)
    if os.path.isfile(test_src):
      tests.append(test_src)
    else:
      print 'no test for', function
  return tests

def main(argv):
  cc, functions = parse_args(argv)
  cc = os.path.abspath(cc)
  src_root = os.getcwd()
  test_src_dir = os.path.join(src_root, 'test', 'builtins', 'Unit')
  inc_dir = os.path.join(src_root, 'lib', 'builtins')
  tests = get_tests(functions, test_src_dir)
  run_py = find_run_py()
  failures = 0
  workdir = tempfile.mkdtemp()

  for test in tests:
    flags = ['-lm']
    exe_suffix = '.pexe' if 'pnacl-clang' in cc else '.nexe'
    nexe = os.path.join(workdir, os.path.basename(test + exe_suffix))
    build_cmd = [cc, '-I' + inc_dir, '-o', nexe, test] + flags
    run_cmd = [run_py, nexe]
    try:
      print ' '.join(build_cmd)
      subprocess.check_call(build_cmd)
      print ' '.join(run_cmd)
      subprocess.check_call(run_cmd)
    except subprocess.CalledProcessError, e:
      print '[  FAILED  ]:', test
      print e
      failures += 1

  shutil.rmtree(workdir)
  return failures

if __name__ == '__main__':
  sys.exit(main(sys.argv))
