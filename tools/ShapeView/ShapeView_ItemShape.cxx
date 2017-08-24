// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#include <inspector/ShapeView_ItemShape.hxx>

#include <Adaptor3d_Curve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>

#include <QObject>
#include <QStringList>

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
QString ToString (const Standard_Boolean& theValue)
{
  return theValue ? "1" : "0";
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
QString ToString (const gp_Pnt& thePoint)
{
  return QString ("(%1, %2, %3)").arg (thePoint.X()).arg (thePoint.Y()).arg (thePoint.Z());
}

// =======================================================================
// function : ToName
// purpose :
// =======================================================================
QString ToName (const TopAbs_ShapeEnum& theShapeType)
{
  Standard_SStream aSStream;
  TopAbs::Print (theShapeType, aSStream);
  return QString (aSStream.str().c_str());
}

// =======================================================================
// function : ToName
// purpose :
// =======================================================================
QString ToName (const TopAbs_Orientation& theOrientation)
{
  Standard_SStream aSStream;
  TopAbs::Print(theOrientation, aSStream);
  return QString (aSStream.str().c_str());
}

// =======================================================================
// function : ToName
// purpose :
// =======================================================================
QString ToName (const GeomAbs_Shape& theType)
{
  switch (theType)
  {
    case GeomAbs_C0: return "GeomAbs_C0";
    case GeomAbs_G1: return "GeomAbs_G1";
    case GeomAbs_C1: return "GeomAbs_C1";
    case GeomAbs_G2: return "GeomAbs_G2";
    case GeomAbs_C2: return "GeomAbs_C2";
    case GeomAbs_C3: return "GeomAbs_C3";
    case GeomAbs_CN: return "GeomAbs_CN";
    default: break;
  }
  return QString();
}

// =======================================================================
// function : ToFlags
// purpose :
// =======================================================================
void ToFlags (const TopoDS_Shape& theShape, QVariant& theValue, QVariant& theInfo)
{
  QStringList aValues;
  aValues << ToString (theShape.Checked()) << ToString (theShape.Closed())
          << ToString (theShape.Infinite()) << ToString (theShape.Locked())
          << ToString (theShape.Modified()) << ToString (theShape.Orientable());

  theValue = aValues.join ("/");
  theInfo = "Checked/Closed/Infinite/Locked/Modified/Orientable";
}

// =======================================================================
// function : ToOtherInfo
// purpose :
// =======================================================================
void ToOtherInfo (const TopoDS_Shape& theShape, QVariant& theValue, QVariant& theInfo)
{
  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    case TopAbs_SHELL:
    case TopAbs_FACE:
    case TopAbs_WIRE:
      break;
    case TopAbs_EDGE:
    {
      TopoDS_Edge anEdge = TopoDS::Edge(theShape);
      double aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);

      GeomAdaptor_Curve aAdaptor(aCurve, aFirst, aLast);
      gp_Pnt aFirstPnt = aAdaptor.Value(aFirst);
      gp_Pnt aLastPnt = aAdaptor.Value(aLast);

      BRepAdaptor_Curve aBRepAdaptor = BRepAdaptor_Curve(anEdge);
      Adaptor3d_Curve* anAdaptor3d = &aBRepAdaptor;

      QStringList aValues, anInfo;
      aValues.append (QString::number (GCPnts_AbscissaPoint::Length(*anAdaptor3d)));
      anInfo.append ("Length");

      aValues.append (aCurve->DynamicType()->Name());
      anInfo.append ("DynamicType");

      aValues.append (ToString (aFirstPnt));
      anInfo.append (QString ("First" + QString::number (aFirst)));

      aValues.append (ToString (aLastPnt));
      anInfo.append (QString ("Last" + QString::number (aLast)));

      aValues.append (ToName (aCurve->Continuity()));
      anInfo.append ("Continuity");

      aValues.append (ToString (aCurve->IsClosed()));
      anInfo.append ("IsClosed");

      if (aCurve->IsPeriodic()) {
        aValues.append (QString::number (aCurve->Period()));
        anInfo.append ("IsPeriodic");
      }
      else
      {
        aValues.append (ToString (aCurve->IsPeriodic()));
        anInfo.append ("IsPeriodic");
      }
      theValue = aValues.join (" / ");
      theInfo = QString ("%1:\n%2").arg (anInfo.join (" / ")).arg (aValues.join ("\n"));
      break;
    }
    case TopAbs_VERTEX:
    {
      TopoDS_Vertex aVertex = TopoDS::Vertex (theShape);
      gp_Pnt aPoint = BRep_Tool::Pnt (aVertex);
      theValue = ToString (aPoint);
      theInfo = "(X, Y, Z) of gp_Pnt";
      break;
    }
    case TopAbs_SHAPE:
    default:
      break;
  }
}

