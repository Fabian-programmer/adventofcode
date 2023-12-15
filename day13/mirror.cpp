#include <algorithm>
#include <cstring>
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
  Grid(const std::vector<std::string> &input) {
    rows = static_cast<std::uint32_t>(input.size());
    cols = static_cast<std::uint32_t>(input[0].size());

    for (const auto &row : input) {
      for (const auto &c : row) {
        grid.push_back(c);
      }
    }
  }

  std::int8_t at(Coordinate coord) {
    if (coord.x < 0 || coord.x > cols) {
      return '.';
    }
    if (coord.y < 0 || coord.y > rows) {
      return '.';
    }

    return grid[coord.x + coord.y * cols];
  }

  std::vector<std::int8_t> getRow(std::uint32_t rowNumber) const {
    // Calculate the starting index of the specified row
    std::size_t startIndex = rowNumber * cols;

    // Extract the row using vector's const_iterator
    auto rowBegin = grid.cbegin() + startIndex;
    auto rowEnd = rowBegin + cols;

    // Create a new vector with the const row's values
    std::vector<std::int8_t> row(rowBegin, rowEnd);

    return row;
  }

  void transpose() {
    std::vector<std::int8_t> transposedGrid;

    for (std::uint32_t col = 0; col < cols; ++col) {
      for (std::uint32_t row = 0; row < rows; ++row) {
        transposedGrid.push_back(grid[row * cols + col]);
      }
    }

    // Update rows, cols, and grid with the transposed values
    std::swap(rows, cols);
    std::swap(grid, transposedGrid);
  }

  std::uint32_t rows = 0;
  std::uint32_t cols = 0;

  std::vector<std::int8_t> grid;
};

bool compareRows(const std::vector<std::int8_t> &row1, const std::vector<std::int8_t> &row2) {
  return std::memcmp(row1.data(), row2.data(), row1.size() * sizeof(std::int8_t)) == 0;
}
std::size_t countDifferentObjects(const std::vector<std::int8_t> &row1, const std::vector<std::int8_t> &row2) {
  std::size_t count = 0;

  for (std::size_t i = 0; i < row1.size(); ++i) {
    if (row1[i] != row2[i]) {
      ++count;
    }
  }

  return count;
}

std::uint32_t getNumber(const Grid &grid, const bool searchForSmudges) {

  std::uint32_t num = 0;

  for (std::size_t y = 0; y < grid.rows - 1; ++y) {
    bool validMirror = true;
    std::size_t smudges = 0;
    const auto currentRow = grid.getRow(y);
    const auto nextRow = grid.getRow(y + 1);

    smudges += countDifferentObjects(currentRow, nextRow);

    bool isSame = false;
    if (!searchForSmudges) {
      if (smudges == 0) {
        isSame = true;
      }
    } else {
      if (smudges == 0 || smudges == 1) {
        isSame = true;
      }
    }

    if (isSame) {
      const auto leftMirror = y;
      const auto rightMirror = y + 1;
      const std::int32_t minRange = std::min(leftMirror - 0, grid.rows - 1 - rightMirror);

      std::size_t distance = 1;

      for (std::int32_t y2 = 0; y2 < minRange; y2++) {
        const auto mirrowRow1 = grid.getRow(leftMirror - distance);
        const auto mirrorRow2 = grid.getRow(rightMirror + distance);

        if (!searchForSmudges) {
          const auto isMirrorSame = compareRows(mirrowRow1, mirrorRow2);

          if (!isMirrorSame) {
            validMirror = false;
            break;
          }
        }

        if (searchForSmudges) {
          smudges += countDifferentObjects(mirrowRow1, mirrorRow2);
          if (smudges > 1) {
            validMirror = false;
            break;
          }
        }

        distance++;
      }

      if (!searchForSmudges) {
        if (validMirror) {
          return y + 1;
        }
      } else {
        if (validMirror && smudges == 1) {
          return y + 1;
        }
      }
    }
  }

  return num;
}

int num = 0;
std::uint32_t processGrid(const std::vector<std::string> &gridRows, const bool smudge = false) {
  auto grid = Grid(gridRows);

  std::uint32_t value = 0;
  value = 100 * getNumber(grid, smudge);

  if (value == 0) {
    grid.transpose();
    value = getNumber(grid, smudge);
  }

  num++;
  return value;
}

int main() {

  std::ifstream file("input");
  std::uint32_t sum_p1 = 0;
  std::uint32_t sum_p2 = 0;

  std::vector<std::string> currentGrid;
  std::string line;

  while (std::getline(file, line)) {
    if (line.empty()) {
      // Empty line indicates the end of the current grid
      sum_p1 += processGrid(currentGrid);
      sum_p2 += processGrid(currentGrid, true);
      currentGrid.clear();
    } else {
      // Non-empty line, add it to the current grid
      currentGrid.push_back(line);
    }
  }

  // Process the last grid if not empty
  if (!currentGrid.empty()) {
    sum_p1 += processGrid(currentGrid);
    sum_p2 += processGrid(currentGrid, true);
  }

  file.close();

  std::cout << "The sum of all mirror thinys is: " << sum_p1 << "\n";
  std::cout << "The sum of all clean mirror thinys is: " << sum_p2 << "\n";

  return 0;
}
