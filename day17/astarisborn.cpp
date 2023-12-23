#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <vector>

struct Node {
public:
  Node(std::uint64_t x, std::uint64_t y) : x(x), y(y) {}

  std::int64_t x;
  std::int64_t y;
  std::int32_t cost;

  bool operator>(const Node &other) const { return cost > other.cost; }

  bool operator==(const Node &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Node &other) const {
    return x != other.x || y != other.y;
  }

  Node goRight() const { return Node(this->x + 1, this->y); }
  Node goDown() const { return Node(this->x, this->y + 1); }
  Node goLeft() const { return Node(this->x - 1, this->y); }
  Node goUp() const { return Node(this->x, this->y - 1); }
};

struct CompareNodes {
  bool operator()(const Node &lhs, const Node &rhs) const {
    return lhs.cost > rhs.cost; // or any other comparison logic
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
  std::int32_t at(Node coord) const {
    static auto outOfBoundsValue = std::numeric_limits<int>::max();
    if (coord.x < 0 || coord.x >= cols) {
      return outOfBoundsValue;
    }
    if (coord.y < 0 || coord.y >= rows) {
      return outOfBoundsValue;
    }

    return grid[coord.x + coord.y * cols];
  }

  std::int32_t &at(Node coord) {
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
        std::cout << this->at(Node(x, y)) << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  std::int32_t rows = 0;
  std::int32_t cols = 0;

  std::vector<std::int32_t> grid;
};

void dijkstra(const Grid &grid, const Node &start) {

  auto distance = grid;
  std::fill(distance.grid.begin(), distance.grid.end(),
            std::numeric_limits<int>::max());

  std::set<Node, CompareNodes> visited;

  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

  distance.at(start) = grid.at(start);
  pq.push(start);

  while (!pq.empty()) {
    Node current = pq.top();
    pq.pop();

    visited.insert(current);

    std::vector<Node> nextNodes;
    nextNodes.push_back(current.goUp());
    nextNodes.push_back(current.goDown());
    nextNodes.push_back(current.goLeft());
    nextNodes.push_back(current.goRight());

    for (auto &next : nextNodes) {
      if (visited.count(next)) {
        continue;
      }
      const auto costs = grid.at(next);

      // out of bounds
      if (costs == std::numeric_limits<int>::max()) {
        continue;
      }

      const auto newCosts = costs + distance.at(current);

      if (distance.at(next) > newCosts) {
        distance.at(next) = newCosts;
        next.cost = newCosts;
        pq.push(next);
      }
    }
  }

  distance.displayGrid();
}

int main() {

  auto grid = Grid("input");
  grid.displayGrid();
  std::cout << "\n";

  dijkstra(grid, {0, 0});

  // int goalRow = 12, goalCol = 12;

  return 0;
}
