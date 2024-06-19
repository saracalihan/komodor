/*
 * Copyright (c) 2023, Alihan SARAÇ <saracalihan@gmail.com>
 *
 * This file is part of ASstdlib project and under the "MIT" license. Please read "/LICENSE" file for more detail.
 *
 * This functions are *NOT MT-SAFE*.
 */

//Example usage
/*
Tests({
  Describe("+ operation", {
      Test("5 + 2 should equal to 7", {
          int a = 5;
          int b = 2;
          a + b == 7; // return value
      });
      Test("This test fail", {
          int a = 5;
          int b = 2;
          a + b == 8; // return value
      });
  });

  int a = 5; // Test scope variable
  Describe("- operation", {
    int b = 2; // Describe scope variable
    Test("5 - 2 should equal to 3", {
        a - b == 3; // return value
    });
  });
});
*/
#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define TAB_CHAR "  "
#ifndef ASSTDLIB_EXIT_WHEN_TEST_FAIL
#define ASSTDLIB_EXIT_WHEN_TEST_FAIL 0
#endif

static bool status = false;
static int groupCount = 0, test_count = 0, passed_test = 0, failed_test = 0;

#define Tests(body) \
do{\
    groupCount = 0, test_count = 0, passed_test = 0, failed_test = 0; \
    (body); \
    printf("\nTests: \x1B[32m%d passed\033[0m, \x1B[31m%d failed\033[0m, %d total\nGroups: %d\n%s\n\n", \
           passed_test, failed_test, test_count, groupCount, __FILE__); \
    if (ASSTDLIB_EXIT_WHEN_TEST_FAIL && failed_test != 0) \
        exit(1);\
}while(0);

#define Describe(label, body) \
do{\
    groupCount++; \
    printf("\n\x1B[1m%s\033[0m:\n", label); \
    (body); \
}while(0);

#define Test(label, body) \
do{\
  test_count++; \
  printf("%s\033[3;43;30m RUNNING \033[0m \033[3m%s\033[0m\n", TAB_CHAR, label); \
  status = (body); \
  printf("%s%s\n", TAB_CHAR, (status ? "   \033[1;42;37m PASS \033[0m" : "   \033[1;41;37m FAIL \033[0m")); \
  status ? passed_test++ : failed_test++; \
}while(0);
