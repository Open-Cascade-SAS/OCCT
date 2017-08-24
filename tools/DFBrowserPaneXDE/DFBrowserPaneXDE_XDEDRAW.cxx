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

#include <inspector/DFBrowserPaneXDE_XDEDRAW.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <TDataStd_AsciiString.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>

#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>

#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

#include <XCAFDoc.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX > 0x060901
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#endif
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Volume.hxx>

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
TCollection_AsciiString DFBrowserPaneXDE_XDEDRAW::GetAttributeInfo (Handle(TDF_Attribute) att)
{
  TCollection_AsciiString anInfo;

  if ( att->IsKind(STANDARD_TYPE (TDataStd_TreeNode)) ) {
    Standard_CString type = "";
    if ( att->ID() == XCAFDoc::ShapeRefGUID() ) type = "Shape Instance Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorGen) ) type = "Generic Color Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorSurf) ) type = "Surface Color Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorCurv) ) type = "Curve Color Link";
    else if ( att->ID() == XCAFDoc::DimTolRefGUID() ) type = "DGT Link";
    else if ( att->ID() == XCAFDoc::DatumRefGUID() ) type = "Datum Link";
    else if ( att->ID() == XCAFDoc::MaterialRefGUID() ) type = "Material Link";
    Handle(TDataStd_TreeNode) TN = Handle(TDataStd_TreeNode)::DownCast (att);
    TCollection_AsciiString ref;
    if ( TN->HasFather() ) {
      TDF_Tool::Entry ( TN->Father()->Label(), ref );
      anInfo = type;
      anInfo += TCollection_AsciiString (" ==> ") + ref.ToCString();
    }
    else {
      anInfo = type;
      anInfo += TCollection_AsciiString (" <== (") + ref.ToCString();
      Handle(TDataStd_TreeNode) child = TN->First();
      while ( ! child.IsNull() ) {
        TDF_Tool::Entry ( child->Label(), ref );
        if ( child != TN->First() ) anInfo +=  ", " ;
        anInfo += ref.ToCString();
        child = child->Next();
      }
      anInfo += ")";
    }
  }
  else if ( att->IsKind(STANDARD_TYPE (TDF_Reference)) ) {
    Handle(TDF_Reference) val = Handle(TDF_Reference)::DownCast ( att );
    TCollection_AsciiString ref;
    TDF_Tool::Entry ( val->Get(), ref );
    anInfo += TCollection_AsciiString ("==> ") + ref.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_Integer)) ) {
    Handle(TDataStd_Integer) val = Handle(TDataStd_Integer)::DownCast ( att );
    anInfo = TCollection_AsciiString ( val->Get() );
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_Real)) ) {
    Handle(TDataStd_Real) val = Handle(TDataStd_Real)::DownCast ( att );
    anInfo = TCollection_AsciiString ( val->Get() );
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_Name)) ) {
    Handle(TDataStd_Name) val = Handle(TDataStd_Name)::DownCast ( att );
    anInfo = TCollection_AsciiString ( val->Get(), '?' );
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_Comment)) ) {
    Handle(TDataStd_Comment) val = Handle(TDataStd_Comment)::DownCast ( att );
    anInfo = TCollection_AsciiString ( val->Get(), '?' );
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_AsciiString)) ) {
    Handle(TDataStd_AsciiString) val = Handle(TDataStd_AsciiString)::DownCast ( att );
    anInfo = TCollection_AsciiString ( val->Get(), '?' );
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_IntegerArray)) ) {
    Handle(TDataStd_IntegerArray) val = Handle(TDataStd_IntegerArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) {
      if ( j > val->Lower() ) anInfo += TCollection_AsciiString ( ", " );
      anInfo += TCollection_AsciiString ( val->Value(j) );
    }
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_RealArray)) ) {
    Handle(TDataStd_RealArray) val = Handle(TDataStd_RealArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) {
      if ( j > val->Lower() ) anInfo += TCollection_AsciiString ( ", " );
      anInfo += TCollection_AsciiString ( val->Value(j) );
    }
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_ByteArray)) ) {
    Handle(TDataStd_ByteArray) val = Handle(TDataStd_ByteArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) {
      if ( j > val->Lower() ) anInfo += TCollection_AsciiString ( ", " );
      anInfo += TCollection_AsciiString ( val->Value(j) );
    }
  }
  else if ( att->IsKind(STANDARD_TYPE (TNaming_NamedShape)) ) {
    Handle(TNaming_NamedShape) val = Handle(TNaming_NamedShape)::DownCast ( att );
    TopoDS_Shape S = val->Get();
    if (!S.IsNull())
      anInfo = S.TShape()->DynamicType()->Name();
    else
      anInfo = "Empty Shape";
    if ( ! S.Location().IsIdentity() ) anInfo += TCollection_AsciiString ( "(located)" );
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_Volume)) ) {
    Handle(XCAFDoc_Volume) val = Handle(XCAFDoc_Volume)::DownCast ( att );
    anInfo += TCollection_AsciiString ( val->Get() );
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_Area)) ) {
    Handle(XCAFDoc_Area) val = Handle(XCAFDoc_Area)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    anInfo = str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_Centroid)) ) {
    Handle(XCAFDoc_Centroid) val = Handle(XCAFDoc_Centroid)::DownCast ( att );
    gp_Pnt myCentroid = val->Get();
    anInfo = "(" ;
    anInfo += TCollection_AsciiString ( myCentroid.X() ).ToCString();
    anInfo += TCollection_AsciiString ( " , " );
    anInfo += TCollection_AsciiString ( TCollection_AsciiString ( myCentroid.Y() ).ToCString() );
    anInfo += TCollection_AsciiString ( " , " );
    anInfo += TCollection_AsciiString ( myCentroid.Z() ).ToCString();
    anInfo += TCollection_AsciiString ( ")" );
  }
  else if ( att->IsKind(STANDARD_TYPE (TDataStd_UAttribute)) ) {
    if ( att->ID() == XCAFDoc::AssemblyGUID() ) anInfo += TCollection_AsciiString ( "is assembly" );
    if ( att->ID() == XCAFDoc::InvisibleGUID() ) anInfo += TCollection_AsciiString ( "invisible" );
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_Color)) ) {
    Handle(XCAFDoc_Color) val = Handle(XCAFDoc_Color)::DownCast ( att );
    Quantity_Color C = val->GetColor();
    char string[260];
    Sprintf ( string, "%s (%g, %g, %g)", C.StringName ( C.Name() ),
              C.Red(), C.Green(), C.Blue() );
    anInfo = string;
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_DimTol)) ) {
    Handle(XCAFDoc_DimTol) val = Handle(XCAFDoc_DimTol)::DownCast ( att );
    Standard_Integer kind = val->GetKind();
    Handle(TColStd_HArray1OfReal) HAR = val->GetVal();
    if(kind<20) { //dimension
      anInfo = "Diameter (ValueRange[";
      anInfo += TCollection_AsciiString ( HAR->Value(1) );
      anInfo += TCollection_AsciiString ( "," );
      anInfo += TCollection_AsciiString ( HAR->Value(2) );
      anInfo += TCollection_AsciiString ( "])" );
    }
    else {
      switch (kind) {
      case 21: anInfo = "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_1"; break;
      case 22: anInfo = "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_2"; break;
      case 23: anInfo = "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_3"; break;
      case 24: anInfo = "AngularityTolerance"; break;
      case 25: anInfo = "CircularRunoutTolerance"; break;
      case 26: anInfo = "CoaxialityTolerance"; break;
      case 27: anInfo = "ConcentricityTolerance"; break;
      case 28: anInfo = "ParallelismTolerance"; break;
      case 29: anInfo = "PerpendicularityTolerance"; break;
      case 30: anInfo = "SymmetryTolerance"; break;
      case 31: anInfo = "TotalRunoutTolerance"; break;
      case 35: anInfo = "ModifiedGeometricTolerance_1"; break;
      case 36: anInfo = "ModifiedGeometricTolerance_2"; break;
      case 37: anInfo = "ModifiedGeometricTolerance_3"; break;
      case 38: anInfo = "CylindricityTolerance"; break;
      case 39: anInfo = "FlatnessTolerance"; break;
      case 40: anInfo = "LineProfileTolerance"; break;
      case 41: anInfo = "PositionTolerance"; break;
      case 42: anInfo = "RoundnessTolerance"; break;
      case 43: anInfo = "StraightnessTolerance"; break;
      case 44: anInfo = "SurfaceProfileTolerance"; break;
      }
      if (anInfo.Length() > 0) {
        anInfo += " (Value=";
        anInfo += TCollection_AsciiString (HAR->Value (1));
        anInfo += ")";
      }
    }
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_Material)) ) {
    Handle(XCAFDoc_Material) val = Handle(XCAFDoc_Material)::DownCast ( att );
    Standard_Real dens = val->GetDensity();
    Standard_CString dimdens = "g/cu sm";
    if(dens==0) 
      anInfo = val->GetName()->ToCString();
    else {
      anInfo = val->GetName()->ToCString();
      anInfo += "(density=";
      anInfo += TCollection_AsciiString ( dens );
      anInfo += dimdens;
      anInfo += ")";
    }
  }
  else if ( att->IsKind(STANDARD_TYPE (XCAFDoc_GraphNode)) ) {
    Standard_CString type;
    if ( att->ID() == XCAFDoc::LayerRefGUID() ) {
      type = "Layer Instance Link";
    }
    else if ( att->ID() == XCAFDoc::SHUORefGUID() ) {
      type = "SHUO Instance Link";
    }
    else if ( att->ID() == XCAFDoc::DatumTolRefGUID() ) {
      type = "DatumToler Link";
    }
#if OCC_VERSION_HEX > 0x060901
    else if ( att->ID() == XCAFDoc::DimensionRefFirstGUID() ) {
      type = "Dimension Link First";
    }
    else if ( att->ID() == XCAFDoc::DimensionRefSecondGUID() ) {
      type = "Dimension Link Second";
    }
    else if ( att->ID() == XCAFDoc::GeomToleranceRefGUID() ){
      type = "GeomTolerance Link";
    }
#endif
    else
      return TCollection_AsciiString();

    Handle(XCAFDoc_GraphNode) DETGN = Handle(XCAFDoc_GraphNode)::DownCast (att);
    TCollection_AsciiString ref;
    Standard_Integer ii = 1;
    if (DETGN->NbFathers()!=0) {

      TDF_Tool::Entry ( DETGN->GetFather(ii)->Label(), ref );
      anInfo = type;
      anInfo += " ==> (";
      anInfo += ref;
      for (ii = 2; ii <= DETGN->NbFathers(); ii++) {
        TDF_Tool::Entry ( DETGN->GetFather(ii)->Label(), ref );
        anInfo += ", ";
        anInfo += ref.ToCString();
      }
      anInfo += ") ";
    }
    ii = 1;
    if (DETGN->NbChildren ()!=0) {
      TDF_Tool::Entry ( DETGN->GetChild(ii)->Label(), ref );
      anInfo += type;
      anInfo += " <== (";
      anInfo += ref;
      for (ii = 2; ii <= DETGN->NbChildren (); ii++) {
        TDF_Tool::Entry ( DETGN->GetChild(ii)->Label(), ref );
        anInfo += ", ";
        anInfo += ref;
      }
      anInfo += ") ";
    }
  }
  return anInfo;
}
