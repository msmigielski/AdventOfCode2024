#include <catch2/catch_all.hpp>
#include <fstream>
#include <utils/Timer.h>

struct SecretNumberSolver
{
  SecretNumberSolver(uint64_t number) : secretNumber(number) {}

  uint64_t GetNumberAfterSteps(size_t n)
  {
    for (size_t i = 0; i < n; ++i)
    {
      uint64_t temp = secretNumber;

      temp = temp << 6;                       // *64
      secretNumber = temp ^ secretNumber;     // mix
      secretNumber = secretNumber % 16777216; // prune

      temp = secretNumber;

      temp = temp >> 5;                       // /5
      secretNumber = temp ^ secretNumber;     // mix
      secretNumber = secretNumber % 16777216; // prune

      temp = secretNumber;

      temp = temp << 11;                      // *2048
      secretNumber = temp ^ secretNumber;     // mix
      secretNumber = secretNumber % 16777216; // prune
    }
    return secretNumber;
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

TEST_CASE("test")
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

TEST_CASE("Task day 22")
{
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 22 - part 1 result: " << Sum2000thSecretNumbers(data) << std::endl;
  }

  // SECTION("part 2")
  // {
  //   std::cout << "Day 5 - part 2 result: " << SumMidElementOfNotValidUpdates(rules, updates) << std::endl;
  // }
}