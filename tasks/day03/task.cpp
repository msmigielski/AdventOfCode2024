#include <catch2/catch_all.hpp>
#include <regex>
#include <iostream>
#include <fstream>

const std::string testData = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
const std::string testDaraWithStates = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";

int SumMuls(std::istream &input)
{
  std::regex mulRegex("mul\\((\\d+),(\\d+)\\)");
  int sum = 0;
  std::string line;

  while (std::getline(input, line))
  {
    std::smatch match;
    std::string::const_iterator search_start(line.cbegin());

    while (std::regex_search(search_start, line.cend(), match, mulRegex))
    {
      int x = std::stoi(match[1].str());
      int y = std::stoi(match[2].str());
      sum += x * y;

      search_start = match.suffix().first;
    }
  }

  return sum;
}

int SumMulsWithStates(std::istream &input)
{
  bool enabled = true;
  std::regex keyRegex("(don't)|(do)|mul\\((\\d+),(\\d+)\\)");
  int sum = 0;
  std::string line;

  while (std::getline(input, line))
  {
    std::smatch match;
    std::string::const_iterator search_start(line.cbegin());

    while (std::regex_search(search_start, line.cend(), match, keyRegex))
    {
      if (match.str() == "do")
      {
        enabled = true;
      }
      else if (match.str() == "don't")
      {
        enabled = false;
      }
      else if (enabled)
      {
        int x = std::stoi(match[3].str());
        int y = std::stoi(match[4].str());
        sum += x * y;
      }
      search_start = match.suffix().first;
    }
  }

  return sum;
}

TEST_CASE("Check sum muls for test data")
{
  std::stringstream input{testData};
  REQUIRE(SumMuls(input) == 161);
}

TEST_CASE("Check sum muls for test data with states")
{
  std::stringstream input{testDaraWithStates};
  REQUIRE(SumMulsWithStates(input) == 48);
}

TEST_CASE("Task day 3")
{
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 3 - part 1 result: " << SumMuls(data) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 3 - part 2 result: " << SumMulsWithStates(data) << std::endl;
  }
}