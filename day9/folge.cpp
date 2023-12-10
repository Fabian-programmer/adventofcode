#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

bool areAllElementsZero(const std::vector<std::int64_t> &sequence) {
  return std::all_of(sequence.begin(), sequence.end(),
                     [](const std::int64_t &element) { return element == 0; });
}

std::int64_t nextElement(const std::vector<std::int64_t> &sequence) {

  auto subSequence = sequence;
  std::vector<std::int64_t> lastRow{sequence.back()};

  while (!areAllElementsZero(subSequence)) {
    std::vector<std::int64_t> tmpSequence;
    for (std::size_t i = 0; i < subSequence.size() - 1; ++i) {
      tmpSequence.push_back(subSequence[i + 1] - subSequence[i]);
    }

    lastRow.push_back(tmpSequence.back());
    subSequence = tmpSequence;
  }

  return std::accumulate(lastRow.begin(), lastRow.end(), 0);
}

int main() {

  std::ifstream file("input");
  std::vector<std::vector<std::int64_t>> sequences;

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      std::istringstream iss(line);

      std::int64_t num;
      std::vector<std::int64_t> sequence;
      while (iss >> num) {
        sequence.push_back(num);
      }
      sequences.push_back(sequence);
    }
    file.close();
  }

  std::int64_t sum_p1 = 0;
  std::int64_t sum_p2 = 0;

  for (auto &sequence : sequences) {
    const auto value_p1 = nextElement(sequence);

    std::reverse(sequence.begin(), sequence.end());
    const auto value_p2 = nextElement(sequence);

    sum_p1 += value_p1;
    sum_p2 += value_p2;
  }

  std::cout << "sum of last elements: " << sum_p1 << "\n";
  std::cout << "sum of first elements: " << sum_p2 << "\n";

  return 0;
}
