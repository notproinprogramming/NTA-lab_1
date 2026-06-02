#include "algorithms.hpp"

#include <sys/types.h>

#include <cmath>
#include <cstdint>
#include <random>
using namespace std;

uint gcd(uint a, uint b) {
  if (b == 0) {
    return a;
  }
  uint result = gcd(b, a % b);
  return result;
}

int Gorner(uint32_t base, uint32_t exp, uint32_t modulo) {
  int result = 1;
  int base_copy = base;
  while (exp > 0) {
    if (exp & 1) {
      result = ((uint64_t)result * base_copy) % modulo;
    }
    base_copy = ((uint64_t)base_copy * base_copy) % modulo;
    exp = exp >> 1;
  }
  return result;
}

random_device rd;  // створення рандомного числа-генератора для інших випадкових чисел
mt19937 gen(rd());

int Miller_Rabin(uint32_t p, int k) {  // повертає 1, якщо p складене, 0 якщо просте
  // 0

  uint32_t p_temp = p - 1;
  uint32_t s = 0;  // степінь
  while (p_temp % 2 == 0) {
    p_temp = p_temp / 2;
    s++;
  }
  uint32_t d = (p - 1) >> s;
  int counter = 0;  // лічильник

  // 1

  while (counter < k) {                             // 4
    uniform_int_distribution<> dist_4_x(2, p - 1);  // діапазон генерації ікса
    uint32_t x = dist_4_x(gen);

    uint temp_gcd = gcd(x, p);
    if (temp_gcd == 1) {
      // 2

      bool strong_pseudoprime = 0;  // показник сильно псевдопростоти числа p за основою x, 0, якщо p не є сильно псевдопростим за основою x, 1 - є
      uint32_t temp = Gorner(x, d, p);
      if (temp == 1 || temp == p - 1) {
        strong_pseudoprime = 1;
      } else {
        for (int r = 1; r < s; r++) {
          uint x_r = Gorner(x, d * pow(2, r), p);
          if (x_r == p - 1) {
            strong_pseudoprime = 1;
            break;
          }
          if (x_r == 1) {
            strong_pseudoprime = 0;
            break;
          } else
            continue;
        }
      }
      // 3

      if (strong_pseudoprime == 0)
        return 1;
      else {
        counter++;
      }
    } else if (temp_gcd > 1)
      return 1;
  }
  return 0;
}
