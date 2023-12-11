#include <fstream>
#include <iostream>
#include <vector>

struct Coordinate {
public:
  Coordinate(std::uint32_t x, std::uint32_t y) : x(x), y(y) {}

  std::uint32_t x;
  std::uint32_t y;

  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }
  bool operator<(const Coordinate &other) const {
    return x < other.x || (x == other.x && y < other.y);
  }
  friend std::ostream &operator<<(std::ostream &os, const Coordinate &coord) {
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
  }

  std::uint32_t manhattanDistance(const Coordinate &other) {
    return std::abs(static_cast<std::int32_t>(this->x) - static_cast<std::int32_t>(other.x)) +
           std::abs(static_cast<std::int32_t>(this->y) - static_cast<std::int32_t>(other.y));
  }
};

class Grid {
public:
  Grid(std::string input, std::uint32_t galaxyAge = 1) {

    if (galaxyAge != 1) {
      galaxyAge--;
    }

    std::ifstream file(input);
    if (file.is_open()) {
      std::string line;
      std::vector<bool> isColEmpty(1000, true);
      while (getline(file, line)) {
        bool isRowEmpty = true;

        for (std::size_t i = 0; i < line.size(); ++i) {
          const auto &ch = line[i];

          if (isGalaxy(ch)) {
            m_galaxyExpanded.push_back(Coordinate(i, rowIdx + galaxyAge * m_emptyRows.size()));
            isRowEmpty = false;
            isColEmpty[i] = false;
          }
        }

        if (isRowEmpty) {
          m_emptyRows.push_back(rowIdx);
        }
        rowIdx++;
        cols = line.size();
      }

      isColEmpty.resize(cols);

      for (auto &galaxy : m_galaxyExpanded) {
        std::int32_t moveX = 0;
        for (std::size_t i = 0; i < galaxy.x; ++i) {
          if (isColEmpty[i]) {
            moveX++;
          }
        }
        galaxy.x += galaxyAge * moveX;
      }

      file.close();
    }
  }

  static bool isGalaxy(std::int8_t element) { return element == '#'; }

  std::uint32_t rowIdx = 0;
  std::uint32_t cols = 0;

  std::vector<Coordinate> m_galaxyExpanded;

  std::vector<std::uint32_t> m_emptyRows;
};

int main() {

  Grid grid("input");

  std::int64_t smallestDistance_p1 = 0;
  for (std::size_t i = 0; i < grid.m_galaxyExpanded.size(); ++i) {
    for (std::size_t j = i + 1; j < grid.m_galaxyExpanded.size(); ++j) {
      smallestDistance_p1 += grid.m_galaxyExpanded[i].manhattanDistance(grid.m_galaxyExpanded[j]);
    }
  }

  grid = Grid("input", 1000000);
  std::int64_t smallestDistance_p2 = 0;
  for (std::size_t i = 0; i < grid.m_galaxyExpanded.size(); ++i) {
    for (std::size_t j = i + 1; j < grid.m_galaxyExpanded.size(); ++j) {
      smallestDistance_p2 += grid.m_galaxyExpanded[i].manhattanDistance(grid.m_galaxyExpanded[j]);
    }
  }

  std::cout << "Smallest Distance: " << smallestDistance_p1 << "\n";
  std::cout << "Smallest Distance: " << smallestDistance_p2 << "\n";

  return 0;
}
