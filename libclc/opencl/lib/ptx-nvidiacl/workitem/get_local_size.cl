//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <clc/opencl/workitem/get_local_size.h>

_CLC_DEF _CLC_OVERLOAD size_t get_local_size(uint dim) {
  switch (dim) {
  case 0:
    return __nvvm_read_ptx_sreg_ntid_x();
  case 1:
    return __nvvm_read_ptx_sreg_ntid_y();
  case 2:
    return __nvvm_read_ptx_sreg_ntid_z();
  default:
    return 0;
  }
}
