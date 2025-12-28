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

TCollection_AsciiString XCAFDoc::AttributeInfo(const occ::handle<TDF_Attribute>& theAtt)
{
  TCollection_AsciiString anInfo;

  if (theAtt->IsKind(STANDARD_TYPE(TDataStd_TreeNode)))
  {
    const char* type = "";
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
    occ::handle<TDataStd_TreeNode> TN = occ::down_cast<TDataStd_TreeNode>(theAtt);
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
      occ::handle<TDataStd_TreeNode> child = TN->First();
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
    occ::handle<TDF_Reference>   val = occ::down_cast<TDF_Reference>(theAtt);
    TCollection_AsciiString ref;
    TDF_Tool::Entry(val->Get(), ref);
    anInfo += TCollection_AsciiString("==> ") + ref;
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDF_TagSource)))
  {
    occ::handle<TDF_TagSource> val = occ::down_cast<TDF_TagSource>(theAtt);
    anInfo += TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Integer)))
  {
    occ::handle<TDataStd_Integer> val = occ::down_cast<TDataStd_Integer>(theAtt);
    anInfo                       = TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Real)))
  {
    occ::handle<TDataStd_Real> val = occ::down_cast<TDataStd_Real>(theAtt);
    anInfo                    = TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Name)))
  {
    occ::handle<TDataStd_Name> val = occ::down_cast<TDataStd_Name>(theAtt);
    anInfo                    = TCollection_AsciiString(val->Get(), '?');
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_Comment)))
  {
    occ::handle<TDataStd_Comment> val = occ::down_cast<TDataStd_Comment>(theAtt);
    anInfo                       = TCollection_AsciiString(val->Get(), '?');
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_AsciiString)))
  {
    occ::handle<TDataStd_AsciiString> val = occ::down_cast<TDataStd_AsciiString>(theAtt);
    anInfo                           = TCollection_AsciiString(val->Get(), '?');
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_LengthUnit)))
  {
    occ::handle<XCAFDoc_LengthUnit> aVal = occ::down_cast<XCAFDoc_LengthUnit>(theAtt);
    anInfo                          = TCollection_AsciiString(aVal->GetUnitValue());
    anInfo += " ";
    anInfo += aVal->GetUnitName();
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_IntegerArray)))
  {
    occ::handle<TDataStd_IntegerArray> val = occ::down_cast<TDataStd_IntegerArray>(theAtt);
    for (int j = val->Lower(); j <= val->Upper(); j++)
    {
      if (j > val->Lower())
        anInfo += TCollection_AsciiString(", ");
      anInfo += TCollection_AsciiString(val->Value(j));
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_RealArray)))
  {
    occ::handle<TDataStd_RealArray> val = occ::down_cast<TDataStd_RealArray>(theAtt);
    for (int j = val->Lower(); j <= val->Upper(); j++)
    {
      if (j > val->Lower())
        anInfo += TCollection_AsciiString(", ");
      anInfo += TCollection_AsciiString(val->Value(j));
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TDataStd_ByteArray)))
  {
    occ::handle<TDataStd_ByteArray> val = occ::down_cast<TDataStd_ByteArray>(theAtt);
    for (int j = val->Lower(); j <= val->Upper(); j++)
    {
      if (j > val->Lower())
        anInfo += TCollection_AsciiString(", ");
      anInfo += TCollection_AsciiString(val->Value(j));
    }
  }
  else if (theAtt->IsKind(STANDARD_TYPE(TNaming_NamedShape)))
  {
    occ::handle<TNaming_NamedShape> val = occ::down_cast<TNaming_NamedShape>(theAtt);
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

    occ::handle<XCAFDoc_ShapeMapTool> anAttr = occ::down_cast<XCAFDoc_ShapeMapTool>(theAtt);
    anInfo += TCollection_AsciiString(anAttr->GetMap().Extent());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Volume)))
  {
    occ::handle<XCAFDoc_Volume> val = occ::down_cast<XCAFDoc_Volume>(theAtt);
    anInfo += TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Area)))
  {
    occ::handle<XCAFDoc_Area> val = occ::down_cast<XCAFDoc_Area>(theAtt);
    anInfo                   = TCollection_AsciiString(val->Get());
  }
  else if (theAtt->IsKind(STANDARD_TYPE(XCAFDoc_Centroid)))
  {
    occ::handle<XCAFDoc_Centroid> val        = occ::down_cast<XCAFDoc_Centroid>(theAtt);
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
    occ::handle<XCAFDoc_Color> val = occ::down_cast<XCAFDoc_Color>(theAtt);
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
    occ::handle<XCAFDoc_DimTol>        val  = occ::down_cast<XCAFDoc_DimTol>(theAtt);
    int              kind = val->GetKind();
    occ::handle<NCollection_HArray1<double>> HAR  = val->GetVal();
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
    occ::handle<XCAFDoc_Material> val     = occ::down_cast<XCAFDoc_Material>(theAtt);
    double            dens    = val->GetDensity();
    const char*         dimdens = "g/cu sm";
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
    const char* type;
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

    occ::handle<XCAFDoc_GraphNode> DETGN = occ::down_cast<XCAFDoc_GraphNode>(theAtt);
    TCollection_AsciiString   ref;
    int          ii = 1;
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
