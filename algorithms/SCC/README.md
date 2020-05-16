# Strongly Connected Components

A directed graph is strongly connected if there is a path between all pairs of vertices.

A strongly connected component (SCC) of a directed graph is a maximal strongly connected subgraph.

#### Algorithms:

- [Kosaraju](https://en.wikipedia.org/wiki/Kosaraju%27s_algorithm) | O(V+E)
- [Tarjan](https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm) | O(V+E)
- [Path-based](https://en.wikipedia.org/wiki/Path-based_strong_component_algorithm)

#### Based on: *DFS*

## Kosaraju

1) dfs and build stack
2) transpose graph
3) pop stack and run dfs


## Tarjan

1) dfs / v.index / v.lowlink
v.index - индекс при dfs обходе (время посещения - входа)
v.lowlink - индекс предка в dfs-дереве


