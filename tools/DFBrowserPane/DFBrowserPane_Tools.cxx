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

#include <inspector/DFBrowserPane_Tools.hxx>

#include <AIS_DisplayMode.hxx>
#include <CDM_CanCloseStatus.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Standard_Version.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd.hxx>
#include <TDataStd_RealEnum.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDF_Tool.hxx>
#include <TNaming.hxx>
#include <TNaming_NameType.hxx>
#include <TNaming_Evolution.hxx>
#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>

#include <QApplication>
#include <QPalette>
#include <QStringList>
#include <QStyle>
#include <QWidget>

#include <sstream>

const int TABLE_COLUMN_0_WIDTH = 200;
const int TABLE_COLUMN_OTHER_WIDTH = 120;

// =======================================================================
// function : DefaultPanelColumnWidth
// purpose :
// =======================================================================
int DFBrowserPane_Tools::DefaultPanelColumnWidth (const int theColumnId)
{
  return theColumnId == 0 ? TABLE_COLUMN_0_WIDTH : TABLE_COLUMN_OTHER_WIDTH;
}

// =======================================================================
// function : GetEntry
// purpose :
// =======================================================================
TCollection_AsciiString DFBrowserPane_Tools::GetEntry (const TDF_Label& theLabel)
{
  if (theLabel.IsNull())
    return "Null";

  TCollection_AsciiString anAsciiEntry;
  TDF_Tool::Entry(theLabel, anAsciiEntry);
  return anAsciiEntry;
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString DFBrowserPane_Tools::GetPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
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
// function : ShapeTypeInfo
// purpose :
// =======================================================================
QVariant DFBrowserPane_Tools::ShapeTypeInfo (const TopoDS_Shape& theShape)
{
  return theShape.IsNull() ? QString ("Empty")
                           : QString (DFBrowserPane_Tools::ToName (DB_SHAPE_TYPE, theShape.ShapeType()).ToCString());
}

// =======================================================================
// function : LightHighlightColor
// purpose :
// =======================================================================
QColor DFBrowserPane_Tools::LightHighlightColor()
{
  QWidget aWidget;
  QPalette aPalette = aWidget.palette();
  return aPalette.highlight().color().lighter();
}

// =======================================================================
// function : ToName
// purpose :
// =======================================================================
TCollection_AsciiString DFBrowserPane_Tools::ToName (const DFBrowserPane_OcctEnumType& theType,
                                                     const Standard_Integer& theEnumId)
{
  Standard_SStream aSStream;
  switch (theType)
  {
    case DB_CONSTRAINT_TYPE: { TDataXtd::Print ((TDataXtd_ConstraintEnum)theEnumId, aSStream); break; }
    case DB_NAMING_TYPE:     { TNaming::Print ((TNaming_NameType)theEnumId, aSStream); break; }
    case DB_SHAPE_TYPE:      { TopAbs::Print ((TopAbs_ShapeEnum)theEnumId, aSStream); break; }
    case DB_NS_TYPE:         { TNaming::Print ((TNaming_Evolution)theEnumId, aSStream); break; }
    case DB_GEOM_TYPE:       { TDataXtd::Print ((TDataXtd_GeometryEnum)theEnumId, aSStream); break; }
    case DB_DIMENSION_TYPE:  { TDataStd::Print ((TDataStd_RealEnum)theEnumId, aSStream); break; }
    case DB_MATERIAL_TYPE:   return Graphic3d_MaterialAspect::MaterialName (theEnumId+1);
    case DB_DISPLAY_MODE:
    {
      switch (theEnumId)
      {
        case AIS_WireFrame: return "WireFrame";
        case AIS_Shaded: return "Shaded";
        default: return "UNKNOWN DISPLAY MODE";
      }
      break;
    }
    case DB_ORIENTATION_TYPE: { TopAbs::Print((TopAbs_Orientation)theEnumId, aSStream); break; }
    case DB_CDM_CAN_CLOSE_STATUS:
    {
      switch (theEnumId)
      {
        case CDM_CCS_OK: return "OK";
        case CDM_CCS_NotOpen: return "NotOpen";
        case CDM_CCS_UnstoredReferenced: return "UnstoredReferenced";
        case CDM_CCS_ReferenceRejection: return "ReferenceRejection";
        default: return "UNKNOWN CDM_CanCloseStatus";
      }
      break;
    }
    default: return "UNKNOWN PARAMETER";
  }
  return aSStream.str().c_str();
}
