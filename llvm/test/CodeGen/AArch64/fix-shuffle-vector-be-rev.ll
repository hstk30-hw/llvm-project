; RUN: llc < %s -mtriple=aarch64_be | FileCheck %s
 
; one `rev` is from `bitcase`, another inserted for be
 
define <4 x i1> @insert_rev_for_reconstructshuffle(<16 x i8>* %A, <16 x i8>* %B) nounwind {
; CHECK-LABEL: insert_rev_for_reconstructshuffle:
; CHECK:    zip2 v[[V1:[0-9]+]].8b, v[[V2:[0-9]+]].8b, v[[V3:[0-9]+]].8b
; CHECK:    rev16 
; CHECK:    rev16 
  %tmp1 = load <16 x i8>, <16 x i8>* %A
  %tmp2 = load <16 x i8>, <16 x i8>* %B
  %tmp3 = shufflevector <16 x i8> %tmp1, <16 x i8> undef, <4 x i32> <i32 3, i32 2, i32 1, i32 0>
  %tmp4 = shufflevector <16 x i8> %tmp2, <16 x i8> undef, <4 x i32> <i32 12, i32 13, i32 14, i32 15>
  %tmp5 = icmp eq <4 x i8> %tmp3, %tmp4
  %tmp6 = freeze <4 x i1> %tmp5
  ret <4 x i1> %tmp6
}