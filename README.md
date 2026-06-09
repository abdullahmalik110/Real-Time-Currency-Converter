<div align="center">

# 💱 Currency Converter

### A desktop currency converter built with C++ and Qt Framework

**Created by GROUP 10**

![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-6.x%20%7C%205.15+-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![OOP](https://img.shields.io/badge/OOP-Concepts-purple?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)

</div>

---

## 📌 Overview

A fully functional desktop currency converter that fetches **live exchange rates** from the [fawazahmed0 Currency API](https://github.com/fawazahmed0/exchange-api) and supports **100+ world currencies** with full names (e.g. `PKR — Pakistani Rupee`, `OMR — Omani Rial`).

Built as an OOP Lab Project, the application demonstrates all five required OOP concepts — Inheritance, Encapsulation, Abstraction, Static Polymorphism, and Dynamic Polymorphism — alongside persistent File Handling via CSV.

---

## 🖥️ Screenshots

><img width="1918" height="1020" alt="image" src="https://github.com/user-attachments/assets/e7cae6c9-f8f5-4fb9-afd1-79cac24fd162" />


---

## ✨ Features

- 🌐 **Live exchange rates** fetched from a free public API (with automatic fallback URL)
- 💱 **100+ currencies** displayed with full names across all world regions
- ⚡ **Auto-refresh** every 60 seconds
- 🔁 **Quick conversion buttons** for popular pairs (USD→PKR, USD→OMR, GBP→PKR, etc.)
- 📂 **Persistent history** — every conversion saved to a CSV file on disk
- 🌙 **Dark theme** UI built with Qt Style Sheets
- 🔄 **Swap button** to instantly reverse the currency pair
- 🗑️ **Clear history** option

---

## 🎓 OOP Concepts Demonstrated

### 1. 🔷 Inheritance
Two clear inheritance chains exist in the project:

```
QObject
  └── BaseFetcher          (abstract base class)
        └── ApiFetcher     (concrete HTTP fetcher)

QMainWindow
  └── MainWindow           (application window)
```

`ApiFetcher` inherits from `BaseFetcher` and gets access to shared signals (`ratesFetched`, `fetchError`) and the protected `isValidRate()` helper without re-declaring them.

---

### 2. 🔒 Encapsulation
Every class keeps its data `private` and exposes only a clean public interface:

| Class | Private Data | What's Exposed |
|-------|-------------|----------------|
| `CurrencyConverter` | `m_rates` (the rate map) | `convert()`, `getRate()`, `format()` |
| `ConversionRecord` | all 6 fields | read-only getters only |
| `HistoryManager` | `m_filePath` | `save()`, `loadFromFile()`, `clearFile()` |
| `ApiFetcher` | `m_manager`, HTTP logic | only `fetchRates()` |
| `MainWindow` | all widgets | Qt signals/slots only |

---

### 3. 🫥 Abstraction
`BaseFetcher` is a **pure abstract class**. The GUI (`MainWindow`) holds a `BaseFetcher*` pointer and never knows or cares about HTTP, JSON, or network details:

```cpp
// BaseFetcher — pure virtual interface
virtual void    fetchRates()       = 0;   // HOW is completely hidden
virtual QString sourceName() const = 0;

// MainWindow — works through the abstract pointer
BaseFetcher* m_fetcher;                   // declared as abstract type
m_fetcher = new ApiFetcher(this);         // concrete type at runtime
m_fetcher->fetchRates();                  // calls ApiFetcher::fetchRates()
```

---

### 4. ⚡ Static Polymorphism (Function Overloading)
Multiple functions are overloaded across the codebase:

**`CurrencyConverter::convert()` — 3 overloads**
```cpp
double convert(double        amount, const QString& from, const QString& to, bool* ok);
double convert(int           amount, const QString& from, const QString& to, bool* ok);
double convert(const QString& amountStr, const QString& from, const QString& to, bool* ok);
```

**`CurrencyConverter::format()` — 2 overloads**
```cpp
QString format(double value, const QString& currency) const; // currency-aware decimals
QString format(double value, int decimals)             const; // explicit decimal count
```

**`HistoryManager::save()` — 2 overloads**
```cpp
void save(const ConversionRecord& record);           // append single record
void save(const QVector<ConversionRecord>& records); // rewrite entire file
```

**`ApiFetcher::buildRequest()` — 2 overloads**
```cpp
QNetworkRequest buildRequest(const QString& urlStr) const;
QNetworkRequest buildRequest(const QUrl& url)       const;
```

---

### 5. 🔀 Dynamic Polymorphism (Virtual Functions)
`MainWindow` holds a `BaseFetcher*` base pointer. The actual method dispatched at runtime depends on which concrete object is assigned — this is dynamic dispatch:

```cpp
// Declared as abstract pointer — does not know the concrete type
BaseFetcher* m_fetcher;

// Assigned at runtime to a concrete subclass
m_fetcher = new ApiFetcher(this);

// Runtime dispatch — C++ vtable decides which fetchRates() runs
m_fetcher->fetchRates();     // → ApiFetcher::fetchRates()
m_fetcher->sourceName();     // → ApiFetcher::sourceName()
```

If a `CachedFetcher` or `MockFetcher` were created tomorrow, `MainWindow` would work with it unchanged.

---

### 6. 📁 File Handling
`HistoryManager` uses raw `std::ifstream` / `std::ofstream` (no Qt wrappers) to persist conversion history as a CSV file:

```cpp
// Writing — std::ofstream with trunc to rewrite trimmed history
std::ofstream file(m_filePath.toStdString(), std::ios::trunc);
file << "timestamp,from,to,amount,result,rate\n";
for (int i = records.size() - 1; i >= 0; --i)
    file << records[i].serialize().toStdString() << '\n';

// Reading — std::ifstream line by line
std::ifstream file(m_filePath.toStdString());
while (std::getline(file, line)) { ... }
```

`ConversionRecord` handles its own serialization/deserialization to and from CSV lines.

---

## 🏗️ Project Structure

```
CurrencyConverter/
│
├── currency_converter.pro     ← qmake project file
├── README.md
│
├── libs/
│   └── json.hpp               ← nlohmann/json (header-only, bundled)
│
└── src/
    ├── main.cpp               ← Entry point — main() ONLY
    │
    ├── basefetcher.h          ← Abstract base (Abstraction + Dynamic Poly)
    ├── apifetcher.h           ← Concrete fetcher header (Inheritance)
    ├── apifetcher.cpp         ← HTTP + JSON logic (Static + Dynamic Poly)
    │
    ├── currencyconverter.h    ← Rate engine (Encapsulation + Static Poly)
    │
    ├── conversionrecord.h     ← Data model (Encapsulation + File Handling)
    ├── historymanager.h       ← CSV persistence (File Handling + Static Poly)
    │
    ├── mainwindow.h           ← GUI class declaration (Inheritance)
    └── mainwindow.cpp         ← GUI implementation (Dynamic Poly + full UI)
```

---

## 🚀 Build & Run

### Prerequisites
- **Qt 5.15+** or **Qt 6.x** with `widgets` and `network` modules
- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **qmake** (bundled with Qt)

### Option 1 — Qt Creator (Recommended)
1. Open `currency_converter.pro` in Qt Creator
2. Click **Configure Project**
3. Press **Run** (Ctrl+R)

### Option 2 — Command Line

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/CurrencyConverter.git
cd CurrencyConverter

# Build
qmake currency_converter.pro
make                  # Linux / macOS
mingw32-make          # Windows with MinGW

# Run
./build/CurrencyConverter        # Linux / macOS
build\CurrencyConverter.exe      # Windows
```

> **Note:** An active internet connection is required to fetch live exchange rates on startup.

---

## 🌍 Supported Currencies (100+)

| Region | Currencies |
|--------|-----------|
| **Major / Global** | USD, EUR, GBP, JPY, CHF, CAD, AUD, NZD |
| **South & SE Asia** | PKR, INR, BDT, LKR, NPR, THB, MYR, IDR, PHP, SGD, CNY, HKD, KRW, TWD, VND |
| **Middle East** | OMR, SAR, AED, KWD, BHD, QAR, JOD, IQD, ILS, IRR, YER |
| **Africa** | EGP, ZAR, NGN, KES, GHS, ETB, MAD, DZD, TND, XOF, XAF |
| **Europe (non-€)** | NOK, SEK, DKK, PLN, CZK, HUF, RON, TRY, RUB, UAH, BGN, HRK |
| **Americas** | BRL, MXN, ARS, CLP, COP, PEN, UYU, BOB, CRC, GTQ, DOP |
| **Pacific** | FJD, PGK, WST, TOP |

---

## 👥 Group Members & Contributions

| Member | Files | OOP Concepts |
|--------|-------|-------------|
| **Kashmalah Kabir** | `main.cpp`, `basefetcher.h`, `apifetcher.h/.cpp` | Abstraction, Inheritance, Dynamic Polymorphism, Static Polymorphism |
| **Haleema Sadia** | `currencyconverter.h` | Encapsulation, Static Polymorphism |
| **Muhammad Abdullah Zulfiqar** | `conversionrecord.h`, `historymanager.h` | Encapsulation, File Handling, Static Polymorphism |
| **Raghib ullah Khan** | `mainwindow.h`, `mainwindow.cpp` | Inheritance, Dynamic Polymorphism, GUI |

> See [`MEMBER_DIVISION.md`](./MEMBER_DIVISION.md) for the detailed breakdown of each member's contribution, the specific functions they wrote, and viva preparation notes.

---

## 🎬 Demo Videos

| Member | LinkedIn |

| [Kashmalah Kabir Chaudhary] | [🔗 Watch Demo](https://www.linkedin.com/posts/kashmala-kabir-chaudhary-32164a3a9_oop-softwaredevelopment-programming-ugcPost-7469709279118237696-OITr/?utm_source=share&utm_medium=member_android&rcm=ACoAAGPw4UkBcnxq635RP4L8MIn3nJWAAS_Dt-k) |
|Haleema Sadia] | [🔗 Watch Demo](https://www.linkedin.com/posts/haleema-sadia-ab58783a7_opentowork-studentproject-currencyconverter-ugcPost-7466252892790841344-5SW1/?utm_source=share&utm_medium=member_desktop&rcm=ACoAAGOeQykBUjffQ8LLgUliAiG4lIaFphtqfNs) |
| Muhammad Abdullah Zulfiqar | [🔗 Watch Demo](https://www.linkedin.com/posts/abdullah-malik-06072b382_softwareengineering-uet-cplusplus-ugcPost-7465861207841923072-1WAg/?utm_source=share&utm_medium=member_desktop&rcm=ACoAAF5voIgBRTF_7ywuPIqS8pNyhn1bLuZ7XdI) |
| [Raghib Ullah Khan] | [🔗 Watch Demo](https://www.linkedin.com/posts/raghib-ullah-khan-08432b370_currency-converter-20-a-clean-and-modern-ugcPost-7465907162444292096-TbFD/?utm_source=share&utm_medium=member_desktop&rcm=ACoAAFvikKQB5CyuJuX5d1_NF_XcPSGTGPOtMek) |

---

## 🛠️ Tech Stack

| Technology | Purpose |
|-----------|---------|
| **C++17** | Core language |
| **Qt 5.15 / Qt 6** | GUI framework, networking, file paths |
| **QNetworkAccessManager** | Async HTTP requests |
| **nlohmann/json** | JSON parsing (header-only, bundled) |
| **std::ifstream / ofstream** | CSV file handling |
| **fawazahmed0 API** | Free live exchange rate data |

---

## 📝 License

This project is created for educational purposes as part of an OOP Lab course.

---

<div align="center">
  <b>Created by GROUP 10</b><br>
  OOP Lab Project — C++ & Qt
</div>
