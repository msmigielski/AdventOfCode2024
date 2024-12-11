#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>

constexpr auto testData = R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)";

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

struct HikingMap
{
  HikingMap(std::istream &input)
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
          pointsHeights.try_emplace({row, column}, c - '0');
        }
        ++column;
      }
      ++row;
    }
  }

  void FindTopsForTrailhead(const Point &currentPoint, std::set<Point> &tops)
  {
    if (pointsHeights[currentPoint] == 9)
    {
      tops.emplace(currentPoint);
      return;
    }
    if (Point upPoint{currentPoint.row + 1, currentPoint.column};
        IsHigherThan(upPoint, currentPoint))
    {
      FindTopsForTrailhead(upPoint, tops);
    }
    if (Point rightPoint{currentPoint.row, currentPoint.column + 1};
        IsHigherThan(rightPoint, currentPoint))
    {
      FindTopsForTrailhead(rightPoint, tops);
    }
    if (Point bottomPoint{currentPoint.row - 1, currentPoint.column};
        IsHigherThan(bottomPoint, currentPoint))
    {
      FindTopsForTrailhead(bottomPoint, tops);
    }
    if (Point leftPoint{currentPoint.row, currentPoint.column - 1};
        IsHigherThan(leftPoint, currentPoint))
    {
      FindTopsForTrailhead(leftPoint, tops);
    }
  }

  int CalcTrailheadRating(const auto &currentPoint)
  {
    int sum = 0;
    if (pointsHeights[currentPoint] == 9)
    {
      return 1;
    }
    if (Point upPoint{currentPoint.row + 1, currentPoint.column};
        IsHigherThan(upPoint, currentPoint))
    {
      sum += CalcTrailheadRating(upPoint);
    }
    if (Point rightPoint{currentPoint.row, currentPoint.column + 1};
        IsHigherThan(rightPoint, currentPoint))
    {
      sum += CalcTrailheadRating(rightPoint);
    }
    if (Point bottomPoint{currentPoint.row - 1, currentPoint.column};
        IsHigherThan(bottomPoint, currentPoint))
    {
      sum += CalcTrailheadRating(bottomPoint);
    }
    if (Point leftPoint{currentPoint.row, currentPoint.column - 1};
        IsHigherThan(leftPoint, currentPoint))
    {
      sum += CalcTrailheadRating(leftPoint);
    }
    return sum;
  }

  bool IsHigherThan(const Point &l, const Point &r)
  {
    return pointsHeights.contains(l) && (pointsHeights[l] > pointsHeights[r]) && (pointsHeights[l] - pointsHeights[r]) == 1;
  }

  int CalcTrailheadsRating()
  {
    int sum = 0;
    for (const auto &[point, height] : pointsHeights)
    {
      if (height == 0)
      {
        sum += CalcTrailheadRating(point);
      }
    }
    return sum;
  }

  int CountTopsForTrailhead()
  {
    int sum = 0;
    for (const auto &[point, height] : pointsHeights)
    {
      if (height == 0)
      {
        std::set<Point> tops;
        FindTopsForTrailhead(point, tops);
        sum += static_cast<int>(tops.size());
      }
    }
    return sum;
  }

  std::unordered_map<Point, int> pointsHeights;
};

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  HikingMap map{testInput};

  SECTION("Part 1")
  {
    REQUIRE(36 == map.CountTopsForTrailhead());
  }

  SECTION("Part 2")
  {
    REQUIRE(81 == map.CalcTrailheadsRating());
  }
}

TEST_CASE("Task day 10")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  HikingMap map{data};

  SECTION("part 1")
  {
    std::cout << "Day 10 - part 1 result: " << map.CountTopsForTrailhead() << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 10 - part 2 result: " << map.CalcTrailheadsRating() << std::endl;
  }
}