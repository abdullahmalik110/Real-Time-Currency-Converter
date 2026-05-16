#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QFont>
#include <QClipboard>
#include <QDateTime>
#include <QSizePolicy>
#include <cmath>

// ── Static data ───────────────────────────────────────────────────────────────
const QStringList MainWindow::CURRENCIES = {
    "USD", "EUR", "GBP", "PKR", "JPY", "AED", "SAR",
    "CAD", "AUD", "CHF", "CNY", "INR", "TRY", "BDT",
    "MYR", "SGD", "KWD", "QAR", "BHD", "OMR", "NGN",
    "EGP", "ZAR", "BRL", "MXN", "IDR", "KRW", "THB",
    "DKK", "NOK", "SEK", "NZD", "HKD", "TWD", "PHP",
    "PLN", "CZK", "HUF", "RON", "ILS"
};

const QStringList MainWindow::QUICK_PAIRS = {
    "USD→PKR", "USD→EUR", "USD→GBP", "USD→JPY",
    "USD→AED", "EUR→GBP", "GBP→PKR", "USD→SAR"
};

// ── Constructor ───────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Real-Time Currency Converter");
    setMinimumWidth(480);
    setMaximumWidth(540);
    setMinimumHeight(640);

    setupUI();
    applyStyleSheet();
    populateCurrencies();

    // ApiFetcher lives on the main thread — uses Qt signals/slots internally
    m_fetcher = new ApiFetcher(this);
    connect(m_fetcher, &ApiFetcher::ratesFetched,
            this,      &MainWindow::onRatesFetched);
    connect(m_fetcher, &ApiFetcher::fetchError,
            this,      &MainWindow::onFetchError);

    // Auto-refresh every 60 seconds
    m_autoTimer = new QTimer(this);
    connect(m_autoTimer, &QTimer::timeout, this, &MainWindow::onRefresh);
    m_autoTimer->start(60000);

    startFetch();
}

MainWindow::~MainWindow() {}

