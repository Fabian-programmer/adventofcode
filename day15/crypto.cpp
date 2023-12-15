#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Lens {
  std::string label;
  std::int32_t focalLength;

  bool operator==(const Lens &other) const { return label == other.label; }
  bool operator==(const std::string &otherLabel) const {
    return label == otherLabel;
  }
};

void addLens(const std::int32_t boxValue, const std::string &label,
             const std::int32_t focalLength);
void removeLens(const std::int32_t boxValue, const std::string &label);

std::vector<std::vector<Lens>> g_boxes(256);

std::int32_t hash(const std::string &sequence) {
  std::uint8_t value = 0;

  for (const auto &ch : sequence) {
    value += ch;
    value *= 17;
  }

  return value;
}

void processLens(const std::string &token) {
  size_t pos = token.find('=');

  if (pos != std::string::npos) {
    // Found '=' in the token, treat it as a key-value pair
    std::string key = token.substr(0, pos);
    std::string value = token.substr(pos + 1);
    addLens(hash(key), key, stoi(value));
    return;
  }

  if (token.back() == '-') {
    std::string key = token.substr(0, token.size() - 1);
    removeLens(hash(key), key);
  }
}

void addLens(const std::int32_t boxValue, const std::string &lens,
             const std::int32_t focalLength) {

  auto &box = g_boxes[boxValue];
  auto it = std::find(box.begin(), box.end(), lens);

  if (it != box.end()) {
    it->focalLength = focalLength;
  } else {
    box.push_back(Lens{lens, focalLength});
  }
}
void removeLens(const std::int32_t boxValue, const std::string &lens) {
  auto &box = g_boxes[boxValue];
  auto it = std::find(box.begin(), box.end(), lens);

  if (it != box.end()) {
    box.erase(it);
  }
}

int main() {

  std::ifstream file("input");
  std::uint64_t sum_p1 = 0;
  std::uint64_t sum_p2 = 0;

  std::string line;
  if (file.is_open()) {
    getline(file, line);
    file.close();
  }

  std::istringstream iss(line);
  std::string token;

  while (std::getline(iss, token, ',')) {
    sum_p1 += hash(token);
    processLens(token);
  }

  for (std::size_t box = 0; box < g_boxes.size(); box++) {
    for (std::size_t lens = 0; lens < g_boxes[box].size(); lens++) {
      sum_p2 += (box + 1) * (lens + 1) * g_boxes[box][lens].focalLength;
    }
  }

  std::cout << "Sum of hashes: " << sum_p1 << "\n";
  std::cout << "Sum of boxes: " << sum_p2 << "\n";

  return 0;
}
