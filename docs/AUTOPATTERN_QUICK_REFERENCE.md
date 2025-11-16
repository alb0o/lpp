# L++ Autopattern Quick Reference

## 🚀 One-Line Design Patterns

The `autopattern` keyword automatically generates complete implementations of Gang of Four design patterns.

---

## 📖 Usage

```lpp
autopattern <PatternName> <ClassName>;
```

---

## 🔨 Creational Patterns

### Singleton
**Purpose:** One instance only  
**Keywords:** `Singleton`, `Config`, `Manager`, `Global`

```lpp
autopattern Singleton AppConfig;
let config = AppConfig::getInstance();
```

---

### Factory
**Purpose:** Object creation without specifying class  
**Keywords:** `Factory`, `Create`, `Make`, `Build`

```lpp
autopattern Factory ShapeFactory;
let circle = ShapeFactory::create("Circle");
```

---

### AbstractFactory
**Purpose:** Families of related objects  
**Keywords:** `AbstractFactory`, `FamilyOf`, `Suite`

```lpp
autopattern AbstractFactory UIFactory;
let button = UIFactory::createProductA();
let label = UIFactory::createProductB();
```

---

### Builder
**Purpose:** Complex object construction  
**Keywords:** `Builder`, `Construct`, `Assemble`

```lpp
autopattern Builder DocumentBuilder;
let builder = DocumentBuilder();
builder.buildPartA();
let doc = builder.getResult();
```

---

### Prototype
**Purpose:** Clone objects  
**Keywords:** `Prototype`, `Clone`, `Copy`

```lpp
autopattern Prototype Shape;
let original = Shape();
let copy = original.clone();
```

---

## 🏗️ Structural Patterns

### Adapter
**Purpose:** Convert interfaces  
**Keywords:** `Adapter`, `Wrapper`, `Convert`

```lpp
autopattern Adapter LegacyAdapter;
let adapter = LegacyAdapter();
adapter.request();
```

---

### Bridge
**Purpose:** Decouple abstraction/implementation  
**Keywords:** `Bridge`, `Decouple`, `Separate`

```lpp
autopattern Bridge DrawingAPI;
let api = DrawingAPI();
api.operation();
```

---

### Composite
**Purpose:** Tree structures  
**Keywords:** `Composite`, `Tree`, `Hierarchy`

```lpp
autopattern Composite FileNode;
let root = FileNode();
root.add(child);
root.operation();
```

---

### Decorator
**Purpose:** Add responsibilities dynamically  
**Keywords:** `Decorator`, `Enhance`, `Wrap`

```lpp
autopattern Decorator RichText;
let text = RichText();
text.operation();
```

---

### Facade
**Purpose:** Unified interface to subsystems  
**Keywords:** `Facade`, `Simplify`, `Unified`

```lpp
autopattern Facade SystemFacade;
let facade = SystemFacade();
facade.operation();
```

---

### Flyweight
**Purpose:** Share common state  
**Keywords:** `Flyweight`, `Shared`, `Pool`

```lpp
autopattern Flyweight CharFactory;
let factory = CharFactory();
factory.operation(extrinsicState);
```

---

### Proxy
**Purpose:** Surrogate/placeholder  
**Keywords:** `Proxy`, `Surrogate`, `Placeholder`

```lpp
autopattern Proxy ImageProxy;
let proxy = ImageProxy();
proxy.request();
```

---

## 🎭 Behavioral Patterns

### Chain of Responsibility
**Purpose:** Chain of request handlers  
**Keywords:** `Chain`, `Handler`, `Pipeline`

```lpp
autopattern Chain ErrorHandler;
let handler = ErrorHandler();
handler.handleRequest(request);
```

---

### Command
**Purpose:** Encapsulate requests  
**Keywords:** `Command`, `Action`, `Execute`

```lpp
autopattern Command RemoteControl;
let remote = RemoteControl();
remote.execute();
remote.undo();
```

---

### Iterator
**Purpose:** Sequential access  
**Keywords:** `Iterator`, `Traverse`, `Cursor`

```lpp
autopattern Iterator ListIterator;
let iter = ListIterator();
while (iter.hasNext()) {
    let item = iter.next();
}
```

---

### Mediator
**Purpose:** Reduce coupling  
**Keywords:** `Mediator`, `Coordinate`, `Broker`

```lpp
autopattern Mediator ChatRoom;
let room = ChatRoom();
room.notify(sender, event);
```

---

### Memento
**Purpose:** Capture/restore state  
**Keywords:** `Memento`, `Snapshot`, `State`

```lpp
autopattern Memento Editor;
let editor = Editor();
let saved = editor.save();
editor.restore(saved);
```

---

