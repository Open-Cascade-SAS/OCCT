//Copyright:	Matra Datavision 1996
// File:	StdSelect_ShapeTypeFilter.cxx
// Created:	Tue Sep 17 10:02:44 1996
// Author:	Odile Olivier
//		<g_design@robox.paris1.matra-dtv.fr>


#include <StdSelect_ShapeTypeFilter.ixx>
#include <StdSelect_BRepOwner.hxx>


//==================================================
// Function: StdSelect_ShapeTypeFilter
// Purpose : Constructeur
//==================================================

StdSelect_ShapeTypeFilter::StdSelect_ShapeTypeFilter(const TopAbs_ShapeEnum aType):
myType(aType){}


//==================================================
// Function: IsOk
// Purpose : Renvoie True si la shape est du type defini a la construction
//==================================================

Standard_Boolean StdSelect_ShapeTypeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO)  const 
{ 
  Handle(StdSelect_BRepOwner) BRO = Handle(StdSelect_BRepOwner)::DownCast( EO );
  if( BRO.IsNull() || !BRO->HasShape() ) return Standard_False;
  const TopoDS_Shape& anobj= BRO->Shape();
  return anobj.ShapeType() == myType;
}
Standard_Boolean StdSelect_ShapeTypeFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const 
{return aStandardMode==myType;}
