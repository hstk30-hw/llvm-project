// RUN: %clang -I/null/hstk -Wmissing-include-dirs -c -o - %s

// WARN: warning: no such include directory: '/null/hstk'
