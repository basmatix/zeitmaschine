/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMQTREEWIDGET_H
#define ZMQTREEWIDGET_H

#include <QtGui/QTreeWidget>
#include <QtGui/QDropEvent>

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
