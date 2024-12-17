#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <unordered_set>
#include <queue>

constexpr auto testDataFirst = R"(#################
#...#...#...#..E#
#.#.#.#.#.#.#.#.#
#.#.#.#...#...#.#
#.#.#.#.###.#.#.#
#...#.#.#.....#.#
#.#.#.#.#.#####.#
#.#...#.#.#.....#
#.#.#####.#.###.#
#.#.#.......#...#
#.#.###.#####.###
#.#.#...#.....#.#
#.#.#.#####.###.#
#.#.#.........#.#
#.#.#.#########.#
#S#.............#
#################)";

enum class Direction : int
{
  N = 0,
  E = 1,
  S = 2,
  W = 3
};

struct Point
{
  int column;
  int row;

  Point operator+(const Point &other) const
  {
    return {column + other.column, row + other.row};
  }

  auto operator<=>(const Point &) const = default;
};

const std::unordered_map<Direction, Point> directionDiff{
    {Direction::N, Point{0, -1}},
    {Direction::E, Point{1, 0}},
    {Direction::S, Point{0, 1}},
    {Direction::W, Point{-1, 0}}};

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

struct PointWithDirection
{
  Point point;
  Direction dir;

  PointWithDirection MoveForward() const
  {
    return {point + directionDiff.at(dir), dir};
  }

  PointWithDirection TurnRight() const
  {
    return {point, Direction{(static_cast<int>(dir) + 1) % 4}};
  }

  PointWithDirection TurnLeft() const
  {
    return {point, Direction{(static_cast<int>(dir) + 3) % 4}};
  }

  auto operator<=>(const PointWithDirection &) const = default;
};

namespace std
{
  template <>
  struct hash<PointWithDirection>
  {
    size_t operator()(const PointWithDirection &pwd) const
    {
      size_t h1 = std::hash<Point>{}(pwd.point);
      size_t h2 = std::hash<int>{}(static_cast<int>(pwd.dir));
      return h1 ^ (h2 << 1);
    }
  };
}

struct ComparePair
{
  bool operator()(const std::pair<int64_t, PointWithDirection> &a, const std::pair<int64_t, PointWithDirection> &b)
  {
    return a.first > b.first;
  }
};
struct Map
{
  Map(std::istream &input)
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
    std::priority_queue<std::pair<int64_t, PointWithDirection>, std::vector<std::pair<int64_t, PointWithDirection>>, ComparePair> queue;
    queue.push({0, {startPosition, Direction::E}});
    std::unordered_map<PointWithDirection, int64_t> visited;

    while (!queue.empty())
    {
      const auto [cost, pointWithDirection] = queue.top();
      queue.pop();

      if (pointWithDirection.point == endPosition)
      {
        return cost;
      }

      if (visited.contains(pointWithDirection) && visited.at(pointWithDirection) <= cost)
      {
        continue;
      }
      visited[pointWithDirection] = cost;

      const auto &pointForward = pointWithDirection.MoveForward();
      if (!wallPoints.contains(pointForward.point))
      {
        queue.push({cost + 1, pointForward});
      }

      const auto &pointLeft = pointWithDirection.TurnLeft();
      queue.push({cost + 1000, pointLeft});

      const auto &pointRight = pointWithDirection.TurnRight();
      queue.push({cost + 1000, pointRight});
    }

