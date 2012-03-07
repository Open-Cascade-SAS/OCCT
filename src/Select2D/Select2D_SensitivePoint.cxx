// Copyright: 	Matra- ()Datavision 1995
// File:	Select2D_SensitivePoint.cxx
// Created:	Thu Jan 26 16:37:33 1995
// Author:	Mister rmi
//		<rmi>

#include <Select2D_SensitivePoint.ixx>
#include <Bnd_Box2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Lin2d.hxx>

//==================================
//function : Constructor
//purpose  : 
//==================================

Select2D_SensitivePoint::
Select2D_SensitivePoint(const Handle(SelectBasics_EntityOwner)& OwnerId,
			const gp_Pnt2d& Location,
			const Standard_Real InitSensitivity):
Select2D_SensitiveEntity(OwnerId),
mylocation(Location),
mysensitivity(InitSensitivity)
{}


//==================================
//function : Areas
//purpose  : 
//==================================

void Select2D_SensitivePoint::
Areas (SelectBasics_ListOfBox2d& boxes)
{ Bnd_Box2d abox;
 abox.Set(mylocation);
 abox.Enlarge(mysensitivity);
 boxes.Append(abox);
}
 
//==================================
//function : Matches
//purpose  : 
//==================================


Standard_Boolean Select2D_SensitivePoint::
Matches (const Standard_Real X,
	 const Standard_Real Y,
	 const Standard_Real aTol,
	 Standard_Real& MinDist)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;
  MinDist = mylocation.Distance(gp_Pnt2d(X,Y));
  if(MinDist<=TheTol+mysensitivity) return Standard_True;
  return Standard_False;
}

Standard_Boolean Select2D_SensitivePoint::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{//distance point-Line....
  Standard_Real MinDist = gp_Lin2d(gp_Pnt2d(XMin,YMin),
		     gp_Vec2d(gp_Pnt2d(XMin,YMin),
			      gp_Pnt2d(XMax,YMax))
		     ).Distance(mylocation);
  
  return (MinDist<=aTol+mysensitivity);
}




