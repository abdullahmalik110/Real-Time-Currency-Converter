# 💱 Currency Converter — OOP Lab Project

> A Qt-based desktop currency converter demonstrating all required OOP concepts.

---

## 👥 Group Members & Contributions

| Member | Contribution |
|---|---|
| **Member 1 — [Name]** | `BaseFetcher`, `ApiFetcher` — Abstraction, Inheritance, Dynamic Polymorphism |
| **Member 2 — [Name]** | `CurrencyConverter` — Static Polymorphism, overloaded convert()/format() |
| **Member 3 — [Name]** | `HistoryManager`, `ConversionRecord` — Encapsulation, File Handling |
| **Member 4 — [Name]** | `MainWindow`, `main.cpp` — GUI, wiring all components, QSS styling |

> Each member is a contributor on this repo. See the commit history for verification.

---

## 🚀 Build & Run

**Requirements:** Qt 6 (or Qt 5.15+) with `network` and `widgets` modules, C++17 compiler.

```bash
# Clone the repo
git clone https://github.com/YOUR_USERNAME/CurrencyConverter.git
cd CurrencyConverter

# Build with qmake
qmake currency_converter.pro
make          # Linux / macOS
mingw32-make  # Windows (MinGW)

# Run
./build/CurrencyConverter        # Linux/macOS
build\CurrencyConverter.exe      # Windows
```

Or simply open `currency_converter.pro` in **Qt Creator** and press **Run**.

---

## 🎯 OOP Concepts Demonstrated

### 1. Inheritance
`ApiFetcher` inherits from `BaseFetcher` which inherits from `QObject`.  
`MainWindow` inherits from `QMainWindow`.

```
QObject
  └── BaseFetcher          (abstract)
        └── ApiFetcher     (concrete — HTTP + JSON parsing)

QMainWindow
  └── MainWindow
```

**Files:** `basefetcher.h`, `apifetcher.h`, `mainwindow.h`

---

### 2. Encapsulation
Every class keeps its data `private` and exposes only the minimum public interface needed.

| Class | Private Data | Public Interface |
|---|---|---|
| `CurrencyConverter` | `m_rates` | `convert()`, `getRate()`, `format()` |
| `ConversionRecord` | `m_from`, `m_to`, `m_amount`, etc. | getters only |
| `HistoryManager` | `m_filePath` | `save()`, `loadFromFile()`, `clearFile()` |
| `MainWindow` | all widgets | Qt slot signals only |

**Files:** all `.h` files

---

### 3. Abstraction
`BaseFetcher` is a **pure abstract class** — callers never know whether they are talking to a live API, a cache, or a mock.

```cpp
// BaseFetcher — two pure virtual methods
virtual void    fetchRates()       = 0;
virtual QString sourceName() const = 0;
```

`MainWindow` holds a `BaseFetcher*` pointer and never includes `apifetcher.h` — it is entirely shielded from HTTP details.

**Files:** `basefetcher.h`

---

### 4. Static Polymorphism (Function Overloading)

**`CurrencyConverter::convert()` — 3 overloads**
```cpp
double convert(double  amount, const QString& from, const QString& to, bool* ok);
double convert(int     amount, const QString& from, const QString& to, bool* ok);
double convert(const QString& amountStr, const QString& from, const QString& to, bool* ok);
```

**`CurrencyConverter::format()` — 2 overloads**
```cpp
QString format(double value, const QString& currency) const;  // currency-aware decimals
QString format(double value, int decimals)             const;  // explicit decimals
```

**`HistoryManager::save()` — 2 overloads**
```cpp
void save(const ConversionRecord& record);              // single record
void save(const QVector<ConversionRecord>& records);    // full vector (rewrites file)
```

**`ApiFetcher::buildRequest()` — 2 overloads**
```cpp
QNetworkRequest buildRequest(const QString& urlStr) const;
QNetworkRequest buildRequest(const QUrl& url)       const;
```

**Files:** `currencyconverter.h`, `historymanager.h`, `apifetcher.h`

---

### 5. Dynamic Polymorphism (Virtual Functions + Runtime Dispatch)

`MainWindow` stores a `BaseFetcher*` and calls through it — the actual method dispatched at runtime is `ApiFetcher::fetchRates()`.

```cpp
// In mainwindow.h — abstract pointer
BaseFetcher* m_fetcher;

// In mainwindow.cpp — assigned at runtime
m_fetcher = new ApiFetcher(this);  // could be swapped for any other fetcher

// Runtime dispatch — which fetchRates() runs is decided at runtime
m_fetcher->fetchRates();           // → ApiFetcher::fetchRates()
m_fetcher->sourceName();           // → ApiFetcher::sourceName()
```

**Files:** `basefetcher.h`, `apifetcher.h`, `mainwindow.cpp`

---

### 6. File Handling

`HistoryManager` uses `std::ifstream` / `std::ofstream` directly (no Qt wrappers).

- **Write:** `save(vector)` rewrites the full CSV (keeps file trimmed to `MAX_RECORDS`).
- **Write:** `save(single)` appends one line when only a new record is needed.
- **Read:** `loadFromFile()` reads CSV on startup, skips the header, deserializes each line.
- **Clear:** `clearFile()` truncates and rewrites the header row.

`ConversionRecord` handles its own **serialization/deserialization**:
```cpp
QString serialize() const;                              // → CSV line
static ConversionRecord deserialize(const QString&);    // ← CSV line
```

CSV file is stored at the OS app-data path (via `QStandardPaths`) so it works correctly regardless of working directory.

**Files:** `historymanager.h`, `conversionrecord.h`

---

## 🐛 Bugs Fixed vs Original Submission

| # | Bug | Fix |
|---|---|---|
| 1 | `tellp()==0` header check failed on append streams | Replaced with `QFile::exists()` check |
| 2 | Dead variable `amount` in `onConvert()` | Removed |
| 3 | Manual conversions not saved to history | `onConvert()` now calls `addToHistory()` |
| 4 | `onSwap()` fired `onConvert()` 3× | Added `blockSignals()` around combo updates |
| 5 | CSV file grew unbounded on disk | `addToHistory()` calls `save(vector)` to rewrite trimmed file |
| 6 | UI cap (15) ≠ `MAX_RECORDS` (50) | Both now use `HistoryManager::MAX_RECORDS` |
| 7 | `save(vector)` overload was dead code | Now called from `addToHistory()` |
| 8 | Hardcoded relative `history.csv` path | Uses `QStandardPaths::AppDataLocation` |
| 9 | No overflow check on conversion result | `std::isinf` / `std::isnan` guard added |

---

## 📁 Project Structure

```
CurrencyConverter/
├── currency_converter.pro   ← qmake project file
├── README.md
├── libs/
│   └── json.hpp             ← nlohmann/json (header-only)
└── src/
    ├── main.cpp             ← main() ONLY
    ├── basefetcher.h        ← Abstract base (Abstraction + Dynamic Poly)
    ├── apifetcher.h/.cpp    ← HTTP fetcher (Inheritance + Static Poly)
    ├── currencyconverter.h  ← Rate logic (Encapsulation + Static Poly)
    ├── conversionrecord.h   ← Data model (Encapsulation + File Handling)
    ├── historymanager.h     ← CSV persistence (File Handling + Static Poly)
    └── mainwindow.h/.cpp    ← Qt GUI (Inheritance + Dynamic Poly)
```

---

## 📽️ Demo Video

Each group member has uploaded a demo video to their LinkedIn profile:

- Member 1: [LinkedIn URL]
- Member 2: [LinkedIn URL]
- Member 3: [LinkedIn URL]
- Member 4: [LinkedIn URL]
