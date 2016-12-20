#include "alignmentproxymodel.h"

AlignmentProxyModel::AlignmentProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{    
}

void AlignmentProxyModel::setColumnAlignment(int col, Qt::Alignment alignment)
{
    if (_alignments.size() <= col){
        _alignments.resize(qMax(col + 1, 16));
    }
    _alignments[col] = QVariant(alignment);
}

QVariant AlignmentProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(role == Qt::TextAlignmentRole){
        int col = proxyIndex.column();
        if(col < _alignments.size()){
            const QVariant v = _alignments.at(col);
            if(v.isValid()){
                return v;
            }
        }
    }
    return QIdentityProxyModel::data(proxyIndex, role);
}

