#include <catch2/catch_all.hpp>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>

constexpr auto testData = R"(3   4
4   3
2   5
1   3
3   9
3   3)";

using DataColumn = std::multiset<uint32_t>;
using DataColumns = std::pair<DataColumn, DataColumn>;

DataColumns ReadColumns(std::istream &input)
{
  DataColumn left;
  DataColumn right;

  uint32_t lval = 0;
  uint32_t rval = 0;

  while (input >> lval >> rval)
  {
    left.insert(lval);
    right.insert(rval);
  }

  return {left, right};
}

uint32_t SumDistances(const DataColumn &left, const DataColumn &right)
{
  return std::inner_product(left.begin(), left.end(), right.begin(), uint32_t(0),
                            std::plus<>(),
                            [](uint32_t a, uint32_t b)
                            {
                              return std::max(a, b) - std::min(a, b);
                            });
}

uint32_t CalculateSimilarity(const DataColumn &left, const DataColumn &right)
{
  return std::accumulate(left.begin(), left.end(), uint32_t(0), [&right](auto acc, const auto &val)
                         { return acc + val * right.count(val); });
}

TEST_CASE("Read columns")
{
  std::stringstream testInput{testData};
  DataColumn expectedLeft{1, 2, 3, 3, 3, 4};
  DataColumn expectedRight{3, 3, 3, 4, 5, 9};

  const auto [left, right] = ReadColumns(testInput);
  REQUIRE(expectedLeft == left);
  REQUIRE(expectedRight == right);
}

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  const auto [left, right] = ReadColumns(testInput);

  SECTION("Sum distances")
  {
    REQUIRE(11u == SumDistances(left, right));
  }

  SECTION("Calculate similarity")
  {
    REQUIRE(31u == CalculateSimilarity(left, right));
  }
}

TEST_CASE("Task day 1")
{
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());
  const auto [left, right] = ReadColumns(data);

  SECTION("part 1")
  {
    std::cout << "Day 1 - part 1 result: " << SumDistances(left, right) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 1 - part 2 result: " << CalculateSimilarity(left, right) << std::endl;
  }
}