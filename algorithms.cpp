#include "algorithms.hpp"

#include <sys/types.h>

#include <cmath>
#include <cstdint>
#include <random>
using namespace std;

random_device rd;  // створення рандомного числа-генератора
mt19937 gen(rd());

// =========== для Міллера-Рабіна =================

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

// =========== для методу пробних ділень =================

uint64_t pascal_mod(const string& N, uint64_t m)  // взяття числа за модулем m
{
  uint64_t result = 0;
  uint64_t r = 1;

  for (int i = (int)N.size() - 1; i >= 0; --i) {
    uint64_t digit = N[i] - '0';

    result = (result + digit * r) % m;
    r = (r * 10) % m;
  }

  return result;
}

string divide_string_by_uint64(const string& N, uint64_t divisor) {
  string quotient;

  uint64_t remainder = 0;

  for (char c : N) {
    remainder = remainder * 10 + (c - '0');

    quotient += char('0' + remainder / divisor);

    remainder %= divisor;
  }

  size_t pos = quotient.find_first_not_of('0');

  if (pos == string::npos) return "0";

  return quotient.substr(pos);
}

// =========== для Ро-методу Поларда =================

// Символ Лежандра (a/p): 1 якщо a є квадратичним лишком mod p, -1 інакше, 0 якщо a = 0
int legendre(uint64_t a, uint64_t p) {
  a = a % p;
  if (a == 0) return 0;
  uint64_t res = Gorner(a, (p - 1) / 2, p);  // Критерій Ойлера
  if (res == p - 1) return -1;
  return (int)res;
}

// Модульне множення
uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod) { return ((__uint128_t)a * b) % mod; }

// gcd для 64-бітних чисел
uint64_t gcd64(uint64_t a, uint64_t b) {
  while (b) {
    a %= b;
    swap(a, b);
  }
  return a;
}

// =========== для Брілхарта-Морісона =================

/* ---- Метод Гауса над GF(2) ----
шукає фундаментальну систему розв'язків (ФСР) однорідної системи A*x = 0 над GF(2), а розмірністю rows*cols (рядки - рівняння, стовпці - змінні xi),
повертає список базисних векторів ФСР
*/

vector<vector<int>> gauss_gf2(vector<vector<int>> A, int rows, int cols) {
  vector<int> pivot_col_of_row(rows, -1);
  vector<int> pivot_row_of_col(cols, -1);
  int cur = 0;

  for (int col = 0; col < cols && cur < rows; col++) {
    int pivot = -1;
    for (int r = cur; r < rows; r++)
      if (A[r][col]) {
        pivot = r;
        break;
      }
    if (pivot == -1) continue;

    swap(A[cur], A[pivot]);
    pivot_col_of_row[cur] = col;
    pivot_row_of_col[col] = cur;

    for (int r = 0; r < rows; r++)
      if (r != cur && A[r][col])
        for (int c = 0; c < cols; c++) A[r][c] ^= A[cur][c];
    cur++;
  }

  vector<int> free_vars;
  for (int c = 0; c < cols; c++)
    if (pivot_row_of_col[c] == -1) free_vars.push_back(c);

  vector<vector<int>> basis;
  for (int fv : free_vars) {
    vector<int> x(cols, 0);
    x[fv] = 1;

    for (int r = 0; r < cur; r++)
      if (pivot_col_of_row[r] != -1) x[pivot_col_of_row[r]] = A[r][fv];
    basis.push_back(x);
  }
  return basis;
}

// =========== Міллер-Рабін =================

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
          uint x_r = Gorner(x, d << r, p);
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

// =========== метод пробних ділень =================

vector<uint64_t> trial_division(string N) {
  vector<uint64_t> factors;

  if (N == "0" || N == "1") return factors;

  uint64_t d = 2;

  while (true) {
    bool divided = false;

    while (pascal_mod(N, d) == 0) {
      factors.push_back(d);

      N = divide_string_by_uint64(N, d);

      divided = true;

      if (N == "1") return factors;
    }

    d++;

    /*
      Для завершення треба знати, чи d > sqrt(N)

      Оскільки після кожного ділення N зменшується, для чисел до 64 біт можна безпечно перетворити N
     */
    if (N.size() < 19) {
      uint64_t current = stoull(N);

      if (d > current / d) {
        if (current > 1) factors.push_back(current);

        return factors;
      }
    }
  }
}

