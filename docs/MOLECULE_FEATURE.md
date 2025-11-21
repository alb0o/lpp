# Molecule/Graph Syntax - Complete Documentation

## Overview

L++ provides native syntax for declaring and manipulating graphs through the `mol` (molecule) keyword. This feature allows you to define graph structures declaratively with minimal boilerplate, making L++ ideal for applications in networking, AI, game development, and data modeling.

## Syntax

### Basic Declaration

```lpp
mol GraphName {
    NodeA - NodeB;
    NodeB - NodeC;
}
```

### Bond Types

L++ supports four types of bonds (edges):

| Operator | Type | Description | Use Case |
|----------|------|-------------|----------|
| `-` | Single | Undirected edge | Social networks, mutual connections |
| `=` | Double | Strong bidirectional edge | Database relationships, strong coupling |
| `->` | Arrow | Directed edge | Dependencies, workflows, DAGs |
| `<->` | Bidirectional | Explicit two-way edge | Communication channels, APIs |

## Examples

### Simple Undirected Graph

```lpp
mol SocialNetwork {
    Alice - Bob;
    Bob - Charlie;
    Charlie - Alice;
}
```

**Generated C++:**
```cpp
Molecule<std::string> SocialNetwork;
SocialNetwork.addAtom("Alice");
SocialNetwork.addAtom("Bob");
SocialNetwork.addAtom("Charlie");
SocialNetwork.addBond("Alice", "Bob", BondType::SINGLE);
SocialNetwork.addBond("Bob", "Charlie", BondType::SINGLE);
SocialNetwork.addBond("Charlie", "Alice", BondType::SINGLE);
```

### Directed Acyclic Graph (DAG)

```lpp
mol BuildPipeline {
    Source -> Compile;
    Compile -> Test;
    Test -> Deploy;
}
```

**Use case:** CI/CD pipelines, dependency graphs

### Mixed Graph Types

```lpp
mol Microservices {
    Gateway - AuthService;
    Gateway - UserService;
    UserService -> Database;
    AuthService -> Database;
    Database = Cache;
}
```

**Use case:** Service architectures, infrastructure diagrams

### Star Topology

```lpp
mol Hub {
    Center - Node1;
    Center - Node2;
    Center - Node3;
    Center - Node4;
}
```

**Use case:** Network topologies, hierarchical structures

### Bidirectional Communication

```lpp
mol API {
    Client <-> Server;
    Server <-> Database;
    Server <-> Cache;
}
```

**Use case:** Two-way communication channels, websockets

## Runtime API

The `Molecule<T>` class provides a comprehensive API for graph manipulation and analysis:

### Basic Operations

```lpp
// Add nodes manually
graph.addAtom("NodeX");

// Add edges manually
graph.addBond("NodeX", "NodeY", BondType::SINGLE);

// Check existence
let exists = graph.hasAtom("NodeX");      // bool
let bondExists = graph.hasBond("A", "B", BondType::SINGLE);

// Get information
let atomCount = graph.atomCount();        // size_t
let bondCount = graph.bondCount();        // size_t
let isEmpty = graph.empty();              // bool

// Clear all data
graph.clear();
```

### Graph Traversal

```lpp
// Get neighbors of a node
let neighbors = graph.neighbors("NodeA");  // vector<T>

// Breadth-First Search from a starting node
let bfsOrder = graph.bfs("Start");         // vector<T>

// Depth-First Search from a starting node
let dfsOrder = graph.dfs("Start");         // vector<T>
```

### Graph Analysis

```lpp
// Check if there's a path between two nodes
let connected = graph.hasPath("A", "B");   // bool

// Check if entire graph is connected
let isConnected = graph.isConnected();     // bool

// Detect cycles
let hasCycle = graph.hasCycle();           // bool
```

## Complete API Reference

### Constructor & Rule of Five

