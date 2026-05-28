#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// CurrencyConverter  —  Holds exchange rates and performs conversions
//
// OOP CONCEPTS DEMONSTRATED:
//   • Encapsulation        : m_rates is private; access only via public methods
//   • Static Polymorphism  : overloaded convert() and format() methods
//
// FIXES vs original:
//   1. convert() now checks for Inf/NaN result (e.g. user types "1e308")
//   2. convert(QString) returns 0.0 for empty string (was returning 0.0 already
//      but now also handles whitespace-only input)
//   3. getRate() is now public so MainWindow can display the live rate clearly
// ─────────────────────────────────────────────────────────────────────────────

#include <QString>
#include <QLocale>
#include <map>
#include <string>
#include <cmath>    // std::isinf, std::isnan

class CurrencyConverter {
public:
    CurrencyConverter() = default;

    // ── Load a fresh rate map (called after every successful API fetch) ───────
    void setRates(const std::map<std::string, double>& rates) {
        m_rates = rates;
    }

    bool hasRates() const { return !m_rates.empty(); }

    // ── Exchange-rate lookup (now public for UI display) ──────────────────────
    // Returns 0.0 if either currency is unknown.
    double getRate(const QString& from, const QString& to) const {
        if (m_rates.empty()) return 0.0;

        auto itFrom = m_rates.find(from.toLower().toStdString());
        auto itTo   = m_rates.find(to.toLower().toStdString());

        if (itFrom == m_rates.end() || itTo == m_rates.end()) return 0.0;
        if (itFrom->second == 0.0) return 0.0;   // guard division by zero

        // All rates are relative to USD, so:
        //   from→USD = 1 / fromRate,  USD→to = toRate
        return itTo->second / itFrom->second;
    }

    // ══ Static Polymorphism: three overloads of convert() ════════════════════

    // Overload 1 — double amount (primary path)
    double convert(double amount,
                   const QString& from,
                   const QString& to,
                   bool* ok = nullptr) const
    {
        if (ok) *ok = false;
        if (amount < 0.0) return 0.0;   // negative amounts are invalid

        double rate = getRate(from, to);
        if (rate == 0.0) return 0.0;

        double result = amount * rate;

        // FIX: guard against Inf/NaN (e.g. amount = 1e308 * large rate)
        if (std::isinf(result) || std::isnan(result)) return 0.0;

        if (ok) *ok = true;
        return result;
    }

    // Overload 2 — integer amount (demonstrates static polymorphism)
    double convert(int amount,
                   const QString& from,
                   const QString& to,
                   bool* ok = nullptr) const
    {
        return convert(static_cast<double>(amount), from, to, ok);
    }

    // Overload 3 — QString amount (parses user-typed text)
    double convert(const QString& amountStr,
                   const QString& from,
                   const QString& to,
                   bool* ok = nullptr) const
    {
        if (ok) *ok = false;

        // FIX: also reject whitespace-only strings
        QString trimmed = amountStr.trimmed();
        if (trimmed.isEmpty()) return 0.0;

        bool parsed = false;
        double amount = trimmed.toDouble(&parsed);
        if (!parsed || amount < 0.0) return 0.0;

        return convert(amount, from, to, ok);
    }

    // ══ Static Polymorphism: two overloads of format() ═══════════════════════

    // Overload 1 — format with currency-appropriate decimal places
    QString format(double value, const QString& currency) const {
        // JPY, KWD etc. have different conventions; default to 2 decimals
        static const QStringList zeroDec = {"jpy","krw","vnd","clp","idr"};
        int decimals = zeroDec.contains(currency.toLower()) ? 0 : 2;
        return format(value, decimals);
    }

    // Overload 2 — format with an explicit decimal place count
    QString format(double value, int decimals) const {
        return QLocale(QLocale::English).toString(value, 'f', decimals);
    }

private:
    // ── Encapsulation: private data ───────────────────────────────────────────
    std::map<std::string, double> m_rates;
};
