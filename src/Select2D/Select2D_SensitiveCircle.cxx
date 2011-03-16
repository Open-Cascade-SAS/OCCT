// Copyright: 	Matra-Datavision 1995
// File:	Select2D_SensitiveCircle.cxx
// Created:	Mon Jan 30 16:58:02 1995
// Author:	Mister rmi
//		<rmi>


#include <Select2D_SensitiveCircle.ixx>
#include <BndLib.hxx>
#include <Bnd_Box2d.hxx>

//=====================================================
// Function : Create
// Purpose  :Constructor
//=====================================================


Select2D_SensitiveCircle::
Select2D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
			 const gp_Circ2d& aCirc,
			 const Select2D_TypeOfSelection atype):
Select2D_SensitiveEntity(OwnerId),
myCirc(aCirc),
mytype(atype){}


//=====================================================
// Function : Areas
// Purpose  :
//=====================================================

void Select2D_SensitiveCircle::
Areas (SelectBasics_ListOfBox2d& boxes) 
{
  Bnd_Box2d abox;
  BndLib::Add(myCirc,myCirc.Radius()/100.,abox);
  boxes.Append(abox);
}


//=====================================================
// Function : Matches
// Purpose  :
//=====================================================
Standard_Boolean Select2D_SensitiveCircle::
Matches (const Standard_Real X,
	 const Standard_Real Y,
	 const Standard_Real aTol,
	 Standard_Real& DMin)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  switch(mytype){
  case Select2D_TOS_INTERIOR:
    {
      if(myCirc.Contains(gp_Pnt2d(X,Y),TheTol) )
	{DMin=0.;
	 return Standard_True;}
      else 
	{DMin=myCirc.Distance(gp_Pnt2d(X,Y));}
    }
  case Select2D_TOS_BOUNDARY:
    {
      DMin = myCirc.Distance(gp_Pnt2d(X,Y));
      if(DMin<= TheTol) return Standard_True;
      
    }
  }  
  return Standard_False;
}

Standard_Boolean Select2D_SensitiveCircle::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  Bnd_Box2d abox, BoundBox;
  BoundBox.Update(XMin-TheTol,YMin-TheTol,XMax+TheTol,YMax+TheTol);
  BndLib::Add(myCirc,myCirc.Radius()/100.,abox);

  if(BoundBox.IsOut(abox)) return Standard_False;
 return Standard_True;
}
 


