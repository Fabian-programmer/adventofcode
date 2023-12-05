#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

struct tuple {
  std::uint64_t destination;
  std::uint64_t source;
  std::uint64_t range;
};
bool compareBySource(const tuple &a, const tuple &b) {
  return a.source < b.source;
}

void updateVector(std::vector<tuple> &vec, const tuple &input) {
  vec.push_back(input);
  // perf: can be done once at the end
  std::sort(vec.begin(), vec.end(), compareBySource);
}

std::uint64_t getDesination(const std::vector<tuple> &vec,
                            std::uint64_t source) {

  auto it = std::lower_bound(vec.begin(), vec.end(), tuple{0, source + 1, 0},
                             compareBySource);

  if (it != vec.begin()) {
    --it;
  }

  auto ret = source;
  if (source < it->source) {
    ret = source;
  } else if (it->source + it->range - 1 < source) {
    ret = source;
  } else {
    const auto range = source - it->source;
    ret = it->destination + range;
  }

  return ret;
}

int main() {

  std::ifstream file("input");

  std::uint64_t smallesValue_p1 = std::numeric_limits<std::uint64_t>::max();
  std::uint64_t smallesValue_p2 = std::numeric_limits<std::uint64_t>::max();
  std::vector<std::uint64_t> seeds;

  std::vector<tuple> seedToSoil;
  std::vector<tuple> soilToFertilizer;
  std::vector<tuple> fertilizerToWater;
  std::vector<tuple> waterToLight;
  std::vector<tuple> lightToTemperature;
  std::vector<tuple> temperatureToHumidity;
  std::vector<tuple> humidityToLocation;

  auto *currentVec = &seedToSoil;

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      std::istringstream iss(line);

      if (line.find("seeds:") != std::string::npos) {
        // ignore prefix
        std::string prefix;
        iss >> prefix;

        std::uint64_t seed;
        while (iss >> seed) {
          seeds.push_back(seed);
        }
      }

      else if (line.find("seed-to-soil") != std::string::npos) {
        currentVec = &seedToSoil;
      }

      else if (line.find("soil-to-fertilizer") != std::string::npos) {
        currentVec = &soilToFertilizer;
      }

      else if (line.find("fertilizer-to-water") != std::string::npos) {
        currentVec = &fertilizerToWater;
      }

      else if (line.find("water-to-light") != std::string::npos) {
        currentVec = &waterToLight;
      }

      else if (line.find("light-to-temperature") != std::string::npos) {
        currentVec = &lightToTemperature;
      }

      else if (line.find("temperature-to-humidity") != std::string::npos) {
        currentVec = &temperatureToHumidity;
      }

      else if (line.find("humidity-to-location") != std::string::npos) {
        currentVec = &humidityToLocation;
      }

      else if (line.size()) {

        std::uint64_t destination, source, range;
        iss >> destination >> source >> range;

        updateVector(*currentVec, {destination, source, range});
      }
    }
    file.close();
  }

  for (std::uint64_t seed : seeds) {
    seed = getDesination(seedToSoil, seed);
    seed = getDesination(soilToFertilizer, seed);
    seed = getDesination(fertilizerToWater, seed);
    seed = getDesination(waterToLight, seed);
    seed = getDesination(lightToTemperature, seed);
    seed = getDesination(temperatureToHumidity, seed);
    seed = getDesination(humidityToLocation, seed);

    smallesValue_p1 = std::min(smallesValue_p1, seed);
  }

  // perf: probably faster, when going backwards e.g. from location --> humidity
  // --> ...
  for (std::size_t i = 0; i < seeds.size(); i += 2) {
    auto current = seeds[i];
    auto next = current + seeds[i + 1];
    while (current < next) {
      auto tmpCurrent = current;
      tmpCurrent = getDesination(seedToSoil, tmpCurrent);
      tmpCurrent = getDesination(soilToFertilizer, tmpCurrent);
      tmpCurrent = getDesination(fertilizerToWater, tmpCurrent);
      tmpCurrent = getDesination(waterToLight, tmpCurrent);
      tmpCurrent = getDesination(lightToTemperature, tmpCurrent);
      tmpCurrent = getDesination(temperatureToHumidity, tmpCurrent);
      tmpCurrent = getDesination(humidityToLocation, tmpCurrent);
      smallesValue_p2 = std::min(smallesValue_p2, tmpCurrent);
      current++;
    }
  }

  std::cout << "Smalles localition value: " << smallesValue_p1 << "\n";
  std::cout << "Smalles localition value: " << smallesValue_p2 << "\n";

  return 0;
}
