// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <XCAFDoc.hxx>
#include <XCAFDoc_ColorType.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_LengthUnit.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_NamedShape.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_Volume.hxx>

//=================================================================================================

const Standard_GUID& XCAFDoc::ShapeRefGUID()
{
  static const Standard_GUID ID("5b896afe-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::AssemblyGUID()
{
  static const Standard_GUID ID("5b896b00-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ExternRefGUID()
{
  static const Standard_GUID ID("6b896b01-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ColorRefGUID(const XCAFDoc_ColorType type)
{
  static const Standard_GUID IDcol("efd212e4-6dfd-11d4-b9c8-0060b0ee281b");
  static const Standard_GUID IDcolSurf("efd212e5-6dfd-11d4-b9c8-0060b0ee281b");
  static const Standard_GUID IDcolCurv("efd212e6-6dfd-11d4-b9c8-0060b0ee281b");

  switch (type)
  {
    default:
    case XCAFDoc_ColorGen:
      return IDcol;
    case XCAFDoc_ColorSurf:
      return IDcolSurf;
    case XCAFDoc_ColorCurv:
      return IDcolCurv;
  }
}

//=================================================================================================

const Standard_GUID& XCAFDoc::DimTolRefGUID()
{
  // static const Standard_GUID IDDimTol("58ed092d-44de-11d8-8776-001083004c77");
  static const Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  // return IDDimTol;
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::DimensionRefFirstGUID()
{
  static const Standard_GUID ID("efd212e3-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::DimensionRefSecondGUID()
{
  static const Standard_GUID ID("efd212e0-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::GeomToleranceRefGUID()
{
  static const Standard_GUID ID("efd213e3-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::DatumRefGUID()
{
  static const Standard_GUID ID("efd212e2-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::DatumTolRefGUID()
{
  // static const Standard_GUID IDDimTol("58ed092d-44de-11d8-8776-001083004c77");
  static const Standard_GUID ID("efd212e7-6dfd-11d4-b9c8-0060b0ee281b");
  // return IDDimTol;
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::LayerRefGUID()
{
  static const Standard_GUID ID("efd212e8-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::MaterialRefGUID()
{
  static const Standard_GUID ID("efd212f7-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::VisMaterialRefGUID()
{
  static const Standard_GUID ID("936F4070-5369-405D-A7AD-2AC76C860EC8");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::NoteRefGUID()
{
  static const Standard_GUID ID("F3599E50-F84A-493e-8D1B-1284E79322F1");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::InvisibleGUID()
{
  static const Standard_GUID ID("5b896aff-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ColorByLayerGUID()
{
  static const Standard_GUID ID("279e8c1e-70af-4130-b626-9cc52a537db8");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::SHUORefGUID()
{
  static const Standard_GUID ID("efd212ea-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ViewRefGUID()
{
  static const Standard_GUID ID("efd213e5-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ViewRefShapeGUID()
{
  static const Standard_GUID ID("efd213e6-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ViewRefGDTGUID()
{
  static const Standard_GUID ID("efd213e7-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ViewRefPlaneGUID()
{
  static const Standard_GUID ID("efd213e9-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ViewRefNoteGUID()
{
  static const Standard_GUID ID("C814ACC6-43AC-4812-9B2A-4E9A2A549354");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::ViewRefAnnotationGUID()
{
  static const Standard_GUID ID("A2B5BA42-DD00-43f5-8882-4B5F8E76B9D2");
  return ID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc::LockGUID()
{
  static const Standard_GUID ID("efd213eb-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=================================================================================================

TCollection_AsciiString XCAFDoc::AttributeInfo(const Handle(TDF_Attribute)& theAtt)
{
  TCollection_AsciiString anInfo;

  if (theAtt->IsKind(STANDARD_TYPE(TDataStd_TreeNode)))
  {
    Standard_CString type = "";
    if (theAtt->ID() == XCAFDoc::ShapeRefGUID())
      type = "Shape Instance Link";
    else if (theAtt->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorGen))
      type = "Generic Color Link";
    else if (theAtt->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorSurf))
      type = "Surface Color Link";
    else if (theAtt->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorCurv))
      type = "Curve Color Link";
    else if (theAtt->ID() == XCAFDoc::DimTolRefGUID())
      type = "DGT Link";
    else if (theAtt->ID() == XCAFDoc::DatumRefGUID())
      type = "Datum Link";
    else if (theAtt->ID() == XCAFDoc::MaterialRefGUID())
      type = "Material Link";
    Handle(TDataStd_TreeNode) TN = Handle(TDataStd_TreeNode)::DownCast(theAtt);
    TCollection_AsciiString   ref;
    if (TN->HasFather())
    {
      TDF_Tool::Entry(TN->Father()->Label(), ref);
      anInfo = type;
      anInfo += TCollection_AsciiString(" ==> ") + ref;
    }
    else
    {
      anInfo = type;
      anInfo += TCollection_AsciiString(" <== (") + ref;
      Handle(TDataStd_TreeNode) child = TN->First();
      while (!child.IsNull())
      {
        TDF_Tool::Entry(child->Label(), ref);
        if (child != TN->First())
          anInfo += ", ";
        anInfo += ref;
        child = child->Next();
      }
      anInfo += ")";
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDF_Reference)))
  {
    Handle(TDF_Reference)   val = Handle(TDF_Reference)::DownCast(theAtt);
    TCollection_AsciiString ref;
    TDF_Tool::Entry(val->Get(), ref);
    anInfo += TCollection_AsciiString("==> ") + ref;
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDF_TagSource)))
  {
    Handle(TDF_TagSource) val = Handle(TDF_TagSource)::DownCast(theAtt);
    anInfo += TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Integer)))
  {
    Handle(TDataStd_Integer) val = Handle(TDataStd_Integer)::DownCast(theAtt);
    anInfo                       = TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Real)))
  {
    Handle(TDataStd_Real) val = Handle(TDataStd_Real)::DownCast(theAtt);
    anInfo                    = TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Name)))
  {
    Handle(TDataStd_Name) val = Handle(TDataStd_Name)::DownCast(theAtt);
    anInfo                    = TCollection_AsciiString(val->Get(), '?');
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Comment)))
  {
    Handle(TDataStd_Comment) val = Handle(TDataStd_Comment)::DownCast(theAtt);
    anInfo                       = TCollection_AsciiString(val->Get(), '?');
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_AsciiString)))
  {
    Handle(TDataStd_AsciiString) val = Handle(TDataStd_AsciiString)::DownCast(theAtt);
    anInfo                           = TCollection_AsciiString(val->Get(), '?');
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_LengthUnit)))
  {
    Handle(XCAFDoc_LengthUnit) aVal = Handle(XCAFDoc_LengthUnit)::DownCast(theAtt);
    anInfo                          = TCollection_AsciiString(aVal->GetUnitValue());
    anInfo += " ";
    anInfo += aVal->GetUnitName();
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_IntegerArray)))
  {
    Handle(TDataStd_IntegerArray) val = Handle(TDataStd_IntegerArray)::DownCast(theAtt);
    for (Standard_Integer j = val->Lower(); j <= val->Upper(); j++)
    {
      if (j > val->Lower())
        anInfo += TCollection_AsciiString(", ");
      anInfo += TCollection_AsciiString(val->Value(j));
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_RealArray)))
  {
    Handle(TDataStd_RealArray) val = Handle(TDataStd_RealArray)::DownCast(theAtt);
    for (Standard_Integer j = val->Lower(); j <= val->Upper(); j++)
    {
      if (j > val->Lower())
        anInfo += TCollection_AsciiString(", ");
      anInfo += TCollection_AsciiString(val->Value(j));
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_ByteArray)))
  {
    Handle(TDataStd_ByteArray) val = Handle(TDataStd_ByteArray)::DownCast(theAtt);
    for (Standard_Integer j = val->Lower(); j <= val->Upper(); j++)
    {
      if (j > val->Lower())
        anInfo += TCollection_AsciiString(", ");
      anInfo += TCollection_AsciiString(val->Value(j));
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TNaming_NamedShape)))
  {
    Handle(TNaming_NamedShape) val = Handle(TNaming_NamedShape)::DownCast(theAtt);
    TopoDS_Shape               S   = val->Get();
    if (!S.IsNull())
      anInfo = S.TShape()->DynamicType()->Name();
    else
      anInfo = "Empty Shape";
    if (!S.Location().IsIdentity())
      anInfo += TCollection_AsciiString("(located)");
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_ShapeMapTool)))
  {

    Handle(XCAFDoc_ShapeMapTool) anAttr = Handle(XCAFDoc_ShapeMapTool)::DownCast(theAtt);
    anInfo += TCollection_AsciiString(anAttr->GetMap().Extent());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Volume)))
  {
    Handle(XCAFDoc_Volume) val = Handle(XCAFDoc_Volume)::DownCast(theAtt);
    anInfo += TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Area)))
  {
    Handle(XCAFDoc_Area) val = Handle(XCAFDoc_Area)::DownCast(theAtt);
    anInfo                   = TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Centroid)))
  {
    Handle(XCAFDoc_Centroid) val        = Handle(XCAFDoc_Centroid)::DownCast(theAtt);
    gp_Pnt                   myCentroid = val->Get();
    anInfo                              = "(";
    anInfo += TCollection_AsciiString(myCentroid.X());
    anInfo += TCollection_AsciiString(" , ");
    anInfo += TCollection_AsciiString(TCollection_AsciiString(myCentroid.Y()));
    anInfo += TCollection_AsciiString(" , ");
    anInfo += TCollection_AsciiString(myCentroid.Z());
    anInfo += TCollection_AsciiString(")");
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_UAttribute)))
  {
    if (theAtt->ID() == XCAFDoc::AssemblyGUID())
      anInfo += TCollection_AsciiString("is assembly");
    if (theAtt->ID() == XCAFDoc::InvisibleGUID())
      anInfo += TCollection_AsciiString("invisible");
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Color)))
  {
    Handle(XCAFDoc_Color) val = Handle(XCAFDoc_Color)::DownCast(theAtt);
    Quantity_ColorRGBA    C   = val->GetColorRGBA();
    char                  string[260];
    Sprintf(string,
            "%s (%g, %g, %g, %g)",
            C.GetRGB().StringName(C.GetRGB().Name()),
            C.GetRGB().Red(),
            C.GetRGB().Green(),
            C.GetRGB().Blue(),
            C.Alpha());
    anInfo = string;
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_DimTol)))
  {
    Handle(XCAFDoc_DimTol)        val  = Handle(XCAFDoc_DimTol)::DownCast(theAtt);
    Standard_Integer              kind = val->GetKind();
    Handle(TColStd_HArray1OfReal) HAR  = val->GetVal();
    if (kind < 20)
    { // dimension
      anInfo = "Diameter (ValueRange[";
      anInfo += TCollection_AsciiString(HAR->Value(1));
      anInfo += TCollection_AsciiString(",");
      anInfo += TCollection_AsciiString(HAR->Value(2));
      anInfo += TCollection_AsciiString("])");
    }
    else
    {
      switch (kind)
      {
        case 21:
          anInfo = "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_1";
          break;
        case 22:
          anInfo = "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_2";
          break;
        case 23:
          anInfo = "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_3";
          break;
        case 24:
          anInfo = "AngularityTolerance";
          break;
        case 25:
          anInfo = "CircularRunoutTolerance";
          break;
        case 26:
          anInfo = "CoaxialityTolerance";
          break;
        case 27:
          anInfo = "ConcentricityTolerance";
          break;
        case 28:
          anInfo = "ParallelismTolerance";
          break;
        case 29:
          anInfo = "PerpendicularityTolerance";
          break;
        case 30:
          anInfo = "SymmetryTolerance";
          break;
        case 31:
          anInfo = "TotalRunoutTolerance";
          break;
        case 35:
          anInfo = "ModifiedGeometricTolerance_1";
          break;
        case 36:
          anInfo = "ModifiedGeometricTolerance_2";
          break;
        case 37:
          anInfo = "ModifiedGeometricTolerance_3";
          break;
        case 38:
          anInfo = "CylindricityTolerance";
          break;
        case 39:
          anInfo = "FlatnessTolerance";
          break;
        case 40:
          anInfo = "LineProfileTolerance";
          break;
        case 41:
          anInfo = "PositionTolerance";
          break;
        case 42:
          anInfo = "RoundnessTolerance";
          break;
        case 43:
          anInfo = "StraightnessTolerance";
          break;
        case 44:
          anInfo = "SurfaceProfileTolerance";
          break;
      }
      if (anInfo.Length() > 0)
      {
        anInfo += " (Value=";
        anInfo += TCollection_AsciiString(HAR->Value(1));
        anInfo += ")";
      }
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Material)))
  {
    Handle(XCAFDoc_Material) val     = Handle(XCAFDoc_Material)::DownCast(theAtt);
    Standard_Real            dens    = val->GetDensity();
    Standard_CString         dimdens = "g/cu sm";
    if (dens == 0)
      anInfo = val->GetName()->ToCString();
    else
    {
      anInfo = val->GetName()->ToCString();
      anInfo += "(density=";
      anInfo += TCollection_AsciiString(dens);
      anInfo += dimdens;
      anInfo += ")";
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_GraphNode)))
  {
    Standard_CString type;
    if (theAtt->ID() == XCAFDoc::LayerRefGUID())
    {
      type = "Layer Instance Link";
    }
    else if (theAtt->ID() == XCAFDoc::SHUORefGUID())
    {
      type = "SHUO Instance Link";
    }
    else if (theAtt->ID() == XCAFDoc::DatumTolRefGUID())
    {
      type = "DatumToler Link";
    }
    else if (theAtt->ID() == XCAFDoc::DimensionRefFirstGUID())
    {
      type = "Dimension Link First";
    }
    else if (theAtt->ID() == XCAFDoc::DimensionRefSecondGUID())
    {
      type = "Dimension Link Second";
    }
    else if (theAtt->ID() == XCAFDoc::GeomToleranceRefGUID())
    {
      type = "GeomTolerance Link";
    }
    else if (theAtt->ID() == XCAFDoc::DatumRefGUID())
    {
      type = "Datum Link";
    }
    else if (theAtt->ID() == XCAFDoc::ViewRefShapeGUID())
    {
      type = "View Shape Link";
    }
    else if (theAtt->ID() == XCAFDoc::ViewRefGDTGUID())
    {
      type = "View GD&T Link";
    }
    else if (theAtt->ID() == XCAFDoc::ViewRefPlaneGUID())
    {
      type = "View Clipping Plane Link";
    }
    else
      return TCollection_AsciiString();

    Handle(XCAFDoc_GraphNode) DETGN = Handle(XCAFDoc_GraphNode)::DownCast(theAtt);
    TCollection_AsciiString   ref;
    Standard_Integer          ii = 1;
    if (DETGN->NbFathers() != 0)
    {

      TDF_Tool::Entry(DETGN->GetFather(ii)->Label(), ref);
      anInfo = type;
      anInfo += " ==> (";
      anInfo += ref;
      for (ii = 2; ii <= DETGN->NbFathers(); ii++)
      {
        TDF_Tool::Entry(DETGN->GetFather(ii)->Label(), ref);
        anInfo += ", ";
        anInfo += ref;
      }
      anInfo += ") ";
    }
    ii = 1;
    if (DETGN->NbChildren() != 0)
    {
      TDF_Tool::Entry(DETGN->GetChild(ii)->Label(), ref);
      anInfo += type;
      anInfo += " <== (";
      anInfo += ref;
      for (ii = 2; ii <= DETGN->NbChildren(); ii++)
      {
        TDF_Tool::Entry(DETGN->GetChild(ii)->Label(), ref);
        anInfo += ", ";
        anInfo += ref;
      }
      anInfo += ") ";
    }
  }
  return anInfo;
}