    return std::numeric_limits<size_t>::max();
  }

  std::unordered_set<Point> wallPoints;
  Point startPosition;
  Point endPosition;
};

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
    std::priority_queue<std::pair<int64_t, PointWithDirection>, std::vector<std::pair<int64_t, PointWithDirection>>, ComparePair> queue;
    queue.push({0, {startPosition, Direction::E}});
    std::unordered_map<PointWithDirection, int64_t> visited;
    visited[{startPosition, Direction::E}] = 0;

    while (!queue.empty())
    {
      const auto [cost, pointWithDirection] = queue.top();
      queue.pop();

      if (visited.contains(pointWithDirection) && visited.at(pointWithDirection) < cost)
      {
        continue;
      }

      const auto &pointForward = pointWithDirection.MoveForward();
      const auto &pointLeft = pointWithDirection.TurnLeft();
      const auto &pointRight = pointWithDirection.TurnRight();

      if (!wallPoints.contains(pointForward.point))
      {
        if (int64_t newCost = cost + 1;
            !visited.contains(pointForward) || newCost < visited[pointForward])
        {
          visited[pointForward] = newCost;
          queue.push({newCost, pointForward});
          predecessors[pointForward] = {pointWithDirection};
        }
        else if (newCost == visited[pointForward])
        {
          predecessors[pointForward].push_back(pointWithDirection);
        }
      }

      if (int64_t newCost = cost + 1000;
          !visited.contains(pointLeft) || newCost < visited[pointLeft])
      {
        visited[pointLeft] = newCost;
        queue.push({newCost, pointLeft});
        predecessors[pointLeft] = {pointWithDirection};
      }
      else if (newCost == visited[pointLeft])
      {
        predecessors[pointLeft].push_back(pointWithDirection);
      }

      if (int64_t newCost = cost + 1000;
          !visited.contains(pointRight) || newCost < visited[pointRight])
      {
        visited[pointRight] = newCost;
        queue.push({newCost, pointRight});
        predecessors[pointRight] = {pointWithDirection};
      }
      else if (newCost == visited[pointRight])
      {
        predecessors[pointRight].push_back(pointWithDirection);
      }
    }

    if (visited.at({endPosition, Direction::E}) < visited.at({endPosition, Direction::N}))
    {
      solution = {endPosition, Direction::E};
    }
    else
    {
      solution = {endPosition, Direction::N};
    }

    return std::numeric_limits<size_t>::max();
  }

  size_t CountOptimalPoints()
  {
    std::unordered_set<Point> uniques;
    GetPredecessors(solution, uniques);
    uniques.emplace(endPosition);
    uniques.emplace(startPosition);
    return uniques.size();
  }

  void GetPredecessors(const PointWithDirection &p, std::unordered_set<Point> &uniques)
  {
    if (predecessors.contains(p))
    {
      for (const auto &pre : predecessors.at(p))
      {
        uniques.emplace(pre.point);
        GetPredecessors(pre, uniques);
      }
    }
  }

  PointWithDirection solution;
  std::unordered_set<Point> wallPoints;
  std::unordered_map<PointWithDirection, std::vector<PointWithDirection>> predecessors;
  Point startPosition;
  Point endPosition;
};

TEST_CASE("Check with test data")
{
  SECTION("First example")
  {
    std::stringstream testInput{testDataFirst};

    SECTION("Part 1")
    {
      Map map{testInput};
      REQUIRE(11048 == map.Dijkstra());
    }

    SECTION("Part 2")
    {
      Map2 map{testInput};
      map.Dijkstra();
      REQUIRE(64 == map.CountOptimalPoints());
    }
  }
}
//   SECTION("Example Long")
//   {
//     std::stringstream testInput{testDataLong};

//     SECTION("Part 1")
//     {
//       Map map{testInput};
//       map.ExecuteMovements();
//       REQUIRE(10092 == map.SumBoxesCoordinates());
//     }
//     SECTION("Part 2")
//     {
//       WideWarehouse map{testInput};
//       map.ExecuteMovements();
//       REQUIRE(9021 == map.SumBoxesCoordinates());
//     }
//   }
// }

TEST_CASE("Task day 16")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    Map map{data};
    std::cout << "Day 16 - part 1 result: " << map.Dijkstra() << std::endl;
  }

  SECTION("part 2")
  {
    Map2 map{data};
    map.Dijkstra();
    std::cout << "Day 16 - part 2 result: " << map.CountOptimalPoints() << std::endl;
  }
}