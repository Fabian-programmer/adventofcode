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
bool compareByDestination(const tuple &a, const tuple &b) {
  return a.destination < b.destination;
}

void updateVector(std::vector<tuple> &vec, const tuple &input) {
  vec.push_back(input);
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

std::uint64_t getSource(const std::vector<tuple> &vec, std::uint64_t dest) {

  auto it = std::lower_bound(vec.begin(), vec.end(), tuple{dest + 1, 0, 0},
                             compareByDestination);

  if (it != vec.begin()) {
    --it;
  }

  auto ret = dest;
  if (dest < it->destination) {
    ret = dest;
  } else if (it->destination + it->range - 1 < dest) {
    ret = dest;
  } else {
    const auto range = dest - it->destination;
    ret = it->source + range;
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

  // part 1
  // sorting
  {
    std::sort(seedToSoil.begin(), seedToSoil.end(), compareBySource);
    std::sort(soilToFertilizer.begin(), soilToFertilizer.end(),
              compareBySource);
    std::sort(fertilizerToWater.begin(), fertilizerToWater.end(),
              compareBySource);
    std::sort(waterToLight.begin(), waterToLight.end(), compareBySource);
    std::sort(lightToTemperature.begin(), lightToTemperature.end(),
              compareBySource);
    std::sort(temperatureToHumidity.begin(), temperatureToHumidity.end(),
              compareBySource);
    std::sort(humidityToLocation.begin(), humidityToLocation.end(),
              compareBySource);
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

  // part 2
  // sorting
  {
    std::sort(seedToSoil.begin(), seedToSoil.end(), compareByDestination);
    std::sort(soilToFertilizer.begin(), soilToFertilizer.end(),
              compareByDestination);
    std::sort(fertilizerToWater.begin(), fertilizerToWater.end(),
              compareByDestination);
    std::sort(waterToLight.begin(), waterToLight.end(), compareByDestination);
    std::sort(lightToTemperature.begin(), lightToTemperature.end(),
              compareByDestination);
    std::sort(temperatureToHumidity.begin(), temperatureToHumidity.end(),
              compareByDestination);
    std::sort(humidityToLocation.begin(), humidityToLocation.end(),
              compareByDestination);
  }

  // only absolute values
  for (std::size_t i = 0; i < seeds.size(); i += 2) {
    seeds[i + 1] = seeds[i] + seeds[i + 1] - 1;
  }
  std::sort(seeds.begin(), seeds.end());

  for (std::size_t locationVal = 1;
       locationVal < std::numeric_limits<std::size_t>::max(); locationVal++) {
    auto seed = locationVal;
    seed = getSource(humidityToLocation, seed);
    seed = getSource(temperatureToHumidity, seed);
    seed = getSource(lightToTemperature, seed);
    seed = getSource(waterToLight, seed);
    seed = getSource(fertilizerToWater, seed);
    seed = getSource(soilToFertilizer, seed);
    seed = getSource(seedToSoil, seed);

    // check if seed is valid
    if (seed < seeds.front() || seed > seeds.back()) {
      continue;
    }
    auto it = std::lower_bound(seeds.begin(), seeds.end(), seed);
    if (it != seeds.begin()) {
      --it;
    }
    int index = it - seeds.begin();
    if (index % 2 == 0) {
      smallesValue_p2 = locationVal;
      break;
    }
  }

  std::cout << "Smalles localition value: " << smallesValue_p1 << "\n";
  std::cout << "Smalles localition value: " << smallesValue_p2 << "\n";

  return 0;
}
