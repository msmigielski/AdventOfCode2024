#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <regex>
#include <fstream>

class Calibration
{
public:
  Calibration(const std::string &line)
  {
    std::regex valRegex("(\\d+)");

    auto matchStart = std::sregex_iterator(line.begin(), line.end(), valRegex);
    auto matchEnd = std::sregex_iterator();

    testNumber = std::stoll(std::smatch(*matchStart).str());

    for (auto i = ++matchStart; i != matchEnd; ++i)
    {
      uint64_t x = std::stoll(std::smatch(*i).str());
      calibrations.push_back(x);
    }
  }

  bool CheckWithAddAndMul(const std::vector<uint64_t> &numbers, size_t index, uint64_t currentResult) const
  {
    if (currentResult > testNumber)
    {
      return false;
    }

    if (index == numbers.size() && currentResult == testNumber)
    {
      return true;
    }

    if (index == numbers.size())
    {
      return false;
    }

    return CheckWithAddAndMul(numbers, index + 1, currentResult + numbers[index]) ||
           CheckWithAddAndMul(numbers, index + 1, currentResult * numbers[index]);
  }

  bool CheckWithAddMulAndConcatenation(const std::vector<uint64_t> &numbers, size_t index, uint64_t currentResult) const
  {
    if (currentResult > testNumber)
    {
      return false;
    }

    if (index == numbers.size() && currentResult == testNumber)
    {
      return true;
    }

    if (index == numbers.size())
    {
      return false;
    }

    return CheckWithAddMulAndConcatenation(numbers, index + 1, currentResult + numbers[index]) ||
           CheckWithAddMulAndConcatenation(numbers, index + 1, currentResult * numbers[index]) ||
           CheckWithAddMulAndConcatenation(numbers, index + 1, std::stoll(std::to_string(currentResult) + std::to_string(numbers[index])));
  }

  bool IsValid() const
  {
    return CheckWithAddAndMul(calibrations, 1, calibrations[0]);
  }

  bool IsValidWithConcatenation() const
  {
    return CheckWithAddMulAndConcatenation(calibrations, 1, calibrations[0]);
  }

  uint64_t testNumber;
  std::vector<uint64_t> calibrations;
};

TEST_CASE("Check with test data")
{
  SECTION("part 1")
  {
    REQUIRE(Calibration("190: 10 19").IsValid());
    REQUIRE(Calibration("3267: 81 40 27").IsValid());
    REQUIRE_FALSE(Calibration("83: 17 5").IsValid());
    REQUIRE_FALSE(Calibration("156: 15 6").IsValid());
    REQUIRE_FALSE(Calibration("7290: 6 8 6 15").IsValid());
    REQUIRE_FALSE(Calibration("161011: 16 10 13").IsValid());
    REQUIRE_FALSE(Calibration("192: 17 8 14").IsValid());
    REQUIRE_FALSE(Calibration("21037: 9 7 18 13").IsValid());
    REQUIRE(Calibration("292: 11 6 16 20  ").IsValid());
  }

  SECTION("part 2")
  {
    REQUIRE(Calibration("190: 10 19").IsValidWithConcatenation());
    REQUIRE(Calibration("3267: 81 40 27").IsValidWithConcatenation());
    REQUIRE_FALSE(Calibration("83: 17 5").IsValidWithConcatenation());
    REQUIRE(Calibration("156: 15 6").IsValidWithConcatenation());
    REQUIRE(Calibration("7290: 6 8 6 15").IsValidWithConcatenation());
    REQUIRE_FALSE(Calibration("161011: 16 10 13").IsValidWithConcatenation());
    REQUIRE(Calibration("192: 17 8 14").IsValidWithConcatenation());
    REQUIRE_FALSE(Calibration("21037: 9 7 18 13").IsValidWithConcatenation());
    REQUIRE(Calibration("292: 11 6 16 20  ").IsValidWithConcatenation());
  }
}

uint64_t SumTestNumbers(std::istream &input)
{
  uint64_t sum = 0;
  std::string line;
  while (std::getline(input, line))
  {
    if (const auto &c = Calibration(line);
        c.IsValid())
    {
      sum += c.testNumber;
    }
  }
  return sum;
}

uint64_t SumTestNumbersWithConcatenation(std::istream &input)
{
  uint64_t sum = 0;
  std::string line;
  while (std::getline(input, line))
  {
    if (const auto &c = Calibration(line);
        c.IsValidWithConcatenation())
    {
      sum += c.testNumber;
    }
  }
  return sum;
}

TEST_CASE("Task day 7")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 7 - part 1 result: " << SumTestNumbers(data) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 7 - part 2 result: " << SumTestNumbersWithConcatenation(data) << std::endl;
  }
}