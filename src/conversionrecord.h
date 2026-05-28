#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// ConversionRecord  —  Represents one completed currency conversion
//
// OOP CONCEPTS DEMONSTRATED:
//   • Encapsulation  : all data members private, exposed only via getters
//   • File Handling  : serialize() / deserialize() for CSV persistence
// ─────────────────────────────────────────────────────────────────────────────

#include <QString>
#include <QStringList>
#include <QDateTime>

class ConversionRecord {
public:
    // ── Constructors ──────────────────────────────────────────────────────────
    ConversionRecord() = default;

    ConversionRecord(const QString& from,
                     const QString& to,
                     double          amount,
                     double          result,
                     double          rate)
        : m_timestamp(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
        , m_from(from.toUpper())
        , m_to(to.toUpper())
        , m_amount(amount)
        , m_result(result)
        , m_rate(rate)
    {}

    // ── Encapsulation: read-only getters ──────────────────────────────────────
    QString timestamp() const { return m_timestamp; }
    QString from()      const { return m_from;      }
    QString to()        const { return m_to;        }
    double  amount()    const { return m_amount;    }
    double  result()    const { return m_result;    }
    double  rate()      const { return m_rate;      }

    // ── Human-readable display line for UI list widget ────────────────────────
    QString toDisplayString() const {
        return QString("[%1]  %2 %3  →  %4 %5  (rate: %6)")
            .arg(m_timestamp)
            .arg(m_amount,    0, 'f', 2)
            .arg(m_from)
            .arg(m_result,    0, 'f', 4)
            .arg(m_to)
            .arg(m_rate,      0, 'f', 6);
    }

    // ── File Handling: serialize to one CSV line ──────────────────────────────
    // FIX: timestamp contains spaces ("2024-01-15 14:30:00") — wrap it in quotes
    // so the comma-split in deserialize() still finds exactly 6 fields.
    QString serialize() const {
        return QString("\"%1\",%2,%3,%4,%5,%6")
            .arg(m_timestamp)
            .arg(m_from)
            .arg(m_to)
            .arg(m_amount, 0, 'f', 6)
            .arg(m_result, 0, 'f', 6)
            .arg(m_rate,   0, 'f', 8);
    }

    // ── File Handling: deserialize from one CSV line ──────────────────────────
    // FIX: handles quoted timestamp field correctly.
    // Returns a default-constructed (invalid) record on parse failure.
    static ConversionRecord deserialize(const QString& line) {
        // Strip leading quote if present, then split on `",` to extract timestamp
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) return {};

        ConversionRecord rec;

        // Timestamp is wrapped in quotes: "yyyy-MM-dd hh:mm:ss",FROM,TO,...
        if (trimmed.startsWith('"')) {
            int closeQuote = trimmed.indexOf('"', 1);
            if (closeQuote < 0) return {};                  // malformed
            rec.m_timestamp = trimmed.mid(1, closeQuote - 1);
            trimmed = trimmed.mid(closeQuote + 2);          // skip closing quote + comma
        } else {
            // Legacy format without quotes — split on first comma
            int firstComma = trimmed.indexOf(',');
            if (firstComma < 0) return {};
            rec.m_timestamp = trimmed.left(firstComma);
            trimmed = trimmed.mid(firstComma + 1);
        }

        QStringList parts = trimmed.split(',');
        if (parts.size() < 5) return {};                    // too few fields

        rec.m_from   = parts[0].trimmed();
        rec.m_to     = parts[1].trimmed();

        bool ok1, ok2, ok3;
        rec.m_amount = parts[2].trimmed().toDouble(&ok1);
        rec.m_result = parts[3].trimmed().toDouble(&ok2);
        rec.m_rate   = parts[4].trimmed().toDouble(&ok3);

        if (!ok1 || !ok2 || !ok3) return {};                // invalid numbers
        return rec;
    }

    // ── Validity check (default-constructed record is invalid) ────────────────
    bool isValid() const { return !m_from.isEmpty() && m_rate > 0.0; }

private:
    // ── Encapsulation: all data members private ───────────────────────────────
    QString m_timestamp;
    QString m_from;
    QString m_to;
    double  m_amount  = 0.0;
    double  m_result  = 0.0;
    double  m_rate    = 0.0;
};
