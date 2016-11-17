#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <QObject>
#include <QModelIndex>

class QAbstractTableModel;
class QTableView;
class QIODevice;
class QTextCodec;

class CsvExporter
{
public:
    CsvExporter(bool withHeader = false, const QChar & fieldDelimiter = ',', const QString & charset = QString());
    void toCSV(const QString &filename) const;

    enum QuoteOption { NoQuotes = 0, SingleQuote = 1, DoubleQuote = 2, BothQuotes = 3,
                       NoEscape = 0, TwoQuoteEscape = 4, BackslashEscape = 8,
                       AlwaysQuoteOutput = 16, DefaultQuoteMode = BothQuotes | BackslashEscape | AlwaysQuoteOutput };
    Q_DECLARE_FLAGS(QuoteMode, QuoteOption)
    QuoteMode quoteMode() const;
    void setQuoteMode(QuoteMode mode);
    void setTable(QTableView * tableView);
private:
    QString addCsvQuotes(QString field) const;
    bool _withHeader;
    QChar _fieldDelimiter;
    QTextCodec* _codec;
    CsvExporter::QuoteMode _quoteMode;
    QTableView * _tableView;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CsvExporter::QuoteMode)

#endif // CSVEXPORTER_H
