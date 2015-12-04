// Created on: 2000-08-11
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <AIS_InteractiveObject.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <XCAFPrs_Driver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFPrs_Driver,TPrsStd_Driver)

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
Standard_Boolean XCAFPrs_Driver::Update (const TDF_Label& L,
					 Handle(AIS_InteractiveObject)& ais)

{
//  cout << "XCAFPrs_Driver::Update" << endl;
// WARNING! The label L can be out of any document 
// (this is a case for reading from the file)
//  Handle(TDocStd_Document) DOC = TDocStd_Document::Get(L);

  XCAFDoc_ShapeTool shapes;
  if ( ! shapes.IsShape(L) ) return Standard_False;
  
  ais = new XCAFPrs_AISObject (L);
  
  return Standard_True;
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFPrs_Driver::GetID()
{
  static Standard_GUID ID("5b896afc-3adf-11d4-b9b7-0060b0ee281b");
  return ID;
}
