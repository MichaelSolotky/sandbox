#ifdef _MSC_VER
#pragma region Definitions
#endif

#define _CRT_SECURE_NO_WARNINGS
#define D_SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <algorithm>
#include <array>
#include <bitset>
#include <cctype> // isalnum, isdigit, isspace
#include <chrono>
#include <climits>
#include <cmath>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <cstring>//memset
#include <cstdio>

using namespace std;

inline bool is_good_digit(const unsigned c) {
  return c - unsigned('0') <= unsigned(9);
}
inline bool is_skip_digit(const unsigned c){
  return unsigned(9) < c - unsigned('0');
}

//This function has no error checks!
template<typename T
  //  ,  typename std::enable_if<std::is_unsigned<T>::value, bool>::type = nullptr
>
inline void ru(T& result) {
  static_assert(is_unsigned<T>::value, "An unsigned type is expected.");

  unsigned c;
  do {
    c = getchar();
  } while (is_skip_digit(c));

  result = 0;
  do {
    result =
      //times 10.
      (result << 3) + (result << 1)
      + c - '0';

    c = getchar();
  } while (is_good_digit(c));
}
//Return the length of the read non-empty string.
//This function has no error checks!
inline unsigned rs(char* s) {
  auto p = s;//Assume the string is not empty.
  *p++ = static_cast<char>(getchar());

  for (;;)
  {
    auto c = getchar();
    if (isspace(c))
    {
      //These are not needed for the first read:
      //
      // if (s == p) continue;//Skip initial white-space
      // *p = 0;
      auto ret = static_cast<unsigned>(p - s);
      return ret;
    }
    *p++ = static_cast<char>(c);
  }
}
//Read a set of strings.
//This function has no error checks!
//
// char TheStrings[MAX_SIZE << 1];
// unsigned Lens[MAX_SIZE];
// Input example:
// //One offset to make comparisons easier!
// read_char_strings(Ss + 1, Lens, n);
inline void read_char_strings(
  char* input_buffer,
  unsigned *lengths,
  const unsigned n){
  char* b = input_buffer;
  for (unsigned i = 0; i < n; ++i)
  {
    const auto le = rs(b);
    lengths[i] = le;
    b += le + 1;
  }
}

//This function has no error checks!
//There are no overflow checks e.g. (INT_MIN) won't be read.
//The stream might end, etc.
template<typename T
  //, typename std::enable_if<!std::is_unsigned<T>::value, bool>::type = nullptr
>
inline void r(T& result){
  static_assert(is_signed<T>::value, "A signed type is expected.");

  T sign = 1;

  unsigned c;
  do
  {
    c = getchar();
    if ('-' == c)
    {
      sign = -1;
      c = getchar();
    }
  } while (is_skip_digit(c));

  result = 0;
  do
  {
    result =
      //times 10.
      (result << 3) + (result << 1)
      + static_cast<T>(c - '0');

    c = getchar();
  } while (is_good_digit(c));

  result *= sign;
}

template<typename number_t>
inline void wu(number_t a){
  static_assert(is_unsigned<number_t>::value, "An unsigned type is expected.");

  if (a < 10)
  {
    putchar('0' + static_cast<int>(a));
  }
  else
  {
    wu(a / 10);
    putchar('0' + static_cast<int>(a % 10));
  }
}

using ll = long long;
using llu = long long unsigned;
using ull = long long unsigned;

constexpr unsigned mod = (1000 * 1000 * 1000) + 7;
constexpr unsigned long long mod2 = 8 * static_cast<llu>(mod) * static_cast<llu>(mod);

#ifdef _MSC_VER
#pragma endregion Definitions
#endif

#ifdef Zeg
constexpr unsigned int MAX_SIZE = ((0 * 1000) + 10);
#else
constexpr unsigned int MAX_SIZE = ((1 * 1000) + 10);
#endif

unsigned Numbers[MAX_SIZE * MAX_SIZE];
//down row, right col.
unsigned Links[MAX_SIZE * MAX_SIZE][2];
unsigned n_rows, m_cols;

inline void finlinks(const unsigned yr, const unsigned xc,
  unsigned&ro, unsigned&col){
  //const auto pos = yr * (m_cols + 1) + xc;

  ro = yr * (m_cols + 1);
  for (unsigned i = 1; i < xc; ++i)
    ro = Links[ro][1];

  col = xc;
  for (unsigned i = 1; i < yr; ++i)
    col = Links[col][0];
}
inline void swap_right(const unsigned p1, const unsigned p2){
  auto temp = Links[p1][1];
  Links[p1][1] = Links[p2][1];
  Links[p2][1] = temp;
}

inline void swap_right_horizontal(const unsigned h,
  unsigned& p1, unsigned& p2) {
  for (unsigned j = 0; ; ) {
    swap_right(p1, p2);
    if (++j == h)
      break;
    p1 = Links[p1][0];
    p2 = Links[p2][0];
  }
}
inline void swap_down(const unsigned p1, const unsigned p2) {
  auto temp = Links[p1][0];
  Links[p1][0] = Links[p2][0];
  Links[p2][0] = temp;
}
inline void swap_down_vertical(const unsigned w,
  unsigned& p1, unsigned& p2){
  for (unsigned j = 0; ; )
  {
    swap_down(p1, p2);
    if (++j == w)
    {
      break;
    }
    p1 = Links[p1][1];
    p2 = Links[p2][1];
  }
}

int main() {
  unsigned q;
  ru(n_rows); ru(m_cols); ru(q);
  for (unsigned r = 1; r <= n_rows; ++r) {
    for (unsigned col = 1; col <= m_cols; ++col) {
      const auto pos = r * (m_cols + 1) + col;
      ru(Numbers[pos]);
      const auto up = pos - m_cols - 1;
      const auto diag = up - 1;
      const auto le = pos - 1;
      Links[diag][0] = le;
      Links[diag][1] = up;
    }
    Links[r * (m_cols + 1) - 1][0] = (r + 1) * (m_cols + 1) - 1;
  }
  for (unsigned col = 1; col <= m_cols; ++col) {
    const auto pos2 = n_rows * (m_cols + 1) + col;
    Links[pos2 - 1][1] = pos2;
  }

  for (unsigned i = 0; i < q; ++i)
  {
    unsigned y1, x1, y2, x2, h, w;
    ru(y1); ru(x1); ru(y2); ru(x2); ru(h); ru(w);

    unsigned row1, col1;
    finlinks(y1, x1, row1, col1);

    unsigned row2, col2;
    finlinks(y2, x2, row2, col2);

    {
      //left
      swap_right_horizontal(h, row1, row2);

      //bottom
      row1 = Links[row1][1];
      row2 = Links[row2][1];
      swap_down_vertical(w, row1, row2);
    }
    {
      //top
      swap_down_vertical(w, col1, col2);

      //right
      col1 = Links[col1][0];
      col2 = Links[col2][0];
      swap_right_horizontal(h, col1, col2);
    }
  }

  for (unsigned r = 1; r <= n_rows; ++r)
  {
    auto pos = r * (m_cols + 1);
    for (unsigned col = 1; col <= m_cols; col++)
    {
      pos = Links[pos][1];
      wu(Numbers[pos]);
      putchar(' ');
    }
    putchar('\n');
  }
  return 0;
}