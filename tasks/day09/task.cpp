#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <list>
#include <utils/Timer.h>

constexpr auto testData = "2333133121414131402";

struct DataBlock
{
  std::vector<int> ids;
  int capacity;
};

class Disk
{
public:
  Disk(std::istream &input)
  {
    int blockId = 0;
    int i = 0;
    std::string line;
    std::getline(input, line);
    dataBlocks = std::vector<DataBlock>(line.size());
    for (const auto &c : line)
    {
      const int blockSize = c - '0';
      if (i % 2 == 0)
      {
        dataBlocks[i].capacity = blockSize;
        dataBlocks[i].ids = std::vector<int>(blockSize, blockId);
        ++blockId;
      }
      else
      {
        dataBlocks[i].capacity = blockSize;
      }
      ++i;
    }
  }

  uint64_t GetFilesystemChecksum()
  {
    auto findBlockWithEmptySpace = [](const auto &el)
    { return el.ids.size() != static_cast<size_t>(el.capacity); };

    auto findFileBlock = [](const auto &el)
    { return el.ids.size() > 0; };

    std::vector<DataBlock>::iterator emptyIt = std::find_if(dataBlocks.begin(), dataBlocks.end(), findBlockWithEmptySpace);
    std::vector<DataBlock>::reverse_iterator fileIt = std::find_if(dataBlocks.rbegin(), dataBlocks.rend(), findFileBlock);

    while (emptyIt != dataBlocks.end() && fileIt != dataBlocks.rend() && emptyIt < std::prev(fileIt.base()))
    {
      while (emptyIt->ids.size() != static_cast<size_t>(emptyIt->capacity) &&
             fileIt->ids.size() > 0)
      {
        auto val = fileIt->ids.back();
        fileIt->ids.pop_back();
        emptyIt->ids.emplace_back(val);
      }

      emptyIt = std::find_if(emptyIt, dataBlocks.end(), findBlockWithEmptySpace);
      fileIt = std::find_if(fileIt, dataBlocks.rend(), findFileBlock);
    }

    uint64_t sum = 0;
    uint64_t index = 0;
    for (const auto &block : dataBlocks)
    {
      for (const auto &id : block.ids)
      {
        sum += index * id;
        ++index;
      }
    }

    return sum;
  }

  uint64_t GetFilesystemChecksumWithWholeBlocks()
  {
    auto findFileBlock = [](const auto &el)
    { return el.ids.size() > 0; };

    std::vector<DataBlock>::reverse_iterator fileIt = std::find_if(dataBlocks.rbegin(), dataBlocks.rend(), findFileBlock);

    while (fileIt != dataBlocks.rend())
    {
      auto findBlockWithEmptySpace = [&fileIt](const auto &el)
      { return (static_cast<size_t>(el.capacity) - el.ids.size()) >= static_cast<size_t>(fileIt->capacity); };

      std::vector<DataBlock>::iterator emptyIt = std::find_if(dataBlocks.begin(), fileIt.base(), findBlockWithEmptySpace);

      if (emptyIt != dataBlocks.end() && emptyIt < std::prev(fileIt.base()))
      {
        while (!fileIt->ids.empty())
        {
          auto val = fileIt->ids.back();
          fileIt->ids.pop_back();
          emptyIt->ids.emplace_back(val);
        }
      }

      fileIt = std::find_if(fileIt + 1, dataBlocks.rend(), findFileBlock);
    }

    uint64_t sum = 0;
    uint64_t index = 0;
    for (const auto &block : dataBlocks)
    {
      for (const auto &id : block.ids)
      {
        sum += index * id;
        ++index;
      }
      for (size_t i = 0; i < block.capacity - block.ids.size(); ++i)
      {
        ++index;
      }
    }

    return sum;
  }

  std::vector<DataBlock>
      dataBlocks;
};

TEST_CASE("Check with test data")
{
  std::stringstream testInput{testData};
  Disk disk{testInput};

  SECTION("Part 1")
  {
    REQUIRE(1928u == disk.GetFilesystemChecksum());
  }

  SECTION("Part 2")
  {
    REQUIRE(2858u == disk.GetFilesystemChecksumWithWholeBlocks());
  }
}

TEST_CASE("Task day 9")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  Disk disk{data};

  SECTION("part 1")
  {
    std::cout << "Day 9 - part 1 result: " << disk.GetFilesystemChecksum() << std::endl;
  }

  SECTION("part 2")
  {
    std::cout << "Day 9 - part 2 result: " << disk.GetFilesystemChecksumWithWholeBlocks() << std::endl;
  }
}