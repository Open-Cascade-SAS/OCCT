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

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "edge.h"
#include "node.h"
#include "graphwidget.h"

#include <TFunction_IFunction.hxx>
#include <TFunction_GraphNode.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Tick.hxx>

Node::Node(GraphWidget *graphWidget)
    : graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setZValue(1);
}

void Node::setFunction(const TDF_Label& func)
{
    myFunction = func;
}

const TDF_Label& Node::getFunction() const
{
    return myFunction;
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge *> Node::edges() const
{
    return edgeList;
}

QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    return QRectF(-15 - adjust, -15 - adjust,
                  33 + adjust, 33 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-15, -15, 30, 30);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-12, -12, 30, 30);

    QColor light_color(Qt::yellow);
    TFunction_IFunction iFunc(myFunction);
    Handle(TFunction_GraphNode) graphNode = iFunc.GetGraphNode();
    TFunction_ExecutionStatus status = graphNode->GetStatus();
    switch (status)
    {
    case TFunction_ES_WrongDefinition:
    case TFunction_ES_Failed:
        light_color = Qt::red;
        break;
    case TFunction_ES_NotExecuted:
        light_color = Qt::green;
        break;
    case TFunction_ES_Executing:
        light_color = Qt::yellow;
        break;
    case TFunction_ES_Succeeded:
        light_color = Qt::blue;
        break;
    }
    if (myFunction.IsAttribute(TDataStd_Tick::GetID()))
        light_color = Qt::white;
    QColor dark_color = light_color.dark(150);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, light_color.light(120));
        gradient.setColorAt(0, dark_color.light(120));
    } else {
        gradient.setColorAt(0, light_color);
        gradient.setColorAt(1, dark_color);
    }
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-15, -15, 30, 30);
    
    QString s;
    Handle(TDataStd_Name) n;
    if (myFunction.FindAttribute(TDataStd_Name::GetID(), n))
        s = TCollection_AsciiString(n->Get()).ToCString();
    painter->drawText(-7, 3, s);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionChange:
        foreach (Edge *edge, edgeList)
            edge->adjust();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
