// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <XCAFDoc.hxx>
#include <XCAFDoc_ColorType.hxx>

#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>


//=======================================================================
//function : ShapeRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::ShapeRefGUID ()
{
  static Standard_GUID ID ("5b896afe-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : AssemblyGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::AssemblyGUID ()
{
  static Standard_GUID ID ("5b896b00-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : ExternRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::ExternRefGUID ()
{
  static Standard_GUID ID ("6b896b01-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : ColorRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::ColorRefGUID (const XCAFDoc_ColorType type)
{
  static Standard_GUID IDcol     ("efd212e4-6dfd-11d4-b9c8-0060b0ee281b");
  static Standard_GUID IDcolSurf ("efd212e5-6dfd-11d4-b9c8-0060b0ee281b");
  static Standard_GUID IDcolCurv ("efd212e6-6dfd-11d4-b9c8-0060b0ee281b");

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

Standard_GUID XCAFDoc::DimTolRefGUID()
{
  //static Standard_GUID IDDimTol("58ed092d-44de-11d8-8776-001083004c77");
  static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  //return IDDimTol;
  return ID;
}


//=======================================================================
//function : DatumRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::DatumRefGUID()
{
  static Standard_GUID ID("efd212e2-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : DatumTolRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::DatumTolRefGUID()
{
  //static Standard_GUID IDDimTol("58ed092d-44de-11d8-8776-001083004c77");
  static Standard_GUID ID("efd212e7-6dfd-11d4-b9c8-0060b0ee281b");
  //return IDDimTol;
  return ID;
}


//=======================================================================
//function : LayerRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::LayerRefGUID ()
{
  static Standard_GUID ID ("efd212e8-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : MaterialRefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::MaterialRefGUID ()
{
  static Standard_GUID ID ("efd212f7-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : InvisibleGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::InvisibleGUID ()
{
  static Standard_GUID ID ("5b896aff-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}


//=======================================================================
//function : SHUORefGUID
//purpose  : 
//=======================================================================

Standard_GUID XCAFDoc::SHUORefGUID ()
{
  static Standard_GUID ID ("efd212ea-6dfd-11d4-b9c8-0060b0ee281b");
  return ID;
}
