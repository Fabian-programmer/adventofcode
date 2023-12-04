#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

std::int32_t getCalibrationValue(const std::string &str) {

  const std::map<std::string, std ::string> numbers = {
      {"one", "1"},   {"two", "2"},   {"three", "3"},
      {"four", "4"},  {"five", "5"},  {"six", "6"},
      {"seven", "7"}, {"eight", "8"}, {"nine", "9"},
  };

  std::stringstream ss;

  for (auto it = str.begin(); it != str.end(); it++) {
    if (std::isdigit(*it)) {
      ss << *it;
      goto labelrev;
    }

    std::string word(str.begin(), it);
    for (const auto &key : numbers) {
      if (word.find(key.first) != std::string::npos) {
        ss << key.second;
        goto labelrev;
      }
    }
  }

labelrev:
  const std::map<std::string, std ::string> revnumbers = {
      {"eno", "1"},   {"owt", "2"},   {"eerht", "3"},
      {"ruof", "4"},  {"evif", "5"},  {"xis", "6"},
      {"neves", "7"}, {"thgie", "8"}, {"enin", "9"},
  };

  for (auto it = str.rbegin(); it != str.rend(); it++) {

    if (std::isdigit(*it)) {
      ss << *it;
      goto labelret;
    }

    std::string word(str.rbegin(), it);
    for (const auto &key : revnumbers) {
      if (word.find(key.first) != std::string::npos) {
        ss << key.second;
        goto labelret;
      }
    }
  }

labelret:
  return stoi(ss.str());
}

int main() {

  std::ifstream file("input");
  std::uint64_t sum = 0;

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      sum += getCalibrationValue(line);
    }
    file.close();
  }

  std::cout << "Sum of first and last number: " << sum << "\n";

  return 0;
}
