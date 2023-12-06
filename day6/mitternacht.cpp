#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::int64_t mitternacht(std::int64_t time, std::int64_t distance) {
  const std::int64_t sqrt_tt = std::sqrt(time * time - 4 * distance);

  std::int64_t low = std::ceil(0.5 * (time - sqrt_tt));
  std::int64_t high = std::floor(0.5 * (time + sqrt_tt));

  if (low * time - low * low == distance) {
    low++;
  }
  if (high * time - high * high == distance) {
    high--;
  }

  return high - low + 1;
}

int main() {
  // for part 2 just tamper with the input
  std::ifstream file("input");
  std::uint64_t product_1 = 1;

  std::vector<std::int64_t> time;
  std::vector<std::int64_t> distance;

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      std::istringstream iss(line);

      std::string label;
      std::int64_t value;

      // Read the label (Time/Distance)
      iss >> label;

      // Read the values and add them to the respective vectors
      while (iss >> value) {
        if (label == "Time:") {
          time.push_back(value);
        } else if (label == "Distance:") {
          distance.push_back(value);
        }
      }
    }
    file.close();
  }

  for (std::size_t race = 0; race < time.size(); race++) {
    product_1 *= mitternacht(time[race], distance[race]);
  }

  std::cout << "product of allowe times: " << product_1 << "\n";

  return 0;
}
