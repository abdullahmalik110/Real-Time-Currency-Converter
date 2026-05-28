// ─────────────────────────────────────────────────────────────────────────────
// mainwindow.cpp  —  Implementation of MainWindow
//
// FIXES vs original:
//   1. onConvert()  now calls addToHistory() — all conversions are saved      ✓
//   2. Dead variable `amount` removed from onConvert()                        ✓
//   3. onSwap()  blocks signals to prevent triple onConvert() fire             ✓
//   4. addToHistory() calls save(vector) after cap → file stays trimmed        ✓
//   5. UI history cap uses MAX_RECORDS constant (no magic number 15)           ✓
//   6. save(vector) overload is now actually called (was dead code)            ✓
//   7. Inf/NaN result shown as an error message in the UI                      ✓
//   8. Full currency names in combo boxes e.g. "PKR — Pakistani Rupee"        ✓
//   9. 100+ currencies including OMR added                                     ✓
//  10. Currency code stored as item userData — logic always uses clean codes   ✓
//  11. No C++17 structured bindings — compiles on all MinGW/MSVC setups        ✓
//  12. "Created by Abdullah Malik" credit in title bar and footer              ✓
// ─────────────────────────────────────────────────────────────────────────────

#include "mainwindow.h"
#include "apifetcher.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QFont>
#include <QSizePolicy>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// CURRENCY TABLE  —  { "CODE", "Full Name" }
// Stored as file-scope data so both populateCurrencies() and codeToName() can
// use it without any duplication.
// ─────────────────────────────────────────────────────────────────────────────
struct CurrencyEntry { const char* code; const char* name; };

