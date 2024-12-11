#include <catch2/catch_all.hpp>
#include <fstream>
#include <utils/Timer.h>

constexpr auto testData = "125 17";

struct StoneToBlinks
{
  uint64_t id;
  int blinks;

  auto operator<=>(const StoneToBlinks &) const = default;
};

namespace std
{
  template <>
  struct hash<StoneToBlinks>
  {
    std::size_t operator()(const StoneToBlinks &stone) const
    {
      std::size_t h1 = std::hash<uint64_t>{}(stone.id);
      std::size_t h2 = std::hash<int>{}(stone.blinks);

      return h1 ^ (h2 << 1);
    }
  };
}

class StonesSplitter
{
public:
  StonesSplitter(std::istream &input)
  {
    uint64_t val;
    while (input >> val)
    {
      ids.emplace_back(val);
    }
  }

  std::vector<uint64_t> SplitStone(uint64_t id)
  {
    size_t digitsNumber = 0;
    auto tmpId = id;
    while (tmpId != 0)
    {
      tmpId /= 10;
      ++digitsNumber;
    }

    std::vector<uint64_t> splitted;
    if (id == 0)
    {
      splitted.emplace_back(1);
    }
    else if (digitsNumber % 2 == 0)
    {
      uint64_t factor = 1;
      for (size_t i = 0; i < digitsNumber / 2; i++)
      {
        factor *= 10;
      }
      splitted.emplace_back(id / factor);
      splitted.emplace_back(id - (id / factor) * factor);
    }
    else
    {
      splitted.emplace_back(id * 2024);
    }
    return splitted;
  }

  uint64_t CountSplittedStones(const std::vector<uint64_t> &stonesIds, int blinksLeft)
  {
    uint64_t sum = 0;
    for (const auto &id : stonesIds)
    {
      if (blinksLeft == 0)
      {
        sum += 1;
      }
      else if (cahceBlinks.contains(StoneToBlinks{id, blinksLeft}))
      {
        sum += cahceBlinks[StoneToBlinks{id, blinksLeft}];
      }
      else
      {
        const auto count = CountSplittedStones(SplitStone(id), blinksLeft - 1);
        cahceBlinks[StoneToBlinks{id, blinksLeft}] = count;
        sum += count;
      }
    }
    return sum;
  }

  uint64_t CountStones(int blinks)
  {
    return CountSplittedStones(ids, blinks);
  }

  std::vector<uint64_t> ids;
  std::unordered_map<StoneToBlinks, uint64_t> cahceBlinks;
};

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  StonesSplitter splitter{testInput};

  SECTION("Part 1")
  {
    REQUIRE(55312u == splitter.CountStones(25));
  }
}

TEST_CASE("Task day 11")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  StonesSplitter splitter{data};

  SECTION("part 1")
  {
    std::cout << "Day 11 - part 1 result: " << splitter.CountStones(25) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 11 - part 2 result: " << splitter.CountStones(75) << std::endl;
  }
}
