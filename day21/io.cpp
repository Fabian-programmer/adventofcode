#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

enum Color {
  WHITE = 1,
  BLACK = 0,
};

struct Coordinate {
public:
  Coordinate(std::uint32_t x, std::uint32_t y) : x(x), y(y) {}

  std::int32_t x;
  std::int32_t y;

  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }
  bool operator<(const Coordinate &other) const {
    if (x != other.x) {
      return x < other.x;
    }
    return y < other.y;
  }

  std::uint32_t manhattanDistance(const Coordinate &other) {
    return std::abs(static_cast<std::int32_t>(this->x) - static_cast<std::int32_t>(other.x)) +
           std::abs(static_cast<std::int32_t>(this->y) - static_cast<std::int32_t>(other.y));
  }

  friend std::ostream &operator<<(std::ostream &os, const Coordinate &coord) {
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
  }

  Coordinate getEast() const {
    return Coordinate(this->x + 1, this->y);
  }
  Coordinate getSouth() const {
    return Coordinate(this->x, this->y + 1);
  }
  Coordinate getWest() const {
    return Coordinate(this->x - 1, this->y);
  }
  Coordinate getNorth() const {
    return Coordinate(this->x, this->y - 1);
  }
};

class Grid {
public:
  Grid(std::string input) {

    std::ifstream file(input);
    if (file.is_open()) {
      std::string line;
      while (getline(file, line)) {

        for (std::size_t i = 0; i < line.size(); ++i) {
          const auto &ch = line[i];
          grid.push_back(ch);

          if (ch == 'S') {
            start = Coordinate(i, rows);
          }
        }

        rows++;
        cols = line.size();
      }
      file.close();
    }
  }

  std::uint8_t &at(Coordinate coord) {
    static std::uint8_t outOfBoundsValue = '#';
    if (coord.x < 0 || coord.x >= cols) {
      return outOfBoundsValue;
    }
    if (coord.y < 0 || coord.y >= rows) {
      return outOfBoundsValue;
    }

    return grid[coord.x + coord.y * cols];
  }

  void setOpposite(Coordinate coord) {
    if (this->at(coord) == 'O') {
      this->at(coord) = '.';
    } else if (this->at(coord) == '.') {
      this->at(coord) = 'O';
    }
  }

  bool isRock(Coordinate coord) {
    if (this->at(coord) == '#') {
      return true;
    }
    return false;
  }

  void displayGrid() {
    for (std::int32_t y = 0; y < rows; ++y) {
      for (std::int32_t x = 0; x < cols; ++x) {
        if (x % 65 == 0) {
          std::cout << BLUE;
        }
        if (y % 65 == 0) {
          std::cout << RED;
        }
        std::cout << this->at(Coordinate(x, y)) << RESET;
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  Coordinate start{0, 0};
  std::int32_t rows = 0;
  std::int32_t cols = 0;

  std::vector<std::uint8_t> grid;
};

std::int32_t countGardenPlots(const std::vector<std::uint8_t> &input) {
  std::size_t count = 0;

  for (const auto &c : input) {
    if (c == 'O') {
      count++;
    }
  }

  return count;
}

std::int32_t calculateSteps(Grid grid, Coordinate startPos, std::uint32_t steps, Color color, bool print = false) {

  std::queue<Coordinate> nextSteps;
  std::queue<Coordinate> currentSteps;
  currentSteps.push(startPos);

  for (std::size_t step = 0; step <= steps; step++) {
    std::set<Coordinate> visited;
    while (!currentSteps.empty()) {
      const auto currentPosition = currentSteps.front();

      if (visited.find(currentPosition) != visited.end()) {
        currentSteps.pop();
        continue;
      }

      const auto north = currentPosition.getNorth();
      const auto south = currentPosition.getSouth();
      const auto west = currentPosition.getWest();
      const auto east = currentPosition.getEast();

      if (!grid.isRock(north)) {
        nextSteps.push(north);
      }
      if (!grid.isRock(south)) {
        nextSteps.push(south);
      }
      if (!grid.isRock(west)) {
        nextSteps.push(west);
      }
      if (!grid.isRock(east)) {
        nextSteps.push(east);
      }

      if (grid.start.manhattanDistance(currentPosition) % 2 == color) {
        grid.at(currentPosition) = 'O';
      }

      visited.insert(currentPosition);
      currentSteps.pop();
    }

    currentSteps = nextSteps;
  }

  if (print) {
    grid.displayGrid();
  }
  return countGardenPlots(grid.grid);
}

int main() {

  Grid grid("input");

  std::cout << "Garden plots for 64 steps: " << calculateSteps(grid, grid.start, 64, Color::BLACK) << "\n";

  const std::uint64_t elvesSteps = 26501365;
  std::cout << "Rows: " << grid.rows << " Cols: " << grid.cols << "\n";
  std::cout << "Middle: " << grid.start << "\n";
  std::cout << "Elves steps: " << elvesSteps << "\n";
  const std::uint64_t divisor = elvesSteps / grid.rows;
  std::cout << "Grids: " << divisor << "\n";
  std::cout << "Initial steps: " << elvesSteps % grid.rows << "\n";

  const std::uint32_t half = (grid.rows - 1) / 2;
  const std::uint32_t full = (grid.rows - 1);

  //// FULL
  const auto fullWhite = calculateSteps(grid, grid.start, full, Color::WHITE);
  const auto fullBlack = calculateSteps(grid, grid.start, full, Color::BLACK);

  ///// ONE DIRECTION
  const auto N = calculateSteps(grid, {half, full}, full, Color::WHITE);
  const auto S = calculateSteps(grid, {half, 0}, full, Color::WHITE);
  const auto W = calculateSteps(grid, {full, half}, full, Color::WHITE);
  const auto E = calculateSteps(grid, {0, half}, full, Color::WHITE);

  ////// OUTER
  const auto NE_o = calculateSteps(grid, {0, 0}, half, Color::BLACK);
  const auto NW_o = calculateSteps(grid, {full, 0}, half, Color::BLACK);
  const auto SE_o = calculateSteps(grid, {0, full}, half, Color::BLACK);
  const auto SW_o = calculateSteps(grid, {full, full}, half, Color::BLACK);

  ////// INNER
  const auto NE_i = calculateSteps(grid, {0, 0}, half + full, Color::WHITE);
  const auto NW_i = calculateSteps(grid, {full, 0}, half + full, Color::WHITE);
  const auto SE_i = calculateSteps(grid, {0, full}, half + full, Color::WHITE);
  const auto SW_i = calculateSteps(grid, {full, full}, half + full, Color::WHITE);

  std::uint64_t sum_white = ((divisor - 1) * (divisor - 1)) * fullWhite;
  std::uint64_t sum_black = (divisor * divisor) * fullBlack;

  std::uint64_t sum_corners = N + S + W + E;

  std::uint64_t sum_outer_black = divisor * (NE_o + NW_o + SE_o + SW_o);
  std::uint64_t sum_inner_white = (divisor - 1) * (NE_i + NW_i + SE_i + SW_i);

  std::cout << "Final: " << sum_white + sum_black + sum_corners + sum_outer_black + sum_inner_white << "\n";

  return 0;
}
