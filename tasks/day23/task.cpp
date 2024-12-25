#include <catch2/catch_all.hpp>
#include <unordered_set>
#include <fstream>
#include <utils/Timer.h>

constexpr auto testData = R"(kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn)";

struct Node
{
  Node(const std::string &name_) : name(name_) {}
  std::string name;
  std::unordered_set<std::string> siblings;
};

struct Graph
{
  Graph(std::istream &input)
  {
    std::string line;
    while (std::getline(input, line))
    {
      std::string first = line.substr(0, 2);
      std::string second = line.substr(3, 2);

      if (!nodes.contains(first))
      {
        nodes[first] = std::make_unique<Node>(first);
      }

      if (!nodes.contains(second))
      {
        nodes[second] = std::make_unique<Node>(second);
      }

      nodes.at(first)->siblings.emplace(nodes[second]->name);
      nodes.at(second)->siblings.emplace(nodes[first]->name);
    }
  }

  std::unordered_map<std::string, std::unique_ptr<Node>> nodes;
};

std::string GetSetKey(const std::string &a, const std::string &b, const std::string &c)
{
  std::vector<std::string> nodes = {a, b, c};
  std::sort(nodes.begin(), nodes.end());
  return nodes[0] + "-" + nodes[1] + "-" + nodes[2];
}

size_t CountSetsWithT(std::istream &input)
{
  std::unordered_set<std::string> sets;
  Graph g{input};
  for (const auto &[name, node] : g.nodes)
  {
    if (node->siblings.size() < 2)
    {
      continue;
    }

    for (const auto &sibling1 : node->siblings)
    {
      for (const auto &sibling2 : node->siblings)
      {
        if (sibling1 == sibling2)
        {
          continue;
        }

        if (g.nodes[sibling1]->siblings.contains(sibling2))
        {
          sets.emplace(GetSetKey(name, sibling1, sibling2));
        }
      }
    }
  }
  return std::ranges::count_if(sets, [](const auto &el)
                               { return el.find('t') == 0 || el.find("-t") != std::string::npos; });
}

void CheckSetWithSibling(std::unordered_set<std::string> &set, const std::string &node, const Graph &graph)
{
  std::vector<std::string> notPresent;
  for (const auto &sibling : set)
  {
    if (!graph.nodes.at(node)->siblings.contains(sibling))
    {
      notPresent.push_back(sibling);
    }
  }
  for (const auto &n : notPresent)
  {
    set.erase(n);
  }
}

void bronKerbosch(
    std::unordered_set<std::string> &R,
    std::unordered_set<std::string> &P,
    std::unordered_set<std::string> &X,
    std::unordered_set<std::string> &maxClique,
    const Graph &graph)
{
  if (P.empty() && X.empty())
  {
    if (R.size() > maxClique.size())
    {
      maxClique = R;
    }
    return;
  }

  auto P_copy = P;

  for (const auto &nodeName : P_copy)
  {
    R.insert(nodeName);

    std::unordered_set<std::string> newP, newX;
    const auto &neighbors = graph.nodes.at(nodeName)->siblings;

    for (const auto &pNode : P)
    {
      if (neighbors.contains(pNode))
      {
        newP.insert(pNode);
      }
    }

    for (const auto &xNode : X)
    {
      if (neighbors.contains(xNode))
      {
        newX.insert(xNode);
      }
    }

    bronKerbosch(R, newP, newX, maxClique, graph);

    R.erase(nodeName);
    P.erase(nodeName);
    X.insert(nodeName);
  }
}

std::unordered_set<std::string> findMaximumClique(const Graph &graph)
{
  std::unordered_set<std::string> R, P, X, maxClique;

  for (const auto &[nodeName, node] : graph.nodes)
  {
    P.insert(nodeName);
  }

  bronKerbosch(R, P, X, maxClique, graph);

  return maxClique;
}

TEST_CASE("Check with test data - part 1")
{
  std::stringstream testInput{testData};

  REQUIRE(7 == CountSetsWithT(testInput));
}

TEST_CASE("Check with test data - part 2")
{
  std::stringstream testInput{testData};
  Graph graph{testInput};

  REQUIRE(4 == findMaximumClique(graph).size());
}

TEST_CASE("Task day 23")
{
  Timer t;
  std::ifstream data("data.txt");

  REQUIRE(data.is_open());

  SECTION("Day 23 - part 1")
  {
    std::cout << "Day 23 - part 1 result: " << CountSetsWithT(data) << std::endl;
  }

  SECTION("Day 23 - part 1")
  {
    Graph g{data};
    const auto &maxClique = findMaximumClique(g);
    std::vector<std::string> solution{maxClique.begin(), maxClique.end()};
    std::ranges::sort(solution);
    std::string password;
    for (const auto &node : solution)
    {
      if (!password.empty())
      {
        password += ',';
      }
      password += node;
    }
    std::cout << "Day 23 - part 2 result: " << password << std::endl;
  }
}