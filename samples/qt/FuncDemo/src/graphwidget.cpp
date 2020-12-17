/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <Standard_WarningsDisable.hxx>
#include <QtGui>
#include <Standard_WarningsRestore.hxx>

#include <math.h>

#include <TFunction_Iterator.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_Scope.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Function.hxx>

#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>

#include "SimpleDriver.h"
#include "PointDriver.h"
#include "CircleDriver.h"
#include "PrismDriver.h"
#include "ConeDriver.h"
#include "CylinderDriver.h"
#include "ShapeSaverDriver.h"
#include "SimpleDriver.h"

//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0),
      myThread1(0),myThread2(0),myThread3(0),myThread4(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Function Mechanism"));

//! [0]

//! [1]
/*
    Node *node1 = new Node(this);
    Node *node2 = new Node(this);
    Node *node3 = new Node(this);
    Node *node4 = new Node(this);
    centerNode = new Node(this);
    Node *node6 = new Node(this);
    Node *node7 = new Node(this);
    Node *node8 = new Node(this);
    Node *node9 = new Node(this);
    scene->addItem(node1);
    scene->addItem(node2);
    scene->addItem(node3);
    scene->addItem(node4);
    scene->addItem(centerNode);
    scene->addItem(node6);
    scene->addItem(node7);
    scene->addItem(node8);
    scene->addItem(node9);
    scene->addItem(new Edge(node1, node2));
    scene->addItem(new Edge(node2, node3));
    scene->addItem(new Edge(node2, centerNode));
    scene->addItem(new Edge(node3, node6));
    scene->addItem(new Edge(node4, node1));
    scene->addItem(new Edge(node4, centerNode));
    scene->addItem(new Edge(centerNode, node6));
    scene->addItem(new Edge(centerNode, node8));
    scene->addItem(new Edge(node6, node9));
    scene->addItem(new Edge(node7, node4));
    scene->addItem(new Edge(node8, node7));
    scene->addItem(new Edge(node9, node8));

    node1->setPos(-50, -50);
    node2->setPos(0, -50);
    node3->setPos(50, -50);
    node4->setPos(-50, 0);
    centerNode->setPos(0, 0);
    node6->setPos(50, 0);
    node7->setPos(-50, 50);
    node8->setPos(0, 50);
    node9->setPos(50, 50);
*/

    setNbThreads(4);
}
//! [1]

GraphWidget::~GraphWidget()
{
    if (myThread1)
    {
        myThread1->wait();
        myThread1->deleteLater();
    }
    if (myThread2)
    {
        myThread2->wait();
        myThread2->deleteLater();
    }
    if (myThread3)
    {
        myThread3->wait();
        myThread3->deleteLater();
    }
    if (myThread4)
    {
        myThread4->wait();
        myThread4->deleteLater();
    }
}

