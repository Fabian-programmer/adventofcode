#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Coordinate {
  std::uint32_t x;
  std::uint32_t y;

  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }

  bool checkNeighbourhood(const Coordinate &other) const {

    bool areNeighbours = false;

    areNeighbours = areNeighbours || (x + 1 == other.x && y + 1 == other.y);
    areNeighbours = areNeighbours || (x + 1 == other.x && y + 0 == other.y);
    areNeighbours = areNeighbours || (x + 1 == other.x && y - 1 == other.y);

    areNeighbours = areNeighbours || (x + 0 == other.x && y + 1 == other.y);
    areNeighbours = areNeighbours || (x + 0 == other.x && y - 1 == other.y);

    areNeighbours = areNeighbours || (x - 1 == other.x && y + 1 == other.y);
    areNeighbours = areNeighbours || (x - 1 == other.x && y + 0 == other.y);
    areNeighbours = areNeighbours || (x - 1 == other.x && y - 1 == other.y);

    return areNeighbours;
  }
};

class Grid {
public:
  Grid(std::string input) {

    std::ifstream file(input);
    if (file.is_open()) {
      std::string line;
      while (getline(file, line)) {
        this->parseLine(line);
        rows++;
        cols = line.size();
      }
      file.close();
    }
  }

  void parseLine(const std::string &line) {

    std::stringstream ss;
    std::pair<std::vector<Coordinate>, std::uint32_t> number;
    auto &[numCoords, num] = number;

    for (std::uint32_t i = 0; i < line.size(); ++i) {
      const auto &ch = line[i];
      const Coordinate coord({rows, i});

      // populate numbers
      if (std::isdigit(ch)) {
        numCoords.push_back(coord);
        ss << ch;
      } else if (ss.str().length() > 0) {
        num = stoi(ss.str());
        m_numbers.push_back(number);

        // clear
        numCoords.clear();
        ss.str(std::string());
      }

      if (isSymbol(ch)) {
        m_symbols.push_back(coord);

        if (isGear(ch)) {
          m_gears.push_back(coord);
        }
      }
    }

    // case: number at eol
    if (ss.str().length() > 0) {
      num = stoi(ss.str());
      m_numbers.push_back(number);
    }
  }

  static bool isSymbol(std::int8_t element) {
    return !(isalnum(element) || element == '.');
  }
  static bool isGear(std::int8_t element) { return element == '*'; }

  std::uint32_t rows = 0;
  std::uint32_t cols = 0;

  std::vector<std::pair<std::vector<Coordinate>, std::uint32_t>> m_numbers;
  std::vector<Coordinate> m_gears;
  std::vector<Coordinate> m_symbols;
};

std::uint64_t getSumOfMechanicalComponents(const Grid &grid) {

  std::uint64_t sum = 0;

  for (const auto &[numberCoords, number] : grid.m_numbers) {
    bool hitSymbol = false;
    for (const auto &numberCoord : numberCoords) {
      for (const auto &symbolCoords : grid.m_symbols) {
        if (numberCoord.checkNeighbourhood(symbolCoords)) {
          sum += number;
          hitSymbol = true;
          break;
        }
      }
      if (hitSymbol) {
        break;
      }
    }
  }
  return sum;
}

std::uint64_t getGearRatio(const Grid &grid) {
  std::uint64_t sum = 0;
  for (const auto &gearCoords : grid.m_gears) {
    std::set<std::uint32_t> factors;
    for (const auto &[numberCoords, number] : grid.m_numbers) {
      for (const auto &numberCoord : numberCoords) {
        if (gearCoords.checkNeighbourhood(numberCoord)) {
          factors.insert(number);
        }
      }
    }
    if (factors.size() > 1) {
      sum += std::accumulate(factors.begin(), factors.end(), 1,
                             std::multiplies<int>());
    }
  }
  return sum;
}

int main() {

  Grid grid("input");
  const auto sum_p1 = getSumOfMechanicalComponents(grid);
  const auto sum_p2 = getGearRatio(grid);
  std::cout << "Sum of mechanical parts: " << sum_p1 << "\n";
  std::cout << "Sum of gear ratio: " << sum_p2 << "\n";
}
