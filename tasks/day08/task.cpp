#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <numeric>
#include <unordered_set>
#include <utils/Timer.h>

constexpr auto testData = R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............)";

struct Point
{
  int row;
  int column;

  auto operator<=>(const Point &) const = default;
};

namespace std
{
  template <>
  struct hash<Point>
  {
    int operator()(const Point &p) const
    {
      int h1 = std::hash<int>{}(p.row);
      int h2 = std::hash<int>{}(p.column);

      return h1 ^ (h2 << 1);
    }
  };
}

struct AntennaMap
{
  AntennaMap(std::istream &input)
  {
    std::string line;

    int row = 0;

    while (std::getline(input, line))
    {
      int column = 0;
      for (const auto &c : line)
      {
        if (c != '.')
        {
          antennaPoints.try_emplace({row, column}, c);
        }
        ++column;
      }
      ++row;
      max_column = line.size() - 1;
    }
    max_row = row - 1;
  }

  int CountAntinodes()
  {
    std::unordered_set<Point> antinodes;
    for (const auto &[point, type] : antennaPoints)
    {
      for (const auto &[otherPoint, otherType] : antennaPoints)
      {
        if (point == otherPoint || type != otherType)
        {
          continue;
        }

        int diffX = otherPoint.column - point.column;
        int diffY = otherPoint.row - point.row;

        Point p = {otherPoint.row + diffY, otherPoint.column + diffX};
        if (p.row >= 0 && p.row <= max_row && p.column >= 0 && p.column <= max_column)
        {
          antinodes.insert(p);
        }
      }
    }

    return static_cast<int>(antinodes.size());
  }

  int CountAntinodesWithHarmonics()
  {
    std::unordered_set<Point> antinodes;
    for (const auto &[point, type] : antennaPoints)
    {
      for (const auto &[otherPoint, otherType] : antennaPoints)
      {
        if (point == otherPoint || type != otherType)
        {
          continue;
        }

        int diffX = otherPoint.column - point.column;
        int diffY = otherPoint.row - point.row;

        for (Point p = {otherPoint.row, otherPoint.column};
             p.row >= 0 && p.row <= max_row && p.column >= 0 && p.column <= max_column;
             p.row += diffY, p.column += diffX)
        {
          antinodes.insert(p);
        }
      }
    }

    return static_cast<int>(antinodes.size());
  }

  int max_column;
  int max_row;

  std::unordered_map<Point, char> antennaPoints;
};

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  AntennaMap map{testInput};

  SECTION("Part 1")
  {
    REQUIRE(14 == map.CountAntinodes());
  }

  SECTION("Part 2")
  {
    REQUIRE(34 == map.CountAntinodesWithHarmonics());
  }
}

TEST_CASE("Task day 8")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  AntennaMap map{data};

  SECTION("part 1")
  {
    std::cout << "Day 8 - part 1 result: " << map.CountAntinodes() << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 8 - part 2 result: " << map.CountAntinodesWithHarmonics() << std::endl;
  }
}