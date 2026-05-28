#include "apifetcher.h"
#include "json.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>

using json = nlohmann::json;

// ── Static constants ──────────────────────────────────────────────────────────
const QString ApiFetcher::PRIMARY_URL =
    "https://cdn.jsdelivr.net/npm/@fawazahmed0/currency-api@latest/v1/currencies/usd.json";

const QString ApiFetcher::FALLBACK_URL =
    "https://latest.currency-api.pages.dev/v1/currencies/usd.json";

// ── Constructor ───────────────────────────────────────────────────────────────
ApiFetcher::ApiFetcher(QObject* parent)
    : BaseFetcher(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_usingFallback(false)
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this,      &ApiFetcher::onReplyFinished);
}

// ── Dynamic Polymorphism: override fetchRates() ───────────────────────────────
void ApiFetcher::fetchRates() {
    m_usingFallback = false;
    // Uses overload 1 — build from QString
    m_manager->get(buildRequest(PRIMARY_URL));
}

// ── Static Polymorphism ───────────────────────────────────────────────────────
// Overload 1: accepts a QString URL
QNetworkRequest ApiFetcher::buildRequest(const QString& urlStr) const {
    return buildRequest(QUrl(urlStr));   // delegates to overload 2
}

// Overload 2: accepts a QUrl directly (different parameter type = static poly)
QNetworkRequest ApiFetcher::buildRequest(const QUrl& url) const {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CurrencyConverter/2.0");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    return request;
}

// ── Handle network response ───────────────────────────────────────────────────
void ApiFetcher::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        // Try fallback URL once on primary failure
        if (!m_usingFallback) {
            m_usingFallback = true;
            m_manager->get(buildRequest(FALLBACK_URL));
            return;
        }
        // Both URLs failed
        emit fetchError("Network error: " + reply->errorString());
        return;
    }

    QByteArray data = reply->readAll();
    if (data.isEmpty()) {
        emit fetchError("Empty response from server.");
        return;
    }

    try {
        json j = json::parse(data.toStdString());

        if (!j.contains("usd") || !j["usd"].is_object()) {
            emit fetchError("Unexpected API response format.");
            return;
        }

        std::map<std::string, double> rates;
        // Iterator loop — compatible with C++11/14/17 (avoids structured bindings)
        for (auto it = j["usd"].begin(); it != j["usd"].end(); ++it) {
            if (it.value().is_number() && isValidRate(it.value().get<double>())) {
                rates[it.key()] = it.value().get<double>();
            }
        }

        if (rates.empty()) {
            emit fetchError("No valid rate data in response.");
            return;
        }

        emit ratesFetched(rates);

    } catch (const std::exception& e) {
        emit fetchError(QString("Parse error: %1").arg(e.what()));
    }
}
