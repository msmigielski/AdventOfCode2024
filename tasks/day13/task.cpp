#include <catch2/catch_all.hpp>
#include <fstream>
#include <utils/Timer.h>
#include <unordered_set>
#include <regex>

constexpr auto testData = R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279)";
struct PairHash
{
  std::size_t operator()(const std::pair<uint64_t, uint64_t> &pair) const
  {
    std::size_t h1 = std::hash<uint64_t>{}(pair.first);
    std::size_t h2 = std::hash<uint64_t>{}(pair.second);

    return h1 ^ (h2 << 1);
  }
};

struct ClawMachine
{
  using PositionsXY = std::pair<uint64_t, uint64_t>;
  using ButtonsClicks = std::pair<uint64_t, uint64_t>;
  using Solutions = std::vector<ButtonsClicks>;

  ClawMachine(std::istream &input)
  {
    std::string line;
    std::getline(input, line);
    buttonA = ReadValues(line);
    std::getline(input, line);
    buttonB = ReadValues(line);
    std::getline(input, line);
    prize = ReadValues(line);
    prize.first += 10000000000000;
    prize.second += 10000000000000;
  }

  std::pair<uint64_t, uint64_t> ReadValues(const std::string &line)
  {
    std::regex pattern(R"(X[+=](\d+), Y[+=](\d+))");
    std::smatch matches;

    if (std::regex_search(line, matches, pattern))
    {
      uint64_t x = std::stoi(matches[1].str());
      uint64_t y = std::stoi(matches[2].str());
      return {x, y};
    }
    return {};
  }

  size_t CalcRequiredTokens()
  {
    PositionsXY clicks;

    if (((prize.first * buttonA.second) > (prize.second * buttonA.first)) && ((buttonA.second * buttonB.first) <= (buttonA.first * buttonB.second)))
    {
      std::cout << "dupa 0" << std::endl;
      return 0;
    }

    if (((buttonA.second * buttonB.first) > (buttonA.first * buttonB.second)) &&
        ((prize.first * buttonA.second) <= (prize.second * buttonA.first)))
    {
      std::cout << "dupa 1" << std::endl;
      return 0;
    }

    if ((buttonA.second * buttonB.first) == (buttonA.first * buttonB.second))
    {
      std::cout << "dupa 2" << std::endl;
      return 0;
    }

    // if ((prize.first * buttonA.second) > (prize.second * buttonA.first) && (buttonA.second * buttonB.first) <= (buttonA.first * buttonB.second))
    //   return 0;

    // if ((buttonA.second * buttonB.first) >= (buttonA.first * buttonB.second))
    //   return 0;

    if ((prize.first * buttonA.second) >= (prize.second * buttonA.first) &&
        (buttonA.second * buttonB.first) >= (buttonA.first * buttonB.second))
    {
      uint64_t first = (prize.first * buttonA.second) - (prize.second * buttonA.first);
      uint64_t second = (buttonA.second * buttonB.first) - (buttonA.first * buttonB.second);
      if (first % second != 0)
      {
        std::cout << "dupa 8" << std::endl;
        return 0;
      }
      clicks.second = ((prize.first * buttonA.second) - (prize.second * buttonA.first)) /
                      ((buttonA.second * buttonB.first) - (buttonA.first * buttonB.second));
    }
    else
    {
      uint64_t first = (prize.second * buttonA.first) - (prize.first * buttonA.second);
      uint64_t second = (buttonA.first * buttonB.second) - (buttonA.second * buttonB.first);
      if (first % second != 0)
      {
        std::cout << "dupa 9" << std::endl;
        return 0;
      }

      clicks.second = ((prize.second * buttonA.first) - (prize.first * buttonA.second)) /
                      ((buttonA.first * buttonB.second) - (buttonA.second * buttonB.first));
    }

    // if (clicks.second > maxPushes)
    // {
    //   std::cout << "dupa 3" << std::endl;
    //   return 0;
    // }

    if ((clicks.second * buttonB.second) > prize.second || buttonA.second == 0)
    {
      std::cout << "dupa 4" << std::endl;
      return 0;
    }

    uint64_t first = (prize.second - clicks.second * buttonB.second);
    uint64_t second = buttonA.second;

    if (first % second != 0)
    {
      return 0;
    }

    // std::cout << (prize.second * buttonA.first) << " - " << (prize.first * buttonA.second) << " = " << ((prize.second * buttonA.first) - (prize.first * buttonA.second)) << std::endl;
    // std::cout << (buttonA.first * buttonB.second) << " - " << (buttonB.second * buttonA.first) << " = " << (buttonA.first * buttonB.second) - (buttonB.second * buttonA.first) << std::endl;
    clicks.first = (prize.second - clicks.second * buttonB.second) / buttonA.second;

    // if (clicks.first > maxPushes)
    // {
    //   std::cout << "dupa 5" << std::endl;
    //   return 0;
    // }
    std::cout << clicks.first << " " << clicks.second << std::endl;
    return clicks.first * costA + clicks.second * costB;
  }

  std::unordered_set<ButtonsClicks, PairHash> alreadyChecked;

  const uint64_t costA = 3;
  const uint64_t costB = 1;
  const uint64_t maxPushes = 100;

  PositionsXY buttonA;
  PositionsXY buttonB;
  PositionsXY prize;
};

size_t CalcRequiredTokens(std::istream &input)
{
  size_t sum = 0;
  char c;
  while (input >> c)
  {
    sum += ClawMachine(input).CalcRequiredTokens();
  }
  return sum;
}

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};

  SECTION("Part 1")
  {
    REQUIRE(480u == CalcRequiredTokens(testInput));
  }
}

TEST_CASE("Task day 13")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 13 - part 1 result: " << CalcRequiredTokens(data) << std::endl;
  }

  // SECTION("part 2")
  // {
  //   std::cout << "Day 11 - part 2 result: " << splitter.CountStones(75) << std::endl;
  // }
}
