#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_set>
#include <vector>

enum Direction {
  NORTH,
  WEST,
  EAST,
  SOUTH,
};

struct Coordinate {
  std::int32_t x;
  std::int32_t y;
  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }

  friend std::ostream &operator<<(std::ostream &os, const Coordinate &node) {
    os << "(" << node.x << ", " << node.y << ")";
    return os;
  }
};

struct Node {
public:
  Node(std::int32_t x, std::int32_t y) : pos({x, y}) {}
  Node(std::int32_t x, std::int32_t y, Direction dir)
      : pos({x, y}), direction(dir) {}

  Direction direction;
  Coordinate pos;
  std::int32_t moves;
  std::int32_t heatloss;

  bool operator==(const Node &other) const {
    return pos == other.pos && moves == other.moves &&
           direction == other.direction;
  }

  Node goEast() const { return Node(this->pos.x + 1, this->pos.y, EAST); }
  Node goSouth() const { return Node(this->pos.x, this->pos.y + 1, SOUTH); }
  Node goWest() const { return Node(this->pos.x - 1, this->pos.y, WEST); }
  Node goNorth() const { return Node(this->pos.x, this->pos.y - 1, NORTH); }

  Node goStraight() const {

    if (this->direction == NORTH) {
      return goNorth();
    }
    if (this->direction == SOUTH) {
      return goSouth();
    }
    if (this->direction == EAST) {
      return goEast();
    }
    return goWest();
  }

  Node goLeft() const {

    if (this->direction == NORTH) {
      return goWest();
    }
    if (this->direction == SOUTH) {
      return goEast();
    }
    if (this->direction == EAST) {
      return goNorth();
    }
    return goSouth();
  }

  Node goRight() const {

    if (this->direction == NORTH) {
      return goEast();
    }
    if (this->direction == SOUTH) {
      return goWest();
    }
    if (this->direction == EAST) {
      return goSouth();
    }
    return goNorth();
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
          grid.push_back(line[i] - '0');
        }

        rows++;
        cols = line.size();
      }
      file.close();
    }
  }
  std::int32_t at(Coordinate coord) const {
    static auto outOfBoundsValue = std::numeric_limits<int>::max();
    if (coord.x < 0 || coord.x >= cols) {
      return outOfBoundsValue;
    }
    if (coord.y < 0 || coord.y >= rows) {
      return outOfBoundsValue;
    }

    return grid[coord.x + coord.y * cols];
  }

  std::int32_t &at(Coordinate coord) {
    static auto outOfBoundsValue = std::numeric_limits<int>::max();
    if (coord.x < 0 || coord.x >= cols) {
      return outOfBoundsValue;
    }
    if (coord.y < 0 || coord.y >= rows) {
      return outOfBoundsValue;
    }

    return grid[coord.x + coord.y * cols];
  }

  void displayGrid() {
    for (std::int32_t y = 0; y < rows; ++y) {
      for (std::int32_t x = 0; x < cols; ++x) {
        std::cout << this->at({x, y}) << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  std::int32_t rows = 0;
  std::int32_t cols = 0;

  std::vector<std::int32_t> grid;
};

struct CompareHeat {
  bool operator()(const Node &s1, const Node &s2) const {
    return s1.heatloss > s2.heatloss;
  }
};

struct Hasher {
  std::size_t operator()(const Node &node) const {
    return node.pos.x + node.pos.y;
  }
};

std::int32_t dijkstra(const Grid &grid, Node start, const Node &goal) {

  std::priority_queue<Node, std::vector<Node>, CompareHeat> pq;
  std::unordered_set<Node, Hasher> visited;

  start.direction = EAST;
  start.heatloss = 0;
  start.moves = 0;
  pq.push(start);

  start.direction = NORTH;
  pq.push(start);

  while (!pq.empty()) {
    Node current = pq.top();
    pq.pop();
    if (visited.find(current) != visited.end()) {
      continue;
    }
    visited.insert(current);

    if (current.pos == goal.pos) {
      return current.heatloss;
    }

    std::vector<Node> nextNodes;
    nextNodes.push_back(current.goRight());
    nextNodes.push_back(current.goLeft());

    if (current.moves < 2) {
      nextNodes.push_back(current.goStraight());
    }

    for (auto &next : nextNodes) {
      const auto heatloss = grid.at(next.pos);

      // out of bounds
      if (heatloss == std::numeric_limits<int>::max()) {
        continue;
      }

      next.heatloss = current.heatloss + heatloss;

      if (next.direction == current.direction) {
        next.moves = current.moves + 1;
      } else {
        next.moves = 0;
      }

      pq.push(next);
    }
  }

  return 0;
}

int main() {

  auto grid = Grid("input");

  std::cout << dijkstra(grid, {0, 0}, {grid.cols - 1, grid.rows - 1}) << "\n";

  return 0;
}
