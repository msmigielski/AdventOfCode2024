#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <unordered_set>
#include <queue>

struct Point
{
  int column;
  int row;

  Point operator+(const Point &other) const
  {
    return {column + other.column, row + other.row};
  }

  Point operator-(const Point &other) const
  {
    return {column - other.column, row - other.row};
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

  template <>
  struct hash<pair<char, char>>
  {
    size_t operator()(const pair<char, char> &p) const
    {
      size_t h1 = std::hash<char>{}(p.first);
      size_t h2 = std::hash<char>{}(p.first);

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

struct TupleHash
{
  template <typename T1, typename T2, typename T3>
  std::size_t operator()(const std::tuple<T1, T2, T3> &t) const
  {
    std::size_t h1 = std::hash<T1>{}(std::get<0>(t));
    std::size_t h2 = std::hash<T2>{}(std::get<1>(t));
    std::size_t h3 = std::hash<T3>{}(std::get<2>(t));
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

struct Keyboard
{
  std::vector<std::string> GetMovementsFromTo(char from, char to)
  {
    return pointsToMovements.at({from, to});
  }

  void CreateMovements()
  {
    for (const auto &[value, point] : valueToPoint)
    {
      for (const auto &[rvalue, rpoint] : valueToPoint)
      {
        pointsToMovements[{value, rvalue}] = Dijkstra(point, rpoint);
      }
    }
  }

  std::vector<std::string> Dijkstra(const Point &startPosition, const Point &endPosition)
  {
    std::unordered_map<Point, std::vector<Point>> predecessors;
    std::priority_queue<std::pair<int64_t, Point>, std::vector<std::pair<int64_t, Point>>, ComparePair> queue;
    std::unordered_map<Point, int64_t> visited;
    queue.push({0, startPosition});
    visited[startPosition] = 0;

    while (!queue.empty())
    {
      const auto [cost, point] = queue.top();
      queue.pop();

      if (visited.contains(point) && visited.at(point) < cost)
      {
        continue;
      }

      if (point == endPosition)
      {
        continue;
      }

      std::vector<Point> siblings{
          point.PointUp(),
          point.PointDown(),
          point.PointLeft(),
          point.PointRight()};

      for (const auto &sibling : siblings)
      {
        if (keys.contains(sibling))
        {
          if (int64_t newCost = cost + 1;
              !visited.contains(sibling) || newCost < visited[sibling])
          {
            visited[sibling] = newCost;
            queue.push({newCost, sibling});
            predecessors[sibling] = {point};
          }
          else if (newCost == visited[sibling])
          {
            predecessors[sibling].push_back(point);
          }
        }
      }
    }

    std::vector<Point> pathPoints;
    std::vector<std::vector<Point>> allPaths;

    FindPaths(endPosition, startPosition, predecessors, pathPoints, allPaths);

    std::vector<std::string> possibleMovements;

    for (const auto &path : allPaths)
    {
      std::string movement;
      for (size_t num = 1; num < path.size(); ++num)
      {
        movement += diffToDirection.at(path[num] - path[num - 1]);
      }

      size_t dirChange = 0;
      for (size_t i = 1; i < movement.size(); ++i)
      {
        if (movement[i - 1] != movement[i])
        {
          ++dirChange;
        }
      }
      if (dirChange <= 1)
      {
        possibleMovements.push_back(movement);
      }
    }

    return possibleMovements;
  }

  void FindPaths(const Point &node,
                 const Point &start,
                 const std::unordered_map<Point, std::vector<Point>> &predecessors,
                 std::vector<Point> &path,
                 std::vector<std::vector<Point>> &allPaths)
  {
    if (node == start)
    {
      path.push_back(node);
      allPaths.push_back(std::vector<Point>(path.rbegin(), path.rend()));
      path.pop_back();
      return;
    }

    path.push_back(node);
    for (const auto &pred : predecessors.at(node))
    {
      FindPaths(pred, start, predecessors, path, allPaths);
    }
    path.pop_back();
  }

  std::unordered_map<std::pair<char, char>, std::vector<std::string>>
      pointsToMovements;
  std::unordered_map<char, Point> valueToPoint;
  std::unordered_map<Point, std::string> diffToDirection{
      {{-1, 0}, "<"},
      {{1, 0}, ">"},
      {{0, -1}, "^"},
      {{0, 1}, "v"},
      {{0, 0}, ""},
  };
  std::unordered_set<Point> keys;
};

struct NumericKeyboard : public Keyboard
{
  NumericKeyboard()
  {
    Point p{0, 0};
    keys.emplace(p);
    valueToPoint.emplace('7', p);

    p = Point{1, 0};
    keys.emplace(p);
    valueToPoint.emplace('8', p);

    p = Point{2, 0};
    keys.emplace(p);
    valueToPoint.emplace('9', p);

    p = Point{0, 1};
    keys.emplace(p);
    valueToPoint.emplace('4', p);

    p = Point{1, 1};
    keys.emplace(p);
    valueToPoint.emplace('5', p);

    p = Point{2, 1};
    keys.emplace(p);
    valueToPoint.emplace('6', p);

    p = Point{0, 2};
    keys.emplace(p);
    valueToPoint.emplace('1', p);

    p = Point{1, 2};
    keys.emplace(p);
    valueToPoint.emplace('2', p);

    p = Point{2, 2};
    keys.emplace(p);
    valueToPoint.emplace('3', p);

    p = Point{1, 3};
    keys.emplace(p);
    valueToPoint.emplace('0', p);

    p = Point{2, 3};
    keys.emplace(p);
    valueToPoint.emplace('A', p);

    CreateMovements();
  }
};

struct DirectionKeyboard : public Keyboard
{
  DirectionKeyboard()
  {
    Point p{1, 0};
    keys.emplace(p);
    valueToPoint.emplace('^', p);

    p = Point{2, 0};
    keys.emplace(p);
    valueToPoint.emplace('A', p);

    p = Point{0, 1};
    keys.emplace(p);
    valueToPoint.emplace('<', p);

    p = Point{1, 1};
    keys.emplace(p);
    valueToPoint.emplace('v', p);

    p = Point{2, 1};
    keys.emplace(p);
    valueToPoint.emplace('>', p);

    CreateMovements();
  }
};

void generateCombinations(
    const std::vector<std::vector<std::string>> &vectors,
    std::vector<std::string> &combination,
    size_t depth,
    std::unordered_set<std::string> &results)
{

  if (depth == vectors.size())
  {
    std::string out;
    for (const auto &el : combination)
    {
      out += el + 'A';
    }
    results.emplace(out);
    return;
  }

  for (const auto &elem : vectors[depth])
  {
    combination[depth] = elem;
    generateCombinations(vectors, combination, depth + 1, results);
  }
}

std::unordered_set<std::string> getAllCombinations(const std::vector<std::vector<std::string>> &vectors)
{
  std::unordered_set<std::string> results;
  std::vector<std::string> combination(vectors.size());
  generateCombinations(vectors, combination, 0, results);
  return results;
}

std::unordered_set<std::string> GetNumericCombinations(const std::string &input)
{
  NumericKeyboard numericKeyboard;

  char keyboardPosition = 'A';

  std::vector<std::vector<std::string>> possibleNumericMovements;

  for (const auto &c : input)
  {
    possibleNumericMovements.emplace_back(numericKeyboard.GetMovementsFromTo(keyboardPosition, c));
    keyboardPosition = c;
  }

  return getAllCombinations(possibleNumericMovements);
}

size_t FindShortestSequence(std::string &sequence,
                            size_t numberOfRobots,
                            std::unordered_map<std::tuple<std::string, size_t, size_t>, size_t, TupleHash> &cache,
                            size_t currentRobotNumber = 0)
{
  DirectionKeyboard keyboard;

  if (numberOfRobots == currentRobotNumber)
  {
    return sequence.size();
  }

  if (cache.contains({sequence, numberOfRobots, currentRobotNumber}))
  {
    return cache.at({sequence, numberOfRobots, currentRobotNumber});
  }

  char currentPosition = 'A';
  std::vector<std::vector<std::string>> arrowSequences;
  for (const auto &key : sequence)
  {
    arrowSequences.push_back(keyboard.GetMovementsFromTo(currentPosition, key));
    currentPosition = key;
  }

  for (auto &sequences : arrowSequences)
  {
    for (auto &sequence : sequences)
    {
      sequence += 'A';
    }
  }

  // std::cout << std::format("command: {}, commandSplits: ",
  //                          sequence);
  // for (const auto &c : arrowSequences)
  // {
  //   std::cout << c << ",";
  // }
  // std::cout << std::endl;

  size_t shortest = 0;
  for (auto &arrowSequence : arrowSequences)
  {
    size_t local = std::numeric_limits<size_t>::max();
    for (auto &sequence : arrowSequence)
    {
      size_t temp = FindShortestSequence(sequence, numberOfRobots, cache, currentRobotNumber + 1);
      if (temp < local)
        local = temp;
    }
    shortest += local;
  }

  std::cout << std::format("command: {}, numRobots: {}, keypad: {}, result: {}\n",
                           sequence, numberOfRobots, currentRobotNumber, shortest);
  cache[{sequence, numberOfRobots, currentRobotNumber}] = shortest;
  return shortest;
}

size_t GetCodeComplexity(const std::string &input, size_t numberOfRobots = 2)
{
  static std::unordered_map<std::tuple<std::string, size_t, size_t>, size_t, TupleHash> cache;

  const auto &numericMovements = GetNumericCombinations(input);

  size_t length = std::numeric_limits<uint64_t>::max();

  std::vector<std::string> tmp;
  for (const auto &numericSequence : numericMovements)
  {
    tmp.push_back(numericSequence);
  }
  std::reverse(tmp.begin(), tmp.end());

  for (auto &numericSequence : tmp)
  {
    size_t temp = FindShortestSequence(numericSequence, numberOfRobots, cache);
    std::cout << std::format("command: {}, commandSplits: {}",
                             numericSequence, temp)
              << std::endl;
    if (temp < length)
      length = temp;
  }

  return length * std::stoi(input);
}

size_t SumComplexity(std::istream &input, size_t numRobots = 2)
{
  size_t sum = 0;
  std::string line;
  while (std::getline(input, line))
  {
    sum += GetCodeComplexity(line, numRobots);
  }
  return sum;
}

TEST_CASE("Check keyboard movements")
{
  SECTION("Numeric")
  {
    NumericKeyboard k;
    CHECK(std::vector<std::string>{"<"} == k.GetMovementsFromTo('A', '0'));
    CHECK(std::vector<std::string>{"^^^"} == k.GetMovementsFromTo('A', '9'));
    CHECK(std::vector<std::string>{"vv"} == k.GetMovementsFromTo('6', 'A'));
    CHECK(std::vector<std::string>{">vvv", "vvv>"} == k.GetMovementsFromTo('8', 'A'));
    CHECK(std::vector<std::string>{"^<<"} == k.GetMovementsFromTo('A', '1'));
  }

  SECTION("Numeric")
  {
    DirectionKeyboard k;
    CHECK(std::vector<std::string>{"<"} == k.GetMovementsFromTo('A', '^'));
    CHECK(std::vector<std::string>{"v<", "<v"} == k.GetMovementsFromTo('A', 'v'));
    CHECK(std::vector<std::string>{"v<<"} == k.GetMovementsFromTo('A', '<'));
    CHECK(std::vector<std::string>{">>^"} == k.GetMovementsFromTo('<', 'A'));
  }
}

TEST_CASE("Check nested combination")
{
  CHECK(68 * 29 == GetCodeComplexity("029A"));
  CHECK(60 * 980 == GetCodeComplexity("980A"));
  CHECK(68 * 179 == GetCodeComplexity("179A"));
  CHECK(64 * 456 == GetCodeComplexity("456A"));
  CHECK(64 * 379 == GetCodeComplexity("379A"));
}

TEST_CASE("Task day 20")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 21 - part 1 result: " << SumComplexity(data, 25) << std::endl;
  }

  //   // SECTION("part 2")
  //   // {
  //   //   Map2 map{data};
  //   //   map.Dijkstra();
  //   //   std::cout << "Day 20 - part 2 result: " << map.CountLongerShortcutsWithDiff(99) << std::endl;
  //   // }
}