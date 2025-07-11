! RUN: %python %S/test_folding.py %s %flang_fc1

! Test character intrinsic function folding

! ICHAR overflow checking is disabled on Windows due to a bogus MSVC
! build error.
! XFAIL: system-windows

module character_intrinsic_tests
  logical, parameter :: test_char1_ok1 = char(0_4, 1).EQ." "
  logical, parameter :: test_char1_ok2 = char(127_4, 1).EQ.""
  logical, parameter :: test_char1_ok3 = char(97_4, 1).EQ."a"
  logical, parameter :: test_char1_ok4 = .NOT.char(97_4, 1).EQ."b"
  !WARN: warning: CHAR(I=355) is out of range for CHARACTER(KIND=1) [-Wfolding-value-checks]
  logical, parameter :: test_char1_ok5 = char(355_4, 1).EQ."c"
  !WARN: warning: CHAR(I=-61) is out of range for CHARACTER(KIND=1) [-Wfolding-value-checks]
  logical, parameter :: test_char1_ok6 = char(-61_1, 1).EQ.char(195_4, 1)

  logical, parameter :: test_char2_ok1 = char(0_4, 2).EQ.2_" "
  logical, parameter :: test_char2_ok2 = char(127_4, 2).EQ.2_""
  logical, parameter :: test_char2_ok3 = char(122_4, 2).EQ.2_"z"

  logical, parameter :: test_char4_ok1 = char(0, 4).EQ.4_" "
  logical, parameter :: test_char4_ok2 = char(INT(12435, 4), 4).EQ.4_"ん"
  logical, parameter :: test_char4_ok3 = char(354_4, 4).EQ.4_"Ţ"
  !WARN: warning: CHAR(I=-1) is out of range for CHARACTER(KIND=4) [-Wfolding-value-checks]
  logical, parameter :: test_char4_ok4 = char(-1_1, 4).EQ.char(x'ffffffff', 4)

  character(kind=4, len=*), parameter :: c4aok(*) = char([97_4, 98_4, 99_4, 20320_4, 22909_4], 4)
  logical, parameter :: test_char4_array = (c4aok(1).EQ.4_"a").AND.(c4aok(2).EQ.4_"b") &
    .AND.(c4aok(3).EQ.4_"c").AND.(c4aok(4).EQ.4_"你").AND.(c4aok(5).EQ.4_"好")

  logical, parameter :: test_achar4_ok1 = achar(0_4, 4).EQ.4_" "
  logical, parameter :: test_achar4_ok2 = achar(127_4, 4).EQ.4_""

  character(kind=1, len=*), parameter :: c1aok(*) = achar([97_4, 0_4, 98_4], 1)
  logical, parameter :: test_char1_array = (c1aok(1).EQ.1_"a").AND.(c1aok(2).EQ.1_" ") &
    .AND.(c1aok(3).EQ.1_"b")


  logical, parameter :: test_ichar1 = char(ichar("a")).EQ."a"
  logical, parameter :: test_ichar2 = ichar(char(255)).EQ.255
  !WARN: warning: Result of intrinsic function 'ichar' (255) overflows its result type [-Wfolding-exception]
  !WARN: warning: CHAR(I=-1) is out of range for CHARACTER(KIND=1) [-Wfolding-value-checks]
  logical, parameter :: test_ichar3 = ichar(char(-1_1), 1).EQ.-1_1
  logical, parameter :: test_ichar4 = ichar(char(2147483647_4, 4), 4).EQ.2147483647_4
  logical, parameter :: test_ichar5 = ichar(char(4294967295_8, 4), 8).EQ.4294967295_8
  !WARN: warning: CHAR(I=4294967296) is out of range for CHARACTER(KIND=4) [-Wfolding-value-checks]
  logical, parameter :: test_ichar6 = ichar(char(4294967296_8, 4), 8).EQ.0_8
  logical, parameter :: test_iachar1 = achar(iachar("a")).EQ."a"
  logical, parameter :: test_iachar2 = iachar(achar(22)).EQ.22
  !WARN: warning: Result of intrinsic function 'ichar' (2147483649) overflows its result type [-Wfolding-exception]
  !WARN: warning: CHAR(I=-2147483647) is out of range for CHARACTER(KIND=4) [-Wfolding-value-checks]
  logical, parameter :: test_iachar3 = ichar(char(-2147483647_4, 4), 4).EQ.(-2147483647_4)
  logical, parameter :: test_iachar5 = ichar(char(65535_4, 2), 4).EQ.65535_4
  !WARN: warning: CHAR(I=65536) is out of range for CHARACTER(KIND=2) [-Wfolding-value-checks]
  logical, parameter :: test_iachar6 = ichar(char(65536_4, 2), 4).EQ.0_4

  ! Not yet recognized as intrinsic
  !character(kind=1, len=1), parameter :: test_c1_new_line = new_line("a")

  logical, parameter :: test_c1_adjustl1 = adjustl("  this is a test").EQ.("this is a test  ")
  logical, parameter :: test_c1_adjustl2 = .NOT."  this is a test".EQ.("this is a test  ")
  logical, parameter :: test_c1_adjustl3 = adjustl("").EQ.("")
  logical, parameter :: test_c1_adjustl4 = adjustl("that").EQ.("that")
  logical, parameter :: test_c1_adjustl5 = adjustl("that ").EQ.("that ")
  logical, parameter :: test_c1_adjustl6 = adjustl(" that ").EQ.("that  ")
  logical, parameter :: test_c1_adjustl7 = adjustl("    ").EQ.("    ")
  character(kind=1, len=*), parameter :: c1_adjustl8(*) = adjustl(["  this is a test", " that is a test "])
  logical, parameter :: test_c1_adjustl8 = (c1_adjustl8(1).EQ.1_"this is a test  ").AND.(c1_adjustl8(2).EQ.1_"that is a test  ")

  logical, parameter :: test_c4_adjustr1 = adjustr(4_"  你好吗 ? ").EQ.(4_"   你好吗 ?")
  logical, parameter :: test_c4_adjustr3 = adjustr(4_"").EQ.(4_"")
  logical, parameter :: test_c4_adjustr4 = adjustr(4_"   ").EQ.(4_"   ")
  logical, parameter :: test_c4_adjustr5 = adjustr(4_"你 好吗?").EQ.(4_"你 好吗?")
  logical, parameter :: test_c4_adjustr6 = adjustr(4_" 你好吗?").EQ.(4_" 你好吗?")
  logical, parameter :: test_c4_adjustr7 = adjustr(4_" 你好吗? ").EQ.(4_"  你好吗?")
  character(kind=4, len=*), parameter :: c4_adjustr8(*) = adjustr([4_"  你 好吗?%%% ", 4_"你a好b 吗c?   "])
  logical, parameter :: test_c4_adjustr8 = (c4_adjustr8(1).EQ.4_"   你 好吗?%%%").AND.(c4_adjustr8(2).EQ.4_"   你a好b 吗c?")

  integer, parameter :: ck = selected_char_kind('DEFAuLT  ')
  character(kind=ck, len=*), parameter :: cks = 'DEFAULT'
  character(len=1), parameter :: ds = ' '
  logical, parameter :: test_selected_char_kind = kind(ds).eq.kind(cks)
  logical, parameter :: test_c1_new_line1 = ichar(new_line(1_"a"), 4).EQ.10_4
  logical, parameter :: test_c2_new_line1 = ichar(new_line(2_"z"), 4).EQ.10_4
  logical, parameter :: test_c4_new_line1 = ichar(new_line(4_" 你好吗? "), 4).EQ.10_4
  character(kind=1, len=10) s1(10,10)
  character(kind=2, len=10) s2(10,10)
  character(kind=4, len=10) s4(10,10)
  logical, parameter :: test_c1_new_line2 = kind(new_line(s1)).EQ.1_4
  logical, parameter :: test_c2_new_line2 = kind(new_line(s2)).EQ.2_4
  logical, parameter :: test_c4_new_line2 = kind(new_line(s4)).EQ.4_4
  logical, parameter :: test_c1_new_line3 = len(new_line(s1)).EQ.1_4
  logical, parameter :: test_c2_new_line3 = len(new_line(s2)).EQ.1_4
  logical, parameter :: test_c4_new_line3 = len(new_line(s4)).EQ.1_4
  logical, parameter :: test_c1_new_line4 = rank(new_line(s1)).EQ.0_4
  logical, parameter :: test_c2_new_line4 = rank(new_line(s2)).EQ.0_4
  logical, parameter :: test_c4_new_line4 = rank(new_line(s4)).EQ.0_4

  logical, parameter :: test_c1_index1 = index("  this is a test", "this is a test") .EQ. 3
  logical, parameter :: test_c1_index2 = index("  this is a test", "this is not a test") .EQ. 0
  logical, parameter :: test_c1_index3 = index("ABBA", "A") .EQ. 1
  logical, parameter :: test_c1_index4 = index("ABBA", "A", back=.true.) .EQ. 4
  logical, parameter :: test_c1_index5 = index("", "") .EQ. 1 .AND. index("","",back=.TRUE.) .EQ. 1
  logical, parameter :: test_c1_index6 = index("", "a") .EQ. 0 .AND. index("","a",back=.TRUE.) .EQ. 0
  logical, parameter :: test_c1_index7 = index("a", "") .EQ. 1 .AND. index("a","",back=.TRUE.) .EQ. 2
  logical, parameter :: test_c4_index1 = index(4_"  this is a test", 4_"this is a test") .EQ. 3
  logical, parameter :: test_c4_index2 = index(4_"  this is a test", 4_"this is not a test") .EQ. 0
  logical, parameter :: test_c4_index3 = index(4_"ABBA", 4_"A") .EQ. 1
  logical, parameter :: test_c4_index4 = index(4_"ABBA", 4_"A", back=.true.) .EQ. 4
  logical, parameter :: test_c4_index5 = index(4_"", 4_"") .EQ. 1 .AND. index(4_"",4_"",back=.TRUE.) .EQ. 1
  logical, parameter :: test_c4_index6 = index(4_"", 4_"a") .EQ. 0 .AND. index(4_"",4_"a",back=.TRUE.) .EQ. 0
  logical, parameter :: test_c4_index7 = index(4_"a", 4_"") .EQ. 1 .AND. index(4_"a",4_"",back=.TRUE.) .EQ. 2
  logical, parameter :: test_c1_index8 = all(index("aaa", "a", [.false., .true.]) == [1, 3])
  logical, parameter :: test_c1_scan1 = scan("ABBA", "ACD") .EQ. 1
  logical, parameter :: test_c1_scan2 = scan("ABBA", "ACD", back=.true.) .EQ. 4
  logical, parameter :: test_c1_scan3 = scan("ABBA", "E") .EQ. 0
  logical, parameter :: test_c1_scan4 = scan("ABBA", "") .EQ. 0
  logical, parameter :: test_c1_scan5 = scan("", "FG") .EQ. 0
  logical, parameter :: test_c1_scan6 = scan("", "") .EQ. 0
  logical, parameter :: test_c4_scan1 = scan(4_"ABBA", 4_"ACD") .EQ. 1
  logical, parameter :: test_c4_scan2 = scan(4_"ABBA", 4_"ACD", back=.true.) .EQ. 4
  logical, parameter :: test_c4_scan3 = scan(4_"ABBA", 4_"E") .EQ. 0
  logical, parameter :: test_c4_scan4 = scan(4_"ABBA", 4_"") .EQ. 0
  logical, parameter :: test_c4_scan5 = scan(4_"", 4_"FG") .EQ. 0
  logical, parameter :: test_c4_scan6 = scan(4_"", 4_"") .EQ. 0
  logical, parameter :: test_c1_verify1 = verify("ABBA", "ACD") .EQ. 2
  logical, parameter :: test_c1_verify2 = verify("ABBA", "ACD", back=.true.) .EQ. 3
  logical, parameter :: test_c1_verify3 = verify("ABBA", "E") .EQ. 1
  logical, parameter :: test_c1_verify4 = verify("ABBA", "") .EQ. 1
  logical, parameter :: test_c1_verify4a = verify("ABBA", "", back=.true.) .EQ. 4
  logical, parameter :: test_c1_verify5 = verify("", "FG") .EQ. 0
  logical, parameter :: test_c1_verify6 = verify("", "") .EQ. 0
  logical, parameter :: test_c4_verify1 = verify(4_"ABBA", 4_"ACD") .EQ. 2
  logical, parameter :: test_c4_verify2 = verify(4_"ABBA", 4_"ACD", back=.true.) .EQ. 3
  logical, parameter :: test_c4_verify3 = verify(4_"ABBA", 4_"E") .EQ. 1
  logical, parameter :: test_c4_verify4 = verify(4_"ABBA", 4_"") .EQ. 1
  logical, parameter :: test_c4_verify4a = verify(4_"ABBA", 4_"", back=.true.) .EQ. 4
  logical, parameter :: test_c4_verify5 = verify(4_"", 4_"FG") .EQ. 0
  logical, parameter :: test_c4_verify6 = verify(4_"", 4_"") .EQ. 0

  logical, parameter :: test_c1_len_trim1 = len_trim("this is a test  ") .EQ. 14
  logical, parameter :: test_c1_len_trim2 = len_trim("this is a test") .EQ. 14
  logical, parameter :: test_c1_len_trim3 = len_trim("  this is a test") .EQ. 16
  logical, parameter :: test_c1_len_trim4 = len_trim("") .EQ. 0
  logical, parameter :: test_c4_len_trim1 = len_trim(4_"this is a test  ") .EQ. 14
  logical, parameter :: test_c4_len_trim2 = len_trim(4_"this is a test") .EQ. 14
  logical, parameter :: test_c4_len_trim3 = len_trim(4_"  this is a test") .EQ. 16
  logical, parameter :: test_c4_len_trim4 = len_trim(4_"") .EQ. 0

  logical, parameter :: test_c1_repeat1 = repeat('ab', 2) .eq. 'abab'
  logical, parameter :: test_c1_repeat2 = repeat('ab', 0) .eq. ''
  logical, parameter :: test_c1_repeat3 = repeat('', 99) .eq. ''
  logical, parameter :: test_c4_repeat1 = repeat(4_'ab', 2) .eq. 4_'abab'
  logical, parameter :: test_c4_repeat2 = repeat(4_'ab', 0) .eq. 4_''
  logical, parameter :: test_c4_repeat3 = repeat(4_'', 99) .eq. 4_''

  logical, parameter :: test_c1_trim1 = trim('ab') .eq. 'ab'
  logical, parameter :: test_c1_trim2 = trim('ab  ') .eq. 'ab'
  logical, parameter :: test_c1_trim3 = trim('  ab  ') .eq. '  ab'
  logical, parameter :: test_c1_trim4 = trim('  ') .eq. ''
  logical, parameter :: test_c1_trim5 = trim('') .eq. ''
  logical, parameter :: test_c4_trim1 = trim(4_'ab') .eq. 4_'ab'
  logical, parameter :: test_c4_trim2 = trim(4_'ab  ') .eq. 4_'ab'
  logical, parameter :: test_c4_trim3 = trim(4_'  ab  ') .eq. 4_'  ab'
  logical, parameter :: test_c4_trim4 = trim(4_'  ') .eq. 4_''
  logical, parameter :: test_c4_trim5 = trim(4_'') .eq. 4_''
end module
