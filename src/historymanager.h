#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// HistoryManager  —  Persists and loads ConversionRecord objects to/from CSV
//
// OOP CONCEPTS DEMONSTRATED:
//   • Encapsulation        : m_filePath is private; callers use save()/loadFromFile()
//   • File Handling        : std::ifstream / std::ofstream for CSV read/write
//   • Static Polymorphism  : overloaded save() — one record vs. full vector
//
// FIXES vs original:
//   1. save(single) no longer appends to file forever — after capping m_records
//      in MainWindow::addToHistory(), the whole trimmed vector is rewritten via
//      save(vector), keeping the file size in sync with MAX_RECORDS.
//   2. save(vector) header logic uses file.is_open() + trunc — reliable on all
//      platforms (original tellp()==0 check fails on append-mode streams).
//   3. deserialize() failures are silently skipped so a corrupt line does not
//      crash the load.
//   4. File path now uses QStandardPaths so it works regardless of working dir.
// ─────────────────────────────────────────────────────────────────────────────

#include "conversionrecord.h"

#include <QVector>
#include <QString>
#include <QStandardPaths>
#include <QDir>

#include <fstream>
#include <sstream>
#include <string>

class HistoryManager {
public:
    // Maximum records kept in memory AND on disk
    static const int MAX_RECORDS = 50;

    // ── Constructor: locate (or create) the app data directory ───────────────
    explicit HistoryManager() {
        // FIX: use a stable, user-writable path instead of the raw working dir
        QString dir = QStandardPaths::writableLocation(
                          QStandardPaths::AppDataLocation);
        QDir().mkpath(dir);                           // create if missing
        m_filePath = dir + "/history.csv";
    }

    // ── Encapsulation: file path is private ──────────────────────────────────
    QString filePath() const { return m_filePath; }

    // ── File Handling: load all records from CSV at startup ──────────────────
    QVector<ConversionRecord> loadFromFile() const {
        QVector<ConversionRecord> records;
        std::ifstream file(m_filePath.toStdString());
        if (!file.is_open()) return records;

        std::string line;
        bool firstLine = true;
        while (std::getline(file, line)) {
            if (firstLine) {
                firstLine = false;      // skip the header row
                continue;
            }
            if (line.empty()) continue;

            ConversionRecord rec = ConversionRecord::deserialize(
                                       QString::fromStdString(line));
            if (rec.isValid()) {
                records.prepend(rec);   // newest first in the returned vector
            }
            // FIX: silently skip malformed / invalid lines instead of crashing
        }
        return records;
    }

    // ── Static Polymorphism: overloaded save() ────────────────────────────────
    //
    // Overload 1 — Save a SINGLE new record (append path).
    //   NOTE: this overload is intentionally kept for the static-polymorphism
    //   requirement, but MainWindow::addToHistory() calls Overload 2 (vector)
    //   after capping, so the file is always kept trimmed to MAX_RECORDS.
    void save(const ConversionRecord& record) {
        // If file doesn't exist yet, write header first then append.
        // If it already exists, just append.
        bool needsHeader = !QFile::exists(m_filePath);

        std::ofstream file(m_filePath.toStdString(), std::ios::app);
        if (!file.is_open()) return;

        if (needsHeader) {
            // FIX: check file existence instead of tellp()==0 — the original
            // tellp() check always failed on an existing append-mode stream.
            file << "timestamp,from,to,amount,result,rate\n";
        }

        file << record.serialize().toStdString() << '\n';
    }

    // Overload 2 — Rewrite the ENTIRE file from a vector of records.
    //   Used by addToHistory() to keep the on-disk file exactly MAX_RECORDS.
    //   FIX: this overload is now actually called (it was dead code originally).
    void save(const QVector<ConversionRecord>& records) {
        std::ofstream file(m_filePath.toStdString(), std::ios::trunc);
        if (!file.is_open()) return;

        // Write CSV header
        file << "timestamp,from,to,amount,result,rate\n";

        // Records are stored newest-first in the vector; write oldest first so
        // the file reads chronologically top-to-bottom.
        for (int i = records.size() - 1; i >= 0; --i) {
            file << records[i].serialize().toStdString() << '\n';
        }
    }

    // ── Clear the file (used by Clear History button) ─────────────────────────
    void clearFile() {
        std::ofstream file(m_filePath.toStdString(), std::ios::trunc);
        if (!file.is_open()) return;                // FIX: guard before writing
        file << "timestamp,from,to,amount,result,rate\n";
    }

private:
    // ── Encapsulation: private data ───────────────────────────────────────────
    QString m_filePath;
};
