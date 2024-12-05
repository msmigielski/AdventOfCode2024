#include <catch2/catch_all.hpp>
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>

constexpr auto testData = R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47)";

using Rules = std::vector<std::vector<int>>;
using Updates = std::vector<std::vector<int>>;
using RulesAndUpdates = std::pair<Rules, Updates>;

RulesAndUpdates ReadRulesAndUpdates(std::istream &input)
{
  std::regex valRegex("(\\d+)");
  Rules rules;
  Updates updates;
  bool isRulesSection = true;
  std::string line;

  while (std::getline(input, line))
  {
    std::vector<int> values;

    if (line.empty())
    {
      isRulesSection = false;
      continue;
    }

    auto matchStart =
        std::sregex_iterator(line.begin(), line.end(), valRegex);
    auto matchEnd = std::sregex_iterator();

    for (auto i = matchStart; i != matchEnd; ++i)
    {
      int x = std::stoi(std::smatch(*i).str());
      values.push_back(x);
    }

    if (isRulesSection)
      rules.push_back(values);
    else
      updates.push_back(values);
  }

  return {rules, updates};
}

bool IsUpdateValid(const std::vector<int> &update, const Rules &rules)
{
  for (const auto &rule : rules)
  {
    const auto &firstPosition = std::ranges::find(update, rule[0]);
    const auto &secondPosition = std::ranges::find(update, rule[1]);
    if (firstPosition != update.end() && secondPosition < firstPosition)
    {
      return false;
    }
  }
  return true;
}

int SumMidElementOfValidUpdates(const Rules &rules, const Updates &updates)
{
  int count = 0;

  for (const auto &update : updates)
  {
    if (IsUpdateValid(update, rules))
    {
      count += update[update.size() / 2];
    }
  }

  return count;
}

std::vector<int> FixUpdateWithRules(std::vector<int> update, const Rules &rules)
{
  bool modified = false;
  do
  {
    modified = false;
    for (const auto &rule : rules)
    {
      auto firstPosition = std::ranges::find(update, rule[0]);
      auto secondPosition = std::ranges::find(update, rule[1]);

      if (firstPosition != update.end() && secondPosition < firstPosition)
      {
        update.insert(firstPosition + 1, *secondPosition);
        secondPosition = std::ranges::find(update, rule[1]);
        update.erase(secondPosition);
        modified = true;
      }
    }
  } while (modified);
  return update;
}

int SumMidElementOfNotValidUpdates(const Rules &rules, const Updates &updates)
{
  int count = 0;

  for (const auto &update : updates)
  {
    if (!IsUpdateValid(update, rules))
    {
      count += FixUpdateWithRules(update, rules)[update.size() / 2];
    }
  }

  return count;
}

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  const auto &[rules, updates] = ReadRulesAndUpdates(testInput);

  SECTION("part 1")
  {
    REQUIRE(143 == SumMidElementOfValidUpdates(rules, updates));
  }

  SECTION("part 2")
  {
    REQUIRE(123 == SumMidElementOfNotValidUpdates(rules, updates));
  }
}

TEST_CASE("Task day 5")
{
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  const auto &[rules, updates] = ReadRulesAndUpdates(data);

  SECTION("part 1")
  {
    std::cout << "Day 5 - part 1 result: " << SumMidElementOfValidUpdates(rules, updates) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 5 - part 2 result: " << SumMidElementOfNotValidUpdates(rules, updates) << std::endl;
  }
}