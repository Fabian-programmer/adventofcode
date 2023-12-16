#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

enum Direction {
  UNKNOWN = -1,
  NORTH = 0,
  EAST = 1,
  SOUTH = 2,
  WEST = 3
};

struct Coordinate {
public:
  Coordinate(std::uint32_t x, std::uint32_t y, Direction dir) : x(x), y(y), direction(dir) {}
  Coordinate(std::uint32_t x, std::uint32_t y) : x(x), y(y) {}

  std::int32_t x;
  std::int32_t y;
  Direction direction;

  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y && direction == other.direction;
  }
  bool operator<(const Coordinate &other) const {
    if (x != other.x) {
      return x < other.x;
    }
    if (y != other.y) {
      return y < other.y;
    }
    return direction < other.direction;
  }
  friend std::ostream &operator<<(std::ostream &os, const Coordinate &coord) {
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
  }

  Coordinate getEast() {
    return Coordinate(this->x + 1, this->y, EAST);
  }
  Coordinate getSouth() {
    return Coordinate(this->x, this->y + 1, SOUTH);
  }
  Coordinate getWest() {
    return Coordinate(this->x - 1, this->y, WEST);
  }
  Coordinate getNorth() {
    return Coordinate(this->x, this->y - 1, NORTH);
  }
};

