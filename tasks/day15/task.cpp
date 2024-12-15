#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <unordered_set>

constexpr auto testDataShort = R"(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<)";

constexpr auto testDataLong = R"(##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^)";

struct Point
{
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
      int h1 = std::hash<int>{}(p.row);
      int h2 = std::hash<int>{}(p.column);

      return h1 ^ (h2 << 1);
    }
  };
}

struct Warehouse
{
  Warehouse(std::istream &input)
  {
    std::string line;
    int row = 0;

    while (std::getline(input, line))
    {
      int column = 0;
      for (const auto &c : line)
      {
        const auto newObject = Point{column, row};
        if (c == '#')
        {
          wallPoints.emplace(newObject);
        }
        else if (c == 'O')
        {
          boxPoints.emplace(newObject);
        }
        else if (c == '@')
        {
          robotPosition = newObject;
        }
        else if (c == '<' || c == '^' || c == '>' || c == 'v')
        {
          movements.push_back(c);
        }
        ++column;
      }
      ++row;
    }
  }

  void ExecuteMovements()
  {
    for (const auto &move : movements)
    {
      if (CanMoveInDirection(robotPosition, move))
      {
        MoveObject(robotPosition, move);
      }
    }
  }

  bool CanMoveInDirection(const Point &objectPosition, char direction)
  {
    const auto newPosition = GetNewPosition(objectPosition, direction);

    if (wallPoints.contains(newPosition))
    {
      return false;
    }

    if (boxPoints.contains(newPosition))
    {
      return CanMoveInDirection(newPosition, direction);
    }

    return true;
  }

  Point GetNewPosition(const Point &objectPosition, char direction)
  {
    Point newPosition;
    if (direction == '^')
    {
      newPosition = objectPosition.PointUp();
    }
    else if (direction == '<')
    {
      newPosition = objectPosition.PointLeft();
    }
    else if (direction == '>')
    {
      newPosition = objectPosition.PointRight();
    }
    else if (direction == 'v')
    {
      newPosition = objectPosition.PointDown();
    }
    return newPosition;
  }

  void MoveObject(const Point &objectPosition, char direction)
  {
    const auto newPosition = GetNewPosition(objectPosition, direction);

    if (boxPoints.contains(newPosition))
    {
      MoveObject(newPosition, direction);
    }

    if (objectPosition == robotPosition)
    {
      robotPosition = newPosition;
    }
    else
    {
      boxPoints.erase(objectPosition);
      boxPoints.emplace(newPosition);
    }
  }

  size_t SumBoxesCoordinates()
  {
    size_t sum = 0;
    for (const auto &box : boxPoints)
    {
      sum += 100 * box.row + box.column;
    }
    return sum;
  }

  void Print()
  {
    const auto max = *(std::ranges::max_element(wallPoints, [](const auto &l, const auto &r)
                                                {
                                                if (l.column == r.column) {
                                                  return l.row < r.row;
                                                }
                                                return l.column < r.column; }));

    for (int y = 0; y <= max.row; ++y)
    {
      for (int x = 0; x <= max.column; ++x)
      {
        Point p{x, y};
        if (wallPoints.contains(p))
        {
          std::cout << '#';
        }
        else if (boxPoints.contains(p))
        {
          std::cout << 'O';
        }
        else if (robotPosition == p)
        {
          std::cout << '@';
        }
        else
        {
          std::cout << '.';
        }
      }
      std::cout << std::endl;
    }
  }

  std::unordered_set<Point> wallPoints;
  std::unordered_set<Point> boxPoints;
  Point robotPosition;
  std::vector<char> movements;
};

/////////
/////////

struct WideWarehouse
{
  WideWarehouse(std::istream &input)
  {
    std::string line;
    int row = 0;

    while (std::getline(input, line))
    {
      int column = 0;
      for (const auto &c : line)
      {
        const auto newObject = Point{column, row};
        ++column;
        const auto newObject2 = Point{column, row};

        if (c == '#')
        {
          wallPoints.emplace(newObject);
          wallPoints.emplace(newObject2);
        }
        else if (c == 'O')
        {
          boxPoints.emplace(newObject, '[');
          boxPoints.emplace(newObject2, ']');
        }
        else if (c == '@')
        {
          robotPosition = newObject;
        }
        else if (c == '<' || c == '^' || c == '>' || c == 'v')
        {
          movements.push_back(c);
        }
        ++column;
      }
      ++row;
    }
  }

  void ExecuteMovements()
  {
    for (const auto &move : movements)
    {
      if (CanMoveInDirection(robotPosition, move))
      {
        MoveObject(robotPosition, move);
      }
    }
  }

