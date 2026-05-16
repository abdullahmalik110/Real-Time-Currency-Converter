#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <map>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// ApiFetcher
// Uses Qt's built-in QNetworkAccessManager — NO libcurl needed.
// Fetches live exchange rates from the free fawazahmed0 currency API.
// Emits ratesFetched() on success, fetchError() on failure.
// ─────────────────────────────────────────────────────────────────────────────
class ApiFetcher : public QObject {
    Q_OBJECT

public:
    explicit ApiFetcher(QObject* parent = nullptr);
    void fetchRates();

signals:
    void ratesFetched(std::map<std::string, double> rates);
    void fetchError(QString message);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_manager;
};
