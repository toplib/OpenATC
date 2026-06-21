#pragma once

#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Util {

template<typename Node, typename Cost>
struct AStarResult {
  std::vector<Node> path;
  Cost totalCost;
  bool found;
};

template<typename Node, typename Cost = double, typename Heuristic, typename Neighbors>
AStarResult<Node, Cost> aStar(
  Node start,
  Node goal,
  Heuristic heuristic,
  Neighbors getNeighbors
) {
  using Entry = std::pair<Cost, Node>;
  std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> open;
  std::unordered_map<Node, Cost> gScore;
  std::unordered_map<Node, Node> cameFrom;

  gScore[start] = Cost(0);
  open.push({heuristic(start), start});

  while (!open.empty()) {
    Node current = open.top().second;
    Cost currentF = open.top().first;
    open.pop();

    if (current == goal) {
      std::vector<Node> path;
      Node node = current;
      while (!(node == start)) {
        path.push_back(node);
        node = cameFrom[node];
      }
      path.push_back(start);
      std::reverse(path.begin(), path.end());
      return {path, gScore[current], true};
    }

    if (currentF > gScore[current] + heuristic(current))
      continue;

    std::vector<std::pair<Node, Cost>> neighbors;
    getNeighbors(current, neighbors);

    for (auto &nb : neighbors) {
      Node neighbor = nb.first;
      Cost edgeCost = nb.second;
      Cost tentativeG = gScore[current] + edgeCost;
      auto it = gScore.find(neighbor);
      if (it == gScore.end() || tentativeG < it->second) {
        cameFrom[neighbor] = current;
        gScore[neighbor] = tentativeG;
        open.push({tentativeG + heuristic(neighbor), neighbor});
      }
    }
  }

  return {{}, Cost(0), false};
}

} // namespace Util
