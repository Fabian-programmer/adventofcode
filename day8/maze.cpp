#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

std::int64_t lcm(const std::vector<std::int64_t> &input) {
  std::int64_t leastCommonMultiply = 1;
  for (const auto &number : input) {
    leastCommonMultiply = std::lcm(leastCommonMultiply, number);
  }

  return leastCommonMultiply;
}

struct paths {
  std::string left;
  std::string right;
};

int main() {

  std::ifstream file("input");

  std::string instructions;
  std::string position{"AAA"};
  std::string goal{"ZZZ"};

  std::vector<std::string> positionNodes;

  std::unordered_map<std::string, paths> maze;
  if (file.is_open()) {
    std::string line;
    std::getline(file, instructions);
    std::getline(file, line);
    while (getline(file, line)) {
      const auto origin = line.substr(0, 3);
      const auto left = line.substr(7, 3);
      const auto right = line.substr(12, 3);

      maze[origin] = {left, right};

      if (origin[2] == 'A') {
        positionNodes.push_back(origin);
      }
    }
    file.close();
  }

  std::int64_t steps_p1 = 0;
  while (position != goal) {
    for (const auto &ch : instructions) {
      steps_p1++;
      if (ch == 'L') {
        position = maze.at(position).left;
      } else if (ch == 'R') {
        position = maze.at(position).right;
      }
    }
  }

  bool allLoopDistanceAvailable = false;
  std::vector<std::int64_t> loopDistances(positionNodes.size());

  std::int64_t steps = 0;
  while (!allLoopDistanceAvailable) {
    for (const auto &ch : instructions) {
      steps++;
      for (std::size_t i = 0; i < positionNodes.size(); ++i) {
        auto &position = positionNodes[i];
        if (ch == 'L') {
          position = maze.at(position).left;
        } else if (ch == 'R') {
          position = maze.at(position).right;
        }

        if (position[2] == 'Z') {
          loopDistances[i] = steps;
        }
      }

      // data: loops begin, at the very beginning of the file
      allLoopDistanceAvailable =
          std::all_of(loopDistances.begin(), loopDistances.end(),
                      [](const std::int64_t &step) { return step != 0; });

      if (allLoopDistanceAvailable) {
        break;
      }
    }
  }

  std::cout << "Steps part1: " << steps_p1 << "\n";
  std::cout << "Steps part2: " << lcm(loopDistances) << "\n";
}
