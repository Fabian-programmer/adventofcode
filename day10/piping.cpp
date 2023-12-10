#include <algorithm>
#include <fstream>
#include <iostream>
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
  Coordinate(std::uint32_t x, std::uint32_t y) : x(x), y(y) {}

  std::uint32_t x;
  std::uint32_t y;

  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }
  bool operator<(const Coordinate &other) const {
    return x < other.x || (x == other.x && y < other.y);
  }
};

bool push_unique(std::vector<Coordinate> &path, const Coordinate value) {
  if (std::find(path.begin(), path.end(), value) == path.end()) {
    path.push_back(value);
    return true;
  }
  return false;
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
          if (ch == 'S') {
            path.push_back(Coordinate(i, rows));
          }
        }

        rows++;
        cols = line.size();
      }
      file.close();
    }
  }

  bool addNextPipeElement(Coordinate coord) {
    const auto currentPipe = this->at(coord);
    if (currentPipe == '-' || currentPipe == '7' || currentPipe == 'J' || currentPipe == 'S') {
      const auto west = Coordinate(coord.x - 1, coord.y);
      const auto nextPipe = this->at(west);
      if (nextPipe == '-' || nextPipe == 'L' || nextPipe == 'F' || nextPipe == 'S') {
        if (push_unique(path, west)) {
          return true;
        }
      }
    }

    if (currentPipe == '|' || currentPipe == 'L' || currentPipe == 'J' || currentPipe == 'S') {
      const auto north = Coordinate(coord.x, coord.y - 1);
      const auto nextPipe = this->at(north);
      if (nextPipe == '|' || nextPipe == '7' || nextPipe == 'F' || nextPipe == 'S') {
        if (push_unique(path, north)) {
          return true;
        }
      }
    }

    if (currentPipe == '|' || currentPipe == '7' || currentPipe == 'F' || currentPipe == 'S') {
      const auto south = Coordinate(coord.x, coord.y + 1);
      const auto nextPipe = this->at(south);
      if (nextPipe == '|' || nextPipe == 'L' || nextPipe == 'J' || nextPipe == 'S') {
        if (push_unique(path, south)) {
          return true;
        }
      }
    }

    if (currentPipe == '-' || currentPipe == 'L' || currentPipe == 'F' || currentPipe == 'S') {
      const auto east = Coordinate(coord.x + 1, coord.y);
      const auto nextPipe = this->at(east);
      if (nextPipe == '-' || nextPipe == '7' || nextPipe == 'J' || nextPipe == 'S') {
        if (push_unique(path, east)) {
          return true;
        }
      }
    }

    return false;
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

  Direction getCurrentDirection(const Direction &fov, const Coordinate &position) {
    Direction direction = fov;
    if (direction == SOUTH && at(position) == 'L') {
      direction = EAST;
    }
    if (direction == SOUTH && at(position) == 'J') {
      direction = WEST;
    }
    if (direction == NORTH && at(position) == 'F') {
      direction = EAST;
    }
    if (direction == NORTH && at(position) == '7') {
      direction = WEST;
    }
    if (direction == EAST && at(position) == 'J') {
      direction = NORTH;
    }
    if (direction == EAST && at(position) == '7') {
      direction = SOUTH;
    }
    if (direction == WEST && at(position) == 'F') {
      direction = SOUTH;
    }
    if (direction == WEST && at(position) == 'L') {
      direction = NORTH;
    }
    return direction;
  }

  void addTiles(const Coordinate &position, std::int32_t orientation = 0) {
    const auto &currentPipe = this->at(position);

    currentDirection = getCurrentDirection(currentDirection, position);

    if ((currentDirection == SOUTH && currentPipe == '|') || (currentDirection == EAST && currentPipe == 'L') || (currentDirection == SOUTH && currentPipe == 'F')) {

      bool iterateTillPath = true;
      std::int32_t idx = 1;
      while (iterateTillPath) {
        const auto nextTile = Coordinate(position.x + idx * orientation, position.y);
        if (std::find(path.begin(), path.end(), nextTile) == path.end()) {
          tiles.insert(nextTile);
          idx++;
        } else {
          iterateTillPath = false;
        }
      }
    }
  }

  std::uint32_t rows = 0;
  std::uint32_t cols = 0;

  Coordinate startPosition{0, 0};
  std::vector<Coordinate> path;
  std::set<Coordinate> tiles;
  Direction currentDirection = Direction::SOUTH;

  std::vector<std::int8_t> grid;
};

int main() {

  Grid grid("input");

  bool finishedLoop = false;
  while (!finishedLoop) {
    const auto &lastElement = grid.path.back();
    finishedLoop = !grid.addNextPipeElement(lastElement);
  }

  // this is cheating, but i was too lazy to implement a function to find out orientation
  const auto orientation = -1;
  for (const auto &position : grid.path) {
    grid.addTiles(position, orientation);
  }

  const std::int64_t furthestDistance_p1 = grid.path.size() / 2;
  const std::int64_t tiles_p2 = grid.tiles.size();

  std::cout << "Furthest Distance: " << furthestDistance_p1 << "\n";
  std::cout << "Tiles : " << tiles_p2 << "\n";

  return 0;
}
