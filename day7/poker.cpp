#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

enum CardValue : int {
  HIGHCARD = 0,
  PAIR,
  TWOPAIR,
  TRIPLE,
  FULLHOUSE,
  FOUR,
  FIVE,
};

struct Card {
  std::uint64_t value;
  std::uint32_t multiplier;
  std::uint32_t rank;
};

bool compareCard(const Card &a, const Card &b) {
  if (a.rank < b.rank)
    return true;
  if (b.rank < a.rank)
    return false;

  // a=b for primary condition, go to secondary
  if (a.value < b.value)
    return true;
  if (b.value < a.value)
    return false;

  return false;
}

Card createJokerCard(const std::string &input, std::uint32_t multiplier) {

  std::unordered_map<char, std::string> mapping = {
      {'J', "10"}, {'2', "11"}, {'3', "12"}, {'4', "13"}, {'5', "14"},
      {'6', "15"}, {'7', "16"}, {'8', "17"}, {'9', "18"}, {'T', "19"},
      {'Q', "21"}, {'K', "22"}, {'A', "23"},
  };
  std::unordered_map<char, std::uint8_t> counter;

  std::string remap;
  for (char ch : input) {
    counter[ch]++;
    remap += mapping[ch];
  }

  std::uint32_t jokerTime = 0;
  if (counter.count('J')) {
    jokerTime = counter.at('J');
  }

  std::uint32_t rank = CardValue::HIGHCARD;

  if (counter.size() == 5) {
    rank = CardValue::HIGHCARD;
    if (jokerTime) {
      rank = CardValue::PAIR;
    }
  } else if (counter.size() == 4) {
    rank = CardValue::PAIR;
    if (jokerTime) {
      rank = CardValue::TRIPLE;
    }
  } else if (counter.size() == 3) {
    for (const auto &[_, count] : counter) {
      if (count == 2) {
        rank = CardValue::TWOPAIR;
        if (jokerTime == 1) {
          rank = CardValue::FULLHOUSE;
        } else if (jokerTime == 2) {
          rank = CardValue::FOUR;
        }
        break;
      } else if (count == 3) {
        rank = CardValue::TRIPLE;
        if (jokerTime) {
          rank = CardValue::FOUR;
        }
        break;
      }
    }
  } else if (counter.size() == 2) {
    if (jokerTime) {
      rank = CardValue::FIVE;
    } else {
      for (const auto &[_, count] : counter) {
        if (count == 3) {
          rank = CardValue::FULLHOUSE;
          break;
        } else if (count == 4) {
          rank = CardValue::FOUR;
          break;
        }
      }
    }
  } else if (counter.size() == 1) {
    rank = CardValue::FIVE;
  }

  return {std::stoull(remap), multiplier, rank};
}

Card createCard(const std::string &input, std::uint32_t multiplier) {

  std::unordered_map<char, std::string> mapping = {
      {'2', "11"}, {'3', "12"}, {'4', "13"}, {'5', "14"}, {'6', "15"},
      {'7', "16"}, {'8', "17"}, {'9', "18"}, {'T', "19"}, {'J', "20"},
      {'Q', "21"}, {'K', "22"}, {'A', "23"},
  };
  std::unordered_map<char, std::uint8_t> counter;

  std::string remap;
  for (char ch : input) {
    counter[ch]++;
    remap += mapping[ch];
  }

  std::uint32_t rank = CardValue::HIGHCARD;

  if (counter.size() == 4) {
    rank = CardValue::PAIR;
  } else if (counter.size() == 3) {
    for (const auto &[_, count] : counter) {
      if (count == 2) {
        rank = CardValue::TWOPAIR;
        break;
      } else if (count == 3) {
        rank = CardValue::TRIPLE;
        break;
      }
    }
  } else if (counter.size() == 2) {
    for (const auto &[_, count] : counter) {
      if (count == 3) {
        rank = CardValue::FULLHOUSE;
        break;
      } else if (count == 4) {
        rank = CardValue::FOUR;
        break;
      }
    }
  } else if (counter.size() == 1) {
    rank = CardValue::FIVE;
  }

  return {std::stoull(remap), multiplier, rank};
}

int main() {

  std::ifstream file("input");
  std::uint64_t sum_p1 = 0;
  std::uint64_t sum_p2 = 0;

  std::vector<Card> cards;
  std::vector<Card> jokerCards;

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      std::istringstream iss(line);

      std::string hand;
      iss >> hand;

      std::uint32_t multiplier;
      iss >> multiplier;

      cards.emplace_back(createCard(hand, multiplier));
      jokerCards.emplace_back(createJokerCard(hand, multiplier));
    }
    file.close();
  }

  std::sort(cards.begin(), cards.end(), compareCard);
  std::sort(jokerCards.begin(), jokerCards.end(), compareCard);

  for (std::size_t i = 0; i < cards.size(); i++) {
    sum_p1 += (i + 1) * cards[i].multiplier;
  }
  for (std::size_t i = 0; i < jokerCards.size(); i++) {
    sum_p2 += (i + 1) * jokerCards[i].multiplier;
  }

  std::cout << "Sum part1: " << sum_p1 << "\n";
  std::cout << "Sum part2: " << sum_p2 << "\n";
}