```cpp
Molecule()                           // Default constructor
Molecule(const Molecule&)            // Copy constructor
Molecule(Molecule&&) noexcept       // Move constructor
Molecule& operator=(const Molecule&) // Copy assignment
Molecule& operator=(Molecule&&) noexcept // Move assignment
~Molecule()                          // Destructor
```

### Modification Methods

| Method | Parameters | Returns | Description |
|--------|-----------|---------|-------------|
| `addAtom` | `const T& atom` | `void` | Add a node (idempotent) |
| `addBond` | `const T& from, const T& to, BondType type` | `void` | Add an edge (checks duplicates) |
| `clear` | - | `void` | Remove all nodes and edges |

### Query Methods

| Method | Parameters | Returns | Description |
|--------|-----------|---------|-------------|
| `hasAtom` | `const T& atom` | `bool` | Check if node exists |
| `hasBond` | `const T& from, const T& to, BondType type` | `bool` | Check if specific edge exists |
| `neighbors` | `const T& atom` | `vector<T>` | Get adjacent nodes |
| `atomCount` | - | `size_t` | Number of nodes |
| `bondCount` | - | `size_t` | Number of edges |
| `empty` | - | `bool` | Check if graph is empty |
| `getAtoms` | - | `const unordered_set<T>&` | All nodes (const ref) |
| `getBonds` | - | `const vector<MoleculeBond<T>>&` | All edges (const ref) |

### Graph Algorithms

| Method | Parameters | Returns | Description |
|--------|-----------|---------|-------------|
| `bfs` | `const T& start` | `vector<T>` | Breadth-first traversal order |
| `dfs` | `const T& start` | `vector<T>` | Depth-first traversal order |
| `hasPath` | `const T& from, const T& to` | `bool` | Check reachability |
| `isConnected` | - | `bool` | Check if graph is fully connected |
| `hasCycle` | - | `bool` | Detect cycles (undirected) |

## Implementation Details

### Internal Representation

Graphs are internally represented using an **adjacency list** for optimal performance on sparse graphs:

```cpp
template <typename T>
class Molecule {
private:
    std::unordered_map<T, std::vector<T>> adjacency;  // O(1) neighbor lookup
    std::vector<MoleculeBond<T>> bonds;                // Edge storage
    std::unordered_set<T> atoms;                       // O(1) existence check
};
```

### Bond Type Behavior

| Bond Type | Direction | Adds to Adjacency |
|-----------|-----------|-------------------|
| `SINGLE` | Undirected | Both `from->to` and `to->from` |
| `DOUBLE` | Bidirectional | Both `from->to` and `to->from` |
| `ARROW` | Directed | Only `from->to` |
| `BIDIRECTIONAL` | Explicit both ways | Both `from->to` and `to->from` |

### Complexity Analysis

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| `addAtom` | O(1) average | O(1) |
| `addBond` | O(E) worst case* | O(1) |
| `hasAtom` | O(1) average | O(1) |
| `neighbors` | O(degree) | O(degree) |
| `bfs/dfs` | O(V + E) | O(V) |
| `hasPath` | O(V + E) | O(V) |
| `hasCycle` | O(V + E) | O(V) |
| `isConnected` | O(V + E) | O(V) |

*Due to duplicate bond detection (can be optimized with a bond set)

## Parser Validation

The parser performs strict validation:

### ✅ Validates:
- Bond operator syntax (only `-`, `=`, `->`, `<->` allowed)
- Semicolons after each bond declaration (required)
- Empty atom names (rejected)
- Duplicate bonds (same `from`, `to`, `type` - ignored)
- Duplicate atoms (silently deduplicated)

### ❌ Rejects:
- Invalid operators (`+`, `*`, `/`, etc.)
- Missing semicolons
- Malformed `<->` operator (e.g., `<-`, `<--`, etc.)

### Error Messages

```
Parse error at line 5, column 10:
Expected valid bond operator (-, =, ->, or <->) after atom 'NodeA'
```

