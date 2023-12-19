#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <vector>

struct Rule {
  char category;
  bool isGreater;
  std::int32_t value;
  std::string result;

  friend std::ostream &operator<<(std::ostream &os, const Rule &rule);
};
std::ostream &operator<<(std::ostream &os, const Rule &rule) {
  os << "Category: " << rule.category << std::endl;
  os << "isGreater: " << std::boolalpha << rule.isGreater << std::endl;
  os << "Value: " << rule.value << std::endl;
  os << "Result: " << rule.result << std::endl;
  return os;
}

struct minMax {
  std::int32_t minValue;
  std::int32_t maxValue;
};
void printMap(const std::map<char, minMax> &parts) {
  for (const auto &entry : parts) {
    std::cout << "Key: " << entry.first
              << ", Min: " << entry.second.minValue
              << ", Max: " << entry.second.maxValue << std::endl;
  }
  std::cout << std::endl;
}

std::vector<Rule> parseRules(const std::string &input) {
  std::vector<Rule> rules;

  // Use a stringstream to tokenize the input string
  std::stringstream ss(input);
  std::string token;

  // Use ',' as the delimiter to separate individual rules
  while (std::getline(ss, token, ',')) {
    Rule rule;

    const auto resultDelimiter = token.find(':');
    if (resultDelimiter == std::string::npos) {
      rule.category = '?';
      rule.result = token;
      rules.push_back(rule);
      continue;
    }

    rule.category = token[0];
    if (token[1] == '<') {
      rule.isGreater = false;
    } else {
      rule.isGreater = true;
    }
    rule.value = std::stoi(token.substr(2, resultDelimiter - 2));
    rule.result = token.substr(resultDelimiter + 1);
    rules.push_back(rule);
  }
  return rules;
}
std::map<char, std::int32_t> parseParts(const std::string &input) {
  std::map<char, std::int32_t> resultMap;

  const auto noCurles = input.substr(1, input.size() - 2);
  std::stringstream ss(noCurles);
  std::string token;

  while (std::getline(ss, token, ',')) {
    size_t pos = token.find('=');

    char key = token[0];
    std::int32_t value = std::stoi(token.substr(pos + 1));

    resultMap[key] = value;
  }

  return resultMap;
}

std::string nextWorkflow(const std::vector<Rule> &rules,
                         const std::map<char, std::int32_t> &parts) {

  for (const auto &rule : rules) {
    if (rule.category == '?') {
      return rule.result;
    }

    const auto partValue = parts.at(rule.category);
    const auto diffParts = partValue - rule.value;

    if (diffParts < 0 && !rule.isGreater) {
      return rule.result;
    }
    if (diffParts > 0 && rule.isGreater) {
      return rule.result;
    }
  }

  return "error";
}

std::vector<std::pair<std::string, std::map<char, minMax>>> nextWorkflowRange(const std::vector<Rule> &rules,
                                                                              std::map<char, minMax> parts) {

  std::vector<std::pair<std::string, std::map<char, minMax>>> possibleBranches;

  for (const auto &rule : rules) {
    if (rule.category == '?') {
      possibleBranches.push_back({rule.result, parts});
      continue;
    }

    // 1351 - 4000
    auto &[minVal, maxVal] = parts.at(rule.category);
    auto tmpMinVal = minVal;
    auto tmpMaxVal = maxVal;

    if (!rule.isGreater) {
      maxVal = std::min(rule.value - 1, tmpMaxVal);
      possibleBranches.push_back({rule.result, parts});

      minVal = std::max(rule.value, tmpMinVal);
      maxVal = tmpMaxVal;
    }

    if (rule.isGreater) {
      minVal = std::max(rule.value + 1, tmpMinVal);
      possibleBranches.push_back({rule.result, parts});

      minVal = tmpMinVal;
      maxVal = std::min(rule.value, tmpMaxVal);
    }
  }

  // for (const auto &branch : possibleBranches) {
  //   std::cout << branch.first << "\n";
  //   printMap(branch.second);
  // }

  return possibleBranches;
}

int main() {

  std::map<std::string, std::vector<Rule>> workflows;
  std::vector<std::map<char, std::int32_t>> partsVector;

  std::int64_t sum_parts_p1 = 0;
  std::int64_t sum_parts_p2 = 0;

  std::string line;
  std::ifstream file("input");
  if (file.is_open()) {
    bool rulesActive = true;
    while (getline(file, line)) {
      if (line.empty()) {
        rulesActive = false;
        continue;
      }

      if (rulesActive) {
        size_t pos = line.find("{");

        std::string workflowStr = line.substr(0, pos);
        std::string rules = line.substr(pos + 1, line.length() - pos - 2);

        const auto ruleSet = parseRules(rules);

        workflows[workflowStr] = ruleSet;
      } else {
        partsVector.push_back(parseParts(line));
      }
    }
    file.close();
  }

  for (const auto &parts : partsVector) {
    std::string current = "in";
    while (true) {
      current = nextWorkflow(workflows.at(current), parts);
      if (current == "A") {
        sum_parts_p1 += parts.at('x');
        sum_parts_p1 += parts.at('m');
        sum_parts_p1 += parts.at('a');
        sum_parts_p1 += parts.at('s');
        break;
      }
      if (current == "R") {
        break;
      }
    }
  }

  std::cout << "Parts p1: " << sum_parts_p1 << "\n";

  std::map<char, minMax> xmas{
      {'x', {1, 4000}},
      {'m', {1, 4000}},
      {'a', {1, 4000}},
      {'s', {1, 4000}}};

  std::string current = "in";

  std::queue<std::pair<std::string, std::map<char, minMax>>> que;
  que.push({current, xmas});

  while (!que.empty()) {
    const auto &[current, parts] = que.front();
    const auto nextSteps = nextWorkflowRange(workflows.at(current), parts);

    que.pop();

    for (const auto &[next, nextParts] : nextSteps) {

      if (next == "A") {
        std::int64_t combinations = 1;
        for (const auto &[_, minMax] : nextParts) {
          combinations *= (minMax.maxValue - minMax.minValue) + 1;
        }
        sum_parts_p2 += combinations;

        continue;
      }
      if (next == "R") {
        continue;
      }

      que.push({next, nextParts});
    }
  }

  std::cout << "Combinations p2: " << sum_parts_p2 << "\n";

  return 0;
}
