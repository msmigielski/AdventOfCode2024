#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <unordered_set>
#include <queue>

constexpr auto testData = R"(5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0)";

struct Point
{
  Point() = default;
  Point(int x, int y) : column(x), row(y) {}
  Point(const std::string &line)
  {
    auto pos = line.find(',');
    column = std::stoi(line.substr(0, pos));
    row = std::stoi(line.substr(pos + 1));
  }

  int column;
  int row;

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
    int operator()(const Point &p) const
    {
      int h2 = std::hash<int>{}(p.column);
      int h1 = std::hash<int>{}(p.row);

      return h1 ^ (h2 << 1);
    }
  };
}

struct ComparePair
{
  bool operator()(const std::pair<int64_t, Point> &a, const std::pair<int64_t, Point> &b)
  {
    return a.first > b.first;
  }
};

struct Map
{
  Map(std::istream &input, int xLimit_, int yLimit_, size_t numOfObstacles) : xLimit(xLimit_), yLimit(yLimit_), endPosition(Point{xLimit, yLimit})
  {
    std::string line;
    size_t num = 0;

    while (std::getline(input, line) && num < numOfObstacles)
    {
      last = Point{line};
      wallPoints.emplace(last);
      ++num;
    }
  }

  size_t Dijkstra()
  {
    std::priority_queue<std::pair<int64_t, Point>, std::vector<std::pair<int64_t, Point>>, ComparePair> queue;
    queue.push({0, startPosition});
    std::unordered_map<Point, int64_t> visited;

    while (!queue.empty())
    {
      const auto [cost, point] = queue.top();
      queue.pop();

      if (point == endPosition)
      {
        return cost;
      }

      if (visited.contains(point) && visited.at(point) <= cost)
      {
        continue;
      }
      visited[point] = cost;

      if (const auto &p = point.PointUp();
          point.row > 0 && !wallPoints.contains(p))
      {
        queue.push({cost + 1, p});
      }

      if (const auto &p = point.PointDown();
          point.row < yLimit && !wallPoints.contains(p))
      {
        queue.push({cost + 1, p});
      }

      if (const auto &p = point.PointLeft();
          point.column > 0 && !wallPoints.contains(p))
      {
        queue.push({cost + 1, p});
      }

      if (const auto &p = point.PointRight();
          point.column < xLimit && !wallPoints.contains(p))
      {
        queue.push({cost + 1, p});
      }
    }

    return std::numeric_limits<size_t>::max();
  }

  std::unordered_set<Point> wallPoints;
  int xLimit = 0;
  int yLimit = 0;
  Point startPosition{0, 0};
  Point endPosition;
  Point last;
};

TEST_CASE("point")
{
  Point p{11, 2};
  Point p2{"11,2"};
  REQUIRE(p == p2);
}

TEST_CASE("Check with test data")
{
  SECTION("Part 1")
  {
    std::stringstream testInput{testData};
    Map map{testInput, 6, 6, 12};
    REQUIRE(22 == map.Dijkstra());
  }

  SECTION("Part 2")
  {
    Point result;
    Point expected{6, 1};
    for (size_t i = 1; i < 30; ++i)
    {
      std::stringstream testInput{testData};
      Map map{testInput, 6, 6, i};
      if (size_t path = map.Dijkstra();
          path == std::numeric_limits<size_t>::max())
      {
        result = map.last;
        break;
      }
    }
    CHECK(expected.row == result.row);
    CHECK(expected.column == result.column);
  }
}

TEST_CASE("Task day 18")
{
  Timer t;

  SECTION("part 1")
  {
    std::ifstream data("data.txt");
    REQUIRE(data.is_open());
    Map map{data, 70, 70, 1024};
    std::cout << "Day 18 - part 1 result: " << map.Dijkstra() << std::endl;
  }

  SECTION("part 2")
  {
    Point result;

    for (size_t i = 1; i < 3500; ++i)
    {
      std::ifstream data("data.txt");
      REQUIRE(data.is_open());
      Map map{data, 70, 70, i};
      if (size_t path = map.Dijkstra();
          path == std::numeric_limits<size_t>::max())
      {
        result = map.last;
        break;
      }
    }

    std::cout << "Day 15 - part 2 result: " << result.column << ',' << result.row << std::endl;
  }
}