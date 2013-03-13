#ifndef ZMQTREEWIDGET_H
#define ZMQTREEWIDGET_H

#include <QtGui/QTreeWidget>

class zmQTreeWidget
        : public QTreeWidget
{
    Q_OBJECT

private:


public:

    explicit zmQTreeWidget(QWidget *parent = 0)
        : QTreeWidget       ( parent )
    {
    }

    virtual ~zmQTreeWidget()
    {
    }

};

#endif // ZMQTREEWIDGET_H

