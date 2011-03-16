// Copyright: 	Matra-Datavision 1996
// File:	StdSelect_EdgeFilter.cxx
// Created:	Wed Mar 13 16:52:25 1996
// Author:	Robert COUBLANC
//		<rob>



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