```
Parse error at line 3, column 15:
Missing ';' after bond declaration 'A ... B'
```

## Use Cases

### 1. Network Programming

```lpp
mol ServerTopology {
    LoadBalancer <-> WebServer1;
    LoadBalancer <-> WebServer2;
    WebServer1 -> Database;
    WebServer2 -> Database;
    Database = Cache;
}

fn main() {
    if ServerTopology.isConnected() {
        println("Network is fully connected");
    }
}
```

### 2. State Machines

```lpp
mol GameState {
    Menu -> Playing;
    Playing -> Paused;
    Paused -> Playing;
    Playing -> GameOver;
    GameOver -> Menu;
}

fn main() {
    if GameState.hasCycle() {
        println("State machine allows loops");
    }
}
```

### 3. Dependency Resolution

```lpp
mol PackageDeps {
    AppModule -> CoreModule;
    AppModule -> UIModule;
    UIModule -> CoreModule;
    CoreModule -> UtilsModule;
}

fn main() {
    let buildOrder = PackageDeps.bfs("UtilsModule");
    println(buildOrder);  // [UtilsModule, CoreModule, UIModule, AppModule]
}
```

### 4. Social Networks

```lpp
mol Friendships {
    Alice - Bob;
    Bob - Charlie;
    Charlie - Diana;
    Alice - Diana;
}

fn main() {
    let aliceFriends = Friendships.neighbors("Alice");
    println(aliceFriends);  // [Bob, Diana]
    
    if Friendships.hasPath("Alice", "Charlie") {
        println("Alice and Charlie are connected");
    }
}
```

### 5. Knowledge Graphs

```lpp
mol Ontology {
    Mammal -> Animal;
    Dog -> Mammal;
    Cat -> Mammal;
    Animal -> LivingBeing;
}

fn main() {
    // Check if Dog is a LivingBeing (through inheritance)
    if Ontology.hasPath("Dog", "LivingBeing") {
        println("Dog is a living being");
    }
}
```

## Advanced Patterns

### Dynamic Graph Modification

```lpp
mol DynamicGraph {
    A - B;
}

fn main() {
    // Add nodes and edges at runtime
    DynamicGraph.addAtom("C");
    DynamicGraph.addBond("B", "C", BondType::SINGLE);
    
    println(DynamicGraph.atomCount());  // 3
}
```

### Graph Copying

```lpp
mol Template {
    Root - Child1;
    Root - Child2;
}

fn main() {
    let graphCopy = Template;  // Uses copy constructor
    graphCopy.addAtom("Child3");
    
    // Template remains unchanged
    println(Template.atomCount());  // 3
    println(graphCopy.atomCount()); // 4
}
```

### Graph Analysis

```lpp
mol ComplexNetwork {
    A - B;
    B - C;
    C - D;
    D - A;
}

fn main() {
    // Cycle detection
    if ComplexNetwork.hasCycle() {
        println("Graph has cycles");
    }
    
    // Connectivity check
    if ComplexNetwork.isConnected() {
        println("All nodes are reachable");
    }
    
    // Traversal comparison
    let bfsOrder = ComplexNetwork.bfs("A");
    let dfsOrder = ComplexNetwork.dfs("A");
    println("BFS:", bfsOrder);
    println("DFS:", dfsOrder);
}
```

## Performance Tips

### 1. Reserve Space (Future Enhancement)

For known graph sizes, pre-allocation improves performance:

```cpp
// Future API (not yet implemented)
molecule.reserve(1000);  // Reserve space for 1000 nodes
```

### 2. Use Appropriate Bond Types

- Use `ARROW` for directed graphs (DAGs) - saves memory (no reverse edges)
- Use `SINGLE` for undirected graphs - automatic bidirectional edges
- Use `BIDIRECTIONAL` for explicit two-way relationships

### 3. Batch Operations

Define all bonds in the `mol` block rather than adding them individually at runtime for better transpiler optimization.

## Limitations & Future Work

