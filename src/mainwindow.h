#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow  —  Qt GUI; owns all widgets and wires OOP components together
//
// OOP CONCEPTS DEMONSTRATED:
//   • Inheritance          : MainWindow : public QMainWindow
//   • Encapsulation        : all widgets and helpers are private
//   • Dynamic Polymorphism : holds BaseFetcher* — runtime dispatch to ApiFetcher
// ─────────────────────────────────────────────────────────────────────────────

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QStatusBar>

#include "basefetcher.h"
#include "currencyconverter.h"
#include "historymanager.h"
#include "conversionrecord.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    // ── Conversion ─────────────────────────────────────────────────────────
    void onConvert();          // triggered by any input change
    void onSwap();             // swap from/to currencies

    // ── Quick-pair buttons ─────────────────────────────────────────────────
    void onQuickPair(const QString& from, const QString& to, double amount);

    // ── API / rate refresh ─────────────────────────────────────────────────
    void startFetch();
    void onRefresh();
    void onRatesFetched(std::map<std::string, double> rates);
    void onFetchError(QString message);

    // ── History ────────────────────────────────────────────────────────────
    void onClearHistory();

private:
    // ── UI setup helpers ───────────────────────────────────────────────────
    void setupUI();
    void setupConnections();
    void applyStyles();
    void populateCurrencies();

    // ── History helpers ────────────────────────────────────────────────────
    void loadHistoryFromFile();
    void addToHistory(const ConversionRecord& record);

    // ── Encapsulation: all members private ────────────────────────────────

    // Dynamic Polymorphism: pointer to abstract base — ApiFetcher at runtime
    BaseFetcher*  m_fetcher;        // owned; deleted in destructor

    // Logic components
    CurrencyConverter m_converter;
    HistoryManager    m_history;
    QVector<ConversionRecord> m_records;

    // Timer for auto-refresh every 60 s
    QTimer* m_autoTimer;

    // Widgets
    QComboBox*    m_fromCombo;
    QComboBox*    m_toCombo;
    QLineEdit*    m_amountInput;
    QLabel*       m_resultLabel;
    QLabel*       m_rateLabel;
    QPushButton*  m_swapBtn;
    QPushButton*  m_refreshBtn;
    QPushButton*  m_clearHistoryBtn;
    QListWidget*  m_historyList;
    QGroupBox*    m_historyGroup;
};
