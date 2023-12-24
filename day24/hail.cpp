#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

struct Vec3d {
  double x;
  double y;
  double z;

  Vec3d operator+(const Vec3d &other) const {
    Vec3d result;
    result.x = x + other.x;
    result.y = y + other.y;
    result.z = z + other.z;
    return result;
  }
  Vec3d operator-(const Vec3d &other) const {
    Vec3d result;
    result.x = x - other.x;
    result.y = y - other.y;
    result.z = z - other.z;
    return result;
  }

  Vec3d operator*(const double &value) const {
    Vec3d result;
    result.x = x * value;
    result.y = y * value;
    result.z = z * value;
    return result;
  }

  double cross(const Vec3d &other) const {
    return x * other.y - y * other.x;
  }
};

std::ostream &operator<<(std::ostream &os, const Vec3d &data) {
  os << "x: " << data.x << ", y: " << data.y << ", z: " << data.z;
  return os;
}

struct Hail {
  Vec3d pos;
  Vec3d vel;
};

std::ostream &operator<<(std::ostream &os, const Hail &data) {
  os << "x: " << data.pos.x << ", y: " << data.pos.y << ", z: " << data.pos.z
     << " | vx: " << data.vel.x << ", vy: " << data.vel.y << ", vz: " << data.vel.z;
  return os;
}

struct Intersection {
  Vec3d position;
  bool past;
};

std::optional<Intersection> calculateIntersectionXY(const Hail &hail1, const Hail &hail2) {

  // t = (q − p) × s / (r × s)
  // u = (q − p) × r / (r × s)
  const auto t_nom = (hail2.pos - hail1.pos).cross(hail2.vel);
  const auto u_nom = (hail2.pos - hail1.pos).cross(hail1.vel);
  const auto denom = (hail1.vel).cross(hail2.vel);

  if (denom == 0) {
    return std::nullopt;
  }

  const auto t = t_nom / denom;
  const auto u = u_nom / denom;

  // If r × s ≠ 0 and 0 ≤ t ≤ 1 and 0 ≤ u ≤ 1, the two line segments meet at the point p + t r = q + u s.
  Intersection intersection;
  intersection.position = hail1.pos + hail1.vel * t;
  intersection.past = (t < 0 || u < 0) ? true : false;

  return intersection;
}

bool isInArea(std::int64_t lower, std::int64_t higher, const Vec3d &p) {
  if (p.x <= lower || p.x >= higher || p.y <= lower || p.y >= higher) {
    return false;
  }
  return true;
}

int main() {
  // Open the file
  std::ifstream file("input");

  std::vector<Hail> hails;

  // Read each line from the file
  std::string line;
  while (std::getline(file, line)) {
    // Create a stringstream to parse the line
    std::stringstream ss(line);

    // Create a temporary Data structure to store the values
    Hail tempData;

    // Read values before "@"
    ss >> tempData.pos.x;
    ss.ignore(); // Ignore comma
    ss >> tempData.pos.y;
    ss.ignore(); // Ignore comma
    ss >> tempData.pos.z;

    // Ignore characters until "@"
    ss.ignore(3); // Ignore "@ "

    // Read values after "@"
    ss >> tempData.vel.x;
    ss.ignore(); // Ignore comma
    ss >> tempData.vel.y;
    ss.ignore(); // Ignore comma
    ss >> tempData.vel.z;

    // Add the temporary Data structure to the vector
    hails.push_back(tempData);
  }

  // Close the file
  file.close();

  const std::int64_t lower = 200000000000000;
  const std::int64_t higher = 400000000000000;

  std::int64_t futureCollisions = 0;

  for (std::size_t i = 0; i < hails.size(); ++i) {
    for (std::size_t j = i + 1; j < hails.size(); ++j) {
      // std::cout << hails[i] << "\n";
      // std::cout << hails[j] << "\n";
      const auto intersection = calculateIntersectionXY(hails[i], hails[j]);

      if (intersection.has_value()) {
        if (!intersection->past) {

          if (isInArea(lower, higher, intersection->position)) {
            // std::cout << intersection->position << "\n";
            futureCollisions++;

          } else {
            // std::cout << "Not in Area: " << intersection->position << "\n";
          }
        } else {
          // std::cout << "Past\n";
        }
      } else {
        // std::cout << "Parallel\n";
      }
      // std::cout << "\n";
    }
  }

  std::cout << "Future collisions: " << futureCollisions << "\n";

  return 0;
}
