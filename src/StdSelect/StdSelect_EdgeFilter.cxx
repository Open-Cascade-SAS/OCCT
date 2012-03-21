// Created on: 1996-03-13
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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




#include <StdSelect_EdgeFilter.ixx>
#include <BRepAdaptor_Curve.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <StdSelect_BRepOwner.hxx>
/*#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
*/

//==================================================
// Function: 
// Purpose :
//==================================================
StdSelect_EdgeFilter
::StdSelect_EdgeFilter (const StdSelect_TypeOfEdge Edge):
mytype(Edge){}

//==================================================
// Function: 
// Purpose :
//==================================================
void StdSelect_EdgeFilter
::SetType(const StdSelect_TypeOfEdge aNewType)
{
  mytype=aNewType;
}



//==================================================
// Function: 
// Purpose :
//==================================================
StdSelect_TypeOfEdge StdSelect_EdgeFilter::Type() const 
{
  return mytype;
}

//==================================================
// Function: 
// Purpose :
//==================================================
Standard_Boolean StdSelect_EdgeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO) const 
{
  if (Handle(StdSelect_BRepOwner)::DownCast(EO).IsNull()) return Standard_False;
  
  const TopoDS_Shape& sh = ((Handle(StdSelect_BRepOwner)&)EO)->Shape();
  if(sh.ShapeType()!= TopAbs_EDGE) return Standard_False;
  
  switch(mytype){
  case StdSelect_AnyEdge:
      return Standard_True;
  case StdSelect_Line:
    {
      BRepAdaptor_Curve curv(TopoDS::Edge(sh));
      return (curv.GetType() == GeomAbs_Line);
    }
    break;
  case StdSelect_Circle:
    BRepAdaptor_Curve curv(TopoDS::Edge(sh));
    return (curv.GetType() == GeomAbs_Circle);
  }
  
  return Standard_False ;
}

Standard_Boolean StdSelect_EdgeFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const 
{return aStandardMode==TopAbs_EDGE;}
