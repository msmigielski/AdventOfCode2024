#include <catch2/catch_all.hpp>
#include <fstream>
#include <numeric>
#include <utils/Timer.h>

constexpr auto testData = R"(x00: 1
x01: 0
x02: 1
x03: 1
x04: 0
y00: 1
y01: 1
y02: 1
y03: 1
y04: 1

ntg XOR fgs -> mjb
y02 OR x01 -> tnw
kwq OR kpj -> z05
x00 OR x03 -> fst
tgd XOR rvg -> z01
vdt OR tnw -> bfw
bfw AND frj -> z10
ffh OR nrd -> bqk
y00 AND y03 -> djm
y03 OR y00 -> psh
bqk OR frj -> z08
tnw OR fst -> frj
gnj AND tgd -> z11
bfw XOR mjb -> z00
x03 OR x00 -> vdt
gnj AND wpb -> z02
x04 AND y00 -> kjc
djm OR pbm -> qhw
nrd AND vdt -> hwm
kjc AND fst -> rvg
y04 OR y02 -> fgs
y01 AND x02 -> pbm
ntg OR kjc -> kwq
psh XOR fgs -> tgd
qhw XOR tgd -> z09
pbm OR djm -> kpj
x03 XOR y03 -> ffh
x00 XOR y04 -> ntg
bfw OR bqk -> z06
nrd XOR fgs -> wpb
frj XOR qhw -> z04
bqk OR frj -> z07
y03 OR x01 -> nrd
hwm AND bqk -> z03
tgd XOR rvg -> z12
tnw OR pbm -> gnj)";

std::string ltrim(const std::string &str)
{
  size_t start = str.find_first_not_of(" \t\n\r");
  return (start == std::string::npos) ? "" : str.substr(start);
}

std::string rtrim(const std::string &str)
{
  size_t end = str.find_last_not_of(" \t\n\r");
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string trim(const std::string &str)
{
  return rtrim(ltrim(str));
}

struct Gate
{
  Gate(const std::string &type_,
       const std::pair<std::string, std::string> &inputs_)
      : type(type_), inputs(inputs_)
  {
  }

  bool AreInputsXY() const
  {
    return (inputs.first.starts_with("x") || inputs.first.starts_with("y")) &&
           (inputs.second.starts_with("x") || inputs.second.starts_with("y"));
  }

  bool AreInputs00() const
  {
    return inputs.first.ends_with("00") && inputs.second.ends_with("00");
  }

  std::string type;
  std::pair<std::string, std::string> inputs;
};

struct WireSet
{
  WireSet(std::istream &input)
  {
    std::string line;
    while (std::getline(input, line) && !line.empty())
    {
      std::string name = line.substr(0, 3);
      std::string value = line.substr(5);
      states[name] = value == "1";
    }
    while (std::getline(input, line))
    {
      std::string first = trim(line.substr(0, 3));
      std::string operation = trim(line.substr(3, 4));
      std::string second = trim(line.substr(3 + operation.size() + 2, 3));
      std::string output = trim(line.substr(line.size() - 3));

      if (output.starts_with("z"))
      {
        lastZ = lastZ > output ? lastZ : output;
      }

      states[output] = {};

      outputsToGates.emplace(output, Gate(operation, {first, second}));
    }
  }

  bool GetState(const std::string &wire)
  {
    if (states.contains(wire) && states[wire].has_value())
    {
      return states[wire].value();
    }

    if (!outputsToGates.contains(wire))
    {
      return false;
    }

    const auto &gate = outputsToGates.at(wire);

    const auto &in0 = GetState(gate.inputs.first);
    const auto &in1 = GetState(gate.inputs.second);

    bool out = false;

    if (gate.type == "AND")
    {
      out = in0 && in1;
    }
    else if (gate.type == "OR")
    {
      out = in0 || in1;
    }
    else if (gate.type == "XOR")
    {
      out = in0 ^ in1;
    }

    states[wire] = out;

    return out;
  }

  uint64_t GetOutputZ()
  {
    uint64_t value = 0;
    for (int i = 63; i >= 0; --i)
    {
      const std::string &wire = std::format("z{:02}", i);

      value = value << 1;
      value += GetState(wire);
    }
    return value;
  }

  std::string GetWrongOutputs()
  {
    std::set<std::string> outputs;
    for (const auto &[output, gate] : outputsToGates)
    {
      if (output.starts_with("z") && output != lastZ && gate.type != "XOR")
      {
        outputs.emplace(output);
      }

      if (!output.starts_with("z") && gate.type == "XOR" && !gate.AreInputsXY())
      {
        outputs.emplace(output);
      }

      if (gate.type == "XOR" && gate.AreInputsXY() && !gate.AreInputs00())
      {
        auto isInputToXOR = [output](auto &outputToGate)
        { return (outputToGate.second.inputs.first == output || outputToGate.second.inputs.second == output) &&
                 outputToGate.second.type == "XOR"; };

        if (bool outputIsDirectedToOtherXORInput = std::any_of(outputsToGates.begin(), outputsToGates.end(), isInputToXOR);
            !outputIsDirectedToOtherXORInput)
        {
          outputs.emplace(output);
        }
      }

      if (gate.type == "AND" && gate.AreInputsXY() && !gate.AreInputs00())
      {
        auto isInputToOR = [output](auto &outputToGate)
        { return (outputToGate.second.inputs.first == output || outputToGate.second.inputs.second == output) &&
                 outputToGate.second.type == "OR"; };

        if (bool outputIsDirectedToOtherXORInput = std::any_of(outputsToGates.begin(), outputsToGates.end(), isInputToOR);
            !outputIsDirectedToOtherXORInput)
        {
          outputs.emplace(output);
        }
      }
    }
    return std::accumulate(std::next(outputs.begin()), outputs.end(), *outputs.begin(), [](const std::string &a, const std::string &b)
                           { return a + "," + b; });
  }

  std::unordered_map<std::string, std::optional<bool>> states;
  std::unordered_map<std::string, Gate> outputsToGates;
  std::string lastZ;
};

TEST_CASE("Check with test data - part 1")
{
  std::stringstream testInput{testData};
  WireSet wireSet{testInput};

  REQUIRE(2024 == wireSet.GetOutputZ());
  REQUIRE(true == wireSet.GetState("y04"));
  REQUIRE(true == wireSet.GetState("z10"));
}

TEST_CASE("Task day 24")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("Day 24 - part 1")
  {
    std::cout << "Day 24 - part 1 result: " << WireSet(data).GetOutputZ() << std::endl;
  }

  SECTION("Day 24 - part 2")
  {
    std::cout << "Day 24 - part 2 result: " << WireSet(data).GetWrongOutputs() << std::endl;
  }
}