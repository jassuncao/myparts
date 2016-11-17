#include "csvexporter.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QAbstractTableModel>
#include <QIODevice>
#include <QTableView>
#include <QTextCodec>

CsvExporter::CsvExporter(bool withHeader, const QChar & fieldDelimiter, const QString & charset) :
    _withHeader(withHeader),
    _fieldDelimiter(fieldDelimiter),
    _quoteMode(DefaultQuoteMode),
    _tableView(0)
{
    if(!charset.isEmpty()){
        _codec = QTextCodec::codecForName(charset.toAscii());
    }
}

CsvExporter::QuoteMode CsvExporter::quoteMode() const
{
   return _quoteMode;
}

void CsvExporter::setQuoteMode(QuoteMode mode)
{
    _quoteMode = mode;
}

void CsvExporter::setTable(QTableView * tableView)
{
    _tableView = tableView;
}

void CsvExporter::toCSV(const QString & filename) const
{
    if(!_tableView){
        return;
    }

    int row, rows, col, cols;
    QString data;
    QAbstractItemModel * model;
    QFile dest(filename);

    model = _tableView->model();
    cols = model->columnCount();

    if(!dest.isOpen()) dest.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream stream(&dest);
    if(_codec) {
        stream.setCodec(_codec);
    }
    stream.setGenerateByteOrderMark(true);

    qDebug()<<"Exporting as CSV with delimiter "<<_fieldDelimiter<<" codec "<<stream.codec()->name();

    QVector<int> colIndexes;
    for(col = 0; col < cols; ++col) {
        if(!_tableView->isColumnHidden(col)){
           colIndexes.append(col);
        }
    }

    if(_withHeader) {
        data = "";
        for(int idx = 0; idx < colIndexes.size(); ++idx){
            if(idx > 0){
                data += _fieldDelimiter;
            }
            int colIdx = colIndexes.at(idx);
            QString colHeader = model->headerData(colIdx, Qt::Horizontal, Qt::DisplayRole).toString();
            data += addCsvQuotes(colHeader);
        }

        stream << data << endl;
    }
    rows = model->rowCount();
    for(row = 0; row < rows; ++row)
    {
        data = "";
        for(int idx = 0; idx < colIndexes.size(); ++idx){
            if(idx > 0){
                data += _fieldDelimiter;
            }
            int colIdx = colIndexes.at(idx);
            QString cellValue = model->index(row, colIdx).data().toString();
            data += addCsvQuotes(cellValue);
        }
        stream << data << endl;
        //SQLite fetches blocks of 256 rows. We must call fetchMore to retrieve the remaining rows.
        if(row == rows - 1){
            const QModelIndex parent;
            if(model->canFetchMore(parent)){
                model->fetchMore(parent);
                rows = model->rowCount();
            }
        }
    }
    stream << flush;
    dest.close();
    qDebug() << "Wrote " << rows <<" rows";
}

QString CsvExporter::addCsvQuotes(QString field) const
{
    bool addDoubleQuotes = ((_quoteMode & CsvExporter::DoubleQuote) && field.contains('"'));
    bool addSingleQuotes = ((_quoteMode & CsvExporter::SingleQuote) && field.contains('\''));
    bool quoteField = (_quoteMode & CsvExporter::AlwaysQuoteOutput) || addDoubleQuotes || addSingleQuotes;
    if(quoteField && !addDoubleQuotes && !addSingleQuotes) {
        if(_quoteMode & CsvExporter::DoubleQuote)
            addDoubleQuotes = true;
        else if(_quoteMode & CsvExporter::SingleQuote)
            addSingleQuotes = true;
    }
    if(_quoteMode & CsvExporter::BackslashEscape) {
        if(addDoubleQuotes)
            return '"' + field.replace("\\", "\\\\").replace("\"", "\\\"") + '"';
        if(addSingleQuotes)
            return '\'' + field.replace("\\", "\\\\").replace("'", "\\'") + '\'';
    } else {
        if(addDoubleQuotes)
            return '"' + field.replace("\"", "\"\"") + '"';
        if(addSingleQuotes)
            return '\'' + field.replace("'", "''") + '\'';
    }
    return field;
}
