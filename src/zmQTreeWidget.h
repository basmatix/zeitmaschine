/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMQTREEWIDGET_H
#define ZMQTREEWIDGET_H

#include "zmTrace.h"

#include <QtGui/QTreeWidget>
#include <QtGui/QDropEvent>
#include <ctime>

class zmQTreeWidgetItem
        : public QTreeWidgetItem
{
    std::time_t m_creationTime;

public:
    zmQTreeWidgetItem(  std::time_t creationTime )
        : QTreeWidgetItem()
        , m_creationTime( creationTime )
    {
        setFlags( flags() | Qt::ItemIsEditable );
    }

    //zmQTreeWidgetItem(QTreeWidget *tree) : QTreeWidgetItem(tree)  {}
    //zmQTreeWidgetItem(QTreeWidget * parent, const QStringList & strings)
     //              : QTreeWidgetItem (parent,strings)  {}
    std::time_t getCreationTime() const
    {
        return m_creationTime;
    }

    void decorate()
    {
        QBrush b = this->foreground(0);
        b.setColor( Qt::darkCyan );
        this->setForeground( 0, b );
        //m_liToday->setBackgroundColor( 0, Qt::lightGray );
    }

    bool operator< (const QTreeWidgetItem &other) const
    {
        //int sortCol = 0;//treeWidget()->sortColumn();
        //int myNumber = text(sortCol).toInt();
        //int otherNumber = other.text(sortCol).toInt();
        const zmQTreeWidgetItem * l_this = reinterpret_cast< const zmQTreeWidgetItem *>(this);
        const zmQTreeWidgetItem * l_that = reinterpret_cast< const zmQTreeWidgetItem *>(&other);
        // tracemessage("compare %d and %d",
        //              l_this->getCreationTime(),
        //              l_that->getCreationTime() );
        return l_this->getCreationTime() < l_that->getCreationTime();
    }
};


class zmQTreeWidget
        : public QTreeWidget
{
    Q_OBJECT

private:


public:

    explicit zmQTreeWidget(QWidget *parent = 0)
        : QTreeWidget       ( parent )
    {
        setAcceptDrops( true );
    }

    virtual ~zmQTreeWidget()
    {
    }

    void unselect()
    {
        foreach(QTreeWidgetItem* item, selectedItems() )
        {
            item->setSelected( false );
        }
    }

    void dropEvent ( QDropEvent * event )
    {
        //const QMimeData* mime_data = event->mimeData();

        /*
        if(!mime_data->hasFormat("application/mytype"))
        {
            event->ignore();
            return;
        }
        */
//        std::cout << mime_data->formats();
        if( selectedItems().size() != 1 )
        {
            event->ignore();
            return;
        }

        QTreeWidgetItem *l_target = itemAt( event->pos());

        emit itemDropped( selectedItems()[0], l_target );

/*
        if(event->keyboardModifiers() == Qt::NoModifier)
        {
            const QPoint pos = event->pos();
            QTreeWidgetItem* target = itemAt( event->pos());

//            bool do_move = true;

//            // this is used to determine if the target is itself a child
//            if(target->parent() != (QTreeWidgetItem*)0)
//            {
//                do_move = false;
//            }
//            else
//            {
//                foreach(QTreeWidgetItem* item, selectedItems() )
//                {
//                    // if target and item don't share the same parent...
//                    if(target->parent() != item->parent())
//                    {
//                        // ...then don't allow the move
//                        do_move = false;
//                        break;
//                    }
//                }
            }

            if(!do_move)
            {
                event->setDropAction(Qt::IgnoreAction);
            }
            else
            {
                QTreeWidget::dropEvent(event);
                event->setDropAction(Qt::TargetMoveAction);
            }

            event->accept();
        }
        else if(event->proposedAction() == Qt::TargetMoveAction)
        {
            QTreeWidget::dropEvent(event);
            event->acceptProposedAction();
        }
        else
        {
            QTreeWidget::dropEvent(event);
        }
        */
    }

signals:

    void itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target );
};

#endif // ZMQTREEWIDGET_H

