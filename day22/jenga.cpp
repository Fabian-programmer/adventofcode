#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

constexpr int floor = 1;
constexpr int firstId = 2;

class TreeNode {
public:
  int value;
  std::vector<TreeNode *> children;
  TreeNode *parent;

  // Constructor
  TreeNode(int val) : value(val) {}

  // Destructor to free memory recursively
  ~TreeNode() {
    for (TreeNode *child : children) {
      delete child;
    }
  }
};

class Tree {
public:
  TreeNode *root;

  // Constructor
  Tree(int rootValue) {
    root = new TreeNode(rootValue);
  }

  // Destructor to free memory
  ~Tree() {
    delete root;
  }

  // Function to add a child to a node
  void addChild(int parentValue, int childValue) {
    TreeNode *parentNode = findNode(root, parentValue);

    TreeNode *childNode = new TreeNode(childValue);
    parentNode->children.push_back(childNode);
    childNode->parent = parentNode;
  }

  std::vector<int> findParents(int targetValue) {
    std::vector<int> parents;
    std::queue<TreeNode *> q;
    q.push(root);

    while (!q.empty()) {
      TreeNode *current = q.front();
      q.pop();

      // Check if the current node has the target value as a child
      for (TreeNode *child : current->children) {
        if (child->value == targetValue) {
          parents.push_back(child->parent->value);
        }
        q.push(child);
      }
    }

    return parents;
  }

private:
  // Function to find a node with a specific value in the tree
  TreeNode *findNode(TreeNode *currentNode, int value) {
    if (!currentNode) {
      return nullptr;
    }

    if (currentNode->value == value) {
      return currentNode;
    }

    for (TreeNode *child : currentNode->children) {
      TreeNode *foundNode = findNode(child, value);
      if (foundNode) {
        return foundNode;
      }
    }

    return nullptr;
  }
};
int countChildren(Tree &tree, int targetValue) {
  int count = 0;

  // Use BFS to traverse the tree
  std::queue<TreeNode *> q;
  q.push(tree.root);

  while (!q.empty()) {
    TreeNode *current = q.front();
    q.pop();

    // Check if the current node has the target value
    if (current->value == targetValue) {
      count += current->children.size();
    }

    // Enqueue the children for further exploration
    for (TreeNode *child : current->children) {
      q.push(child);
    }
  }

  return count;
}
int countParents(Tree &tree, int targetValue) {
  int count = 0;

  // Use BFS to traverse the tree
  std::queue<TreeNode *> q;
  q.push(tree.root);

  while (!q.empty()) {
    TreeNode *current = q.front();
    q.pop();

    // Check if the current node has the target value
    if (current->value == targetValue) {
      count++;
    }

    // Enqueue the children for further exploration
    for (TreeNode *child : current->children) {
      q.push(child);
    }
  }

  return count;
}

