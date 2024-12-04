#include <catch2/catch_all.hpp>
#include <iostream>
#include <fstream>

constexpr auto testData = R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX)";

std::vector<std::string> ReadLines(std::istream &input)
{
  std::vector<std::string> lines;
  std::string line;

  while (std::getline(input, line))
  {
    lines.push_back(line);
  }
  return lines;
}

int CountXmas(const std::vector<std::string> &lines)
{
  int xmasCount = 0;
  auto checkWords = [](const std::vector<std::string> &words)
  {
    return std::count_if(words.begin(), words.end(), [](const std::string &word)
                         { return word == "XMAS" || std::string(word.rbegin(), word.rend()) == "XMAS"; });
  };

  for (size_t row = 0; row < lines.size(); ++row)
  {
    for (size_t column = 0; column < lines[row].size(); ++column)
    {
      std::vector<std::string> words;

      // horizontal
      if (column < lines[row].size() - 3)
        words.push_back(lines[row].substr(column, 4));

      // vertical
      if (row < lines.size() - 3)
        words.push_back({lines[row][column], lines[row + 1][column], lines[row + 2][column], lines[row + 3][column]});

      // diagonally down-right
      if (row < lines.size() - 3 && column < lines[row].size() - 3)
        words.push_back({lines[row][column], lines[row + 1][column + 1], lines[row + 2][column + 2], lines[row + 3][column + 3]});

      // diagonally down-left
      if (row < lines.size() - 3 && column >= 3)
        words.push_back({lines[row][column], lines[row + 1][column - 1], lines[row + 2][column - 2], lines[row + 3][column - 3]});

      xmasCount += checkWords(words);
    }
  }

  return xmasCount;
}

int CountCrossedMas(const std::vector<std::string> &lines)
{
  int xmasCount = 0;

  for (size_t row = 1; row < lines.size() - 1; ++row)
  {
    for (size_t column = 1; column < lines.at(row).size() - 1; ++column)
    {
      std::vector<std::string> words;
      std::string xmas = "MAS";

      words.push_back(std::string{lines.at(row - 1)[column - 1], lines.at(row)[column], lines.at(row + 1)[column + 1]});
      words.push_back(std::string{lines.at(row - 1)[column + 1], lines.at(row)[column], lines.at(row + 1)[column - 1]});

      xmasCount += (words[0] == xmas || std::string(words[0].rbegin(), words[0].rend()) == xmas) &&
                   (words[1] == xmas || std::string(words[1].rbegin(), words[1].rend()) == xmas);
    }
  }

  return xmasCount;
}

TEST_CASE("Check part 1 with test data")
{
  std::stringstream testInput{testData};
  REQUIRE(CountXmas(ReadLines(testInput)) == 18);
}

TEST_CASE("Check part 2 with test data")
{
  std::stringstream testInput{testData};
  REQUIRE(CountCrossedMas(ReadLines(testInput)) == 9);
}

TEST_CASE("Task day 4")
{
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("part 1")
  {
    std::cout << "Day 4 - part 1 result: " << CountXmas(ReadLines(data)) << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 4 - part 2 result: " << CountCrossedMas(ReadLines(data)) << std::endl;
  }
}