#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <unordered_set>
#include <format>

struct Computer3Bit
{
  Computer3Bit(std::istream &input)
  {
    std::string line;
    std::getline(input, line);
    WriteValueToRegister(line, registerA);
    std::getline(input, line);
    WriteValueToRegister(line, registerB);
    std::getline(input, line);
    WriteValueToRegister(line, registerC);
    std::getline(input, line);
    std::getline(input, line);
    ReadProgram(line);
    MakeInstructions();
  }

  void MakeInstructions()
  {
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction0(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction1(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction2(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction3(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction4(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction5(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction6(operand, currentPos); });
    instructions.push_back([this](uint64_t operand, uint64_t &currentPos)
                           { return this->instruction7(operand, currentPos); });
  }

  Computer3Bit(int64_t RegA, const std::vector<uint64_t> &program_) : registerA(RegA), program(program_)
  {
    MakeInstructions();
  }

  std::string instruction0(uint64_t operand, uint64_t &currentPos)
  {
    uint64_t pow = this->ComboOperand(operand);
    uint64_t base = 2;
    uint64_t den = 2;
    if (pow == 0)
    {
      den = 1;
    }
    else
    {
      for (uint64_t i = 1; i < pow; ++i)
        den *= base;
    }
    registerA = registerA / den;
    currentPos += 2;
    return {};
  };

  std::string instruction1(uint64_t operand, uint64_t &currentPos)
  {
    this->registerB = registerB ^ operand;
    currentPos += 2;
    return {};
  };

  std::string instruction2(uint64_t operand, uint64_t &currentPos)
  {
    this->registerB = this->ComboOperand(operand) % 8;
    currentPos += 2;
    return {};
  };

  std::string instruction3(uint64_t operand, uint64_t &currentPos)
  {
    if (this->registerA == 0)
    {
      currentPos += 2;
      return {};
    }
    currentPos = operand;
    return {};
  };

  std::string instruction4(uint64_t, uint64_t &currentPos)
  {
    this->registerB = this->registerB ^ this->registerC;
    currentPos += 2;
    return {};
  };

  std::string instruction5(uint64_t operand, uint64_t &currentPos)
  {
    currentPos += 2;
    return std::to_string(ComboOperand(operand) % 8);
  };

  std::string instruction6(uint64_t operand, uint64_t &currentPos)
  {
    uint64_t pow = this->ComboOperand(operand);
    uint64_t base = 2;
    uint64_t den = 2;
    if (pow == 0)
    {
      den = 1;
    }
    else
    {
      for (uint64_t i = 1; i < pow; ++i)
        den *= base;
    }
    this->registerB = this->registerA / den;
    currentPos += 2;
    return {};
  };

  std::string instruction7(uint64_t operand, uint64_t &currentPos)
  {
    uint64_t pow = this->ComboOperand(operand);
    uint64_t base = 2;
    uint64_t den = 2;
    if (pow == 0)
    {
      den = 1;
    }
    else
    {
      for (uint64_t i = 1; i < pow; ++i)
        den *= base;
    }
    this->registerC = this->registerA / den;
    currentPos += 2;
    return {};
  };

  void WriteValueToRegister(const std::string &line, uint64_t &registerNum)
  {
    registerNum = std::stoi(line.substr(std::string("Register A: ").size()));
  }

  void ReadProgram(const std::string &line)
  {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos)
    {
      std::string numbersPart = line.substr(colonPos);

      std::istringstream iss(numbersPart);
      uint64_t num;
      char c;
      while (iss >> c)
      {
        iss >> num;
        program.push_back(num);
      }
    }
  }

  uint64_t ComboOperand(uint64_t num)
  {
    if (num >= 0 && num <= 3)
    {
      return num;
    }
    else if (num == 4)
    {
      return registerA;
    }
    else if (num == 5)
    {
      return registerB;
    }
    else if (num == 6)
    {
      return registerC;
    }
    else if (num == 7)
    {
      throw;
    }
    return 0;
  }

  std::string Process()
  {
    uint64_t currentPos = 0;
    std::string printed;
    while (currentPos < program.size())
    {
      uint64_t instructionNum = program.at(currentPos);
      uint64_t operand = program.at(currentPos + 1);
      auto instruction = instructions.at(instructionNum);
      const auto to_print = instruction(operand, currentPos);
      if (!to_print.empty() && !printed.empty())
      {
        printed += ',';
      }
      printed += to_print;
    }

    return printed;
  }

  uint64_t ProcessFind()
  {
    uint64_t solution = 0;
    uint64_t currentProgramPos = program.size() - 1;

    CheckSolution(0, currentProgramPos, solution);

    return solution;
  }

  bool CheckSolution(uint64_t currentRegA, uint64_t currentProgramPos, uint64_t &solution)
  {
    std::string expected;
    for (size_t i = currentProgramPos; i < program.size(); ++i)
    {
      if (!expected.empty())
      {
        expected += ',';
      }
      expected += std::to_string(program.at(i));
    }

    for (size_t i = 0; i < 8; ++i)
    {
      auto comp = Computer3Bit((currentRegA << 3) + i, program);
      auto str = comp.Process();
      if (currentProgramPos == 0)
      {
        std::cout << str << std::endl;
        if (str == expected)
        {
          solution = (currentRegA << 3) + i;
          return true;
        }
      }
      if (str == expected)
      {

        CheckSolution((currentRegA << 3) + i, currentProgramPos - 1, solution);
      }
    }

    return false;
  }

  uint64_t registerA = 0;
  uint64_t registerB = 0;
  uint64_t registerC = 0;

  std::vector<std::function<std::string(uint64_t, uint64_t &)>> instructions;

  std::vector<uint64_t> program;
  std::string programPrinted;
};

TEST_CASE("Check with examples")
{
  SECTION("Example 1")
  {
    constexpr auto testData = R"(Register A: 0
Register B: 0
Register C: 9

Program: 2,6)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    c.Process();
    REQUIRE(1 == c.registerB);
  }

  SECTION("Example 2")
  {
    constexpr auto testData = R"(Register A: 10
Register B: 0
Register C: 0

Program: 5,0,5,1,5,4)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    REQUIRE(std::string("0,1,2") == c.Process());
  }

  SECTION("Example 3")
  {
    constexpr auto testData = R"(Register A: 2024
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    REQUIRE(std::string("4,2,5,6,7,7,7,7,3,1,0") == c.Process());
    REQUIRE(0 == c.registerA);
  }

  SECTION("Example 4")
  {
    constexpr auto testData = R"(Register A: 0
Register B: 29
Register C: 0

Program: 1,7)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    c.Process();
    REQUIRE(26 == c.registerB);
  }

  SECTION("Example 5")
  {
    constexpr auto testData = R"(Register A: 0
Register B: 2024
Register C: 43690

Program: 4,0)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    c.Process();
    REQUIRE(44354 == c.registerB);
  }

  SECTION("Example 6")
  {
    constexpr auto testData = R"(Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    REQUIRE(std::string("4,6,3,5,6,3,5,2,1,0") == c.Process());
  }

  SECTION("Example 7")
  {
    Timer t;
    constexpr auto testData = R"(Register A: 2024
Register B: 0
Register C: 0

Program: 0,3,5,4,3,0)";
    std::stringstream testInput{testData};
    Computer3Bit c{testInput};
    REQUIRE(117440 == c.ProcessFind());
  }
}

TEST_CASE("Task day 17")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    Computer3Bit c{data};
    std::cout << "Day 17 - part 1 result: " << c.Process() << std::endl;
  }

  SECTION("part 2")
  {
    Computer3Bit c{data};
    std::cout << "Day 17 - part 2 result: " << c.ProcessFind() << std::endl;
  }
}