static const CurrencyEntry CURRENCIES[] = {
    // ── Major / Global ────────────────────────────────────────────────────────
    { "USD", "United States Dollar"            },
    { "EUR", "Euro"                            },
    { "GBP", "British Pound Sterling"          },
    { "JPY", "Japanese Yen"                    },
    { "CHF", "Swiss Franc"                     },
    { "CAD", "Canadian Dollar"                 },
    { "AUD", "Australian Dollar"               },
    { "NZD", "New Zealand Dollar"              },
    // ── South & South-East Asia ───────────────────────────────────────────────
    { "CNY", "Chinese Yuan Renminbi"           },
    { "HKD", "Hong Kong Dollar"                },
    { "SGD", "Singapore Dollar"                },
    { "INR", "Indian Rupee"                    },
    { "PKR", "Pakistani Rupee"                 },
    { "BDT", "Bangladeshi Taka"                },
    { "LKR", "Sri Lankan Rupee"                },
    { "NPR", "Nepalese Rupee"                  },
    { "MMK", "Myanmar Kyat"                    },
    { "KHR", "Cambodian Riel"                  },
    { "LAK", "Laotian Kip"                     },
    { "VND", "Vietnamese Dong"                 },
    { "THB", "Thai Baht"                       },
    { "MYR", "Malaysian Ringgit"               },
    { "IDR", "Indonesian Rupiah"               },
    { "PHP", "Philippine Peso"                 },
    { "KRW", "South Korean Won"                },
    { "TWD", "Taiwanese New Dollar"            },
    { "MNT", "Mongolian Tugrik"                },
    { "KZT", "Kazakhstani Tenge"               },
    { "UZS", "Uzbekistani Som"                 },
    { "AFN", "Afghan Afghani"                  },
    // ── Middle East ───────────────────────────────────────────────────────────
    { "SAR", "Saudi Arabian Riyal"             },
    { "AED", "UAE Dirham"                      },
    { "KWD", "Kuwaiti Dinar"                   },
    { "OMR", "Omani Rial"                      },
    { "BHD", "Bahraini Dinar"                  },
    { "QAR", "Qatari Riyal"                    },
    { "JOD", "Jordanian Dinar"                 },
    { "IQD", "Iraqi Dinar"                     },
    { "ILS", "Israeli New Shekel"              },
    { "LBP", "Lebanese Pound"                  },
    { "SYP", "Syrian Pound"                    },
    { "YER", "Yemeni Rial"                     },
    { "IRR", "Iranian Rial"                    },
    // ── Africa ────────────────────────────────────────────────────────────────
    { "EGP", "Egyptian Pound"                  },
    { "ZAR", "South African Rand"              },
    { "NGN", "Nigerian Naira"                  },
    { "KES", "Kenyan Shilling"                 },
    { "GHS", "Ghanaian Cedi"                   },
    { "ETB", "Ethiopian Birr"                  },
    { "TZS", "Tanzanian Shilling"              },
    { "UGX", "Ugandan Shilling"                },
    { "MAD", "Moroccan Dirham"                 },
    { "DZD", "Algerian Dinar"                  },
    { "TND", "Tunisian Dinar"                  },
    { "LYD", "Libyan Dinar"                    },
    { "SDG", "Sudanese Pound"                  },
    { "AOA", "Angolan Kwanza"                  },
    { "ZMW", "Zambian Kwacha"                  },
    { "MZN", "Mozambican Metical"              },
    { "XOF", "West African CFA Franc"          },
    { "XAF", "Central African CFA Franc"       },
    // ── Europe (non-Euro) ─────────────────────────────────────────────────────
    { "NOK", "Norwegian Krone"                 },
    { "SEK", "Swedish Krona"                   },
    { "DKK", "Danish Krone"                    },
    { "PLN", "Polish Zloty"                    },
    { "CZK", "Czech Koruna"                    },
    { "HUF", "Hungarian Forint"                },
    { "RON", "Romanian Leu"                    },
    { "BGN", "Bulgarian Lev"                   },
    { "HRK", "Croatian Kuna"                   },
    { "RSD", "Serbian Dinar"                   },
    { "ALL", "Albanian Lek"                    },
    { "MKD", "Macedonian Denar"                },
    { "BAM", "Bosnia-Herzegovina Conv. Mark"   },
    { "ISK", "Icelandic Krona"                 },
    { "UAH", "Ukrainian Hryvnia"               },
    { "RUB", "Russian Ruble"                   },
    { "BYN", "Belarusian Ruble"                },
    { "GEL", "Georgian Lari"                   },
    { "AMD", "Armenian Dram"                   },
    { "AZN", "Azerbaijani Manat"               },
    { "MDL", "Moldovan Leu"                    },
    { "TRY", "Turkish Lira"                    },
    // ── Americas ──────────────────────────────────────────────────────────────
    { "BRL", "Brazilian Real"                  },
    { "MXN", "Mexican Peso"                    },
    { "ARS", "Argentine Peso"                  },
    { "CLP", "Chilean Peso"                    },
    { "COP", "Colombian Peso"                  },
    { "PEN", "Peruvian Sol"                    },
    { "UYU", "Uruguayan Peso"                  },
    { "BOB", "Bolivian Boliviano"              },
    { "PYG", "Paraguayan Guarani"              },
    { "VES", "Venezuelan Bolivar Soberano"     },
    { "CRC", "Costa Rican Colon"               },
    { "GTQ", "Guatemalan Quetzal"              },
    { "HNL", "Honduran Lempira"                },
    { "NIO", "Nicaraguan Cordoba"              },
    { "DOP", "Dominican Peso"                  },
    { "CUP", "Cuban Peso"                      },
    { "JMD", "Jamaican Dollar"                 },
    { "TTD", "Trinidad and Tobago Dollar"      },
    { "BBD", "Barbadian Dollar"                },
    { "BSD", "Bahamian Dollar"                 },
    // ── Pacific & Other ───────────────────────────────────────────────────────
    { "FJD", "Fijian Dollar"                   },
    { "PGK", "Papua New Guinean Kina"          },
    { "WST", "Samoan Tala"                     },
    { "TOP", "Tongan Paanga"                   },
};

static const int CURRENCY_COUNT =
    static_cast<int>(sizeof(CURRENCIES) / sizeof(CURRENCIES[0]));

// ── Helpers ───────────────────────────────────────────────────────────────────

// Returns "PKR — Pakistani Rupee" from code "PKR"
static QString displayText(const QString& code) {
    for (int i = 0; i < CURRENCY_COUNT; ++i) {
        if (code.compare(CURRENCIES[i].code, Qt::CaseInsensitive) == 0)
            return QString("%1 \xe2\x80\x94 %2").arg(CURRENCIES[i].code)
                                                .arg(CURRENCIES[i].name);
    }
    return code;
}

// Extracts "PKR" from "PKR — Pakistani Rupee" (uses item userData instead)
static QString codeOf(QComboBox* box) {
    return box->currentData().toString();
}

