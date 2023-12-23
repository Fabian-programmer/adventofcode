#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

constexpr int floor = 1;
constexpr int firstBrick = floor + 1;

template <typename T>
void push_unique(std::vector<T> &vec, const T &value) {
  if (std::find(vec.begin(), vec.end(), value) == vec.end()) {
    vec.push_back(value);
  }
}

class TreeNode {
public:
  int id;
  std::vector<TreeNode *> children;
  std::vector<TreeNode *> parent;

  // Constructor
  TreeNode(int nodeId) : id(nodeId) {}
};

class Tree {
public:
  TreeNode *root;
  std::unordered_map<int, TreeNode *> idToNodeMap;

  Tree(int rootId) {
    root = new TreeNode(rootId);
    idToNodeMap[rootId] = root;
  }

  void addChild(int parentId, int childId) {

    TreeNode *parentNode = idToNodeMap[parentId];
    TreeNode *childNode;

    if (idToNodeMap.find(childId) == idToNodeMap.end()) {
      childNode = new TreeNode(childId);
    } else {
      childNode = idToNodeMap[childId];
    }

    parentNode->children.push_back(childNode);
    childNode->parent.push_back(parentNode);
    idToNodeMap[childId] = childNode;
  }

  std::vector<int> findParent(int targetId) {
    std::vector<int> parents;
    const auto parentNodes = idToNodeMap[targetId]->parent;

    for (const auto &node : parentNodes) {
      parents.push_back(node->id);
    }

    return parents;
  }

  std::vector<int> findChildren(int targetId) {
    std::vector<int> children;
    const auto childrenNodes = idToNodeMap[targetId]->children;

    for (const auto &node : childrenNodes) {
      children.push_back(node->id);
    }

    return children;
  }

  std::vector<int> findAllChildrenAndStopAtFork(const int parentId) {
    std::vector<int> childrenIds;
    std::set<int> visited;
    std::queue<TreeNode *> q;

    for (const auto &children : idToNodeMap[parentId]->children) {
      q.push(children);
    }
    visited.insert(parentId);

    while (!q.empty()) {
      TreeNode *current = q.front();
      q.pop();

      const auto parents = findParent(current->id);

      bool hasFork = false;
      for (const auto &parent : parents) {
        // parent which hasnt been visited, is another branch.
        if (visited.count(parent) == 0) {
          hasFork = true;
          break;
        }
      }

      if (!hasFork) {
        // Add the ID of the current node to the result set
        push_unique(childrenIds, current->id);

        // Enqueue all children of the current node
        for (TreeNode *child : current->children) {
          q.push(child);
        }
        visited.insert(current->id);
      }
    }

    return childrenIds;
  }
};

struct Coordinate {
  int x;
  int y;
  int z;
};

std::ostream &operator<<(std::ostream &os, const Coordinate &coord) {
  os << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")";
  return os;
}

struct Brick {
  Coordinate start;
  Coordinate end;
  int id;
};

std::ostream &operator<<(std::ostream &os, const Brick &brick) {
  os << "Start: (" << brick.start.x << ", " << brick.start.y << ", " << brick.start.z << ") ";
  os << "End: (" << brick.end.x << ", " << brick.end.y << ", " << brick.end.z << ") ";
  os << "Value: " << brick.id;
  return os;
}

bool compareLowestZ(const Brick &brick1, const Brick &brick2) {
  int minZ1 = std::min(brick1.start.z, brick1.end.z);
  int minZ2 = std::min(brick2.start.z, brick2.end.z);
  return minZ1 < minZ2;
}

