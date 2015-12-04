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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_ShapeMapTool,TDF_Attribute)

#define AUTONAMING // automatically set names for labels

// attribute methods //////////////////////////////////////////////////

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_ShapeMapTool::GetID() 
{
  static Standard_GUID ShapeToolID ("3B913F4D-4A82-44ef-A0BF-9E01E9FF317A");
  return ShapeToolID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_ShapeMapTool) XCAFDoc_ShapeMapTool::Set(const TDF_Label& L) 
{
  Handle(XCAFDoc_ShapeMapTool) A;
  if (!L.FindAttribute (XCAFDoc_ShapeMapTool::GetID(), A)) {
    A = new XCAFDoc_ShapeMapTool ();
    L.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_ShapeMapTool::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_ShapeMapTool::Restore(const Handle(TDF_Attribute)& /*with*/) 
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_ShapeMapTool::NewEmpty() const
{
  return new XCAFDoc_ShapeMapTool;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_ShapeMapTool::Paste (const Handle(TDF_Attribute)& /*into*/,
                                  const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_ShapeMapTool::XCAFDoc_ShapeMapTool()
{
}


//=======================================================================
//function : IsSubShape
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ShapeMapTool::IsSubShape(const TopoDS_Shape& sub) const
{
  return myMap.Contains(sub);
}


//=======================================================================
//function : SetShape
//purpose  : auxilary
//=======================================================================
static void AddSubShape(const TopoDS_Shape& S,
                        TopTools_IndexedMapOfShape& myMap) 
{
  myMap.Add(S);
  for ( TopoDS_Iterator it(S); it.More(); it.Next() )
    AddSubShape(it.Value(),myMap);
}


//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================

void XCAFDoc_ShapeMapTool::SetShape(const TopoDS_Shape& S)
{
  myMap.Clear();
  for ( TopoDS_Iterator it(S); it.More(); it.Next() )
    AddSubShape(it.Value(),myMap);
  
}


//=======================================================================
//function : GetMap
//purpose  : 
//=======================================================================

const TopTools_IndexedMapOfShape& XCAFDoc_ShapeMapTool::GetMap() const
{
  return myMap;
}
