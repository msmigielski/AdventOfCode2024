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
  std::size_t operator()(const std::pair<int64_t, int64_t> &pair) const
  {
    std::size_t h1 = std::hash<int64_t>{}(pair.first);
    std::size_t h2 = std::hash<int64_t>{}(pair.second);

    return h1 ^ (h2 << 1);
  }
};

struct ClawMachineWithLimits
{
  using PositionsXY = std::pair<int64_t, int64_t>;

  ClawMachineWithLimits(std::istream &input)
  {
    std::string line;
    std::getline(input, line);
    buttonA = ReadValues(line);
    std::getline(input, line);
    buttonB = ReadValues(line);
    std::getline(input, line);
    prize = ReadValues(line);
  }

  std::pair<int64_t, int64_t> ReadValues(const std::string &line)
  {
    std::regex pattern(R"(X[+=](\d+), Y[+=](\d+))");
    std::smatch matches;

    if (std::regex_search(line, matches, pattern))
    {
      int64_t x = std::stoi(matches[1].str());
      int64_t y = std::stoi(matches[2].str());
      return {x, y};
    }
    return {};
  }

  size_t CalcRequiredTokens()
  {
    std::pair<int64_t, int64_t> clicks;

    // a0 = X * a1 + Y * a2
    // b0 = X * b1 + Y * b2
    // X - clicks of button A
    // Y - clicks of button B
    // it is solution for X and Y

    int64_t yQuantifier = prize.second * buttonA.first - prize.first * buttonA.second;
    int64_t yDenominator = buttonA.first * buttonB.second - buttonA.second * buttonB.first;

    if (yDenominator == 0 || yQuantifier % yDenominator != 0)
    {
      return 0;
    }

    clicks.second = yQuantifier / yDenominator;

    int64_t xQuantifier = prize.second - clicks.second * buttonB.second;
    int64_t xDenominator = buttonA.second;

    if (xDenominator == 0 || xQuantifier % xDenominator != 0)
    {
      return 0;
    }

    clicks.first = xQuantifier / xDenominator;

    if (clicks.first > maxClicks || clicks.second > maxClicks)
    {
      return 0;
    }

    return clicks.first * costA + clicks.second * costB;
  }

  const int64_t costA = 3;
  const int64_t costB = 1;
  int64_t maxClicks = 100;

  PositionsXY buttonA;
  PositionsXY buttonB;
  PositionsXY prize;
};

struct ClawMachine : public ClawMachineWithLimits
{
  ClawMachine(std::istream &input) : ClawMachineWithLimits(input)
  {
    maxClicks = std::numeric_limits<int64_t>::max();
    prize.first += 10000000000000;
    prize.second += 10000000000000;
  }
};

size_t CalcRequiredTokensWithLimits(std::istream &input)
{
  size_t sum = 0;
  char c;
  while (input >> c)
  {
    sum += ClawMachineWithLimits(input).CalcRequiredTokens();
  }
  return sum;
}

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
    REQUIRE(480u == CalcRequiredTokensWithLimits(testInput));
  }
}

TEST_CASE("Task day 13")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 13 - part 1 result: " << CalcRequiredTokensWithLimits(data) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 13 - part 2 result: " << CalcRequiredTokens(data) << std::endl;
  }
}
