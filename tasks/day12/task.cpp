#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <unordered_set>
#include <fstream>

struct Point
{
  int row;
  int column;

  auto operator<=>(const Point &) const = default;

  Point PointUp() const
  {
    return Point{row + 1, column};
  }

  Point PointUpLeft() const
  {
    return Point{row + 1, column - 1};
  }

  Point PointUpRight() const
  {
    return Point{row + 1, column + 1};
  }

  Point PointDown() const
  {
    return Point{row - 1, column};
  }

  Point PointDownLeft() const
  {
    return Point{row - 1, column - 1};
  }

  Point PointDownRight() const
  {
    return Point{row - 1, column + 1};
  }

  Point PointLeft() const
  {
    return Point{row, column - 1};
  }
  Point PointRight() const
  {
    return Point{row, column + 1};
  }
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

struct Group
{
  std::unordered_set<Point> points;

  size_t GetPerimeter() const
  {
    size_t perimeter = 0;
    for (const auto &p : points)
    {
      std::vector<Point> siblings{p.PointDown(), p.PointLeft(), p.PointRight(), p.PointUp()};

      for (const auto &sibling : siblings)
      {
        if (!points.contains(sibling))
        {
          ++perimeter;
        }
      }
    }
    return perimeter;
  }

  std::unordered_map<std::string, Point> GetSiblings(const Point &p) const
  {
    std::unordered_map<std::string, Point> siblings{
        {"UpLeft", p.PointUpLeft()},
        {"Up", p.PointUp()},
        {"UpRight", p.PointUpRight()},
        {"Left", p.PointLeft()},
        {"Right", p.PointRight()},
        {"Down", p.PointDown()},
        {"DownLeft", p.PointDownLeft()},
        {"DownRight", p.PointDownRight()}};
    return siblings;
  }

  bool IsOutCorner(const Point &horizontal, const Point &vertical) const
  {
    return !points.contains(horizontal) && !points.contains(vertical);
  }

  bool IsInCorner(const Point &horizontal, const Point &vertical, const Point &diagonal) const
  {
    return points.contains(horizontal) && points.contains(vertical) && !points.contains(diagonal);
  }

  size_t CountCorners(const Point &p) const
  {
    const auto &siblings = GetSiblings(p);
    size_t corners = 0;
    corners += IsOutCorner(siblings.at("Left"), siblings.at("Up")) || IsInCorner(siblings.at("Left"), siblings.at("Up"), siblings.at("UpLeft"));
    corners += IsOutCorner(siblings.at("Right"), siblings.at("Up")) || IsInCorner(siblings.at("Right"), siblings.at("Up"), siblings.at("UpRight"));
    corners += IsOutCorner(siblings.at("Left"), siblings.at("Down")) || IsInCorner(siblings.at("Left"), siblings.at("Down"), siblings.at("DownLeft"));
    corners += IsOutCorner(siblings.at("Right"), siblings.at("Down")) || IsInCorner(siblings.at("Right"), siblings.at("Down"), siblings.at("DownRight"));
    return corners;
  }

  size_t GetSides() const
  {
    if (points.size() == 1 || points.size() == 2)
    {
      return 4;
    }

    size_t sides = 0;

    for (const auto &p : points)
    {
      sides += CountCorners(p);
    }

    return sides;
  }
};

struct GardenMap
{
  GardenMap(std::istream &input)
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
          pointsHeights.emplace(Point{row, column}, c);
        }
        ++column;
      }
      ++row;
    }

    for (const auto &[p, type] : pointsHeights)
    {
      if (InAnyGroup(p))
      {
        continue;
      }
      CreateGroup(p);
    }
  }

  bool InAnyGroup(const Point &p)
  {
    for (const auto &g : groups)
    {
      if (g.points.contains(p))
      {
        return true;
      }
    }
    return false;
  }

  void CreateGroup(const Point &p)
  {
    Group g;
    AddGroupMembers(p, g);

    groups.emplace_back(g);
  }

  void AddGroupMembers(const Point &p, Group &group)
  {
    group.points.emplace(p);

    std::vector<Point> siblings{p.PointDown(), p.PointLeft(), p.PointRight(), p.PointUp()};

    for (const auto &sibling : siblings)
    {
      if (pointsHeights.contains(sibling) && pointsHeights[p] == pointsHeights[sibling] && !group.points.contains(sibling))
      {
        AddGroupMembers(sibling, group);
      }
    }
  }

  size_t CalcFencePrice()
  {
    size_t price = 0;
    for (const auto &group : groups)
    {
      price += group.points.size() * group.GetPerimeter();
    }
    return price;
  }

  size_t CalcFencePriceWithDiscunt()
  {
    size_t price = 0;
    for (const auto &group : groups)
    {
      price += group.points.size() * group.GetSides();
    }
    return price;
  }

  std::unordered_map<Point, char> pointsHeights;
  std::vector<Group> groups;
};

TEST_CASE("Check with test data")
{
  SECTION("Example 1")
  {
    constexpr auto testData = R"(AAAA
BBCD
BBCC
EEEC)";
    std::stringstream testInput{testData};
    GardenMap map{testInput};

    SECTION("Part 1")
    {
      REQUIRE(140u == map.CalcFencePrice());
    }
    SECTION("Part 2")
    {
      REQUIRE(80u == map.CalcFencePriceWithDiscunt());
    }
  }
}

//     SECTION("Example 2")
//     {
//       constexpr auto testData = R"(RRRRIICCFF
// RRRRIICCCF
// VVRRRCCFFF
// VVRCCCJFFF
// VVVVCJJCFE
// VVIVCCJJEE
// VVIIICJJEE
// MIIIIIJJEE
// MIIISIJEEE
// MMMISSJEEE)";
//       std::stringstream testInput{testData};
//       GardenMap map{testInput};

//       SECTION("Part 1")
//       {
//         REQUIRE(11u == map.CountGroups());
//       }
//     }
// }
// SECTION("Part 2")
// {
//   REQUIRE(81 == map.CalcTrailheadsRating());
// }
// }

TEST_CASE("Task day 12")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  GardenMap map{data};

  SECTION("part 1")
  {
    std::cout << "Day 12 - part 1 result: " << map.CalcFencePrice() << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 12 - part 2 result: " << map.CalcFencePriceWithDiscunt() << std::endl;
  }
}