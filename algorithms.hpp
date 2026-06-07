#include <cstdint>
#include <string>
#include <vector>

using namespace std;
// === для Міллера-Рабіна ===
uint32_t gcd(uint32_t a, uint32_t b);
int Gorner(uint32_t base, uint32_t exp, uint32_t modulo);
int Miller_Rabin(uint32_t p, int k);

int Miller_Rabin_64(uint64_t n);

// === для методу пробних ділень ===
uint64_t pascal_mod(const string& N, uint64_t m);
string divide_string_by_uint64(const string& N, uint64_t divisor);
vector<uint64_t> trial_division(string N);

// === для РО-методу Полларда ===
uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod);
uint64_t gcd64(uint64_t a, uint64_t b);
uint64_t f_pollard(uint64_t x, uint64_t c, uint64_t n);
uint64_t pollard_rho(uint64_t n);

// === для методу Брілхарта-Моррісона ===
int legendre(uint64_t a, uint64_t p);
vector<vector<int>> gauss_gf2(vector<vector<int>> A, int rows, int cols);
uint64_t brillhart_morrison(uint64_t n);

// === для канонічного розкладу ===
struct FactorEntry {
  uint64_t p;
  const char* method;
  double found_at_ms;
};

bool canonical_factorize(uint64_t n, vector<FactorEntry>& out, double start_us);

// === для бенчмарку ===
struct BenchResult {
  uint64_t n;
  uint64_t divisor_pollard;
  double time_pollard_ms;
  uint64_t divisor_bm;
  double time_bm_ms;
};

BenchResult benchmark_factorizers(uint64_t n);