### Observer
**Purpose:** Event notification  
**Keywords:** `Observer`, `Notify`, `Subscribe`

```lpp
autopattern Observer EventBus;
let bus = EventBus();
bus.attach(listener);
bus.notify();
```

---

### State
**Purpose:** Behavior based on state  
**Keywords:** `State`, `Context`, `Status`

```lpp
autopattern State Connection;
let conn = Connection();
conn.handle();
conn.setState(newState);
```

---

### Strategy
**Purpose:** Interchangeable algorithms  
**Keywords:** `Strategy`, `Algorithm`, `Policy`

```lpp
autopattern Strategy SortStrategy;
let sorter = SortStrategy();
sorter.execute();
```

---

### Template Method
**Purpose:** Algorithm skeleton  
**Keywords:** `Template`, `Skeleton`, `Framework`

```lpp
autopattern Template DataProcessor;
let processor = DataProcessor();
processor.templateMethod();
```

---

### Visitor
**Purpose:** Add operations without modifying  
**Keywords:** `Visitor`, `Operation`, `Visit`

```lpp
autopattern Visitor ShapeVisitor;
let visitor = ShapeVisitor();
visitor.visit(element);
```

---

### Interpreter
**Purpose:** Interpret language grammar  
**Keywords:** `Interpreter`, `Grammar`, `Parse`

```lpp
autopattern Interpreter ExpressionParser;
let parser = ExpressionParser();
parser.interpret();
```

---

## 🎯 Pattern Selection Guide

| Use Case | Pattern | Code |
|----------|---------|------|
| Global configuration | Singleton | `autopattern Singleton Config;` |
| Object creation | Factory | `autopattern Factory ObjectFactory;` |
| UI component families | AbstractFactory | `autopattern AbstractFactory UIFactory;` |
| Complex object assembly | Builder | `autopattern Builder Builder;` |
| Clone objects | Prototype | `autopattern Prototype Proto;` |
| Interface conversion | Adapter | `autopattern Adapter Adapter;` |
| Decouple abstraction | Bridge | `autopattern Bridge Bridge;` |
| Tree structures | Composite | `autopattern Composite Tree;` |
| Dynamic enhancement | Decorator | `autopattern Decorator Decorator;` |
| Simplify subsystems | Facade | `autopattern Facade Facade;` |
| Share state | Flyweight | `autopattern Flyweight Pool;` |
| Lazy loading | Proxy | `autopattern Proxy Proxy;` |
| Request chain | Chain | `autopattern Chain Handler;` |
| Undo/redo | Command | `autopattern Command Command;` |
| Collection traversal | Iterator | `autopattern Iterator Iterator;` |
| Object coordination | Mediator | `autopattern Mediator Mediator;` |
| State snapshots | Memento | `autopattern Memento Memento;` |
| Event system | Observer | `autopattern Observer EventBus;` |
| State-based behavior | State | `autopattern State State;` |
| Algorithm selection | Strategy | `autopattern Strategy Strategy;` |
| Algorithm template | Template | `autopattern Template Template;` |
| External operations | Visitor | `autopattern Visitor Visitor;` |
| Language parsing | Interpreter | `autopattern Interpreter Parser;` |

---

## 🔍 Intelligent Detection

The compiler automatically detects patterns based on keywords in the problem domain:

```lpp
// Automatically recognizes as Singleton
autopattern Singleton DatabaseConnectionManager;

// Automatically recognizes as Factory  
autopattern Factory VehicleCreator;

// Automatically recognizes as Observer
autopattern Observer NewsNotificationSystem;
```

**Alternative keywords also work:**
```lpp
autopattern Config AppSettings;        // → Singleton
autopattern Create ShapeBuilder;       // → Factory
autopattern Notify EventSystem;        // → Observer
autopattern Tree FileSystem;           // → Composite
```

---

## 💡 Pro Tips

1. **Pattern names are case-sensitive**: Use exact pattern names or keywords
2. **One pattern per class**: Each autopattern generates one complete class
3. **Combine with paradigms**: Works in all 5 paradigms (HYBRID, FUNCTIONAL, OOP, IMPERATIVE, GOLFED)
4. **Extend generated classes**: Add custom methods after pattern generation
5. **Type safety**: All generated code is fully type-safe C++

---

## 📚 Complete Examples

See `examples/test_all_23_gof_patterns.lpp` for comprehensive demonstrations of all patterns.

---

## 🎓 Learn More

- [Complete Pattern Catalog](COMPLETE_PATTERN_CATALOG.md) - Detailed documentation
- [Gang of Four Book](https://en.wikipedia.org/wiki/Design_Patterns) - Original design patterns reference
- [L++ Documentation](QUICKSTART.md) - Language guide

---

**✨ Design patterns have never been easier!**