// ═════════════════════════════════════════════════════════════════════════════
// Constructor / Destructor
// ═════════════════════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_fetcher(nullptr)
    , m_autoTimer(nullptr)
{
    setupUI();
    setupConnections();
    applyStyles();
    loadHistoryFromFile();

    // Dynamic Polymorphism: assign concrete type to abstract base pointer
    m_fetcher = new ApiFetcher(this);
    connect(m_fetcher, &BaseFetcher::ratesFetched,
            this,      &MainWindow::onRatesFetched);
    connect(m_fetcher, &BaseFetcher::fetchError,
            this,      &MainWindow::onFetchError);

    // Auto-refresh every 60 seconds
    m_autoTimer = new QTimer(this);
    connect(m_autoTimer, &QTimer::timeout, this, &MainWindow::onRefresh);
    m_autoTimer->start(60000);

    startFetch();
}

MainWindow::~MainWindow() {
    // m_fetcher is parented to this — Qt deletes it via the object tree.
}

// ═════════════════════════════════════════════════════════════════════════════
// UI Setup
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::setupUI() {
    setWindowTitle("Currency Converter  |  Created by Abdullah Malik");
    setMinimumWidth(600);
    resize(640, 780);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    // ── Title ─────────────────────────────────────────────────────────────────
    QLabel* title = new QLabel("\xf0\x9f\x92\xb1 Currency Converter", this);
    QFont tf = title->font();
    tf.setPointSize(18);
    tf.setBold(true);
    title->setFont(tf);
    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("titleLabel");
    root->addWidget(title);

    // ── Credit ────────────────────────────────────────────────────────────────
    QLabel* credit = new QLabel("Created by Abdullah Malik", this);
    credit->setAlignment(Qt::AlignCenter);
    credit->setObjectName("creditLabel");
    root->addWidget(credit);

    // ── Source ────────────────────────────────────────────────────────────────
    QLabel* src = new QLabel(
        "Rates: fawazahmed0 CDN API  |  Auto-refresh every 60 s", this);
    src->setAlignment(Qt::AlignCenter);
    src->setObjectName("sourceLabel");
    root->addWidget(src);

    // ── Converter group ───────────────────────────────────────────────────────
    QGroupBox* converterGroup = new QGroupBox("Converter", this);
    QGridLayout* grid = new QGridLayout(converterGroup);
    grid->setSpacing(10);

    // Amount
    grid->addWidget(new QLabel("Amount:", this), 0, 0);
    m_amountInput = new QLineEdit("1", this);
    m_amountInput->setObjectName("amountInput");
    m_amountInput->setPlaceholderText("Enter amount...");
    m_amountInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grid->addWidget(m_amountInput, 0, 1, 1, 2);

    // From
    grid->addWidget(new QLabel("From:", this), 1, 0);
    m_fromCombo = new QComboBox(this);
    m_fromCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_fromCombo->setMaxVisibleItems(20);
    grid->addWidget(m_fromCombo, 1, 1, 1, 2);

    // Swap
    m_swapBtn = new QPushButton("\xe2\x87\x85 Swap", this);
    m_swapBtn->setObjectName("swapBtn");
    grid->addWidget(m_swapBtn, 2, 1);

    // To
    grid->addWidget(new QLabel("To:", this), 3, 0);
    m_toCombo = new QComboBox(this);
    m_toCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_toCombo->setMaxVisibleItems(20);
    grid->addWidget(m_toCombo, 3, 1, 1, 2);

    // Result
    m_resultLabel = new QLabel("\xe2\x80\x94", this);
    m_resultLabel->setObjectName("resultLabel");
    m_resultLabel->setAlignment(Qt::AlignCenter);
    QFont rf = m_resultLabel->font();
    rf.setPointSize(22);
    rf.setBold(true);
    m_resultLabel->setFont(rf);
    grid->addWidget(m_resultLabel, 4, 0, 1, 3);

    // Rate label
    m_rateLabel = new QLabel("", this);
    m_rateLabel->setAlignment(Qt::AlignCenter);
    m_rateLabel->setObjectName("rateLabel");
    grid->addWidget(m_rateLabel, 5, 0, 1, 3);

    // Refresh button
    m_refreshBtn = new QPushButton("\xf0\x9f\x94\x84 Refresh Rates", this);
    grid->addWidget(m_refreshBtn, 6, 0, 1, 3);

    root->addWidget(converterGroup);

    // ── Quick-pair group ──────────────────────────────────────────────────────
    QGroupBox* quickGroup = new QGroupBox("Quick Conversions (1 unit)", this);
    QGridLayout* qg = new QGridLayout(quickGroup);

    // Use a simple struct — avoids C++17 structured bindings (MinGW workaround)
    struct QP { const char* label; const char* from; const char* to; };
    const QP pairs[] = {
        { "USD -> PKR", "USD", "PKR" },
        { "USD -> EUR", "USD", "EUR" },
        { "USD -> GBP", "USD", "GBP" },
        { "USD -> OMR", "USD", "OMR" },
        { "USD -> AED", "USD", "AED" },
        { "USD -> SAR", "USD", "SAR" },
        { "EUR -> PKR", "EUR", "PKR" },
        { "GBP -> PKR", "GBP", "PKR" },
        { "USD -> INR", "USD", "INR" },
    };
    const int pairCount = static_cast<int>(sizeof(pairs) / sizeof(pairs[0]));

    int col = 0, row = 0;
    for (int i = 0; i < pairCount; ++i) {
        QPushButton* btn = new QPushButton(pairs[i].label, this);
        btn->setObjectName("quickBtn");
        QString f = pairs[i].from;
        QString t = pairs[i].to;
        connect(btn, &QPushButton::clicked, this, [this, f, t]() {
            onQuickPair(f, t, 1.0);
        });
        qg->addWidget(btn, row, col);
        if (++col == 3) { col = 0; ++row; }
    }
    root->addWidget(quickGroup);

    // ── History group ─────────────────────────────────────────────────────────
    m_historyGroup = new QGroupBox("Recent Conversions (saved to disk)", this);
    QVBoxLayout* hv = new QVBoxLayout(m_historyGroup);

    m_historyList = new QListWidget(this);
    m_historyList->setMaximumHeight(180);
    hv->addWidget(m_historyList);

    m_clearHistoryBtn = new QPushButton("\xf0\x9f\x97\x91  Clear History", this);
    m_clearHistoryBtn->setObjectName("clearBtn");
    hv->addWidget(m_clearHistoryBtn);

    root->addWidget(m_historyGroup);

    // ── Footer credit ─────────────────────────────────────────────────────────
    QLabel* footer = new QLabel(
        "\xc2\xa9 Created by Group 10  |  OOP Lab Project", this);
    footer->setAlignment(Qt::AlignCenter);
    footer->setObjectName("footerLabel");
    root->addWidget(footer);

    // ── Status bar ────────────────────────────────────────────────────────────
    statusBar()->showMessage("Fetching exchange rates...");
}

