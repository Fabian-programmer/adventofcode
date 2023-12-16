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

  std::int8_t &at(Coordinate coord) {
    return grid[coord.x + coord.y * cols];
  }

  const std::int8_t &at(Coordinate coord) const {
    return grid[coord.x + coord.y * cols];
  }

  static bool isBoulder(std::int8_t element) { return element == 'O'; }
  static bool isWall(std::int8_t element) { return element == '#'; }

  void removeBoulders() {
    std::replace(grid.begin(), grid.end(), 'O', '.');
  }

  void inverse() {
    std::reverse(grid.begin(), grid.end());
  }

  void displayGrid() {
    for (std::size_t y = 0; y < rows; ++y) {
      for (std::size_t x = 0; x < cols; ++x) {
        std::cout << this->at(Coordinate(x, y));
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  std::uint32_t rows = 0;
  std::uint32_t cols = 0;

  std::vector<std::int8_t> grid;
};

Grid tiltOnceCycle(Grid grid) {
  auto gridTilted = grid;
  gridTilted.removeBoulders();

  // tilt north
  {
    std::vector<std::int32_t> activeRowNorth(grid.cols, grid.rows);
    for (std::size_t y = 0; y < grid.rows; ++y) {
      for (std::size_t x = 0; x < grid.cols; ++x) {
        const auto &element = grid.at(Coordinate(x, y));
        if (grid.isBoulder(element)) {
          gridTilted.at(Coordinate(x, grid.rows - activeRowNorth[x])) = 'O';
          activeRowNorth[x] = std::max(activeRowNorth[x] - 1, 0);
        }
        if (grid.isWall(element)) {
          std::int32_t belowWall = grid.rows - y - 1;
          activeRowNorth[x] = std::max(belowWall, 0);
        }
      }
    }
  }

  grid = gridTilted;
  gridTilted.removeBoulders();

  // tilt west
  {
    std::vector<std::int32_t> activeColsWest(grid.rows, 0);
    for (std::size_t y = 0; y < grid.rows; ++y) {
      for (std::size_t x = 0; x < grid.cols; ++x) {
        const auto &element = grid.at(Coordinate(x, y));
        if (grid.isBoulder(element)) {
          gridTilted.at(Coordinate(activeColsWest[y], y)) = 'O';
          activeColsWest[y] = std::min(activeColsWest[y] + 1, (int)grid.cols);
        }
        if (grid.isWall(element)) {
          std::uint32_t rightOfWall = x + 1;
          activeColsWest[y] = std::min(rightOfWall, grid.cols);
        }
      }
    }
  }

  grid = gridTilted;
  gridTilted.removeBoulders();

  // tilt south
  {
    grid.inverse();
    gridTilted.inverse();
    std::vector<std::int32_t> activeRowNorth(grid.cols, grid.rows);
    for (std::size_t y = 0; y < grid.rows; ++y) {
      for (std::size_t x = 0; x < grid.cols; ++x) {
        const auto &element = grid.at(Coordinate(x, y));
        if (grid.isBoulder(element)) {
          gridTilted.at(Coordinate(x, grid.rows - activeRowNorth[x])) = 'O';
          activeRowNorth[x] = std::max(activeRowNorth[x] - 1, 0);
        }
        if (grid.isWall(element)) {
          std::int32_t belowWall = grid.rows - y - 1;
          activeRowNorth[x] = std::max(belowWall, 0);
        }
      }
    }
    gridTilted.inverse();
  }

  grid = gridTilted;
  gridTilted.removeBoulders();

  // tilt east
  {
    grid.inverse();
    gridTilted.inverse();
    std::vector<std::int32_t> activeColsWest(grid.rows, 0);
    for (std::size_t y = 0; y < grid.rows; ++y) {
      for (std::size_t x = 0; x < grid.cols; ++x) {
        const auto &element = grid.at(Coordinate(x, y));
        if (grid.isBoulder(element)) {
          gridTilted.at(Coordinate(activeColsWest[y], y)) = 'O';
          activeColsWest[y] = std::min(activeColsWest[y] + 1, (int)grid.cols);
        }
        if (grid.isWall(element)) {
          std::uint32_t rightOfWall = x + 1;
          activeColsWest[y] = std::min(rightOfWall, grid.cols);
        }
      }
    }
    gridTilted.inverse();
  }

  grid = gridTilted;
  return grid;
}

std::uint32_t processGrid_p2(const std::vector<std::string> &gridRows) {
  auto grid = Grid(gridRows);

  for (int i = 0; i < 10000; i++) {

    grid = tiltOnceCycle(grid);
    std::uint32_t value = 0;
    for (std::size_t y = 0; y < grid.rows; ++y) {
      for (std::size_t x = 0; x < grid.cols; ++x) {
        const auto &element = grid.at(Coordinate(x, y));
        if (grid.isBoulder(element)) {
          value += grid.rows - y;
        }
      }
    }
    std::cout << "Load after " << i << " cycle: " << value << "\n";
  }

  return 0;
}

std::uint32_t processGrid(const std::vector<std::string> &gridRows) {
  auto grid = Grid(gridRows);
  auto gridTilted = grid;
  gridTilted.removeBoulders();

  std::uint32_t value = 0;

  // tilt north
  {
    std::vector<std::int32_t> activeRowNorth(grid.cols, grid.rows);
    for (std::size_t y = 0; y < grid.rows; ++y) {
      for (std::size_t x = 0; x < grid.cols; ++x) {
        const auto &element = grid.at(Coordinate(x, y));
        if (grid.isBoulder(element)) {
          value += activeRowNorth[x];
          gridTilted.at(Coordinate(x, grid.rows - activeRowNorth[x])) = 'O';
          activeRowNorth[x] = std::max(activeRowNorth[x] - 1, 0);
        }
        if (grid.isWall(element)) {
          std::int32_t belowWall = grid.rows - y - 1;
          activeRowNorth[x] = std::max(belowWall, 0);
        }
      }
    }
  }

  return value;
}

int main() {

  std::ifstream file("input");
  std::uint32_t sum_p1 = 0;
  std::uint32_t sum_p2 = 0;

  std::vector<std::string> currentGrid;
  std::string line;

  while (std::getline(file, line)) {
    currentGrid.push_back(line);
  }

  // Process the last grid if not empty
  if (!currentGrid.empty()) {
    sum_p1 += processGrid(currentGrid);
    processGrid_p2(currentGrid);

    // cheating here. too lazy to find the start of the graph.
    // cycle 1000 value 83502
    // cycle 1001 value 83489
    // cycle 1002 value 83488
    // cycle 1003 value 83482
    // cycle 1004 value 83477
    // cycle 1005 value 83473
    // cycle 1006 value 83484
    // cycle 1007 value 83491
    // cycle 1008 value 83507
    // cycle 1009 value 83516
    // cycle 1010 value 83516
    //
    // cycle 1000000000 mod 11 (size of graph) == 10 (10th value in cylic graph)
  }

  file.close();

  std::cout << sum_p1 << "\n";
  std::cout << sum_p2 << "\n";

  return 0;
}