// =======================================================================
// function : locationInfo
// purpose :
// =======================================================================
QString locationInfo (const TopLoc_Location& theLocation)
{
  QString anInfo;

  gp_Trsf aTrsf = theLocation.Transformation();
  QStringList aValues, aRowValues;
  for (int aRowId = 1; aRowId <= 3; aRowId++)
  {
    aRowValues.clear();
    for (int aColumnId = 1; aColumnId <= 4; aColumnId++)
      aRowValues.append (QString::number (aTrsf.Value(aRowId, aColumnId)));
    aValues.append (aRowValues.join (","));
  }
  anInfo.append (aValues.join ("  "));
  return anInfo;
}

// =======================================================================
// function : GetShape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_ItemShape::GetShape (const int theRowId) const
{
  TopoDS_Iterator aSubShapeIt (myShape);
  for (int aCurrentIndex = 0; aSubShapeIt.More(); aSubShapeIt.Next(), aCurrentIndex++)
  {
    if (aCurrentIndex != theRowId)
      continue;
    break;
  }
  return aSubShapeIt.Value();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant ShapeView_ItemShape::initValue(const int theRole) const
{
  TopoDS_Shape aShape = getShape();
  if (aShape.IsNull())
    return QVariant();

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  bool isDisplayRole = theRole == Qt::DisplayRole;
  switch (Column())
  {
    case 0: return isDisplayRole ? ToName (aShape.ShapeType()) : "ShapeType";
    case 1: return isDisplayRole ? (rowCount() > 0 ? QVariant (rowCount()) : QVariant())
                                 : QVariant ("Number of sub shapes");
    case 2: return isDisplayRole ? TShapePointer().ToCString() : "TShape pointer";
    case 3: return isDisplayRole ? ToName(aShape.Orientation()) : "Orientation";
    case 4: return isDisplayRole ? locationInfo(aShape.Location()) : "Location";
    case 5:
    case 6:
    case 7:
    {
      QVariant aDataInfo, aTooTipInfo;
      if (Column() == 5)
        ToFlags(aShape, aDataInfo, aTooTipInfo);
      else if (Column() == 6)
        ToOtherInfo(aShape, aDataInfo, aTooTipInfo);
      return isDisplayRole ? aDataInfo : aTooTipInfo;
    }
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int ShapeView_ItemShape::initRowCount() const
{
  TopoDS_Shape aShape = getShape();
  if (aShape.IsNull())
    return 0;

  int aRowsCount = 0;
  for (TopoDS_Iterator aSubShapeIt(aShape); aSubShapeIt.More(); aSubShapeIt.Next())
    aRowsCount++;
  return aRowsCount;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr ShapeView_ItemShape::createChild (int theRow, int theColumn)
{
  return ShapeView_ItemShape::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ShapeView_ItemShape::Init()
{
  ShapeView_ItemRootPtr aRootItem = itemDynamicCast<ShapeView_ItemRoot> (Parent());
  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape> (Parent());
  myShape = aRootItem ? aRootItem->GetShape (Row()) : aShapeItem->GetShape (Row());
}

// =======================================================================
// function : getShape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_ItemShape::getShape() const
{
  initItem();
  return myShape;
}

// =======================================================================
// function : getPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString ShapeView_ItemShape::getPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  std::ostringstream aPtrStr;
  aPtrStr << thePointer.operator->();
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString(aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend("0x");
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void ShapeView_ItemShape::Reset()
{
  myFileName = QString();

  ShapeView_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void ShapeView_ItemShape::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<ShapeView_ItemShape*>(this)->Init();
}