// ═════════════════════════════════════════════════════════════════════════════
// Connections
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::setupConnections() {
    connect(m_amountInput, &QLineEdit::textChanged,
            this, &MainWindow::onConvert);
    connect(m_fromCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { onConvert(); });
    connect(m_toCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { onConvert(); });
    connect(m_swapBtn,         &QPushButton::clicked, this, &MainWindow::onSwap);
    connect(m_refreshBtn,      &QPushButton::clicked, this, &MainWindow::onRefresh);
    connect(m_clearHistoryBtn, &QPushButton::clicked, this, &MainWindow::onClearHistory);
}

// ═════════════════════════════════════════════════════════════════════════════
// populateCurrencies — fills both combos with "CODE — Full Name"
// The currency CODE is stored as item userData (Qt::UserRole) so that
// onConvert / onSwap always retrieve a clean 3-letter code via codeOf().
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::populateCurrencies() {
    // Remember the currently selected codes before clearing
    const QString prevFrom = codeOf(m_fromCombo);
    const QString prevTo   = codeOf(m_toCombo);

    m_fromCombo->blockSignals(true);
    m_toCombo->blockSignals(true);

    m_fromCombo->clear();
    m_toCombo->clear();

    for (int i = 0; i < CURRENCY_COUNT; ++i) {
        const QString code = CURRENCIES[i].code;
        const QString text = displayText(code);   // "USD — United States Dollar"
        m_fromCombo->addItem(text, code);          // userData = "USD"
        m_toCombo->addItem(text,   code);
    }

    // Restore previous selection; default to USD (from) / PKR (to) on first run
    const QString wantFrom = prevFrom.isEmpty() ? "USD" : prevFrom;
    const QString wantTo   = prevTo.isEmpty()   ? "PKR" : prevTo;

    int fi = m_fromCombo->findData(wantFrom);
    int ti = m_toCombo->findData(wantTo);
    m_fromCombo->setCurrentIndex(fi >= 0 ? fi : 0);
    m_toCombo->setCurrentIndex(  ti >= 0 ? ti : 1);

    m_fromCombo->blockSignals(false);
    m_toCombo->blockSignals(false);
}

