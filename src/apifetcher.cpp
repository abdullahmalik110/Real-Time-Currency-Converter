#include "apifetcher.h"
#include "json.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

using json = nlohmann::json;

// ── Constructor ───────────────────────────────────────────────────────────────
ApiFetcher::ApiFetcher(QObject* parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this,      &ApiFetcher::onReplyFinished);
}

// ── Trigger a rate fetch ──────────────────────────────────────────────────────
void ApiFetcher::fetchRates() {
    // Primary API endpoint — free, no key required
    QUrl url("https://cdn.jsdelivr.net/npm/@fawazahmed0/"
             "currency-api@latest/v1/currencies/usd.json");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CurrencyConverter/1.0");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

    m_manager->get(request);
}

// ── Handle API response ───────────────────────────────────────────────────────
void ApiFetcher::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();

    // Network-level error
    if (reply->error() != QNetworkReply::NoError) {
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
        for (auto& [key, val] : j["usd"].items()) {
            if (val.is_number()) {
                rates[key] = val.get<double>();
            }
        }

        if (rates.empty()) {
            emit fetchError("No rate data in response.");
            return;
        }

        emit ratesFetched(rates);

    } catch (const std::exception& e) {
        emit fetchError(QString("Parse error: %1").arg(e.what()));
    } catch (...) {
        emit fetchError("Unknown error while parsing rates.");
    }
}
