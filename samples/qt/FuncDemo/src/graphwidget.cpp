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

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QApplication>

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

GraphWidget::GraphWidget(QWidget* parent):QGraphicsView(parent),
    myThread1(0),myThread2(0),myThread3(0),myThread4(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(1, 1, parent->width(), parent->height());
    setScene(scene);
    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

    scale(0.7, 0.81);
    setMinimumSize(400, 400);
    setWindowTitle(tr("Function Mechanism"));

    setNbThreads(1);
}

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
    int dx = width() / nbx, dy = height() / nby;
    int x0 = dx / 2, y0 = dy / 2; // start position

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

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
	painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
	painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
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