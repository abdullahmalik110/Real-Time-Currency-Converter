#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <map>
#include <string>
#include "apifetcher.h"

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow  —  Main application window
// ─────────────────────────────────────────────────────────────────────────────
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onRatesFetched(std::map<std::string, double> rates);
    void onFetchError(QString message);
    void onConvert();
    void onSwap();
    void onRefresh();
    void onQuickPair(const QString& pair);
    void onCopyResult();

private:
    void setupUI();
    void applyStyleSheet();
    void populateCurrencies();
    void startFetch();
    void addToHistory(const QString& entry);
    double getRate(const std::string& from, const std::string& to) const;
    QString formatNumber(double value, const QString& currency) const;

    // ── Widgets ───────────────────────────────────────────────────
    QLineEdit*   m_amountInput;
    QComboBox*   m_fromCombo;
    QComboBox*   m_toCombo;
    QLabel*      m_resultLabel;
    QLabel*      m_resultCurrencyLabel;
    QLabel*      m_rateLabel;
    QLabel*      m_statusLabel;
    QLabel*      m_lastUpdatedLabel;
    QPushButton* m_refreshBtn;
    QPushButton* m_copyBtn;
    QListWidget* m_historyList;

    // ── Data ──────────────────────────────────────────────────────
    std::map<std::string, double> m_rates;
    ApiFetcher* m_fetcher;
    QTimer*     m_autoTimer;

    // ── Currency lists ────────────────────────────────────────────
    static const QStringList CURRENCIES;
    static const QStringList QUICK_PAIRS;
};
