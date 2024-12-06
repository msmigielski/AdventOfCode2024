#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <numeric>
#include <unordered_set>

constexpr auto testData = R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...)";

struct Point
{
  size_t row;
  size_t column;

  auto operator<=>(const Point &) const = default;
};

namespace std
{
  template <>
  struct hash<Point>
  {
    std::size_t operator()(const Point &p) const
    {
      std::size_t h1 = std::hash<size_t>{}(p.row);
      std::size_t h2 = std::hash<size_t>{}(p.column);

      return h1 ^ (h2 << 1);
    }
  };
}

struct LabMapWalker
{
  LabMapWalker(std::istream &input)
  {
    std::string line;

    size_t row = 0;

    while (std::getline(input, line))
    {
      size_t column = 0;
      for (const auto &c : line)
      {
        if (c == '#')
        {
          obstructionPoints.insert({row, column});
        }
        if (c == '^')
        {
          startPosition = {row, column};
          currentPosition = {row, column};
        }
        ++column;
      }
      ++row;
      max_column = line.size() - 1;
    }
    max_row = row - 1;
  }

  LabMapWalker(
      const size_t &max_column_,
      const size_t &max_row_,
      const std::unordered_set<Point> &obstructionPoints_,
      const Point &startPosition_,
      const Point &additionalObstruction)
      : max_column(max_column_),
        max_row(max_row_),
        obstructionPoints(obstructionPoints_),
        startPosition(startPosition_),
        currentPosition(startPosition)
  {
    obstructionPoints.insert(additionalObstruction);
  }

  void Walk()
  {
    if (currentDirection == 'u')
    {
      currentPosition = {currentPosition.row - 1, currentPosition.column};
    }
    else if (currentDirection == 'r')
    {
      currentPosition = {currentPosition.row, currentPosition.column + 1};
    }
    else if (currentDirection == 'd')
    {
      currentPosition = {currentPosition.row + 1, currentPosition.column};
    }
    else if (currentDirection == 'l')
    {
      currentPosition = {currentPosition.row, currentPosition.column - 1};
    }
  };

  bool IsNotGoingOutside()
  {
    if (currentDirection == 'u' && currentPosition.row == 0)
    {
      return false;
    }

    if (currentDirection == 'r' && currentPosition.column == max_column)
    {
      return false;
    }

    if (currentDirection == 'd' && currentPosition.row == max_row)
    {
      return false;
    }

    if (currentDirection == 'l' && currentPosition.column == 0)
    {
      return false;
    }

    return true;
  }

  bool CanWalkInCurrentDirection()
  {
    if (currentDirection == 'u' && !obstructionPoints.contains({currentPosition.row - 1, currentPosition.column}))
    {
      return true;
    }

    if (currentDirection == 'r' && !obstructionPoints.contains({currentPosition.row, currentPosition.column + 1}))
    {
      return true;
    }

    if (currentDirection == 'd' && !obstructionPoints.contains({currentPosition.row + 1, currentPosition.column}))
    {
      return true;
    }

    if (currentDirection == 'l' && !obstructionPoints.contains({currentPosition.row, currentPosition.column - 1}))
    {
      return true;
    }

    return false;
  }

  int CountMapWalkPoints()
  {
    while (IsNotGoingOutside())
    {
      if (visitedPoints.contains(currentPosition) && visitedPoints.at(currentPosition) == currentDirection)
      {
        return 0;
      }

      if (CanWalkInCurrentDirection())
      {
        visitedPoints[currentPosition] = currentDirection;
        Walk();
      }
      else
      {
        currentDirection = directionChange.at(currentDirection);
      }
    }
    visitedPoints[currentPosition] = currentDirection;

    return static_cast<int>(visitedPoints.size());
  }

  int CountPossibleLoopObstructions()
  {
    int count = 0;
    CountMapWalkPoints();
    auto originalPathPoints = visitedPoints;

    for (const auto &[point, direction] : visitedPoints)
    {
      LabMapWalker walker(max_column,
                          max_row,
                          obstructionPoints,
                          startPosition,
                          point);

      count += (0 == walker.CountMapWalkPoints());
    }
    return count;
  }

  size_t max_column;
  size_t max_row;

  std::unordered_set<Point> obstructionPoints;
  std::unordered_map<Point, char> visitedPoints;
  Point startPosition;
  Point currentPosition;
  char startDirection{'u'};
  char currentDirection{'u'};

  const std::unordered_map<char, char> directionChange =
      {
          {'u', 'r'},
          {'r', 'd'},
          {'d', 'l'},
          {'l', 'u'}};
};

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  LabMapWalker labMap{testInput};

  SECTION("Part 1")
  {
    REQUIRE(41 == labMap.CountMapWalkPoints());
  }

  SECTION("Part 2")
  {
    REQUIRE(6 == labMap.CountPossibleLoopObstructions());
  }
}

TEST_CASE("Task day 6")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  LabMapWalker labMap{data};

  SECTION("part 1")
  {
    std::cout << "Day 6 - part 1 result: " << labMap.CountMapWalkPoints() << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 6 - part 2 result: " << labMap.CountPossibleLoopObstructions() << std::endl;
  }
}