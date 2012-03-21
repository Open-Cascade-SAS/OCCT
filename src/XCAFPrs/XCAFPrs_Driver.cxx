// Created on: 2000-08-11
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <XCAFPrs_Driver.ixx>
#include <XCAFPrs_AISObject.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>

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
