#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// ApiFetcher  —  Concrete fetcher using live fawazahmed0 currency API
//
// OOP CONCEPTS DEMONSTRATED:
//   • Inheritance          : derives from BaseFetcher
//   • Encapsulation        : all HTTP logic private, only fetchRates() is public
//   • Dynamic Polymorphism : overrides pure virtual fetchRates() and sourceName()
//   • Static Polymorphism  : overloaded buildRequest() methods
// ─────────────────────────────────────────────────────────────────────────────

#include "basefetcher.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

class ApiFetcher : public BaseFetcher {
    Q_OBJECT

public:
    explicit ApiFetcher(QObject* parent = nullptr);

    // ── Dynamic Polymorphism: override pure virtual methods ──────────────────
    void    fetchRates()           override;
    QString sourceName() const     override { return "fawazahmed0 CDN API"; }

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    // ── Encapsulation: HTTP details fully hidden from outside ────────────────
    QNetworkAccessManager* m_manager;
    bool                   m_usingFallback;   // tracks whether fallback is active

    // ── Static Polymorphism: overloaded buildRequest() ───────────────────────
    // Overload 1 — build request from a URL string
    QNetworkRequest buildRequest(const QString& urlStr) const;
    // Overload 2 — build request from a QUrl object (different param type)
    QNetworkRequest buildRequest(const QUrl& url)       const;

    // Primary and fallback API endpoints
    static const QString PRIMARY_URL;
    static const QString FALLBACK_URL;
};
