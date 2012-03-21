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



#include <XCAFDoc_ShapeMapTool.ixx>
#include <TopoDS_Iterator.hxx>

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
