#include <catch2/catch_all.hpp>
#include <iostream>
#include <fstream>

struct IReport
{
public:
  virtual bool IsSafe() = 0;

protected:
  enum class State
  {
    Unknown,
    Increasing,
    Decreasing
  };

  static State GetState(int val, int next)
  {
    if (next > val)
    {
      return State::Increasing;
    }
    else if (next < val)
    {
      return State::Decreasing;
    }
    return State::Unknown;
  }
};

class Report : public IReport
{
public:
  Report(const std::string &report_) : report(report_) {};
  bool IsSafe() override
  {
    std::istringstream stream(report);
    int val = 0;
    int last = 0;
    stream >> last;

    State lastState = State::Unknown;

    while (stream >> val)
    {
      auto state = GetState(last, val);
      if (std::abs(val - last) > 3 ||
          state == State::Unknown ||
          (state != lastState && lastState != State::Unknown))
      {
        return false;
      }

      lastState = state;
      last = val;
    }
    return true;
  }

private:
  std::string report;
};

class ReportWithDampener : public IReport
{
public:
  ReportWithDampener(const std::string &report_)
  {
    std::istringstream stream(report_);
    report_values = std::vector<int>(std::istream_iterator<int>(stream), std::istream_iterator<int>());
  };

  bool IsSafe() override
  {
    return IsSafeWithDampener(report_values) || IsSafeWithDampener({report_values.rbegin(), report_values.rend()});
  }

private:
  bool IsSafeWithDampener(const std::vector<int> &values)
  {
    State lastState = State::Unknown;
    bool wasSkipped = false;
    int last = values[0];

    for (size_t i = 1; i < values.size(); ++i)
    {
      int val = values[i];
      auto state = GetState(last, val);

      if (std::abs(val - last) > 3 ||
          state == State::Unknown ||
          (state != lastState && lastState != State::Unknown))
      {
        if (!wasSkipped)
        {
          wasSkipped = true;
          continue;
        }
        return false;
      }
      lastState = state;
      last = val;
    }
    return true;
  }

  std::vector<int> report_values;
};

int CountSafeReports(std::istream &input)
{
  std::string line;
  int count = 0;

  while (std::getline(input, line))
  {
    count += Report(line).IsSafe();
  }
  return count;
}

int CountSafeReportsWithDampener(std::istream &input)
{
  std::string line;
  int count = 0;

  while (std::getline(input, line))
  {
    count += ReportWithDampener(line).IsSafe();
  }
  return count;
}

TEST_CASE("Check if safe")
{
  REQUIRE(Report("7 6 4 2 1").IsSafe());
  REQUIRE_FALSE(Report("1 2 7 8 9").IsSafe());
  REQUIRE_FALSE(Report("9 7 6 2 1").IsSafe());
  REQUIRE_FALSE(Report("1 3 2 4 5").IsSafe());
  REQUIRE_FALSE(Report("8 6 4 4 1").IsSafe());
  REQUIRE(Report("1 3 6 7 9").IsSafe());
}

TEST_CASE("Check with dampener if safe")
{
  REQUIRE(ReportWithDampener("7 6 4 2 1").IsSafe());
  REQUIRE_FALSE(ReportWithDampener("1 2 7 8 9").IsSafe());
  REQUIRE_FALSE(ReportWithDampener("9 7 6 2 1").IsSafe());
  REQUIRE(ReportWithDampener("1 3 2 4 5").IsSafe());
  REQUIRE(ReportWithDampener("8 6 4 4 1").IsSafe());
  REQUIRE(ReportWithDampener("1 3 6 7 9").IsSafe());
}

TEST_CASE("Task day 2")
{
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 2 - part 1 result: " << CountSafeReports(data) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 2 - part 2 result: " << CountSafeReportsWithDampener(data) << std::endl;
  }
}