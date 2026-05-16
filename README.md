# Real-Time Currency Converter — C++ Qt (No libcurl needed)

A clean desktop currency converter using **Qt Network** only.
No libcurl. No extra DLLs. Just Qt.

---

## Features
- Live exchange rates (170+ currencies)
- Auto-refreshes every 60 seconds
- Swap button to flip currencies
- 8 quick-pair buttons
- Conversion history (last 12)
- Copy result to clipboard
- Works on Windows, macOS, Linux

---

## Project Structure

```
currency_converter_v2/
├── src/
│   ├── main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   ├── apifetcher.h
│   └── apifetcher.cpp
├── libs/
│   └── json.hpp          ← DOWNLOAD THIS (see Step 1 below)
├── assets/
├── build/
├── README.md
└── currency_converter.pro
```

---

## STEP 1 — Download json.hpp (ONE TIME ONLY)

Open this URL in your browser and save the file to the libs\ folder:

  https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp

Save it as:  currency_converter_v2\libs\json.hpp

---

## STEP 2 — Install Qt (if not installed)

Download from: https://www.qt.io/download-open-source
During install select:
  [x] Qt 6.x latest
  [x] MinGW 64-bit
  [x] Qt Creator

---

## STEP 3 — Open in Qt Creator

1. Open Qt Creator
2. File -> Open File or Project
3. Select: currency_converter.pro
4. Click Configure Project
5. Press Ctrl+B to Build
6. Press Ctrl+R to Run

---

## NO libcurl needed!

This version uses Qt's built-in QNetworkAccessManager.
You do NOT need to install libcurl or any extra libraries.

---

## Troubleshooting

| Error                    | Fix                                      |
|--------------------------|------------------------------------------|
| json.hpp not found       | Download it (see Step 1)                 |
| Qt not found             | Add Qt bin folder to your PATH           |
| Network error at runtime | Check your internet connection           |
| Build errors on Qt 5     | Change #include path or upgrade to Qt 6  |
