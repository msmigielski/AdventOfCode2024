#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <unordered_set>
#include <queue>

constexpr auto testDataFirst = R"(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############)";

struct Point
{
  int column;
  int row;

  Point operator+(const Point &other) const
  {
    return {column + other.column, row + other.row};
  }

  auto operator<=>(const Point &) const = default;

  Point PointUp() const
  {
    return Point{column, row - 1};
  }

  Point PointDown() const
  {
    return Point{column, row + 1};
  }

  Point PointLeft() const
  {
    return Point{column - 1, row};
  }
  Point PointRight() const
  {
    return Point{column + 1, row};
  }
};

namespace std
{
  template <>
  struct hash<Point>
  {
    size_t operator()(const Point &p) const
    {
      size_t h1 = std::hash<int>{}(p.row);
      size_t h2 = std::hash<int>{}(p.column);

      return h1 ^ (h2 << 1);
    }
  };
}

struct Map2
{
  Map2(std::istream &input)
  {
    std::string line;
    int row = 0;

    while (std::getline(input, line))
    {
      int column = 0;
      for (const auto &c : line)
      {
        const auto &p = Point{column, row};
        if (c == '#')
        {
          wallPoints.emplace(p);
        }
        else if (c == 'S')
        {
          startPosition = p;
        }
        else if (c == 'E')
        {
          endPosition = p;
        }
        ++column;
      }
      ++row;
    }
  }

  size_t Dijkstra()
  {
    std::queue<std::pair<int64_t, Point>> queue;
    queue.push({0, startPosition});
    visited[startPosition] = 0;

    while (!queue.empty())
    {
      const auto [cost, currentPoint] = queue.back();
      queue.pop();

      if (currentPoint == endPosition)
      {
        return cost;
      }

      const std::vector<Point> siblings{
          currentPoint.PointUp(),
          currentPoint.PointLeft(),
          currentPoint.PointRight(),
          currentPoint.PointDown()};

      for (const auto &sibling : siblings)
      {
        if (wallPoints.contains(sibling) || visited.contains(sibling))
        {
          continue;
        }

        visited[sibling] = cost + 1;
        queue.push({cost + 1, sibling});
      }
    }

    return std::numeric_limits<size_t>::max();
  }

  size_t CountShortcutsWithDiff(int64_t diff)
  {
    size_t count = 0;
    for (const auto &[point, cost] : visited)
    {
      const std::vector<Point> siblings{
          point.PointUp().PointUp(),
          point.PointLeft().PointLeft(),
          point.PointRight().PointRight(),
          point.PointDown().PointDown()};
      for (const auto &sibling : siblings)
      {
        if (!visited.contains(sibling))
        {
          continue;
        }
        if (visited[sibling] - cost - 2 > diff)
        {
          ++count;
        }
      }
    }
    return count;
  }

  size_t CountLongerShortcutsWithDiff(int64_t diff)
  {
    size_t count = 0;
    for (const auto &[point, cost] : visited)
    {
      for (const auto &[goalPoint, goalCost] : visited)
      {
        int64_t distance = std::abs(goalPoint.column - point.column) + std::abs(goalPoint.row - point.row);
        if (distance >= 2 && distance <= 20 && (goalCost - cost - distance) > diff)
        {
          ++count;
        }
      }
    }
    return count;
  }

  std::unordered_map<Point, int64_t> visited;
  std::unordered_set<Point> wallPoints;
  Point startPosition;
  Point endPosition;
};

TEST_CASE("Check with test data")
{
  SECTION("First example")
  {
    std::stringstream testInput{testDataFirst};
    Map2 map{testInput};

    SECTION("Part 1")
    {
      map.Dijkstra();
      REQUIRE(4 == map.CountShortcutsWithDiff(20));
    }
  }
}

TEST_CASE("Task day 20")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    Map2 map{data};
    map.Dijkstra();
    std::cout << "Day 20 - part 1 result: " << map.CountShortcutsWithDiff(99) << std::endl;
  }

  SECTION("part 2")
  {
    Map2 map{data};
    map.Dijkstra();
    std::cout << "Day 20 - part 2 result: " << map.CountLongerShortcutsWithDiff(99) << std::endl;
  }
}