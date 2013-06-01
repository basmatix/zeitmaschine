/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMQTREEWIDGET_H
#define ZMQTREEWIDGET_H

#include "zmQtGtdModel.h"
#include <zm/zmTrace.h>

#include <QtGui/QTreeWidget>
#include <QtGui/QDropEvent>
#include <ctime>

class zmQTreeWidgetItem
        : public QTreeWidgetItem
{
    std::time_t     m_creationTime;
    zmQtGtdModel   &m_model;
    std::string     m_uid;
    int             m_importance;

public:

    zmQTreeWidgetItem( zmQtGtdModel &model, const std::string &uid )
        : QTreeWidgetItem   ()
        , m_creationTime    ( 0 )
        , m_model           ( model )
        , m_uid             ( uid )
        , m_importance      ( 0 )
    {
        m_creationTime = m_model.getCreationTime( uid );

        setText( 2, QString().sprintf("%ld", m_creationTime ) );

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
        m_importance = m_model.getImportance( m_uid );
        setText( 1, QString().sprintf("%d", m_importance ) );
        setText( 0, m_model.getCaption( m_uid ) );
        QBrush b = foreground( 0 );
        QFont f = font( 0 );
        //f = QFont("" , 9 , QFont::Bold );
        b.setColor( Qt::red );
        if( m_model.isInboxItem( m_uid ) )
        {
            b.setColor( Qt::darkMagenta );
            f.setBold( false );
            f.setItalic( false );
        }
        if( m_model.isProjectItem( m_uid, false ) )
        {
            if( m_model.getNextTask( m_uid ) != "" )
            {
                b.setColor( Qt::darkCyan );
            }
            else
            {
                b.setColor( Qt::darkRed );
            }
            f.setBold( false );
            f.setItalic( true );
        }
        if( m_model.isTaskItem( m_uid, true ) )
        {
            b.setColor( Qt::darkBlue );
            f.setItalic( false );
            f.setBold( false );
            if( m_model.getParentProject( m_uid ) != "" )
            {
                if( m_model.getNextTask( m_model.getParentProject( m_uid ) ) == m_uid )
                {
                    f.setBold( true );
                }
            }
        }

        setFont( 0, f );
        setForeground( 0, b );
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

public:

    explicit zmQTreeWidget(QWidget *parent = 0)
        : QTreeWidget       ( parent )
    {
        setAcceptDrops( true );
    }

    void resizeEvent(QResizeEvent* event)
    {
        setColumnWidth( 0, width()-50 );
    }

    void showEvent(QShowEvent *)
    {
        //setColumnCount( 2 );
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

