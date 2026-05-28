#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// BaseFetcher  —  Abstract base class for all rate-fetching strategies
//
// OOP CONCEPTS DEMONSTRATED:
//   • Abstraction          : hides HOW rates are fetched behind a clean interface
//   • Dynamic Polymorphism : pure virtual fetchRates() overridden by subclasses
//   • Inheritance          : ApiFetcher derives from this
// ─────────────────────────────────────────────────────────────────────────────

#include <QObject>
#include <QString>
#include <map>
#include <string>

class BaseFetcher : public QObject {
    Q_OBJECT

public:
    explicit BaseFetcher(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~BaseFetcher() = default;

    // ── Pure virtual interface (Abstraction + Dynamic Polymorphism) ──────────
    virtual void    fetchRates()        = 0;   // every subclass MUST implement
    virtual QString sourceName() const  = 0;   // identify the data source

signals:
    // Shared signals — emitted by any concrete subclass
    void ratesFetched(std::map<std::string, double> rates);
    void fetchError(QString message);

protected:
    // Shared helper — used by subclasses to validate a rate value
    static bool isValidRate(double rate) { return rate > 0.0; }
};
