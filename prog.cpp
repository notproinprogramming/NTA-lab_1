#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "algorithms.hpp"

using namespace std;

// =========== канонічний розклад =================

static void run_canonical(uint64_t n) {
  using Clock = chrono::high_resolution_clock;
  using ms = chrono::duration<double, chrono::microseconds::period>;

  double start_ms = ms(Clock::now().time_since_epoch()).count();

  vector<FactorEntry> factors;
  bool ok = canonical_factorize(n, factors, start_ms);

  double total_ms = ms(Clock::now().time_since_epoch()).count() - start_ms;

  cout << "\n--- canonical_factorize(" << n << ") ---\n";

  if (!ok) {
    cout << "я не можу знайти канонiчний розклад числа :(\n";
    return;
  }

  cout << left << setw(20) << "factor" << setw(22) << "method"
       << "found at (ms)\n"
       << string(56, '-') << "\n";

  for (auto& f : factors) {
    cout << setw(20) << f.p << setw(22) << f.method << fixed << setprecision(2) << f.found_at_ms << "\n";
  }

  cout << "\n" << n << " = ";
  for (int i = 0; i < (int)factors.size(); i++) {
    if (i) cout << " * ";
    cout << factors[i].p;
  }
  cout << "\ntotal: " << fixed << setprecision(2) << total_ms << " us\n";
}

// =========== бенчмарк для Ро-методу Полларда та БМ =================

static void run_benchmark(const vector<uint64_t>& numbers) {
  cout << left << setw(22) << "n" << setw(18) << "Pollard div" << setw(14) << "Pollard ms" << setw(18) << "B-M div" << setw(14) << "B-M ms"
       << "faster\n"
       << string(96, '-') << "\n";

  for (uint64_t n : numbers) {
    BenchResult r = benchmark_factorizers(n);

    const char* faster = "tie";
    if (r.time_pollard_ms < r.time_bm_ms * 0.95)
      faster = "Pollard";
    else if (r.time_bm_ms < r.time_pollard_ms * 0.95)
      faster = "B-M";

    cout << left << setw(22) << r.n << setw(18) << (r.divisor_pollard > 1 && r.divisor_pollard < n ? to_string(r.divisor_pollard) : "fail")
         << setw(14) << fixed << setprecision(1) << r.time_pollard_ms << setw(18)
         << (r.divisor_bm > 1 && r.divisor_bm < n ? to_string(r.divisor_bm) : "fail") << setw(14) << fixed << setprecision(1) << r.time_bm_ms
         << faster << "\n";
  }
}

int main() {
  cout << "=== канонічний розклад ===";
  run_canonical(323324583518541583ULL);

  vector<uint64_t> bench_numbers = {
      3009182572376191ULL, 1021514194991569ULL, 4000852962116741ULL, 15196946347083ULL, 499664789704823ULL,
      269322119833303ULL,  679321846483919ULL,  96267366284849ULL,   61333127792637ULL, 2485021628404193ULL,
  };

  cout << "\n=== Ро-метод Полларда проти методу Брілхарта-Морісона ===\n\n";

  run_benchmark(bench_numbers);

  return 0;
}
