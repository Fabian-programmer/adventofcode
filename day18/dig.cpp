#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Coordinate {
public:
  Coordinate(std::uint64_t x, std::uint64_t y) : x(x), y(y) {}

  std::int64_t x;
  std::int64_t y;

  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }
  friend std::ostream &operator<<(std::ostream &os, const Coordinate &coord) {
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
  }

  Coordinate goRight(std::int64_t meter) const {
    return Coordinate(this->x + 1 * meter, this->y);
  }
  Coordinate goDown(std::int64_t meter) const {
    return Coordinate(this->x, this->y + 1 * meter);
  }
  Coordinate goLeft(std::int64_t meter) const {
    return Coordinate(this->x - 1 * meter, this->y);
  }
  Coordinate goUp(std::int64_t meter) const {
    return Coordinate(this->x, this->y - 1 * meter);
  }
};

struct Instruction {
  std::int8_t direction;
  std::int64_t meter;
  std::string hexColor;

  friend std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
    os << "(" << instruction.direction << ", " << instruction.meter << ")";
    return os;
  }
};

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

class Polygon {
public:
  Polygon(std::string input, bool stupidGoblins = false) {

    std::ifstream file(input);
    if (file.is_open()) {
      std::string line;
      while (getline(file, line)) {
        Instruction instruction;
        std::istringstream iss(line);

        iss >> instruction.direction >> instruction.meter;

        char c;
        iss >> c; // Read '('
        while (iss >> std::noskipws >> c && c != ')') {
          instruction.hexColor += c;
        }
        if (stupidGoblins) {
          instruction = fixStupidGoblinBug(instruction);
        }
        m_instructions.push_back(instruction);
      }
      file.close();
    }

    createPoly();
  }

  Instruction fixStupidGoblinBug(Instruction wrongInstruction) {
    std::string hexValue = wrongInstruction.hexColor.substr(1, 5);

    std::stringstream ss;
    ss << std::hex << hexValue;
    ss >> wrongInstruction.meter;

    char lastChar = wrongInstruction.hexColor.back();
    if (lastChar == '0') {
      wrongInstruction.direction = 'R';
    } else if (lastChar == '1') {
      wrongInstruction.direction = 'D';
    } else if (lastChar == '2') {
      wrongInstruction.direction = 'L';
    } else if (lastChar == '3') {
      wrongInstruction.direction = 'U';
    }

    return wrongInstruction;
  }

  void createPoly() {
    // polygon starts at 0,0
    auto currentEdge = Coordinate(0, 0);
    m_vertices.push_back(currentEdge);

    std::int64_t sizeToIncrease = 0;
    bool previousConvex = false;

    for (std::size_t i = 0; i < m_instructions.size(); i++) {
      const auto &currentInstruction = m_instructions[i];
      const auto &nextInstruction = m_instructions[(i + 1) % m_instructions.size()];

      bool convex = isConvex(currentEdge, currentInstruction, nextInstruction);

      if (convex && previousConvex) {
        sizeToIncrease = sgn(sizeToIncrease + 1);
      } else if (convex != previousConvex) {
        sizeToIncrease = 0;
      } else if (!convex && !previousConvex) {
        sizeToIncrease = sgn(sizeToIncrease - 1);
      }
      currentEdge = createNextEdge(currentEdge, currentInstruction, sizeToIncrease);

      m_vertices.push_back(currentEdge);

      previousConvex = convex;
    }
  }

  bool isConvex(const Coordinate &p0, const Coordinate &p1, const Coordinate &p2) {
    double crossProduct = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
    return crossProduct > 0.0; // Convex if cross product is positive
  }

  bool isConvex(const Coordinate &p0, const Instruction &current, const Instruction &next) {

    Coordinate p1(0, 0);
    Coordinate p2(0, 0);

    if (current.direction == 'D') {
      p1 = p0.goDown(current.meter);
    } else if (current.direction == 'U') {
      p1 = p0.goUp(current.meter);
    } else if (current.direction == 'L') {
      p1 = p0.goLeft(current.meter);
    } else if (current.direction == 'R') {
      p1 = p0.goRight(current.meter);
    }

    if (next.direction == 'D') {
      p2 = p1.goDown(next.meter);
    } else if (next.direction == 'U') {
      p2 = p1.goUp(next.meter);
    } else if (next.direction == 'L') {
      p2 = p1.goLeft(next.meter);
    } else if (next.direction == 'R') {
      p2 = p1.goRight(next.meter);
    }

    return isConvex(p0, p1, p2);
  }

  Coordinate
  createNextEdge(const Coordinate &currentEdge, const Instruction &instruction, std::int64_t additionally) {

    Coordinate nextEdge(0, 0);

    const auto &meter = instruction.meter;
    if (instruction.direction == 'D') {
      nextEdge = currentEdge.goDown(meter + additionally);
    }
    if (instruction.direction == 'U') {
      nextEdge = currentEdge.goUp(meter + additionally);
    }
    if (instruction.direction == 'R') {
      nextEdge = currentEdge.goRight(meter + additionally);
    }
    if (instruction.direction == 'L') {
      nextEdge = currentEdge.goLeft(meter + additionally);
    }

    return nextEdge;
  }

  std::vector<Instruction> m_instructions;
  std::vector<Coordinate> m_vertices;
};

std::int64_t calculatePolygonArea(const std::vector<Coordinate> &vertices) {
  int n = vertices.size();

  std::int64_t area = 0;

  for (std::int32_t i = 0; i < n; ++i) {
    std::int32_t j = (i + 1) % n;
    area += (vertices[i].x * vertices[j].y) - (vertices[j].x * vertices[i].y);
  }

  area = std::abs(area) / 2;

  return area;
}

int main() {

  Polygon poly("input");
  std::cout << "Getting that much cubic meter dirt: " << calculatePolygonArea(poly.m_vertices) << "\n";

  Polygon poly_stupid_goblins("input", true);
  std::cout << "Getting that much cubic meter dirt part2: " << calculatePolygonArea(poly_stupid_goblins.m_vertices) << "\n";

  return 0;
}