void printTreeBFS(TreeNode *root) {
  if (!root) {
    return;
  }

  std::queue<TreeNode *> q;
  q.push(root);

  while (!q.empty()) {
    int levelSize = q.size();

    for (int i = 0; i < levelSize; ++i) {
      TreeNode *current = q.front();
      q.pop();

      std::cout << current->value << " ";

      for (TreeNode *child : current->children) {
        q.push(child);
      }
    }

    std::cout << std::endl;
  }
}

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
    defaultGrid();
  }
  void defaultGrid() {
    for (int x = 0; x < sizeX; ++x) {
      for (int y = 0; y < sizeY; ++y) {
        for (int z = 0; z < sizeZ; ++z) {
          // floor becomes 1, everything else 0
          data[x][y][z] = !z;
        }
      }
    }
  }

  int &at(const Coordinate coord) {
    return data[coord.x][coord.y][coord.z];
  }

  int at(const Coordinate coord) const {
    return data[coord.x][coord.y][coord.z];
  }

  void setBrick(const Brick &brick) {

    Coordinate start = brick.start;
    Coordinate end = brick.end;
    // Ensure the range is a straight line
    if (!(start.x == end.x || start.y == end.y || start.z == end.z)) {
      std::cerr << "Error: Range must be a straight line.\n";
      return;
    }

    for (int x = std::min(start.x, end.x); x <= std::max(start.x, end.x); ++x) {
      for (int y = std::min(start.y, end.y); y <= std::max(start.y, end.y); ++y) {
        for (int z = std::min(start.z, end.z); z <= std::max(start.z, end.z); ++z) {
          this->at({x, y, z}) = brick.id;
        }
      }
    }
  }

  std::vector<int> whatsUnderTheBrick(const Brick &brick) {

    std::set<int> underIt;

    Coordinate start = brick.start;
    Coordinate end = brick.end;

    for (int x = std::min(start.x, end.x); x <= std::max(start.x, end.x); ++x) {
      for (int y = std::min(start.y, end.y); y <= std::max(start.y, end.y); ++y) {
        const auto z = std::min(start.z, end.z);
        const auto under = this->at({x, y, z - 1});
        if (under != 0) {
          underIt.insert(under);
        }
      }
    }

    return {underIt.begin(), underIt.end()};
  }

  int findHighestZBelowBrick(const Brick &brick) const {
    int highestZ = 0;

    for (int x = std::min(brick.start.x, brick.end.x); x <= std::max(brick.start.x, brick.end.x); ++x) {
      for (int y = std::min(brick.start.y, brick.end.y); y <= std::max(brick.start.y, brick.end.y); ++y) {
        for (int z = 1; z < sizeZ; ++z) {
          if (this->at({x, y, z}) != 0) {
            highestZ = std::max(highestZ, z);
          }
        }
      }
    }

    return highestZ;
  }

  void displayGridXZ() const {
    for (int z = sizeZ - 1; z >= 0; --z) {
      for (int x = 0; x < sizeX; ++x) {
        // Find the highest occupied y value at this (x, z) position
        int highestY = -1;
        for (int y = sizeY - 1; y >= 0; --y) {
          if (this->at({x, y, z}) != 0) {
            highestY = y;
            break;
          }
        }

        // Print the highest occupied y value or a dot if no occupation
        if (highestY != -1) {
          std::cout << this->at({x, highestY, z});
        } else {
          std::cout << ".";
        }
      }
      std::cout << std::endl;
    }
  }

  void displayGridYZ() const {
    for (int z = sizeZ - 1; z >= 0; --z) {
      for (int y = 0; y < sizeY; ++y) {
        // Find the highest occupied x value at this (y, z) position
        int highestX = -1;
        for (int x = sizeX - 1; x >= 0; --x) {
          if (this->at({x, y, z}) != 0) {
            highestX = x;
            break;
          }
        }

        // Print the highest occupied y value or a dot if no occupation
        if (highestX != -1) {
          std::cout << this->at({highestX, y, z});
        } else {
          std::cout << ".";
        }
      }
      std::cout << std::endl;
    }
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

  int id = firstId;
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

      Brick brick = {{x1, y1, z1}, {x2, y2, z2}, id};
      bricks.push_back(brick);
      id++;
    }
  }

  std::sort(bricks.begin(), bricks.end(), compareLowestZ);

  Grid grid(11, 11, 350);

  for (const auto &brick : bricks) {
    const auto z = grid.findHighestZBelowBrick(brick) + 1;
    const auto brickHeight = std::abs(brick.end.z - brick.start.z);
    Brick fallDown{{brick.start.x, brick.start.y, z}, {brick.end.x, brick.end.y, z + brickHeight}, brick.id};
    grid.setBrick(fallDown);

    fallenBricks.push_back(fallDown);
  }

  auto tree = Tree(floor);

  for (const auto fallenBrick : fallenBricks) {
    const auto bricksUnder = grid.whatsUnderTheBrick(fallenBrick);
    if (bricksUnder.empty()) {
      tree.addChild(floor, fallenBrick.id);
      continue;
    }

    for (const auto under : bricksUnder) {
      tree.addChild(under, fallenBrick.id);
      continue;
    }
  }

  // printTreeBFS(tree.root);
  // std::cout << "\n";

  std::set<int> removeBricks;

  std::set<int> keepBricks;
  std::set<int> maybeRemoveBricks;

  for (int i = firstId; i < id; i++) {
    const auto parents = tree.findParents(i);
    const auto children = countChildren(tree, i);

    // nothing above. Safe to remove
    if (children == 0) {
      removeBricks.insert(i);
      continue;
    }

    // only one under. Have to be kept
    if (parents.size() == 1) {
      for (int item : parents) {
        keepBricks.insert(item);
      }
      continue;
    }

    // two under. Maybe can be removed. If it doesn't have to be kept
    if (parents.size() >= 2) {
      for (int item : parents) {
        maybeRemoveBricks.insert(item);
      }
      continue;
    }
  }

  for (int brick : maybeRemoveBricks) {
    if (keepBricks.find(brick) == keepBricks.end()) {
      removeBricks.insert(brick);
    }
  }

  std::cout << "Removable Bricks: " << removeBricks.size() << "\n";
}
