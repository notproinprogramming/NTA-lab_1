#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include "algorithms.hpp"

// Виводить вектор множників у вигляді n = p1 * p2 * ...
void print_factors(uint64_t n, const vector<uint64_t>& factors) {
  cout << n << " = ";
  for (int i = 0; i < (int)factors.size(); i++) {
    if (i) cout << " * ";
    cout << factors[i];
  }
  cout << "\n";
}

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

  // --- РО-метод Полларда ---

  cout << "=== РО-метод Полларда ===\n";

  vector<uint64_t> pollard_tests = {
      15,           // 3 * 5
      8051,         // 83 * 97
      1234567,      // 127 * 9721
      15770708441,  // 115979 * 135979
  };

  for (uint64_t n : pollard_tests) {
    uint64_t d = pollard_rho(n);
    cout << n << " -> " << d << " * " << n / d << "\n";
  }

  // --- Метод Брілхарта-Моррісона ---

  cout << "\n=== Метод Брілхарта-Моррісона ===\n";

  vector<uint64_t> bm_tests = {
      15,           // 3 * 5
      8051,         // 83 * 97
      1234567,      // 127 * 9721
      15770708441,  // 115979 * 135979
  };

  for (uint64_t n : bm_tests) {
    uint64_t d = brillhart_morrison(n);
    if (d > 1 && d < n)
      cout << n << " -> " << d << " * " << n / d << "\n";
    else
      cout << n << " -> не знайдено\n";
  }

  return 0;
}
