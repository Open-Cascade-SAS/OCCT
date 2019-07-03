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

#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_GUID.hxx>

//=======================================================================
//function : ShapeRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ShapeRefGUID ()
{
  static const Standard_GUID ID ("5b896afe-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : AssemblyGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::AssemblyGUID ()
{
  static const Standard_GUID ID ("5b896b00-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : ExternRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ExternRefGUID ()
{
  static const Standard_GUID ID ("6b896b01-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : ColorRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ColorRefGUID (const XCAFDoc_ColorType type)
{
  static const Standard_GUID IDcol     ("efd212e4-6dfd-11d4-b9c8-0060b0ee281b");
  static const Standard_GUID IDcolSurf ("efd212e5-6dfd-11d4-b9c8-0060b0ee281b");
  static const Standard_GUID IDcolCurv ("efd212e6-6dfd-11d4-b9c8-0060b0ee281b");

  switch ( type ) {
  default:
  case XCAFDoc_ColorGen : return IDcol;
  case XCAFDoc_ColorSurf: return IDcolSurf;
  case XCAFDoc_ColorCurv: return IDcolCurv;
  }
}


//=======================================================================
//function : DimTolRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::DimTolRefGUID()
{
  //static const Standard_GUID IDDimTol("58ed092d-44de-11d8-8776-001083004c77");
  static const Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  //return IDDimTol;
  return ID;
}

//=======================================================================
//function : DimensionRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::DimensionRefFirstGUID()
{
  static const Standard_GUID ID("efd212e3-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : DimensionRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::DimensionRefSecondGUID()
{
  static const Standard_GUID ID("efd212e0-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : GeomToleranceRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::GeomToleranceRefGUID()
{
  static const Standard_GUID ID("efd213e3-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : DatumRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::DatumRefGUID()
{
  static const Standard_GUID ID("efd212e2-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : DatumTolRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::DatumTolRefGUID()
{
  //static const Standard_GUID IDDimTol("58ed092d-44de-11d8-8776-001083004c77");
  static const Standard_GUID ID("efd212e7-6dfd-11d4-b9c8-0060b0ee281b");
  //return IDDimTol;
  return ID;
}


//=======================================================================
//function : LayerRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::LayerRefGUID ()
{
  static const Standard_GUID ID ("efd212e8-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : MaterialRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::MaterialRefGUID ()
{
  static const Standard_GUID ID ("efd212f7-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : VisMaterialRefGUID
//purpose  :
//=======================================================================
const Standard_GUID& XCAFDoc::VisMaterialRefGUID()
{
  static const Standard_GUID ID ("936F4070-5369-405D-A7AD-2AC76C860EC8");
  return ID;
}

//=======================================================================
//function : NoteRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::NoteRefGUID()
{
  static const Standard_GUID ID ("F3599E50-F84A-493e-8D1B-1284E79322F1");
  return ID;
}

//=======================================================================
//function : InvisibleGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::InvisibleGUID ()
{
  static const Standard_GUID ID ("5b896aff-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : ColorByLayerGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ColorByLayerGUID ()
{
  static const Standard_GUID ID ("279e8c1e-70af-4130-b626-9cc52a537db8");
  return ID;
}


//=======================================================================
//function : SHUORefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::SHUORefGUID ()
{
  static const Standard_GUID ID ("efd212ea-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : ViewRefGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ViewRefGUID()
{
  static const Standard_GUID ID("efd213e5-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : ViewRefShapeGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ViewRefShapeGUID()
{
  static const Standard_GUID ID("efd213e6-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : ViewRefGDTGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ViewRefGDTGUID()
{
  static const Standard_GUID ID("efd213e7-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : ViewRefPlaneGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ViewRefPlaneGUID()
{
  static const Standard_GUID ID("efd213e9-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}

//=======================================================================
//function : ViewRefPlaneGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ViewRefNoteGUID()
{
  static const Standard_GUID ID("C814ACC6-43AC-4812-9B2A-4E9A2A549354");
  return ID;
}

//=======================================================================
//function : ViewRefPlaneGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::ViewRefAnnotationGUID()
{
  static const Standard_GUID ID("A2B5BA42-DD00-43f5-8882-4B5F8E76B9D2");
  return ID;
}

//=======================================================================
//function : LockGUID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc::LockGUID()
{
  static const Standard_GUID ID("efd213eb-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}
