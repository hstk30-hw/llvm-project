! REQUIRES: openmp_runtime
! RUN: %not_todo_cmd bbc -emit-fir %openmp_flags -fopenmp -fopenmp-version=50 -o - %s 2>&1 | FileCheck %s
! RUN: %not_todo_cmd %flang_fc1 -emit-fir %openmp_flags -fopenmp -fopenmp-version=50 -o - %s 2>&1 | FileCheck %s

!===============================================================================
! `detach` clause
!===============================================================================

! CHECK: not yet implemented: DETACH clause is not implemented yet
subroutine omp_task_detach()
  use omp_lib
  integer (kind=omp_event_handle_kind) :: event
  !$omp task detach(event)
  call foo()
  !$omp end task
end subroutine omp_task_detach