  bool CanMoveInDirection(const Point &objectPosition, char direction)
  {
    const auto newPosition = GetNewPosition(objectPosition, direction);

    if (wallPoints.contains(newPosition))
    {
      return false;
    }

    if (boxPoints.contains(newPosition) && boxPoints.at(newPosition) == '[' && direction == '>')
    {
      return CanMoveInDirection({newPosition.column + 1, newPosition.row}, direction);
    }
    if (boxPoints.contains(newPosition) && boxPoints.at(newPosition) == ']' && direction == '<')
    {
      return CanMoveInDirection({newPosition.column - 1, newPosition.row}, direction);
    }
    else if (boxPoints.contains(newPosition) && boxPoints.at(newPosition) == '[')
    {
      return CanMoveInDirection(newPosition, direction) && CanMoveInDirection({newPosition.column + 1, newPosition.row}, direction);
    }
    else if (boxPoints.contains(newPosition) && boxPoints.at(newPosition) == ']')
    {
      return CanMoveInDirection(newPosition, direction) && CanMoveInDirection({newPosition.column - 1, newPosition.row}, direction);
    }

    return true;
  }

  Point GetNewPosition(const Point &objectPosition, char direction)
  {
    Point newPosition;
    if (direction == '^')
    {
      newPosition = objectPosition.PointUp();
    }
    else if (direction == '<')
    {
      newPosition = objectPosition.PointLeft();
    }
    else if (direction == '>')
    {
      newPosition = objectPosition.PointRight();
    }
    else if (direction == 'v')
    {
      newPosition = objectPosition.PointDown();
    }
    return newPosition;
  }

  void MoveObject(const Point &objectPosition, char direction)
  {
    const auto newPosition = GetNewPosition(objectPosition, direction);

    if (boxPoints.contains(newPosition) && boxPoints.at(newPosition) == '[')
    {
      MoveObject({newPosition.column + 1, newPosition.row}, direction);
      MoveObject(newPosition, direction);
    }
    else if (boxPoints.contains(newPosition) && boxPoints.at(newPosition) == ']')
    {
      MoveObject({newPosition.column - 1, newPosition.row}, direction);
      MoveObject(newPosition, direction);
    }

    if (objectPosition == robotPosition)
    {
      robotPosition = newPosition;
    }
    else
    {
      const auto c = boxPoints.at(objectPosition);
      boxPoints.erase(objectPosition);
      boxPoints.emplace(newPosition, c);
    }
  }

  size_t SumBoxesCoordinates()
  {
    size_t sum = 0;
    for (const auto &[pos, type] : boxPoints)
    {
      if (type == '[')
      {
        sum += 100 * pos.row + pos.column;
      }
    }
    return sum;
  }

  void Print()
  {
    const auto max = *(std::ranges::max_element(wallPoints, [](const auto &l, const auto &r)
                                                {
                                                if (l.column == r.column) {
                                                  return l.row < r.row;
                                                }
                                                return l.column < r.column; }));
    int i = 0;
    for (int y = 0; y <= max.row; ++y)
    {
      for (int x = 0; x <= max.column; ++x)
      {
        Point p{x, y};
        if (wallPoints.contains(p))
        {
          std::cout << '#';
        }
        else if (boxPoints.contains(p))
        {
          if (i % 2 == 0)
            std::cout << '[';
          else
            std::cout << ']';
          ++i;
        }
        else if (robotPosition == p)
        {
          std::cout << '@';
        }
        else
        {
          std::cout << '.';
        }
      }
      std::cout << std::endl;
    }
  }

  std::unordered_set<Point> wallPoints;
  std::unordered_map<Point, char> boxPoints;
  Point robotPosition;
  std::vector<char> movements;
};

TEST_CASE("Check with test data")
{
  SECTION("Example Short")
  {
    std::stringstream testInput{testDataShort};

    SECTION("Part 1")
    {
      Warehouse map{testInput};
      map.ExecuteMovements();
      REQUIRE(2028 == map.SumBoxesCoordinates());
    }
  }

  SECTION("Example Long")
  {
    std::stringstream testInput{testDataLong};

    SECTION("Part 1")
    {
      Warehouse map{testInput};
      map.ExecuteMovements();
      REQUIRE(10092 == map.SumBoxesCoordinates());
    }
    SECTION("Part 2")
    {
      WideWarehouse map{testInput};
      map.ExecuteMovements();
      REQUIRE(9021 == map.SumBoxesCoordinates());
    }
  }
}

TEST_CASE("Task day 15")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    Warehouse map{data};
    map.ExecuteMovements();
    std::cout << "Day 15 - part 1 result: " << map.SumBoxesCoordinates() << std::endl;
  }

  SECTION("part 2")
  {
    WideWarehouse map{data};
    map.ExecuteMovements();
    std::cout << "Day 15 - part 2 result: " << map.SumBoxesCoordinates() << std::endl;
  }
}