bool GraphWidget::createModel(const Handle(TDocStd_Document)& doc)
{
    myDocument = doc;
    
    TFunction_Iterator fIterator(myDocument->Main());
    fIterator.SetUsageOfExecutionStatus(false);
    Handle(TFunction_Scope) scope = TFunction_Scope::Set(myDocument->Main());

    // Find out the size of the grid: number of functions in X and Y directions
    int nbx = 0, nby = 0;
    while (!fIterator.Current().IsEmpty())
    {
        const TDF_LabelList& funcs = fIterator.Current();
        if (funcs.Extent() > nbx)
            nbx = funcs.Extent();
        nby++;
        fIterator.Next();
    }
    if (!nbx || !nby)
        return false;

    // Grid of functions
    int dx = width() / nbx / 2, dy = height() / nby;
    int x0 = int(-double(width()) / 1.5) + dx, y0 = int(-double(height()) / 1.5) + dy; // start position

    // Draw functions
    double x = x0, y = y0;
    fIterator.Init(myDocument->Main());
    while (!fIterator.Current().IsEmpty())
    {
        const TDF_LabelList& funcs = fIterator.Current();
        TDF_ListIteratorOfLabelList itrl(funcs);
        for (; itrl.More(); itrl.Next())
        {
            TDF_Label L = itrl.Value();
            Node *node = new Node(this);
            node->setFunction(L);
            scene()->addItem(node);
            node->setPos(x, y);
            x += dx;
            if (x > width())
                x = x0;
        }
        y += dy;
        if (y > height())
            y = y0;
        fIterator.Next();
        x = x0 + dx;
    }

    // Draw dependencies
    fIterator.Init(myDocument->Main());
    while (!fIterator.Current().IsEmpty())
    {
        const TDF_LabelList& funcs = fIterator.Current();
        TDF_ListIteratorOfLabelList itrl(funcs);
        for (; itrl.More(); itrl.Next())
        {
            TDF_Label L = itrl.Value();
            Node* node = findNode(L);
            if (!node)
                continue;

            // Find backward dependencies of the function
            TFunction_IFunction iFunc(L);
            Handle(TFunction_GraphNode) graphNode = iFunc.GetGraphNode();
            const TColStd_MapOfInteger& prev = graphNode->GetPrevious();
            TColStd_MapIteratorOfMapOfInteger itrm(prev);
            for (; itrm.More(); itrm.Next())
            {
                const int argID = itrm.Key();
                const TDF_Label& argL = scope->GetFunction(argID);
                Node* n = findNode(argL);
                if (!n)
                    continue;
                scene()->addItem(new Edge(n, node));
            }
        }
        fIterator.Next();
    }

    return !myDocument.IsNull();
}

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
//! [4]

//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]

//! [6]
void GraphWidget::drawBackground(QPainter* painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    Q_UNUSED(painter);

//    // Shadow
//    QRectF sceneRect = this->sceneRect();
//    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
//    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
//    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
//	painter->fillRect(rightShadow, Qt::darkGray);
//    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
//	painter->fillRect(bottomShadow, Qt::darkGray);
//
//    // Fill
//    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
//    gradient.setColorAt(0, Qt::white);
//    gradient.setColorAt(1, Qt::lightGray);
//    painter->fillRect(rect.intersect(sceneRect), gradient);
//    painter->setBrush(Qt::NoBrush);
//    painter->drawRect(sceneRect);
//
//#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5)
//    // Text
//    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
//                    sceneRect.width() - 4, sceneRect.height() - 4);
//    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
//                       "wheel or the '+' and '-' keys"));
//
//    QFont font = painter->font();
//    font.setBold(true);
//    font.setPointSize(14);
//    painter->setFont(font);
//    painter->setPen(Qt::lightGray);
//    painter->drawText(textRect.translated(2, 2), message);
//    painter->setPen(Qt::black);
//    painter->drawText(textRect, message);
//#endif
}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}


// Find node of the function
Node* GraphWidget::findNode(const TDF_Label& L)
{
    Node* node = 0;
    for (int i = 0; i < scene()->items().size(); i++)
    {
        Node* n = qgraphicsitem_cast<Node *>(scene()->items().at(i));
        if (n && n->getFunction() == L)
        {
            node = n;
            break;
        }
    }
    return node;
}

void GraphWidget::compute()
{
    myNbFinishedThreads = 0;

    TFunction_Iterator fIterator(myDocument->Main());
    fIterator.SetUsageOfExecutionStatus(true);

    myThread1 = new FThread();
    if (myNbThreads > 1)
        myThread2 = new FThread();
    if (myNbThreads > 2)
        myThread3 = new FThread();
    if (myNbThreads > 3)
        myThread4 = new FThread();

    // Logbook
    Handle(TFunction_Logbook) log = TFunction_Scope::Set(myDocument->Main())->GetLogbook();
    myThread1->setLogbook(log);
    if (myNbThreads > 1)
        myThread2->setLogbook(log);
    if (myNbThreads > 2)
        myThread3->setLogbook(log);
    if (myNbThreads > 3)
        myThread4->setLogbook(log);

    myThread1->setIterator(fIterator);
    if (myNbThreads > 1)
        myThread2->setIterator(fIterator);
    if (myNbThreads > 2)
        myThread3->setIterator(fIterator);
    if (myNbThreads > 3)
        myThread4->setIterator(fIterator);

    myThread1->setGraph(this);
    if (myNbThreads > 1)
        myThread2->setGraph(this);
    if (myNbThreads > 2)
        myThread3->setGraph(this);
    if (myNbThreads > 3)
        myThread4->setGraph(this);

    myThread1->setThreadIndex(1);
    if (myNbThreads > 1)
        myThread2->setThreadIndex(2);
    if (myNbThreads > 2)
        myThread3->setThreadIndex(3);
    if (myNbThreads > 3)
        myThread4->setThreadIndex(4);

    myThread1->setMutex(&myMutex);
    if (myNbThreads > 1)
        myThread2->setMutex(&myMutex);
    if (myNbThreads > 2)
        myThread3->setMutex(&myMutex);
    if (myNbThreads > 3)
        myThread4->setMutex(&myMutex);

    QThread::Priority priority = QThread::LowestPriority;
    if (!myThread1->isRunning())
        myThread1->start(priority);
    if (myNbThreads > 1 && !myThread2->isRunning())
        myThread2->start(priority);
    if (myNbThreads > 2 && !myThread3->isRunning())
        myThread3->start(priority);
    if (myNbThreads > 3 && !myThread4->isRunning())
        myThread4->start(priority);
}

