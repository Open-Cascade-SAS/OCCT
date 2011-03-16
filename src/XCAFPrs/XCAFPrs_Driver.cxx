// File:	XCAFPrs_Driver.cxx
// Created:	Fri Aug 11 16:34:45 2000
// Author:	data exchange team
//		<det@doomox>

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