// ── UI Setup ──────────────────────────────────────────────────────────────────
void MainWindow::setupUI() {
    QWidget*     central = new QWidget(this);
    QVBoxLayout* main    = new QVBoxLayout(central);
    main->setSpacing(10);
    main->setContentsMargins(18, 16, 18, 16);

    // ── Title ─────────────────────────────────────────────────────
    QLabel* title = new QLabel("Real-Time Currency Converter");
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    main->addWidget(title);

    m_statusLabel = new QLabel("Connecting to live rates...");
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(m_statusLabel);

    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("separator");
    main->addWidget(sep);

    // ── FROM ──────────────────────────────────────────────────────
    QGroupBox*   fromBox    = new QGroupBox("Amount to Convert");
    QHBoxLayout* fromLayout = new QHBoxLayout(fromBox);
    fromLayout->setSpacing(8);

    m_amountInput = new QLineEdit("1");
    m_amountInput->setObjectName("amountInput");
    m_amountInput->setPlaceholderText("Enter amount...");
    m_amountInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_amountInput, &QLineEdit::textChanged, this, &MainWindow::onConvert);

    m_fromCombo = new QComboBox();
    m_fromCombo->setObjectName("currencyCombo");
    m_fromCombo->setMinimumWidth(95);
    connect(m_fromCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onConvert);

    fromLayout->addWidget(m_amountInput);
    fromLayout->addWidget(m_fromCombo);
    main->addWidget(fromBox);

    // ── SWAP ──────────────────────────────────────────────────────
    QPushButton* swapBtn = new QPushButton("Swap Currencies");
    swapBtn->setObjectName("swapButton");
    connect(swapBtn, &QPushButton::clicked, this, &MainWindow::onSwap);
    main->addWidget(swapBtn);

    // ── TO ────────────────────────────────────────────────────────
    QGroupBox*   toBox    = new QGroupBox("Converted Amount");
    QVBoxLayout* toOuter  = new QVBoxLayout(toBox);

    QHBoxLayout* toLayout = new QHBoxLayout();
    toLayout->setSpacing(8);

    QVBoxLayout* resultCol = new QVBoxLayout();
    resultCol->setSpacing(2);
    m_resultLabel = new QLabel("---");
    m_resultLabel->setObjectName("resultLabel");
    m_resultCurrencyLabel = new QLabel("");
    m_resultCurrencyLabel->setObjectName("resultCurrencyLabel");
    resultCol->addWidget(m_resultLabel);
    resultCol->addWidget(m_resultCurrencyLabel);

    m_toCombo = new QComboBox();
    m_toCombo->setObjectName("currencyCombo");
    m_toCombo->setMinimumWidth(95);
    connect(m_toCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onConvert);

    toLayout->addLayout(resultCol, 1);
    toLayout->addWidget(m_toCombo);
    toOuter->addLayout(toLayout);

    m_copyBtn = new QPushButton("Copy Result");
    m_copyBtn->setObjectName("copyButton");
    m_copyBtn->setEnabled(false);
    connect(m_copyBtn, &QPushButton::clicked, this, &MainWindow::onCopyResult);
    toOuter->addWidget(m_copyBtn);

    main->addWidget(toBox);

    // ── Rate info bar ─────────────────────────────────────────────
    m_rateLabel = new QLabel("Rate info will appear after loading...");
    m_rateLabel->setObjectName("rateLabel");
    m_rateLabel->setAlignment(Qt::AlignCenter);
    m_rateLabel->setWordWrap(true);
    main->addWidget(m_rateLabel);

    m_lastUpdatedLabel = new QLabel("");
    m_lastUpdatedLabel->setObjectName("lastUpdated");
    m_lastUpdatedLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(m_lastUpdatedLabel);

    // ── Action buttons ────────────────────────────────────────────
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    m_refreshBtn = new QPushButton("Refresh Rates");
    m_refreshBtn->setObjectName("actionButton");
    m_refreshBtn->setEnabled(false);
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefresh);
    btnRow->addWidget(m_refreshBtn);

    QPushButton* clearBtn = new QPushButton("Clear History");
    clearBtn->setObjectName("actionButton");
    connect(clearBtn, &QPushButton::clicked, [this](){ m_historyList->clear(); });
    btnRow->addWidget(clearBtn);
    main->addLayout(btnRow);

    // ── Quick pairs ───────────────────────────────────────────────
    QGroupBox*   qpBox    = new QGroupBox("Quick Pairs  (click to select)");
    QGridLayout* qpLayout = new QGridLayout(qpBox);
    qpLayout->setSpacing(6);

    for (int i = 0; i < QUICK_PAIRS.size(); ++i) {
        QPushButton* btn = new QPushButton(QUICK_PAIRS[i]);
        btn->setObjectName("quickPairBtn");
        QString pair = QUICK_PAIRS[i];
        connect(btn, &QPushButton::clicked,
                [this, pair](){ onQuickPair(pair); });
        qpLayout->addWidget(btn, i / 4, i % 4);
    }
    main->addWidget(qpBox);

    // ── History ───────────────────────────────────────────────────
    QGroupBox*   histBox    = new QGroupBox("Recent Conversions");
    QVBoxLayout* histLayout = new QVBoxLayout(histBox);
    m_historyList = new QListWidget();
    m_historyList->setObjectName("historyList");
    m_historyList->setMaximumHeight(110);
    m_historyList->setAlternatingRowColors(true);
    histLayout->addWidget(m_historyList);
    main->addWidget(histBox);

    // ── Footer ────────────────────────────────────────────────────
    QLabel* footer = new QLabel("Made by Abdullah Malik👨‍🎓  |  Auto-refreshes every 60s");
    footer->setObjectName("footerLabel");
    footer->setAlignment(Qt::AlignCenter);
    main->addWidget(footer);

    setCentralWidget(central);
}

