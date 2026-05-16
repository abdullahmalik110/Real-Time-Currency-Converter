# 💱 Real-Time Currency Converter

A fully functional desktop **Currency Converter** built in **C++** with a **Qt GUI** — developed as an Object-Oriented Programming (OOP) semester project at **UET**.

Fetches **live exchange rates** from a free public API with no API key required.

---

## 📸 Preview

<img width="675" height="1031" alt="image" src="https://github.com/user-attachments/assets/7ed9978c-1b1e-48ac-8226-4aab32dd7de2" />



---

## ✨ Features

- 🌐 **Live exchange rates** — fetched from free public API
- 💱 **170+ currencies** — USD, PKR, EUR, GBP, AED, SAR, JPY and more
- 🔄 **Auto-refreshes** every 60 seconds
- ⇅ **Swap button** — instantly reverse currencies
- ⚡ **8 Quick-pair buttons** — one click for common conversions
- 📋 **Conversion history** — tracks your last 12 conversions
- 📎 **Copy to clipboard** — copy result with one click
- 🎨 **Clean modern UI** — built with Qt Fusion style
- ✅ **No libcurl needed** — uses Qt's built-in network module

---

## 🛠️ Tech Stack

| Technology | Purpose |
|---|---|
| C++17 | Core programming language |
| Qt 6 (Widgets + Network) | GUI framework & HTTP requests |
| nlohmann/json | JSON parsing (single header) |
| fawazahmed0/currency-api | Free live exchange rate API |

---

## 📐 OOP Concepts Applied

| Concept | Where Used |
|---|---|
| **Classes & Objects** | `MainWindow`, `ApiFetcher` |
| **Encapsulation** | API logic fully isolated in `ApiFetcher` |
| **Abstraction** | `MainWindow` doesn't know HOW rates are fetched |
| **Inheritance** | Inherits from `QMainWindow`, `QObject` |
| **Signals & Slots** | Qt's event-driven OOP communication pattern |
| **Single Responsibility** | Each class has one clear job |

---

## 📁 Project Structure

```
currency_converter/
├── src/
│   ├── main.cpp           ← App entry point
│   ├── mainwindow.h       ← Main window class declaration
│   ├── mainwindow.cpp     ← UI layout, styling, and logic
│   ├── apifetcher.h       ← API fetcher class declaration
│   └── apifetcher.cpp     ← HTTP requests + JSON parsing
├── libs/
│   └── json.hpp           ← nlohmann/json (download separately)
├── assets/                ← App icons / images
├── build/                 ← Compiled output (auto-generated)
├── README.md
└── currency_converter.pro ← Qt project file
```

---

## ⚙️ Installation & Build

### Requirements

- [Qt 6](https://www.qt.io/download-open-source) (with MinGW 64-bit on Windows)
- [nlohmann/json](https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp) single header

### Step 1 — Clone the repo

```bash
git clone https://github.com/YOUR_USERNAME/currency-converter.git
cd currency-converter
```

### Step 2 — Download json.hpp

Download and place in the `libs/` folder:

```bash
# Linux / macOS
curl -L -o libs/json.hpp \
  https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
```

**Windows** — open this URL in your browser and save to `libs\json.hpp`:
```
https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
```

### Step 3 — Build

**Qt Creator (All platforms):**
1. Open `currency_converter.pro` in Qt Creator
2. Click **Configure Project**
3. Press `Ctrl + B` to build
4. Press `Ctrl + R` to run

**Linux / macOS Terminal:**
```bash
cd build
qmake ../currency_converter.pro
make -j4
./CurrencyConverter
```

**Windows (Qt MinGW Terminal):**
```cmd
cd build
qmake ..\currency_converter.pro
mingw32-make
CurrencyConverter.exe
```

---

## 🌐 API Used

**[fawazahmed0/currency-api](https://github.com/fawazahmed0/exchange-api)**

- ✅ Completely free
- ✅ No API key required
- ✅ 170+ currencies
- ✅ Updates daily

**Endpoint used:**
```
https://cdn.jsdelivr.net/npm/@fawazahmed0/currency-api@latest/v1/currencies/usd.json
```

**Rate conversion formula:**
```
result = amount × (toRate / fromRate)
```
All rates are relative to USD as the base currency.

---

## 🐛 Troubleshooting

| Problem | Solution |
|---|---|
| `json.hpp not found` | Download it and place in `libs/` folder |
| `QNetworkAccessManager` error | Make sure `network` is in your `.pro` QT modules |
| App shows "Network error" | Check your internet connection |
| Blank result on conversion | Wait for rates to load (status bar shows progress) |
| Build fails on Qt 5 | Upgrade to Qt 6 or adjust `#include` paths |

---

## 📚 What I Learned

- Applying **OOP design principles** in a real C++ project
- Using **Qt Signals & Slots** for event-driven programming
- Making **HTTP requests** and parsing **JSON** in C++
- Separating **business logic** from **UI logic**
- Building and deploying a **cross-platform desktop application**

---

## 👨‍💻 Author

**Abdullah Malik**
Student @ University of Engineering & Technology (UET)
Object-Oriented Programming — Semester Project

---

## 📄 License

This project is open source under the [MIT License](LICENSE).

---

## ⭐ Show Your Support

If you found this project helpful, please give it a **star** ⭐ on GitHub!
