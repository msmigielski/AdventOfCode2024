#include <catch2/catch_all.hpp>
#include <utils/Timer.h>
#include <fstream>
#include <list>
#include <utils/Timer.h>

constexpr auto testData = "2333133121414131402";

struct DataBlock
{
  int id;
  int size;
};

constexpr int emptyId = -1;

class Disk
{
public:
  Disk(std::istream &input)
  {
    int blockId = 0;
    int i = 0;
    char c;
    while (input >> c)
    {
      int blockSize = c - '0';
      if (i % 2 == 1)
      {
        dataBlocks.emplace_back(DataBlock{emptyId, blockSize});
      }
      else
      {
        dataBlocks.emplace_back(DataBlock{blockId, blockSize});
        ++blockId;
      }
      ++i;
    }
  }

  uint64_t GetFilesystemChecksum()
  {
    auto findEmptyBlock = [](const auto &el)
    { return el.id == emptyId && el.size > 0; };

    auto findFileBlock = [](const auto &el)
    { return el.id != emptyId && el.size > 0; };

    auto emptyIt = std::find_if(dataBlocks.begin(), dataBlocks.end(), findEmptyBlock);
    auto fileIt = std::find_if(dataBlocks.rbegin(), dataBlocks.rend(), findFileBlock);

    while (emptyIt != dataBlocks.end() && fileIt != dataBlocks.rend())
    {
      if (fileIt->size >= emptyIt->size)
      {
        dataBlocks.emplace(emptyIt, DataBlock{fileIt->id, emptyIt->size});
        fileIt->size -= emptyIt->size;
        emptyIt->size = 0;
      }
      else
      {
        dataBlocks.emplace(emptyIt, DataBlock{fileIt->id, fileIt->size});
        emptyIt->size -= fileIt->size;
        fileIt->size = 0;
      }
      emptyIt = std::find_if(emptyIt, dataBlocks.end(), findEmptyBlock);
      fileIt = std::find_if(fileIt, dataBlocks.rend(), findFileBlock);
    }

    uint64_t sum = 0;
    uint64_t index = 0;
    for (const auto &block : dataBlocks)
    {
      for (size_t i = 0; i < static_cast<size_t>(block.size); ++i)
      {
        const auto val = block.id != -1 ? block.id : 0;
        sum += index * val;
        ++index;
      }
    }

    return sum;
  }

  uint64_t GetFilesystemChecksumWithWholeBlocks()
  {
    auto findFileBlock = [](const auto &el)
    { return el.id != emptyId && el.size > 0; };

    auto fileIt = std::find_if(dataBlocks.rbegin(), dataBlocks.rend(), findFileBlock);

    while (fileIt != dataBlocks.rend())
    {
      auto findEmptyBlock = [fileIt](const auto &el)
      { return el.id == emptyId && el.size >= fileIt->size; };
      auto emptyIt = std::find_if(dataBlocks.begin(), dataBlocks.end(), findEmptyBlock);

      if (emptyIt != dataBlocks.end() && std::distance(dataBlocks.begin(), emptyIt) < std::distance(dataBlocks.begin(), fileIt.base()))
      {
        dataBlocks.emplace(emptyIt, DataBlock{fileIt->id, fileIt->size});
        emptyIt->size -= fileIt->size;
        fileIt->id = emptyId;
      }

      fileIt = std::find_if(std::next(fileIt), dataBlocks.rend(), findFileBlock);
    }

    uint64_t sum = 0;
    uint64_t index = 0;
    for (const auto &block : dataBlocks)
    {
      for (size_t i = 0; i < static_cast<size_t>(block.size); ++i)
      {
        const auto val = block.id != -1 ? block.id : 0;
        sum += index * val;
        ++index;
      }
    }

    return sum;
  }

  std::list<DataBlock> dataBlocks;
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