// =========== РО-метод Полларда =================

uint64_t f_pollard(uint64_t x, uint64_t c, uint64_t n) { return (((__uint128_t)x * x) + c) % n; }

// Повертає нетривіальний дільник числа n, або n якщо не вдалося знайти
uint64_t pollard_rho(uint64_t n) {
  if (n % 2 == 0) return 2;

  uniform_int_distribution<uint64_t> dist(2, n - 1);

  while (true) {
    // 1. x0 = 2, y0 = 2, d = 1
    uint64_t x = 2;
    uint64_t y = 2;
    uint64_t d = 1;
    uint64_t c = dist(gen);  // випадкова константа; змінюється при невдачі (крок 3)

    // 2 та 4: цикл Флойда
    while (d == 1) {
      x = f_pollard(x, c, n);                   // xi = f(x_{i-1})
      y = f_pollard(f_pollard(y, c, n), c, n);  // yi = f(f(y_{i-1}))

      uint64_t diff = (x > y) ? (x - y) : (y - x);
      d = gcd(diff % n, n);  // d = gcd(xi - yi, n)
    }

    // 3. Якщо x == y (d!=n) - алгоритм не знайшов дільника, повертаємось на крок 1 з новим c
    if (d != n) return d;
    // інакше -  новий c підбирається автоматично на наступній ітерації while(true)
  }
}

// =========== Метод Брілхарта-Моррісона =================