### Current Limitations

1. **Generic Types**: Only `Molecule<std::string>` currently supported
2. **Weighted Edges**: No support for edge weights yet
3. **Node Attributes**: Nodes cannot have custom data
4. **Shortest Path**: No Dijkstra/A* implementation yet
5. **Serialization**: No built-in save/load functionality

### Planned Features

- [ ] Weighted edges: `A -[5]> B`
- [ ] Generic atom types: `mol Graph<int>`
- [ ] Node attributes: `A[x=10, y=20] - B`
- [ ] Shortest path algorithms
- [ ] Graph visualization export (DOT format)
- [ ] Iterators for atoms and bonds
- [ ] Connected components detection
- [ ] Topological sort for DAGs

## Best Practices

### ✅ Do:

```lpp
// Use descriptive node names
mol UserFlow {
    LoginPage -> Dashboard;
    Dashboard -> ProfilePage;
}

// Check before operations
if graph.hasAtom("NodeX") {
    let neighbors = graph.neighbors("NodeX");
}

// Use appropriate bond types
mol DirectedWorkflow {
    Start -> Process -> End;  // Directed
}

mol UndirectedNetwork {
    Server1 - Server2 - Server3;  // Undirected
}
```

### ❌ Don't:

```lpp
// Don't use single-letter names (unless mathematical)
mol G {
    a - b;  // Hard to understand
}

// Don't mix semantics
mol Confused {
    A - B;   // Undirected
    B -> C;  // Directed
    // Mixing can lead to unexpected behavior
}

// Don't forget to check existence
let neighbors = graph.neighbors("NonExistent");  // Returns empty vector, no error
```

## Comparison with Other Languages

### Python (NetworkX)

**Before (Python):**
```python
import networkx as nx

G = nx.Graph()
G.add_edge('A', 'B')
G.add_edge('B', 'C')
G.add_edge('C', 'A')

if nx.has_path(G, 'A', 'C'):
    print("Connected")
```

**After (L++):**
```lpp
mol G {
    A - B;
    B - C;
    C - A;
}

if G.hasPath("A", "C") {
    println("Connected");
}
```

**Advantages:**
- ✅ 3 lines vs 8 lines
- ✅ Declarative syntax
- ✅ Native performance (C++)
- ✅ Compile-time type checking

### C++ (Boost.Graph)

**Before (C++):**
```cpp
#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<> Graph;
Graph g;
auto vA = boost::add_vertex(g);
auto vB = boost::add_vertex(g);
auto vC = boost::add_vertex(g);
boost::add_edge(vA, vB, g);
boost::add_edge(vB, vC, g);
```

**After (L++):**
```lpp
mol G {
    A - B;
    B - C;
}
```

**Advantages:**
- ✅ No boilerplate
- ✅ No manual vertex management
- ✅ Readable syntax
- ✅ Automatic memory management

### JavaScript (Graph libraries)

**Before (JavaScript):**
```javascript
const graph = {
    'A': ['B'],
    'B': ['C'],
    'C': ['A']
};

// Manual BFS implementation required
```

**After (L++):**
```lpp
mol graph {
    A - B;
    B - C;
    C - A;
}

let bfs = graph.bfs("A");
```

**Advantages:**
- ✅ Built-in algorithms
- ✅ Type safety
- ✅ Better performance

## Conclusion

The `mol` syntax in L++ provides a powerful, intuitive way to work with graphs that combines:

- **Declarative syntax** - Minimal boilerplate
- **Native performance** - Compiles to optimized C++
- **Type safety** - Compile-time checking
- **Rich API** - Graph algorithms built-in
- **Production-ready** - Robust validation and error handling

This feature makes L++ uniquely suited for applications in networking, AI, game development, infrastructure as code, and any domain where graph structures are fundamental.

---

**Version:** L++ v0.8.17  
**Feature Status:** Production-Ready ✅  
**Last Updated:** 2025-11-19
