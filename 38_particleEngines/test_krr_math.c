#include "krr_math.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
  krr_math_rand_seed_time();
  printf("krr_math_rand_int = %d\n", krr_math_rand_int(10));
  printf("krr_math_rand_int2 100-150 = %d\n", krr_math_rand_int2(100, 150));
  printf("krr_math_rand_float2(100,150) = %.2f\n", krr_math_rand_float2(100, 150));
  printf("krr_math_rand_float2(-100,200) = %.2f\n", krr_math_rand_float2(-100, 200));
  printf("krr_math_rand_float2(0,0) = %.2f\n", krr_math_rand_float2(0,0));
  return 0;
}
