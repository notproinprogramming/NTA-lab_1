#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include "algorithms.hpp"

struct TestCase {
  uint32_t n;
  bool expected_prime;
};

static const TestCase CASES[] = {
    // прості
    {3u, true},
    {5u, true},
    {7u, true},
    {11u, true},
    {97u, true},
    {997u, true},
    {7919u, true},
    // складені
    {4u, false},
    {9u, false},
    {15u, false},
    {25u, false},
    {561u, false},
    {1729u, false},  // числа Кармайкла
};

void run_correctness_check(uint32_t k) {
  std::cout << "=== Перевірка коректності (k=" << k << ") ===\n\n";
  bool all_ok = true;

  for (auto& tc : CASES) {
    int result = Miller_Rabin(tc.n, k);
    bool is_prime = (result == 0);
    bool ok = (is_prime == tc.expected_prime);

    std::cout << "  n=" << std::setw(6) << tc.n << "  очікується: " << (tc.expected_prime ? "ПРОСТЕ   " : "СКЛАДЕНЕ ")
              << "  отримано: " << (is_prime ? "ПРОСТЕ   " : "СКЛАДЕНЕ ") << (ok ? "" : "  <-- ПОМИЛКА") << "\n";

    if (!ok) all_ok = false;
  }

  std::cout << "\nПідсумок: " << (all_ok ? "всі тести пройшли успішно" : "Є ПОМИЛКИ") << "\n";
}

void run_timing(uint32_t p, uint32_t k) {
  std::cout << "\n--- Замір часу: n=" << p << ", k=" << k << " ---\n";

  auto t_start = std::chrono::high_resolution_clock::now();

  int result = Miller_Rabin(p, k);

  auto t_end = std::chrono::high_resolution_clock::now();
  double us = std::chrono::duration<double, std::micro>(t_end - t_start).count();

  std::cout << "Результат: " << (result == 0 ? "ПРОСТЕ" : "СКЛАДЕНЕ") << "\n";
  std::cout << "Час:       " << std::fixed << std::setprecision(3) << us << " мкс\n";
}

int main() {
  const uint32_t K = 10;

  run_correctness_check(K);

  std::cout << "\n=== Заміри часу ===\n";
  run_timing(7919u, K);    // просте, мале
  run_timing(999983u, K);  // просте ~10^6
  run_timing(999984u, K);  // складене

  return 0;
}