uint64_t brillhart_morrison(uint64_t n) {  // Повертає нетривіальний дільник числа n, або 0 якщо не знайдено

  if (n < 4) return 0;
  if (n % 2 == 0) return 2;

  // Перевіряємо чи n - точний квадрат
  uint64_t isqrt_n = (uint64_t)sqrt((double)n);
  while ((isqrt_n + 1) * (isqrt_n + 1) <= n) isqrt_n++;
  while (isqrt_n * isqrt_n > n) isqrt_n--;
  if (isqrt_n * isqrt_n == n) return isqrt_n;

  double sqrt_n = (double)isqrt_n;

  // 1
  double logn = log((double)n);
  double loglogn = log(logn > 1.0 ? logn : 1.1);
  int limit = max(150, (int)pow(exp(sqrt(logn * loglogn)), 1.0 / sqrt(2.0)));
  if (limit > 10000) limit = 10000;

  // Решето Ератосфена
  vector<bool> sieve(limit + 1, true);
  sieve[0] = sieve[1] = false;
  for (int i = 2; i <= limit; i++)
    if (sieve[i])
      for (int j = 2 * i; j <= limit; j += i) sieve[j] = false;

  vector<int64_t> B = {-1};
  for (int p = 2; p <= limit; p++) {
    if (!sieve[p]) continue;
    if (n % p == 0) return (uint64_t)p;
    if (legendre(n % p, (uint64_t)p) == 1) B.push_back((int64_t)p);
  }
  int K = (int)B.size();  // розмір бази (включає -1 на позиції 0)

  // Буфери для системи GF(2)
  vector<vector<int>> gf2_rows;  // вектори s_i
  vector<uint64_t> bi_vals;      // b_i mod n
  vector<vector<int>> exp_rows;  // повні степені e_{i,j}

  // 2: ланцюговий дріб
  uint64_t u = isqrt_n;   // u_0 = floor(sqrt(n))
  uint64_t v = 1;         // v_0 = 1
  uint64_t ai = isqrt_n;  // a_0
  uint64_t bp2 = 0;       // b_{-2} = 0
  uint64_t bp1 = 1;       // b_{-1} = 1
  // Знак (-1)^(i+1): при i=0 -> -1 (b_0^2 = -v_1 mod n)
  int sign = -1;

  int max_iter = K * 40 + 2000;

  for (int iter = 0; iter < max_iter; iter++) {
    uint64_t b_cur = (mulmod(ai, bp1, n) + bp2) % n;

    // Обчислюємо v_{i+1}
    if (v == 0) break;
    uint64_t v_next = (n - u * u) / v;
    if (v_next == 0) break;

    // розкладаємо v_next по факторній базі
    {
      vector<int> exps(K, 0);
      if (sign < 0) exps[0] = 1;

      int64_t rem = (int64_t)v_next;
      bool smooth = true;
      for (int j = 1; j < K && smooth; j++)
        while (rem % B[j] == 0) {
          rem /= B[j];
          exps[j]++;
        }
      if (rem != 1) smooth = false;

      if (smooth) {
        // s_i = exps mod 2
        vector<int> s(K);
        for (int j = 0; j < K; j++) s[j] = exps[j] & 1;

        // Допоміжні лямбди
        auto build_Y = [&](const vector<int>& e) -> uint64_t {
          uint64_t Y = 1;
          for (int j = 1; j < K; j++)
            for (int t = 0; t < e[j] / 2; t++) Y = mulmod(Y, (uint64_t)B[j], n);
          return Y;
        };

        auto try_factor = [&](uint64_t X, uint64_t Y) -> uint64_t {
          // 5
          uint64_t d1 = gcd64((X + Y) % n, n);
          uint64_t d2 = gcd64((X >= Y ? X - Y : n - Y + X), n);
          if (d1 > 1 && d1 < n) return d1;
          if (d2 > 1 && d2 < n) return d2;
          return 0;
        };

        bool is_zero = true;  // якщо всі степені парні, одразу видаємо розв'язок
        for (int j = 0; j < K; j++)
          if (s[j]) {
            is_zero = false;
            break;
          }
        if (is_zero) {
          uint64_t d = try_factor(b_cur, build_Y(exps));
          if (d) return d;
          goto update_cf;
        }

        for (int prev = 0; prev < (int)gf2_rows.size(); prev++) {
          if (gf2_rows[prev] != s) continue;
          uint64_t X = mulmod(b_cur, bi_vals[prev], n);
          vector<int> combined(K);
          for (int j = 0; j < K; j++) combined[j] = exps[j] + exp_rows[prev][j];
          uint64_t d = try_factor(X, build_Y(combined));
          if (d) return d;
          break;
        }

        gf2_rows.push_back(s);
        bi_vals.push_back(b_cur);
        exp_rows.push_back(exps);

        // 3: при K+1 векторах розв'язуємо систему x0*s0 + ... + xk*sk = 0 над GF(2)
        int NV = (int)gf2_rows.size();
        if (NV > K) {
          // Будуємо матрицю
          vector<vector<int>> sys(K, vector<int>(NV, 0));
          for (int i = 0; i < NV; i++)
            for (int j = 0; j < K; j++) sys[j][i] = gf2_rows[i][j];

          // Знаходимо ФСР методом Гауса над GF(2)
          auto basis = gauss_gf2(sys, K, NV);

          for (auto& sol : basis) {
            // 4
            uint64_t X = 1;
            vector<int64_t> gamma(K, 0);
            for (int i = 0; i < NV; i++) {
              if (!sol[i]) continue;
              X = mulmod(X, bi_vals[i], n);
              for (int j = 0; j < K; j++) gamma[j] += exp_rows[i][j];
            }
            // gamma_j мають бути парними (інакше рішення некоректне)
            bool ok = true;
            for (int j = 0; j < K; j++)
              if (gamma[j] & 1) {
                ok = false;
                break;
              }
            if (!ok) continue;

            uint64_t Y = 1;
            for (int j = 1; j < K; j++)
              for (int64_t t = 0; t < gamma[j] / 2; t++) Y = mulmod(Y, (uint64_t)B[j], n);

            uint64_t d = try_factor(X, Y);
            if (d) return d;
          }
        }
      }
    }

  update_cf:
    // Оновлення параметрів ланцюгового дробу:
    uint64_t a_next = (uint64_t)((sqrt_n + (double)u) / (double)v_next);
    uint64_t u_next = a_next * v_next - u;

    bp2 = bp1;
    bp1 = b_cur;
    ai = a_next;
    u = u_next;
    v = v_next;
    sign = -sign;
  }

  return 0;
}