void GraphWidget::setNbThreads(const int nb)
{
    myNbThreads = nb;
    if (myNbThreads < 4 && myThread4)
    {
        myThread4->wait();
        myThread4->deleteLater();
        myThread4 = 0;
    }
    if (myNbThreads < 3 && myThread3)
    {
        myThread3->wait();
        myThread3->deleteLater();
        myThread3 = 0;
    }
    if (myNbThreads < 2 && myThread2)
    {
        myThread2->wait();
        myThread2->deleteLater();
        myThread2 = 0;
    }

    for (int i = 1; i <= myNbThreads; i++)
    {
        TFunction_DriverTable::Get()->AddDriver(PointDriver::GetID(), new PointDriver(), i);
        TFunction_DriverTable::Get()->AddDriver(CircleDriver::GetID(), new CircleDriver(), i);
        TFunction_DriverTable::Get()->AddDriver(PrismDriver::GetID(), new PrismDriver(), i);
        TFunction_DriverTable::Get()->AddDriver(ConeDriver::GetID(), new ConeDriver(), i);
        TFunction_DriverTable::Get()->AddDriver(CylinderDriver::GetID(), new CylinderDriver(), i);
        TFunction_DriverTable::Get()->AddDriver(ShapeSaverDriver::GetID(), new ShapeSaverDriver(), i);
        TFunction_DriverTable::Get()->AddDriver(SimpleDriver::GetID(), new SimpleDriver(), i);
    }
}

int GraphWidget::getNbThreads()
{
    return myNbThreads;
}

void GraphWidget::setFinished()
{
    myNbFinishedThreads++;
}

bool GraphWidget::isFinished()
{
    return myNbThreads == myNbFinishedThreads ;
}

void GraphWidget::accelerateThread(const int thread_index)
{
    bool all_slow = true;
    if (myThread1 && myThread1->priority() != QThread::LowPriority)
        all_slow = false;
    if (all_slow && myThread2 && myThread2->priority() != QThread::LowPriority)
        all_slow = false;
    if (all_slow && myThread3 && myThread3->priority() != QThread::LowPriority)
        all_slow = false;
    if (all_slow && myThread4 && myThread4->priority() != QThread::LowPriority)
        all_slow = false;
    if (!all_slow)
        return;

    QThread::Priority priority = QThread::NormalPriority;
    switch (thread_index)
    {
    case 1:
        myThread1->setPriority(priority);
        break;
    case 2:
        myThread2->setPriority(priority);
        break;
    case 3:
        myThread3->setPriority(priority);
        break;
    case 4:
        myThread4->setPriority(priority);
        break;
    }
}

void GraphWidget::slowDownThread(const int thread_index)
{
    QThread::Priority priority = QThread::LowPriority;
    switch (thread_index)
    {
    case 1:
        myThread1->setPriority(priority);
        break;
    case 2:
        myThread2->setPriority(priority);
        break;
    case 3:
        myThread3->setPriority(priority);
        break;
    case 4:
        myThread4->setPriority(priority);
        break;
    }
}