// ═════════════════════════════════════════════════════════════════════════════
// Fetch / Rate handling
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::startFetch() {
    if (!m_fetcher) return;
    statusBar()->showMessage(
        "Fetching rates from " + m_fetcher->sourceName() + "...");
    m_refreshBtn->setEnabled(false);
    // Dynamic Polymorphism: runtime dispatch → ApiFetcher::fetchRates()
    m_fetcher->fetchRates();
}

void MainWindow::onRefresh() { startFetch(); }

void MainWindow::onRatesFetched(std::map<std::string, double> rates) {
    m_converter.setRates(rates);
    populateCurrencies();
    onConvert();
    m_refreshBtn->setEnabled(true);
    statusBar()->showMessage(
        QString("Rates updated  —  %1 currencies loaded.").arg(rates.size()),
        5000);
}

void MainWindow::onFetchError(QString message) {
    m_refreshBtn->setEnabled(true);
    statusBar()->showMessage("Error: " + message, 8000);
}

// ═════════════════════════════════════════════════════════════════════════════
// Conversion
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::onConvert() {
    if (!m_converter.hasRates()) {
        m_resultLabel->setText("\xe2\x80\x94");
        m_rateLabel->setText("Waiting for rates...");
        return;
    }

    // codeOf() reads item userData → always a clean 3-letter code like "PKR"
    const QString from = codeOf(m_fromCombo);
    const QString to   = codeOf(m_toCombo);

    // Static Polymorphism — Overload 3: convert from a QString
    bool ok = false;
    double result = m_converter.convert(m_amountInput->text(), from, to, &ok);

    if (!ok) {
        m_resultLabel->setText("\xe2\x80\x94");
        m_rateLabel->setText("Enter a valid positive amount.");
        return;
    }

    if (std::isinf(result) || std::isnan(result)) {
        m_resultLabel->setText("Overflow");
        m_rateLabel->setText("Amount too large.");
        return;
    }

    double rate = m_converter.getRate(from, to);

    // Show result: amount + full name, e.g. "285.50  PKR — Pakistani Rupee"
    m_resultLabel->setText(
        m_converter.format(result, to) + "  " + displayText(to));

    // Rate line: "1 USD — United States Dollar  =  285.123456  PKR"
    m_rateLabel->setText(
        QString("1 %1  =  %2  %3")
            .arg(displayText(from))
            .arg(rate, 0, 'f', 6)
            .arg(to));

    // FIX 1: save every conversion to history
    double inputAmount = m_amountInput->text().trimmed().toDouble();
    if (inputAmount > 0.0) {
        addToHistory(ConversionRecord(from, to, inputAmount, result, rate));
    }
}

// FIX 3: blockSignals stops triple-fire on swap
void MainWindow::onSwap() {
    const QString from = codeOf(m_fromCombo);
    const QString to   = codeOf(m_toCombo);

    m_fromCombo->blockSignals(true);
    m_toCombo->blockSignals(true);

    // Use findData on the code (userData) not findText
    int fi = m_fromCombo->findData(to);
    int ti = m_toCombo->findData(from);
    if (fi >= 0) m_fromCombo->setCurrentIndex(fi);
    if (ti >= 0) m_toCombo->setCurrentIndex(ti);

    m_fromCombo->blockSignals(false);
    m_toCombo->blockSignals(false);

    onConvert();   // fire exactly once
}

// ═════════════════════════════════════════════════════════════════════════════
// Quick pair
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::onQuickPair(const QString& from, const QString& to, double amount) {
    m_fromCombo->blockSignals(true);
    m_toCombo->blockSignals(true);
    m_amountInput->blockSignals(true);

    int fi = m_fromCombo->findData(from);
    int ti = m_toCombo->findData(to);
    if (fi >= 0) m_fromCombo->setCurrentIndex(fi);
    if (ti >= 0) m_toCombo->setCurrentIndex(ti);
    m_amountInput->setText(QString::number(amount));

    m_fromCombo->blockSignals(false);
    m_toCombo->blockSignals(false);
    m_amountInput->blockSignals(false);

    // Static Polymorphism — Overload 2: integer amount
    bool ok = false;
    double result = m_converter.convert(static_cast<int>(amount), from, to, &ok);
    if (!ok) return;

    double rate = m_converter.getRate(from, to);

    m_resultLabel->setText(
        m_converter.format(result, to) + "  " + displayText(to));
    m_rateLabel->setText(
        QString("1 %1  =  %2  %3")
            .arg(displayText(from))
            .arg(rate, 0, 'f', 6)
            .arg(to));

    addToHistory(ConversionRecord(from, to, amount, result, rate));
}