class Grid {
public:
  Grid(int sizeX, int sizeY, int sizeZ) : sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ) {
    data.resize(sizeX, std::vector<std::vector<int>>(sizeY, std::vector<int>(sizeZ, 0)));
  }

  int &at(const Coordinate &coord) {
    return data[coord.x][coord.y][coord.z];
  }

  int at(const Coordinate &coord) const {
    return data[coord.x][coord.y][coord.z];
  }

  void setBrick(const Brick &brick) {

    const auto start = brick.start;
    const auto end = brick.end;

    for (int x = std::min(start.x, end.x); x <= std::max(start.x, end.x); ++x) {
      for (int y = std::min(start.y, end.y); y <= std::max(start.y, end.y); ++y) {
        for (int z = std::min(start.z, end.z); z <= std::max(start.z, end.z); ++z) {
          this->at({x, y, z}) = brick.id;
        }
      }
    }
  }

  std::vector<int> whatsOverTheBrick(const Brick &brick) {

    std::set<int> overIt;

    Coordinate start = brick.start;
    Coordinate end = brick.end;

    for (int x = std::min(start.x, end.x); x <= std::max(start.x, end.x); ++x) {
      for (int y = std::min(start.y, end.y); y <= std::max(start.y, end.y); ++y) {
        const auto z = std::max(start.z, end.z);
        const auto over = this->at({x, y, z + 1});
        if (over != 0) {
          overIt.insert(over);
        }
      }
    }

    return {overIt.begin(), overIt.end()};
  }

  int findHighestZBelowBrick(const Brick &brick) const {
    int highestZ = -1;

    for (int x = std::min(brick.start.x, brick.end.x); x <= std::max(brick.start.x, brick.end.x); ++x) {
      for (int y = std::min(brick.start.y, brick.end.y); y <= std::max(brick.start.y, brick.end.y); ++y) {
        for (int z = 0; z < sizeZ; ++z) {
          if (this->at({x, y, z}) != 0) {
            highestZ = std::max(z, highestZ);
          }
        }
      }
    }

    return highestZ;
  }

private:
  int sizeX;
  int sizeY;
  int sizeZ;
  std::vector<std::vector<std::vector<int>>> data;
};

int main() {

  std::string line;
  std::ifstream file("input");

  std::vector<Brick> bricks;
  std::vector<Brick> fallenBricks;

  int brickId = firstBrick;
  // Initialize modules
  if (file.is_open()) {
    while (getline(file, line)) {
      std::stringstream ss(line);

      // Temporary variables to store parsed values
      int x1, y1, z1, x2, y2, z2;
      char tilde;

      // Read the start coordinates
      ss >> x1;
      ss.ignore(); // Ignore the comma
      ss >> y1;
      ss.ignore(); // Ignore the comma
      ss >> z1;

      // Read the tilde character
      ss >> tilde;

      // Read the end coordinates
      ss >> x2;
      ss.ignore(); // Ignore the comma
      ss >> y2;
      ss.ignore(); // Ignore the comma
      ss >> z2;

      Brick brick = {{x1, y1, z1}, {x2, y2, z2}, brickId};
      bricks.push_back(brick);
      brickId++;
    }
  }

  Grid grid(11, 11, 350);
  Brick floorBrick{{0, 0, 0}, {10, 10, 0}, floor};
  bricks.push_back(floorBrick);

  std::sort(bricks.begin(), bricks.end(), compareLowestZ);

  for (const auto &brick : bricks) {
    const auto z = grid.findHighestZBelowBrick(brick) + 1;
    const auto brickHeight = std::abs(brick.end.z - brick.start.z);
    Brick fallDown{{brick.start.x, brick.start.y, z}, {brick.end.x, brick.end.y, z + brickHeight}, brick.id};
    grid.setBrick(fallDown);

    fallenBricks.push_back(fallDown);
  }

  auto tree = Tree(floor);

  for (const auto fallenBrick : fallenBricks) {
    const auto bricksOver = grid.whatsOverTheBrick(fallenBrick);
    for (const auto over : bricksOver) {
      tree.addChild(fallenBrick.id, over);
    }
  }

  std::set<int> removeBricks;
  std::set<int> keepBricks;
  std::set<int> maybeRemoveBricks;

  for (auto brick = firstBrick; brick < brickId; brick++) {
    const auto parents = tree.findParent(brick);
    const auto children = tree.findChildren(brick);

    // nothing above. Safe to remove
    if (children.empty()) {
      removeBricks.insert(brick);
    }

    // only one under. Have to be kept
    if (parents.size() == 1) {
      for (int item : parents) {
        if (item == floor) {
          continue;
        }
        keepBricks.insert(item);
      }
    }

    // two under. Maybe can be removed. If it doesn't have to be kept
    if (parents.size() >= 2) {
      for (int item : parents) {
        maybeRemoveBricks.insert(item);
      }
      continue;
    }
  }

  for (const auto brickId : maybeRemoveBricks) {
    if (keepBricks.find(brickId) == keepBricks.end()) {
      removeBricks.insert(brickId);
    }
  }

  int brickWillFall = 0;
  for (auto brickId : keepBricks) {
    brickWillFall += tree.findAllChildrenAndStopAtFork(brickId).size();
  }

  std::cout << "Removable Bricks: " << removeBricks.size() << "\n";
  std::cout << "Bricks will fall: " << brickWillFall << "\n";
}
