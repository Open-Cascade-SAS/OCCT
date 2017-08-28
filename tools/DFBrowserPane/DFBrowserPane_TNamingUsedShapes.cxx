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

#include <inspector/DFBrowserPane_TNamingUsedShapes.hxx>

#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>
#include <inspector/DFBrowserPane_AttributePaneSelector.hxx>
#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDF_Label.hxx>

#include <TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_PtrRefShape.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_UsedShapes.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#include <QItemSelectionModel>
#include <QTableView>
#include <QVariant>
#include <QWidget>

const int COLUMN_TYPE_WIDTH = 70;
const int COLUMN_POINTER_WIDTH = 90;
const int COLUMN_REFERENCE_WIDTH = 90;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TNamingUsedShapes::DFBrowserPane_TNamingUsedShapes()
: DFBrowserPane_AttributePane()
{
  getPaneModel()->SetColumnCount (4);
  getPaneModel()->SetItalicColumns (QList<int>());

  QList<QVariant> aValues;
  aValues << "ShapeType" << "TShape" << "Label" << "RefShape";
  getPaneModel()->SetHeaderValues (aValues, Qt::Horizontal);
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TNamingUsedShapes::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TNaming_UsedShapes) anAttribute = Handle(TNaming_UsedShapes)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  std::list<TCollection_AsciiString> aReferences;
  if (myAttributeRefs.Find (anAttribute, aReferences))
  {
    QMap<TCollection_AsciiString, QList<QVariant> > anEntryValues;
    QList<QVariant> aValues;
    TNaming_DataMapOfShapePtrRefShape& aMap = anAttribute->Map();
    for (TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape aRefIt (aMap); aRefIt.More(); aRefIt.Next())
    {
      TCollection_AsciiString anEntry = DFBrowserPane_Tools::GetEntry (aRefIt.Value()->Label());
      aValues.clear();
      TopoDS_Shape aShape = aRefIt.Key();
      aValues.append (!aShape.IsNull() ? DFBrowserPane_Tools::ToName (DB_SHAPE_TYPE, aShape.ShapeType()).ToCString()
                                       : "EMPTY SHAPE");
      aValues.append (!aShape.IsNull() ? DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()->This()).ToCString() : "");
      const TopoDS_Shape aValueShape = aRefIt.Value()->Shape();
      aValues.append (!aValueShape.IsNull() ?
                                    DFBrowserPane_Tools::GetPointerInfo (aValueShape.TShape()->This()).ToCString() : "");
      anEntryValues[anEntry] = aValues;
    }

    for (std::list<TCollection_AsciiString>::const_iterator aRefIt = aReferences.begin(); aRefIt != aReferences.end(); aRefIt++)
    {
      aValues = anEntryValues[*aRefIt];
      theValues << aValues[0] << aValues[1] << QString ((*aRefIt).ToCString()) << aValues[2];
    }
  }
  else
  {
    TNaming_DataMapOfShapePtrRefShape& aMap = anAttribute->Map();
    for (TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape aRefIt (aMap); aRefIt.More(); aRefIt.Next())
    {
      TopoDS_Shape aShape = aRefIt.Key();
      theValues.append (!aShape.IsNull() ? DFBrowserPane_Tools::ToName (DB_SHAPE_TYPE, aShape.ShapeType()).ToCString()
                                         : "EMPTY SHAPE");
      theValues.append (!aShape.IsNull() ? DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()->This()).ToCString() : "");
      theValues.append (DFBrowserPane_Tools::GetEntry (aRefIt.Value()->Label()).ToCString());
      const TopoDS_Shape aValueShape = aRefIt.Value()->Shape();
      theValues.append (!aValueShape.IsNull() ? DFBrowserPane_Tools::GetPointerInfo (aValueShape.TShape()->This()).ToCString() : "");
    }
  }
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowserPane_TNamingUsedShapes::GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute, int theRole,
                                                            int theColumnId)
{
  if (theColumnId != 0)
    return DFBrowserPane_AttributePane::GetAttributeInfo (theAttribute, theRole, theColumnId);

  switch (theRole)
  {
    case Qt::ForegroundRole: return QColor (myAttributeRefs.IsEmpty() ? Qt::gray : Qt::black);
    case Qt::ToolTipRole:
      return QVariant (myAttributeRefs.IsEmpty() ? QString (QObject::tr ("Content is not sorted yet")) : "");
    default:
      break;
  }
  return DFBrowserPane_AttributePane::GetAttributeInfo (theAttribute, theRole, theColumnId);
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_TNamingUsedShapes::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                             QList<QVariant>& theValues)
{
  Handle(TNaming_UsedShapes) anAttribute = Handle(TNaming_UsedShapes)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  theValues.append (QString ("%1").arg (anAttribute->Map().Extent()));
}

// =======================================================================
// function : GetAttributeReferences
// purpose :
// =======================================================================
void DFBrowserPane_TNamingUsedShapes::GetAttributeReferences (const Handle(TDF_Attribute)& theAttribute,
                                                              NCollection_List<Handle(TDF_Attribute)>& theRefAttributes,
                                                              Handle(Standard_Transient)& /*theRefPresentation*/)
{
  Handle(TNaming_UsedShapes) anAttribute = Handle(TNaming_UsedShapes)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  QStringList aSelectedEntries = DFBrowserPane_TableView::GetSelectedColumnValues (getTableView()->GetTableView(), 2);
  if (aSelectedEntries.isEmpty())
    return;

  for (TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape aRefIt (anAttribute->Map()); aRefIt.More(); aRefIt.Next())
  {
    if (aSelectedEntries.contains (DFBrowserPane_Tools::GetEntry (aRefIt.Value()->Label()).ToCString()))
      theRefAttributes.Append (aRefIt.Value()->NamedShape());
  }
}

// =======================================================================
// function : getTableColumnWidths
// purpose :
// =======================================================================
QMap<int, int> DFBrowserPane_TNamingUsedShapes::getTableColumnWidths() const
{
  QMap<int, int> aValues;
  aValues[0] = COLUMN_TYPE_WIDTH; // "ShapeType"
  aValues[1] = COLUMN_POINTER_WIDTH; // "Key_TShape"
  aValues[2] = COLUMN_REFERENCE_WIDTH; // "Label Entry"
  return aValues;
}
