#include <catch2/catch_all.hpp>
#include <fstream>
#include <utils/Timer.h>
#include <regex>

constexpr auto testData = R"(p=0,4 v=3,-3
p=6,3 v=-1,-3
p=10,3 v=-1,2
p=2,0 v=2,-1
p=0,0 v=1,3
p=3,0 v=-2,-2
p=7,6 v=-1,-3
p=3,0 v=-1,-2
p=9,3 v=2,3
p=7,3 v=-1,2
p=2,4 v=2,-3
p=9,5 v=-3,-3)";

struct Robot
{
  Robot(const std::string &line, int64_t limitX_, int64_t limitY_)
      : limitX(limitX_), limitY(limitY_)
  {
    std::regex pattern(R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+))");
    std::smatch matches;

    if (std::regex_search(line, matches, pattern))
    {
      startPosition.first = std::stoi(matches[1].str());
      startPosition.second = std::stoi(matches[2].str());
      velocity.first = std::stoi(matches[3].str());
      velocity.second = std::stoi(matches[4].str());
    }
  }

  std::pair<int64_t, int64_t> GetPositionAfterSteps(int steps)
  {
    auto currentPosition = startPosition;
    currentPosition.first = (currentPosition.first + steps * velocity.first) % limitX;
    currentPosition.second = (currentPosition.second + steps * velocity.second) % limitY;

    currentPosition.first = currentPosition.first >= 0 ? currentPosition.first : limitX + currentPosition.first;
    currentPosition.second = currentPosition.second >= 0 ? currentPosition.second : limitY + currentPosition.second;

    return currentPosition;
  }

  std::pair<int64_t, int64_t> startPosition;
  std::pair<int64_t, int64_t> velocity;
  const int64_t limitX;
  const int64_t limitY;
};

size_t CountRobotsInQuadrants(std::istream &input, int limitX, int limitY)
{
  size_t q1 = 0;
  size_t q2 = 0;
  size_t q3 = 0;
  size_t q4 = 0;
  std::string line;
  while (std::getline(input, line))
  {
    const auto pos = Robot(line, limitX, limitY).GetPositionAfterSteps(100);
    if (pos.first < limitX / 2 && pos.second < limitY / 2)
    {
      ++q1;
    }
    else if (pos.first > limitX / 2 && pos.second < limitY / 2)
    {
      ++q2;
    }
    else if (pos.first < limitX / 2 && pos.second > limitY / 2)
    {
      ++q3;
    }
    else if (pos.first > limitX / 2 && pos.second > limitY / 2)
    {
      ++q4;
    }
  }
  return q1 * q2 * q3 * q4;
}

void PrintPositions(int limitX, int limitY, int iterations)
{
  Timer t;
  std::string line;
  for (int i = 0; i < iterations; ++i)
  {
    std::cout << "Iteration: " << i << std::endl;
    std::ifstream data("data.txt");
    std::set<std::pair<int, int>> points;
    while (std::getline(data, line))
    {
      points.emplace(Robot(line, limitX, limitY).GetPositionAfterSteps(i));
    }
    for (int x = 0; x < limitX; ++x)
    {
      for (int y = 0; y < limitY; ++y)
      {
        if (points.contains({x, y}))
        {
          std::cout << '*';
        }
        else
        {
          std::cout << '.';
        }
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}

TEST_CASE("Check witch example data")
{
  std::stringstream testInput{testData};
  REQUIRE(12 == CountRobotsInQuadrants(testInput, 11, 7));
}

TEST_CASE("Task day 14")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 14 - part 1 result: " << CountRobotsInQuadrants(data, 101, 103) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 14 - part 2 result: " << std::endl;
    PrintPositions(101, 103, 10000);
  }
}