// ── Stylesheet ────────────────────────────────────────────────────────────────
void MainWindow::applyStyleSheet() {
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #f0f4f8;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
            color: #2c3e50;
        }
        QLabel#titleLabel {
            font-size: 18px;
            font-weight: bold;
            color: #1a3a5c;
            padding: 6px 0;
        }
        QLabel#statusLabel {
            font-size: 11px;
            color: #7f8c8d;
        }
        QFrame#separator {
            color: #d0dae8;
            margin: 2px 0;
        }
        QGroupBox {
            background-color: #ffffff;
            border: 1px solid #d0dae8;
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 8px;
            font-weight: bold;
            color: #4a5568;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 14px;
            top: 2px;
            padding: 0 4px;
            font-size: 11px;
            color: #718096;
        }
        QLineEdit#amountInput {
            font-size: 22px;
            font-weight: bold;
            padding: 8px 12px;
            border: 1.5px solid #cbd5e0;
            border-radius: 8px;
            background: #f7fafc;
            color: #1a3a5c;
        }
        QLineEdit#amountInput:focus {
            border: 1.5px solid #3498db;
            background: #ffffff;
        }
        QComboBox#currencyCombo {
            font-size: 14px;
            font-weight: bold;
            padding: 6px 10px;
            border: 1.5px solid #cbd5e0;
            border-radius: 8px;
            background: #ebf4ff;
            color: #1a3a5c;
        }
        QComboBox#currencyCombo:hover {
            border-color: #3498db;
        }
        QComboBox#currencyCombo::drop-down {
            border: none;
            width: 18px;
        }
        QLabel#resultLabel {
            font-size: 28px;
            font-weight: bold;
            color: #1a7a4a;
            padding: 4px 0;
        }
        QLabel#resultCurrencyLabel {
            font-size: 12px;
            color: #718096;
        }
        QLabel#rateLabel {
            background-color: #ebf8ff;
            color: #1a5276;
            border: 1px solid #bee3f8;
            border-radius: 8px;
            padding: 9px 14px;
            font-size: 12px;
        }
        QLabel#lastUpdated {
            font-size: 11px;
            color: #27ae60;
        }
        QPushButton#swapButton {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                        stop:0 #3498db, stop:1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton#swapButton:hover {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                        stop:0 #2980b9, stop:1 #2471a3);
        }
        QPushButton#swapButton:pressed {
            background: #1f618d;
        }
        QPushButton#actionButton {
            background-color: #edf2f7;
            color: #2c3e50;
            border: 1px solid #cbd5e0;
            border-radius: 8px;
            padding: 8px;
            font-size: 12px;
        }
        QPushButton#actionButton:hover {
            background-color: #e2e8f0;
        }
        QPushButton#actionButton:disabled {
            color: #a0aec0;
        }
        QPushButton#copyButton {
            background-color: #f0fff4;
            color: #1a7a4a;
            border: 1px solid #9ae6b4;
            border-radius: 8px;
            padding: 7px;
            font-size: 12px;
        }
        QPushButton#copyButton:hover {
            background-color: #c6f6d5;
        }
        QPushButton#copyButton:disabled {
            color: #a0aec0;
            border-color: #e2e8f0;
            background: #f7fafc;
        }
        QPushButton#quickPairBtn {
            background-color: #f7fafc;
            color: #2d3748;
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            padding: 6px 4px;
            font-size: 11px;
        }
        QPushButton#quickPairBtn:hover {
            background-color: #ebf4ff;
            border-color: #3498db;
            color: #1a3a5c;
        }
        QPushButton#quickPairBtn:pressed {
            background-color: #dbeafe;
        }
        QListWidget#historyList {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 12px;
            background: #f7fafc;
            alternate-background-color: #edf2f7;
        }
        QListWidget#historyList::item {
            padding: 3px 8px;
        }
        QLabel#footerLabel {
            font-size: 10px;
            color: #a0aec0;
            padding-top: 4px;
        }
        QScrollBar:vertical {
            width: 6px;
            background: #f0f4f8;
        }
        QScrollBar::handle:vertical {
            background: #cbd5e0;
            border-radius: 3px;
        }
    )");
}

// ── Populate currency dropdowns ───────────────────────────────────────────────
void MainWindow::populateCurrencies() {
    m_fromCombo->clear();
    m_toCombo->clear();
    for (const QString& c : CURRENCIES) {
        m_fromCombo->addItem(c);
        m_toCombo->addItem(c);
    }
    m_fromCombo->setCurrentText("USD");
    m_toCombo->setCurrentText("PKR");
}

// ── Fetch ─────────────────────────────────────────────────────────────────────
void MainWindow::startFetch() {
    m_statusLabel->setText("Fetching live rates...");
    m_statusLabel->setStyleSheet("color: #e67e22; font-size: 11px;");
    m_refreshBtn->setEnabled(false);
    m_fetcher->fetchRates();
}

