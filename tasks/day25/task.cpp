#include <catch2/catch_all.hpp>
#include <fstream>
#include <ranges>
#include <utils/Timer.h>

constexpr auto testData = R"(#####
.####
.####
.####
.#.#.
.#...
.....

#####
##.##
.#.##
...##
...#.
...#.
.....

.....
#....
#....
#...#
#.#.#
#.###
#####

.....
.....
#.#..
###..
###.#
###.#
#####

.....
.....
.....
#....
#.#..
#.#.#
#####)";

using Schema = std::vector<std::vector<int>>;

std::pair<Schema, Schema> ReadKeysAndLocks(std::istream &input)
{
  Schema locks;
  Schema keys;

  std::string line;

  while (input.good())
  {
    std::vector<int> elems(5, 0);
    std::string lastLine;

    while (std::getline(input, line) && !line.empty())
    {
      lastLine = line;
      for (size_t i = 0; i < line.size(); ++i)
      {
        if (line[i] == '#')
        {
          ++elems[i];
        }
      }
    }
    if (lastLine == "#####")
    {
      keys.push_back(elems);
    }
    else
    {
      locks.push_back(elems);
    }
  }
  return {keys, locks};
}

size_t CountFittingConfigurations(const Schema &keys, const Schema &locks)
{
  size_t count = 0;

  for (const auto &key : keys)
  {
    for (const auto &lock : locks)
    {
      bool isValid = true;
      for (size_t i = 0; i < lock.size(); ++i)
      {
        if (lock[i] + key[i] >= 8)
        {
          isValid = false;
          break;
        }
      }
      if (isValid)
        ++count;
    }
  }

  return count;
}

TEST_CASE("Check with test data - part 1")
{
  std::stringstream testInput{testData};

  const auto &[keys, locks] = ReadKeysAndLocks(testInput);

  REQUIRE(3 == keys.size());
  REQUIRE(2 == locks.size());

  REQUIRE(3 == CountFittingConfigurations(keys, locks));
}

TEST_CASE("Task day 25")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());
  const auto &[keys, locks] = ReadKeysAndLocks(data);

  SECTION("Day 25 - part 1")
  {
    std::cout << "Day 25 - part 1 result: " << CountFittingConfigurations(keys, locks) << std::endl;
  }
}