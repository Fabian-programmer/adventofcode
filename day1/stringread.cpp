#include "algorithm"
#include "input.hpp"
#include <iostream>

constexpr std::array<std::pair<std::string_view, std::uint8_t>, 9> numbers = {
    {{"one", 1},
     {"two", 2},
     {"three", 3},
     {"four", 4},
     {"five", 5},
     {"six", 6},
     {"seven", 7},
     {"eight", 8},
     {"nine", 9}}};

std::uint8_t getCalibrationValue(const std::string_view str,
                                 const bool letteredNumber = false) {
  std::array<std::uint8_t, 2> number = {0, 0};

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (std::isdigit(str[i])) {
      number[0] = str[i] - '0';
      goto labelbackwards;
    }

    if (letteredNumber) {
      const std::string_view letteredNumber = str.substr(i);

      for (const auto &[key, value] : numbers) {
        if (letteredNumber.starts_with(key)) {
          number[0] = value;
          goto labelbackwards;
        }
      }
    }
  }

labelbackwards:
  for (std::size_t i = str.size() - 1; i >= 0; --i) {
    if (std::isdigit(str[i])) {
      number[1] = str[i] - '0';
      goto labelreturn;
    }

    if (letteredNumber) {
      const std::string_view letteredNumber = str.substr(0, i);

      for (const auto &[key, value] : numbers) {
        if (letteredNumber.ends_with(key)) {
          number[1] = value;
          goto labelreturn;
        }
      }
    }
  }

labelreturn:
  return number[0] * 10 + number[1];
}

int main() {
  std::uint32_t sum_p1 = 0;
  std::uint32_t sum_p2 = 0;

  for (const auto item : compileTimeInput) {
    sum_p1 += getCalibrationValue(item, false);
    sum_p2 += getCalibrationValue(item, true);
  }

  std::cout << "Part1: Sum of first and last number: " << sum_p1 << "\n";
  std::cout << "Part2: Sum of first and last number: " << sum_p2 << "\n";

  return 0;
}
