// Copyright: 	Matra-Datavision 1995
// File:	Select2D_SensitiveSegment.cxx
// Created:	Thu Jan 26 11:50:08 1995
// Author:	Mister rmi
//		<rmi>

#include <Select2D_SensitiveSegment.ixx>
#include <Bnd_Box2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <SelectBasics_BasicTool.hxx>

//=====================================================
// Function : Create
// Purpose  :Constructor
//=====================================================


Select2D_SensitiveSegment::
Select2D_SensitiveSegment(const Handle(SelectBasics_EntityOwner)& OwnerId,
			       const gp_Pnt2d& FirstP,
			       const gp_Pnt2d& LastP,
			       const Standard_Integer MaxRect):
Select2D_SensitiveEntity(OwnerId),
mymaxrect(MaxRect),
mystart(FirstP),
myend(LastP)
{}


//=====================================================
// Function : Areas 
// Purpose  :
//=====================================================
void  Select2D_SensitiveSegment::
Areas(SelectBasics_ListOfBox2d& boxes) 
{
  // look for the angle with X axis if x % pi/4 <pi/12 -> one box
  //gp_Dir2d dy (0.,1.);
  
  gp_Vec2d MyVec(mystart,myend),VAxx(gp_Dir2d(0.,1.));

  
  Standard_Real theangle = (VAxx.Angle(MyVec));
  theangle = Abs(theangle);
  if(theangle>=PI/2.) theangle-=PI/2;

  
  if(theangle>=PI/12. && theangle <=5*PI/12.)
    {
      TColgp_Array1OfPnt2d BoxPoint (1,mymaxrect+1);
      BoxPoint (1) = mystart;BoxPoint(mymaxrect+1)=myend;
      gp_Vec2d Vtr = MyVec/mymaxrect;
//      for (Standard_Integer i=2;i<=mymaxrect;i++)
      Standard_Integer i;
      for ( i=2;i<=mymaxrect;i++)
	{BoxPoint (i) = BoxPoint (i-1).Translated(Vtr);}
      for (i=2;i<=mymaxrect+1;i++)
	{ Bnd_Box2d curbox;
	  curbox.Set(BoxPoint(i-1));
	  curbox.Add(BoxPoint(i));
	  boxes.Append(curbox);
	}
    }
  else
    {
      Bnd_Box2d curbox;
      curbox.Set(mystart);
      curbox.Add(myend);
      boxes.Append(curbox);
    }
}


//=====================================================
// Function :Matches
// Purpose  :
//=====================================================
 Standard_Boolean Select2D_SensitiveSegment::
Matches(const Standard_Real X,
	const Standard_Real Y,
	const Standard_Real aTol,
	Standard_Real&  DMin)
{
//  gp_Pnt2d pickpoint(X,Y);
//  gp_Vec2d V(mystart,myend);
//  gp_Lin2d Lin(mystart,V);
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;
  DMin=0.0;
  if(mystart.Distance(myend)<=TheTol) DMin = Min(mystart.Distance(gp_Pnt2d(X,Y)),
					       myend.Distance(gp_Pnt2d(X,Y)));
  else 
    DMin =Abs (gp_Lin2d(mystart,gp_Vec2d(mystart,myend)).Distance(gp_Pnt2d(X,Y)));
  
  if(DMin<=TheTol) return Standard_True;
  
  return Standard_False;
  
}

Standard_Boolean Select2D_SensitiveSegment::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-TheTol,YMin-TheTol,XMax+TheTol,YMax+TheTol);
  
  if (BoundBox.IsOut(mystart)&&BoundBox.IsOut(myend)) return Standard_False;
  return Standard_True;
}