Coordinate tiltEast(Coordinate current) {
  return Coordinate(current.x + 1, current.y, EAST);
}
Coordinate tiltSouth(Coordinate current) {
  return Coordinate(current.x, current.y + 1, SOUTH);
}
Coordinate tiltWest(Coordinate current) {
  return Coordinate(current.x - 1, current.y, WEST);
}
Coordinate tiltNorth(Coordinate current) {
  return Coordinate(current.x, current.y - 1, NORTH);
}

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
        }

        rows++;
        cols = line.size();
      }
      file.close();
    }
    gridVisited.resize(grid.size(), '.');
  }

  std::vector<Coordinate> getNextBeams(Coordinate coord) {
    std::vector<Coordinate> nextBeams;

    if (coord.direction == EAST) {
      const auto &element = this->at(coord);
      if (element == '.') {
        nextBeams.push_back(coord.getEast());
      }
      if (element == '\\') {
        nextBeams.push_back(tiltSouth(coord));
      }
      if (element == '/') {
        nextBeams.push_back(tiltNorth(coord));
      }
      if (element == '-') {
        nextBeams.push_back(coord.getEast());
      }
      if (element == '|') {
        nextBeams.push_back(tiltNorth(coord));
        nextBeams.push_back(tiltSouth(coord));
      }
    }
    if (coord.direction == SOUTH) {
      const auto &element = this->at(coord);
      if (element == '.') {
        nextBeams.push_back(coord.getSouth());
      }
      if (element == '\\') {
        nextBeams.push_back(tiltEast(coord));
      }
      if (element == '/') {
        nextBeams.push_back(tiltWest(coord));
      }
      if (element == '-') {
        nextBeams.push_back(tiltEast(coord));
        nextBeams.push_back(tiltWest(coord));
      }
      if (element == '|') {
        nextBeams.push_back(coord.getSouth());
      }
    }
    if (coord.direction == WEST) {
      const auto &element = this->at(coord);
      if (element == '.') {
        nextBeams.push_back(coord.getWest());
      }
      if (element == '\\') {
        nextBeams.push_back(tiltNorth(coord));
      }
      if (element == '/') {
        nextBeams.push_back(tiltSouth(coord));
      }
      if (element == '-') {
        nextBeams.push_back(coord.getWest());
      }
      if (element == '|') {
        nextBeams.push_back(tiltSouth(coord));
        nextBeams.push_back(tiltNorth(coord));
      }
    }
    if (coord.direction == NORTH) {
      const auto &element = this->at(coord);
      if (element == '.') {
        nextBeams.push_back(coord.getNorth());
      }
      if (element == '\\') {
        nextBeams.push_back(tiltWest(coord));
      }
      if (element == '/') {
        nextBeams.push_back(tiltEast(coord));
      }
      if (element == '-') {
        nextBeams.push_back(tiltEast(coord));
        nextBeams.push_back(tiltWest(coord));
      }
      if (element == '|') {
        nextBeams.push_back(coord.getNorth());
      }
    }

    return nextBeams;
  }

  void followBeam(Coordinate coord) {
    std::fill(gridVisited.begin(), gridVisited.end(), '.');
    visited.clear();

    visited.insert(coord);
    activeBeams.push(coord);
    this->setVisited(coord, '#');

    while (!activeBeams.empty()) {
      const auto beam = activeBeams.front();
      activeBeams.pop();

      // std::cout << "current beam: " << beam << " " << beam.direction << "\n";
      const auto nextBeams = getNextBeams(beam);

      for (const auto &nextBeam : nextBeams) {
        if (this->at(nextBeam) == '?') {
          continue;
        }

        // std::cout << "next beam: " << nextBeam << " " << nextBeam.direction << "\n";
        if (visited.count(nextBeam) == 0) {
          if (this->at(nextBeam) == '.') {
            visited.insert(nextBeam);
          }
          activeBeams.push(nextBeam);
          this->setVisited(nextBeam, '#');
          // this->displayVisitedGrid();
        }
      }
    }
  }

  std::int8_t at(Coordinate coord) {
    if (coord.x < 0 || coord.x >= cols) {
      return '?';
    }
    if (coord.y < 0 || coord.y >= rows) {
      return '?';
    }

    return grid[coord.x + coord.y * cols];
  }

  std::int8_t atVisited(Coordinate coord) {
    if (coord.x < 0 || coord.x >= cols) {
      return '?';
    }
    if (coord.y < 0 || coord.y >= rows) {
      return '?';
    }
    return gridVisited[coord.x + coord.y * cols];
  }

  void setVisited(Coordinate coord, std::int8_t ch) {
    if (coord.x < 0 || coord.x > cols) {
      return;
    }
    if (coord.y < 0 || coord.y > rows) {
      return;
    }
    // gridVisited[coord.x + coord.y * cols] = (char)(count % 10) + '0';
    // count++;
    gridVisited[coord.x + coord.y * cols] = '#';
  }

  void displayGrid() {
    for (std::int32_t y = 0; y < rows; ++y) {
      for (std::int32_t x = 0; x < cols; ++x) {
        std::cout << this->at(Coordinate(x, y));
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  void displayVisitedGrid() {
    for (std::int32_t y = 0; y < rows; ++y) {
      for (std::int32_t x = 0; x < cols; ++x) {
        std::cout << this->atVisited(Coordinate(x, y));
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  std::int32_t rows = 0;
  std::int32_t cols = 0;

  std::int32_t count = 0;

  std::vector<std::int8_t> grid;

  std::queue<Coordinate> activeBeams;
  std::set<Coordinate> visited;
  std::vector<std::int8_t> gridVisited;
};

std::int32_t countEnergized(const std::vector<std::int8_t> &input) {
  std::size_t count = 0;

  for (const auto &c : input) {
    if (c == '#') {
      count++;
    }
  }

  return count;
}

int main() {

  Grid grid("input");

  grid.followBeam(Coordinate(0, 0, EAST));
  std::cout << "energized tiles part 1: " << countEnergized(grid.gridVisited) << "\n";

  std::int32_t maxEnergize = 0;
  for (std::int32_t x = 0; x < grid.cols; x++) {
    grid.followBeam(Coordinate(x, 0, SOUTH));
    maxEnergize = std::max(maxEnergize, countEnergized(grid.gridVisited));
    grid.followBeam(Coordinate(x, grid.rows - 1, NORTH));
    maxEnergize = std::max(maxEnergize, countEnergized(grid.gridVisited));
  }
  for (std::int32_t y = 0; y < grid.rows; y++) {
    grid.followBeam(Coordinate(0, y, EAST));
    maxEnergize = std::max(maxEnergize, countEnergized(grid.gridVisited));
    grid.followBeam(Coordinate(grid.cols - 1, y, WEST));
    maxEnergize = std::max(maxEnergize, countEnergized(grid.gridVisited));
  }
  std::cout << "max energized tiles part 2: " << maxEnergize << "\n";

  return 0;
}
