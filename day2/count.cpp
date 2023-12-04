#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>

constexpr std::uint32_t maxRed = 12;
constexpr std::uint32_t maxGreen = 13;
constexpr std::uint32_t maxBlue = 14;

std::uint32_t getHighestNumber(const std::string &line,
                               const std::string &color) {

  std::regex pattern("(" + std::string(R"((\d+)\s+)") + color + ")");

  // Iterator for matching
  std::sregex_iterator iterator(line.begin(), line.end(), pattern);
  std::sregex_iterator end;

  std::int32_t highestNumber = 0;
  while (iterator != end) {
    std::smatch match = *iterator;
    std::int32_t number = std::stoi(match[1].str());
    if (number > highestNumber) {
      highestNumber = number;
    }
    ++iterator;
  }
  return highestNumber;
}

std::uint32_t getIdOfPossibleGame(const std::string &line) {

  if (getHighestNumber(line, "red") > maxRed) {
    return false;
  }
  if (getHighestNumber(line, "green") > maxGreen) {
    return false;
  }
  if (getHighestNumber(line, "blue") > maxBlue) {
    return false;
  }

  std::regex pattern("Game (\\d+)");
  std::smatch match;

  if (std::regex_search(line, match, pattern)) {
    if (match.size() > 1) {
      return stoi(match[1]);
    }
  }

  return false;
};

std::uint32_t getMinimumPossibleGame(const std::string &line) {

  return getHighestNumber(line, "red") * getHighestNumber(line, "green") *
         getHighestNumber(line, "blue");
};

int main() {

  std::ifstream file("input");
  std::uint64_t sum_p1 = 0;
  std::uint64_t sum_p2 = 0;

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      sum_p1 += getIdOfPossibleGame(line);
      sum_p2 += getMinimumPossibleGame(line);
    }
    file.close();
  }

  std::cout << "Sum of Game Ids of possible games: " << sum_p1 << "\n";
  std::cout << "Sum of multiplication of min num of sets: " << sum_p2 << "\n";
}