void MainWindow::onRatesFetched(std::map<std::string, double> rates) {
    m_rates = rates;
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss AP");
    m_statusLabel->setText("Live rates loaded successfully");
    m_statusLabel->setStyleSheet("color: #27ae60; font-size: 11px;");
    m_lastUpdatedLabel->setText("Last updated: " + time);
    m_refreshBtn->setEnabled(true);
    onConvert();
}

void MainWindow::onFetchError(QString message) {
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet("color: #e74c3c; font-size: 11px;");
    m_refreshBtn->setEnabled(true);
    m_rateLabel->setText("Could not fetch rates. Click Refresh to try again.");
}

// ── Conversion logic ──────────────────────────────────────────────────────────
double MainWindow::getRate(const std::string& from,
                            const std::string& to) const {
    if (m_rates.empty()) return 0.0;
    double fromRate = (from == "usd") ? 1.0
        : (m_rates.count(from) ? m_rates.at(from) : 0.0);
    double toRate   = (to   == "usd") ? 1.0
        : (m_rates.count(to)   ? m_rates.at(to)   : 0.0);
    if (fromRate == 0.0 || toRate == 0.0) return 0.0;
    return toRate / fromRate;
}

QString MainWindow::formatNumber(double value, const QString& currency) const {
    static const QStringList noDecimal    = {"JPY","KRW","IDR","HUF","TWD"};
    static const QStringList threeDecimal = {"BHD","KWD","OMR"};
    if (noDecimal.contains(currency))
        return QString::number(std::round(value), 'f', 0);
    if (threeDecimal.contains(currency))
        return QString::number(value, 'f', 3);
    return QString::number(value, 'f', 2);
}

void MainWindow::onConvert() {
    if (m_rates.empty()) return;

    QString fromUpper = m_fromCombo->currentText();
    QString toUpper   = m_toCombo->currentText();
    QString fromLower = fromUpper.toLower();
    QString toLower   = toUpper.toLower();

    bool   ok;
    double amount = m_amountInput->text().toDouble(&ok);
    if (!ok || amount < 0) {
        m_resultLabel->setText("---");
        m_resultCurrencyLabel->setText("");
        m_copyBtn->setEnabled(false);
        return;
    }

    double rate   = getRate(fromLower.toStdString(), toLower.toStdString());
    double result = amount * rate;

    m_resultLabel->setText(formatNumber(result, toUpper));
    m_resultCurrencyLabel->setText(toUpper);

    double reverseRate = (rate > 0.0) ? (1.0 / rate) : 0.0;
    m_rateLabel->setText(
        QString("1 %1  =  %2 %3        |        1 %3  =  %4 %1")
            .arg(fromUpper)
            .arg(formatNumber(rate, toUpper))
            .arg(toUpper)
            .arg(formatNumber(reverseRate, fromUpper)));

    m_copyBtn->setEnabled(true);
}

// ── Button handlers ───────────────────────────────────────────────────────────
void MainWindow::onSwap() {
    QString from = m_fromCombo->currentText();
    QString to   = m_toCombo->currentText();
    m_fromCombo->setCurrentText(to);
    m_toCombo->setCurrentText(from);
    onConvert();
}

void MainWindow::onRefresh() {
    startFetch();
}

void MainWindow::onCopyResult() {
    QString text = m_resultLabel->text() + " " + m_toCombo->currentText();
    QApplication::clipboard()->setText(text);
    m_copyBtn->setText("Copied!");
    QTimer::singleShot(1500, [this](){ m_copyBtn->setText("Copy Result"); });
}

void MainWindow::onQuickPair(const QString& pair) {
    QStringList parts = pair.split(QString::fromUtf8("→"));
    if (parts.size() != 2) return;
    m_fromCombo->setCurrentText(parts[0]);
    m_toCombo->setCurrentText(parts[1]);
    m_amountInput->setText("1");
    onConvert();

    if (!m_rates.empty()) {
        double rate = getRate(parts[0].toLower().toStdString(),
                              parts[1].toLower().toStdString());
        QString entry = QString("[%1]  1 %2 = %3 %4")
            .arg(QDateTime::currentDateTime().toString("hh:mm"))
            .arg(parts[0])
            .arg(formatNumber(rate, parts[1]))
            .arg(parts[1]);
        addToHistory(entry);
    }
}

void MainWindow::addToHistory(const QString& entry) {
    m_historyList->insertItem(0, entry);
    if (m_historyList->count() > 12)
        delete m_historyList->takeItem(12);
}
