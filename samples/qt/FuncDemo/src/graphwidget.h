/****************************************************************************
**
** Copyright (C) 2006-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>

#include "FThread.h"

#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>

class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget* parent);
    ~GraphWidget();
    
    bool createModel(const Handle(TDocStd_Document)& doc);
    Handle(TDocStd_Document) getDocument() { return myDocument; }

    Node* findNode(const TDF_Label& );
    void  setNbThreads(const int nb);
    int   getNbThreads();
    void  accelerateThread(const int thread_index);
    void  slowDownThread(const int thread_index);
    void  compute();

    void setFinished();
    bool isFinished();

protected:
    void wheelEvent(QWheelEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void scaleView(qreal scaleFactor);

private:
    Handle(TDocStd_Document) myDocument;
    int                      myNbThreads;
    FThread*                 myThread1;
    FThread*                 myThread2;
    FThread*                 myThread3;
    FThread*                 myThread4;
    int                      myNbFinishedThreads;
};

#endif
