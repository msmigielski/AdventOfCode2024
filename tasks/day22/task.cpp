#include <catch2/catch_all.hpp>
#include <fstream>
#include <utils/Timer.h>
#include <queue>
#include <numeric>

using Sequence = std::tuple<int64_t, int64_t, int64_t, int64_t>;
struct TupleHash
{
  size_t operator()(const std::tuple<int64_t, int64_t, int64_t, int64_t> &t) const
  {
    const auto &[a, b, c, d] = t;

    size_t h1 = std::hash<int64_t>{}(a);
    size_t h2 = std::hash<int64_t>{}(b);
    size_t h3 = std::hash<int64_t>{}(c);
    size_t h4 = std::hash<int64_t>{}(d);

    size_t combined = h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    return combined;
  }
};

struct SecretNumberSolver
{
  SecretNumberSolver(uint64_t number) : secretNumber(number) {}

  uint64_t GetNumberAfterSteps(size_t n)
  {
    uint64_t tmp = secretNumber;
    for (size_t i = 0; i < n; ++i)
    {
      tmp = MakeStep(tmp);
    }
    return tmp;
  }

  std::unordered_map<Sequence, uint64_t, TupleHash> GetSequencesToValues(size_t n)
  {
    std::unordered_map<Sequence, uint64_t, TupleHash> sequencesToValues;
    std::deque<int64_t> q;
    uint64_t tmp = secretNumber;

    for (size_t i = 0; i < n; ++i)
    {
      uint64_t previousPrice = tmp % 10;
      tmp = MakeStep(tmp);
      uint64_t currentPrice = tmp % 10;

      int64_t diff = currentPrice - previousPrice;

      q.emplace_back(diff);
      if (q.size() > 4)
      {
        q.pop_front();
      }

      if (q.size() == 4)
      {
        sequencesToValues[{q[0], q[1], q[2], q[3]}] = currentPrice;
      }
    }
    return sequencesToValues;
  }

  uint64_t MakeStep(uint64_t secret)
  {
    uint64_t temp = secret;

    temp = temp << 6;           // *64
    secret = temp ^ secret;     // mix
    secret = secret % 16777216; // prune

    temp = secret;

    temp = temp >> 5;           // /5
    secret = temp ^ secret;     // mix
    secret = secret % 16777216; // prune

    temp = secret;

    temp = temp << 11;          // *2048
    secret = temp ^ secret;     // mix
    secret = secret % 16777216; // prune

    return secret;
  }

  uint64_t secretNumber;
};

size_t Sum2000thSecretNumbers(std::istream &input)
{
  uint64_t sum = 0;
  uint64_t num;
  while (input >> num)
  {
    sum += SecretNumberSolver(num).GetNumberAfterSteps(2000);
  }
  return sum;
}

size_t CountBananas(std::istream &input)
{
  uint64_t num;
  std::unordered_map<Sequence, std::vector<uint64_t>, TupleHash> allSequencesAndValues;
  while (input >> num)
  {
    const auto &sequenceToValues = SecretNumberSolver(num).GetSequencesToValues(2000);
    for (const auto &[sequence, value] : sequenceToValues)
    {
      if (allSequencesAndValues.contains(sequence))
      {
        allSequencesAndValues[sequence].push_back(value);
      }
      else
      {
        allSequencesAndValues[sequence] = {value};
      }
    }
  }

  uint64_t sum = 0;

  for (const auto &[sequence, values] : allSequencesAndValues)
  {
    uint64_t localSum = std::accumulate(values.begin(), values.end(), 0);
    if (localSum > sum)
      sum = localSum;
  }

  return sum;
}

TEST_CASE("Check secret number after n steps")
{
  REQUIRE(15887950 == SecretNumberSolver(123).GetNumberAfterSteps(1));
  REQUIRE(16495136 == SecretNumberSolver(123).GetNumberAfterSteps(2));
  REQUIRE(527345 == SecretNumberSolver(123).GetNumberAfterSteps(3));
  REQUIRE(704524 == SecretNumberSolver(123).GetNumberAfterSteps(4));
  REQUIRE(1553684 == SecretNumberSolver(123).GetNumberAfterSteps(5));
  REQUIRE(12683156 == SecretNumberSolver(123).GetNumberAfterSteps(6));
  REQUIRE(11100544 == SecretNumberSolver(123).GetNumberAfterSteps(7));
  REQUIRE(12249484 == SecretNumberSolver(123).GetNumberAfterSteps(8));
  REQUIRE(7753432 == SecretNumberSolver(123).GetNumberAfterSteps(9));
  REQUIRE(5908254 == SecretNumberSolver(123).GetNumberAfterSteps(10));
}

TEST_CASE("Part 2")
{
  constexpr auto testData = R"(1
2
3
2024)";
  std::stringstream testInput{testData};

  REQUIRE(23 == CountBananas(testInput));
}

TEST_CASE("Task day 22")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 22 - part 1 result: " << Sum2000thSecretNumbers(data) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 22 - part 2 result: " << CountBananas(data) << std::endl;
  }
}