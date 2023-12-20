#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <vector>

enum ModuleType {
  BUTTON,
  BROADCASTER,
  FLIPFLOP,
  CONJUNCTION
};

enum PulseType {
  LOW,
  HIGH,
  UNKNOWN,
};

struct Pulse {
  std::string sender = "unknown";
  std::string receiver = "unknown";
  PulseType type = PulseType::UNKNOWN;
};

std::ostream &operator<<(std::ostream &os, const Pulse &pulse) {

  std::string pulseTypeStr;
  if (pulse.type == HIGH) {
    pulseTypeStr = "high";
  } else if (pulse.type == LOW) {
    pulseTypeStr = "low";
  }

  os << pulse.sender << " -" << pulseTypeStr << "-> " << pulse.receiver;
  return os;
}

class Module {
public:
  explicit Module(const std::string &sender, const std::vector<std::string> &receivers) : sender(sender), receivers(receivers){};
  virtual ~Module() = default;

  virtual std::vector<Pulse> recvsend(Pulse receive) = 0;
  virtual std::vector<std::string> getReceivers() {
    return receivers;
  }

protected:
  std::string sender;
  std::vector<std::string> receivers;
};

class FlipFlop : public Module {
public:
  FlipFlop(const std::string &sender, const std::vector<std::string> &receivers) : Module(sender, receivers) {}

  std::vector<Pulse> recvsend(Pulse pulse_recv) override {
    if (pulse_recv.type == PulseType::HIGH) {
      return {};
    }
    isOn = !isOn;

    std::vector<Pulse> send;
    for (const auto &receiver : receivers) {
      if (isOn) {
        send.push_back({sender, receiver, PulseType::HIGH});
      } else {
        send.push_back({sender, receiver, PulseType::LOW});
      }
    }

    return send;
  }

private:
  bool isOn = false;
};

class Conjunction : public Module {
public:
  Conjunction(const std::string &sender, const std::vector<std::string> &receivers) : Module(sender, receivers) {}

  std::vector<Pulse> recvsend(Pulse pulse_recv) override {

    // add / update memory
    memory[pulse_recv.sender] = pulse_recv.type;

    std::vector<Pulse> send;
    for (const auto &receiver : receivers) {
      if (allHigh()) {
        send.push_back({sender, receiver, PulseType::LOW});
      } else {
        send.push_back({sender, receiver, PulseType::HIGH});
      }
    }
    return send;
  }

  void addMemory(const std::string &name) {
    memory[name] = PulseType::LOW;
  }

  bool allHigh() {
    return std::all_of(memory.begin(), memory.end(), [](const auto &pair) { return pair.second == PulseType::HIGH; });
  }

  std::map<std::string, PulseType> memory;
};

class BroadCaster : public Module {
public:
  BroadCaster(const std::string &sender, const std::vector<std::string> &receivers) : Module(sender, receivers) {}

  std::vector<Pulse> recvsend(Pulse pulse_recv) override {
    std::vector<Pulse> send;
    for (const auto &receiver : receivers) {
      send.push_back({sender, receiver, pulse_recv.type});
    }
    return send;
  }
};

std::string trimWhitespaces(const std::string &str) {
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) { return std::isspace(c); }).base();
  return (start < end ? std::string(start, end) : std::string());
}

std::int32_t g_i = 0;

std::pair<std::int64_t, std::int64_t> pushButton(std::map<std::string, std::unique_ptr<Module>> &moduleMap) {

  std::queue<Pulse> que;
  que.push({"button", "broadcaster", PulseType::LOW});
  std::int64_t lowPulseSent = 0;
  std::int64_t highPulseSent = 0;

  while (!que.empty()) {

    const auto currentPulse = que.front();
    if (currentPulse.type == LOW) {
      lowPulseSent++;
    } else if (currentPulse.type == HIGH) {
      highPulseSent++;
    }

    // sink
    if (moduleMap.count(currentPulse.receiver) == 0) {
      que.pop();
      continue;
    }

    auto &module = moduleMap.at(currentPulse.receiver);
    const auto nextPulses = module->recvsend(currentPulse);

    // lk zv sp xt
    if (currentPulse.receiver == "lk" || currentPulse.receiver == "zv" || currentPulse.receiver == "sp" || currentPulse.receiver == "xt") {
      auto conjunctionModule = static_cast<Conjunction *>(moduleMap.at(currentPulse.receiver).get());

      const auto allHigh = conjunctionModule->allHigh();

      if (!allHigh) {
        std::cout << currentPulse.receiver << ": " << g_i << "\n";
      }
    }

    que.pop();

    for (const auto &nextPulse : nextPulses) {
      que.push(nextPulse);
    }
  }

  return {lowPulseSent, highPulseSent};
}

int main() {

  std::map<std::string, std::unique_ptr<Module>> moduleMap;
  std::vector<std::string> conjunctionNames;

  std::string line;
  std::ifstream file("input");

  // Initialize modules
  if (file.is_open()) {
    while (getline(file, line)) {

      const size_t arrowPos = line.find("->");
      const std::string name = line.substr(1, arrowPos - 2);

      const std::string restOfLine = line.substr(arrowPos + 3); // Skip '->'
      std::istringstream receiversStream(restOfLine);
      std::string receiver;
      std::vector<std::string> receivers;
      while (std::getline(receiversStream, receiver, ',')) {
        receivers.push_back(trimWhitespaces(receiver));
      }

      if (line[0] == 'b') {
        moduleMap["broadcaster"] = std::make_unique<BroadCaster>("broadcaster", receivers);
      } else if (line[0] == '%') {
        moduleMap[name] = std::make_unique<FlipFlop>(name, receivers);
      } else if (line[0] == '&') {
        moduleMap[name] = std::make_unique<Conjunction>(name, receivers);
        conjunctionNames.push_back(name);
      }
    }
    file.close();
  }

  // Initialize memory of conjunctions
  for (const auto &conjunctionName : conjunctionNames) {
    auto conjunctionModule = static_cast<Conjunction *>(moduleMap.at(conjunctionName).get());

    for (const auto &[name, module_ptr] : moduleMap) {
      const auto receivers = module_ptr->getReceivers();
      auto it = std::find(receivers.begin(), receivers.end(), conjunctionName);
      if (it != receivers.end()) {
        conjunctionModule->addMemory(name);
      }
    }
  }

  std::int64_t lowPulse = 0;
  std::int64_t highPulse = 0;

  for (std::int64_t i = 0; i < 20000; ++i) {
    const auto [low, high] = pushButton(moduleMap);
    lowPulse += low;
    highPulse += high;
    g_i++;
  }

  std::cout << "low: " << lowPulse << " high: " << highPulse << "\n";
  std::cout << "Part1: " << lowPulse * highPulse << "\n";
}
