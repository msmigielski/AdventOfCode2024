#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <regex>
#include <unordered_set>

constexpr auto testData = R"(r, wr, b, g, bwu, rb, gb, br

brwrr
bggr
gbbr
rrbgbr
ubwu
bwurrg
brgr
bbrgwb)";

std::unordered_set<size_t> FindAllOccurrences(const std::string &text, const std::string &pattern)
{
  std::unordered_set<size_t> positions;

  std::regex regexPattern("(?=" + pattern + ")");
  auto matchesBegin = std::sregex_iterator(text.begin(), text.end(), regexPattern);
  auto matchesEnd = std::sregex_iterator();

  for (auto it = matchesBegin; it != matchesEnd; ++it)
  {
    positions.emplace(it->position());
  }

  return positions;
}

struct TowelProduction
{
  using PositionsOfWords = std::vector<std::pair<std::unordered_set<size_t>, std::string>>;

  TowelProduction(std::istream &input)
  {
    std::string line;
    std::getline(input, line);
    stripes = SplitByComma(line);

    std::getline(input, line);

    while (std::getline(input, line))
    {
      designs.push_back(line);
    }
  }

  std::vector<std::string> SplitByComma(const std::string &input)
  {
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, ','))
    {
      size_t start = token.find_first_not_of(" ");
      size_t end = token.find_last_not_of(" ");
      if (start != std::string::npos)
      {
        token = token.substr(start, end - start + 1);
      }
      result.push_back(token);
    }

    return result;
  }

  size_t CountProducableTowels()
  {
    size_t count = 0;
    for (const auto &design : designs)
    {
      count += IsProducible(design);
    }
    return count;
  }

  size_t CountPossibleCombinations()
  {
    size_t count = 0;
    for (const auto &design : designs)
    {
      if (!IsProducible(design))
      {
        continue;
      }
      for (size_t i = 0; i <= design.size(); ++i)
      {
        auto sub = design.substr(design.size() - i);
        if (!wordToCost.contains(sub))
          wordToCost[sub] = CountPossibleCombinations(sub);
      }
      count += wordToCost[design];
      // std::cout << design << " current solutions: " << count << std::endl;
    }

    return count;
  }

  bool IsProducible(const std::string &design)
  {
    std::vector<std::pair<std::unordered_set<size_t>, std::string>> positionsOfWords;
    for (const auto &stripe : stripes)
    {
      const auto &positions = FindAllOccurrences(design, stripe);
      if (!positions.empty())
      {
        positionsOfWords.push_back({positions, stripe});
      }
    }

    std::string currentWord;

    return CanDesignBeCreated(design, currentWord, positionsOfWords);
  }

  size_t CountPossibleCombinations(const std::string &design)
  {
    if (design.empty())
    {
      return 0;
    }

    std ::vector<std::pair<std::unordered_set<size_t>, std::string>> positionsOfWords;
    for (const auto &stripe : stripes)
    {
      const auto &positions = FindAllOccurrences(design, stripe);
      if (!positions.empty())
      {
        positionsOfWords.push_back({positions, stripe});
      }
    }

    std::string currentWord;
    size_t combinations = 0;
    CountPossibleCombinations(design, currentWord, positionsOfWords, combinations);

    return combinations;
  }

  void CountPossibleCombinations(const std::string &design, const std::string &currentWord, const PositionsOfWords &positionsOfWords, size_t &numOfSolutions)
  {
    if (design == currentWord)
    {
      ++numOfSolutions;
      return;
    }

    if (wordToCost.contains(design.substr(currentWord.size())))
    {
      numOfSolutions += wordToCost[design.substr(currentWord.size())];
      return;
    }

    for (const auto &[positions, word] : positionsOfWords)
    {
      if (positions.contains(currentWord.size()))
      {
        CountPossibleCombinations(design, currentWord + word, positionsOfWords, numOfSolutions);
      }
    }
  }

  bool CanDesignBeCreated(const std::string &design, const std::string &currentWord, const PositionsOfWords &positionsOfWords)
  {
    if (currentWord == design)
    {
      return true;
    }
    if (currentWord.size() >= design.size())
    {
      return false;
    }

    for (const auto &[positions, word] : positionsOfWords)
    {
      if (positions.contains(currentWord.size()))
      {
        if (CanDesignBeCreated(design, currentWord + word, positionsOfWords))
        {
          return true;
        }
      }
    }
    return false;
  }

  std::vector<std::string> stripes;
  std::vector<std::string> designs;
  std::unordered_map<std::string, size_t> wordToCost;
};

TEST_CASE("Find all")
{
  REQUIRE(std::unordered_set<size_t>{0u, 2u, 4u} == FindAllOccurrences("apapapap", "apap"));
}

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  TowelProduction tp{testInput};

  SECTION("Part 1")
  {
    REQUIRE(6 == tp.CountProducableTowels());
  }

  SECTION("Part 2")
  {
    REQUIRE(16 == tp.CountPossibleCombinations());
  }
}

TEST_CASE("Task day 19")
{
  Timer t;
  std::ifstream data("data.txt");
  REQUIRE(data.is_open());
  TowelProduction tp{data};

  SECTION("part 1")
  {
    std::cout << "Day 19 - part 1 result: " << tp.CountProducableTowels() << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 19 - part 2 result: " << tp.CountPossibleCombinations() << std::endl;
  }
}