// ═════════════════════════════════════════════════════════════════════════════
// History
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::loadHistoryFromFile() {
    m_records = m_history.loadFromFile();
    for (int i = 0; i < m_records.size(); ++i) {
        m_historyList->addItem(m_records[i].toDisplayString());
    }
    if (!m_records.isEmpty())
        m_historyList->scrollToTop();
}

// FIX 4, 5, 6: caps both memory and file; calls save(vector) so file is trimmed
void MainWindow::addToHistory(const ConversionRecord& record) {
    m_records.prepend(record);
    m_historyList->insertItem(0, record.toDisplayString());

    while (m_records.size() > HistoryManager::MAX_RECORDS)
        m_records.removeLast();

    while (m_historyList->count() > HistoryManager::MAX_RECORDS)
        delete m_historyList->takeItem(m_historyList->count() - 1);

    // FIX 6: save(vector) — was dead code in original, now actually called
    m_history.save(m_records);
}

void MainWindow::onClearHistory() {
    m_records.clear();
    m_historyList->clear();
    m_history.clearFile();
    statusBar()->showMessage("History cleared.", 3000);
}

// ═════════════════════════════════════════════════════════════════════════════
// Styles
// ═════════════════════════════════════════════════════════════════════════════

void MainWindow::applyStyles() {
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            font-family: 'Segoe UI', sans-serif;
            font-size: 13px;
        }
        QGroupBox {
            border: 1px solid #45475a;
            border-radius: 8px;
            margin-top: 10px;
            padding: 10px;
            font-weight: bold;
            color: #89b4fa;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }
        QComboBox, QLineEdit {
            background-color: #313244;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 6px 10px;
            color: #cdd6f4;
            min-height: 28px;
        }
        QComboBox:focus, QLineEdit:focus { border-color: #89b4fa; }
        QComboBox::drop-down { border: none; padding-right: 8px; }
        QComboBox QAbstractItemView {
            background-color: #313244;
            color: #cdd6f4;
            selection-background-color: #45475a;
            border: 1px solid #45475a;
        }
        QLineEdit#amountInput { font-size: 15px; font-weight: bold; }
        QLabel#resultLabel {
            color: #a6e3a1;
            padding: 8px;
            background-color: #181825;
            border-radius: 6px;
            font-size: 18px;
        }
        QLabel#rateLabel   { color: #89dceb; font-size: 11px; }
        QLabel#titleLabel  { color: #cba6f7; }
        QLabel#creditLabel {
            color: #f9e2af;
            font-size: 13px;
            font-weight: bold;
            letter-spacing: 1px;
        }
        QLabel#sourceLabel { color: #6c7086; font-size: 11px; }
        QLabel#footerLabel {
            color: #f9e2af;
            font-size: 11px;
            padding-top: 4px;
            border-top: 1px solid #313244;
        }
        QPushButton {
            background-color: #89b4fa;
            color: #1e1e2e;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover   { background-color: #b4befe; }
        QPushButton:pressed { background-color: #74c7ec; }
        QPushButton#swapBtn { background-color: #f38ba8; min-width: 90px; }
        QPushButton#quickBtn {
            background-color: #313244;
            color: #cdd6f4;
            font-weight: normal;
            font-size: 12px;
        }
        QPushButton#quickBtn:hover { background-color: #45475a; }
        QPushButton#clearBtn {
            background-color: #f38ba8;
            font-size: 12px;
            padding: 5px;
        }
        QListWidget {
            background-color: #181825;
            border: 1px solid #45475a;
            border-radius: 6px;
            color: #a6e3a1;
            font-size: 11px;
            font-family: monospace;
        }
        QListWidget::item:selected { background-color: #313244; }
        QStatusBar {
            background-color: #181825;
            color: #6c7086;
            font-size: 11px;
        }
    )");
}
