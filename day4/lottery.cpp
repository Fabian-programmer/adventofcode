#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

std::uint32_t getHits(const std::string &line) {

  std::size_t pos_start_winning = line.find(":");
  std::size_t pos_start_mine = line.find("|");

  std::string winningCard =
      line.substr(pos_start_winning + 1, pos_start_mine - pos_start_winning);
  std::string mineCard = line.substr(pos_start_mine + 1);

  // wining numbers
  std::vector<std::uint32_t> winning;
  std::istringstream issw(winningCard);
  std::uint32_t num;
  while (issw >> num) {
    winning.push_back(num);
  }

  // my numbers
  std::vector<std::uint32_t> mine;
  std::istringstream issm(mineCard);
  while (issm >> num) {
    mine.push_back(num);
  }
  std::sort(mine.begin(), mine.end());

  uint32_t hits = 0;

  for (const auto &number : winning) {
    if (std::binary_search(mine.begin(), mine.end(), number)) {
      hits++;
    }
  }
  return hits;
}

int main() {

  std::ifstream file("input");
  std::uint64_t sum_p1 = 0;
  std::uint64_t sum_p2 = 0;

  std::vector<std::uint32_t> cardInstances;
  cardInstances.resize(300);

  static int currentRow = 0;
  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      const auto points = getHits(line);

      if (points != 0) {
        sum_p1 += std::pow(2, points - 1);
      }

      cardInstances[currentRow] += 1; // orginal
      for (std::size_t j = 0; j != points; j++) {
        cardInstances[currentRow + j + 1] += cardInstances[currentRow];
      }

      currentRow++;
    }
    file.close();
  }

  cardInstances.resize(currentRow);
  sum_p2 = std::accumulate(cardInstances.begin(), cardInstances.end(), 0);

  std::cout << "Sum : " << sum_p1 << "\n";
  std::cout << "Sum : " << sum_p2 << "\n";
}
