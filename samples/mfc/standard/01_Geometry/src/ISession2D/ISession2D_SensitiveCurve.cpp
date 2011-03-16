// Copyright: 	Matra-Datavision 1995

#include "stdafx.h"

#include <ISession2D_SensitiveCurve.h>

IMPLEMENT_STANDARD_HANDLE(ISession2D_SensitiveCurve,Select2D_SensitiveEntity)
IMPLEMENT_STANDARD_RTTIEXT(ISession2D_SensitiveCurve,Select2D_SensitiveEntity)

#include <Bnd_Box2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <SelectBasics_BasicTool.hxx>
#include "GCPnts_TangentialDeflection.hxx"
#include "Geom2dAdaptor_Curve.hxx"

//=====================================================
// Function : Create
// Purpose  :Constructor
//=====================================================


ISession2D_SensitiveCurve::
ISession2D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                        const Handle(Geom2d_Curve)& C,
                        const Standard_Real CDeflect,
                        const Standard_Integer MaxRect):
Select2D_SensitiveEntity(OwnerId),
myMaxRect(MaxRect),
myCurve(C),
myCDeflect(CDeflect)
{
  Compute();
}

void     ISession2D_SensitiveCurve::Compute()
{
   Geom2dAdaptor_Curve Curve (myCurve);
   Standard_Real ADeflect = 180; //Angular deflection
    
   GCPnts_TangentialDeflection PointsOnCurve;
   PointsOnCurve.Initialize (Curve, ADeflect, myCDeflect,myMaxRect,1.0e-9); 


   myPolyP2d = new TColgp_HArray1OfPnt2d(1,PointsOnCurve.NbPoints());

   gp_Pnt P; 
   for (Standard_Integer i=1; i<=PointsOnCurve.NbPoints();i++) {
   P = PointsOnCurve.Value (i);
   myPolyP2d->SetValue(i,gp_Pnt2d(P.X(),P.Y()));
   }

}


//=====================================================
// Function : Areas 
// Purpose  :  return the bounding boxes
//=====================================================
void  ISession2D_SensitiveCurve::Areas(SelectBasics_ListOfBox2d& boxes) 
{ 
    // calcul des Areas --> le nombre voulu est myMaxRect
    //  mais il y a myPolyP2d->Length() segments

  Standard_Integer NbSeg = myPolyP2d->Length()-1;
  Standard_Integer nbPerBoxes= NbSeg/myMaxRect;

  Standard_Integer CurrentPoint =1;
  for (Standard_Integer i=1;i<=myMaxRect-1;i++)
  { 
    Bnd_Box2d abox;
    abox.Set(myPolyP2d->Value(CurrentPoint));
    for(Standard_Integer j=1;j<=nbPerBoxes;j++)
    {
      CurrentPoint++;
      abox.Add(myPolyP2d->Value(CurrentPoint));
    }
   boxes.Append(abox);
  }
    Bnd_Box2d abox;
    abox.Set(myPolyP2d->Value(CurrentPoint));
    for(Standard_Integer j=CurrentPoint;j<=myPolyP2d->Length()-1;j++)
    {
      CurrentPoint++;
      abox.Add(myPolyP2d->Value(CurrentPoint));
    }
   boxes.Append(abox);

} 

//=======================================================
// Function : Matches 
// Purpose  : test : segments in the bounding boxe
//=======================================================
Standard_Boolean ISession2D_SensitiveCurve::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{

  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);
  
  for(Standard_Integer j=1;j<=myPolyP2d->Length()-1;j++)
    {
      if(BoundBox.IsOut(myPolyP2d->Value(j))) return Standard_False;
    }
  return Standard_True;

}

//====================================================
// Function : Matches 
// Purpose  : test the real dist to the segments
//====================================================
Standard_Boolean ISession2D_SensitiveCurve::
  Matches(const Standard_Real X,
        const Standard_Real Y,
        const Standard_Real aTol,
        Standard_Real&  DMin)
{
  // VERY VERY IMPORTANT : set the selector sensibility !!! ( it's aTol !! )
  Standard_Integer Rank=0; // New in 2.0
  if (aTol == 0) {TRACE0("VERY VERY IMPORTANT : set the selector sensibility !!! ( it's aTol !! )");}

  Standard_Boolean Result =  SelectBasics_BasicTool::MatchPolyg2d(myPolyP2d->Array1(),
                                                X,Y,
                                                aTol,
                                                DMin,
												Rank); // new in 2.0
  return Result;
}


Handle(TColgp_HArray1OfPnt2d) ISession2D_SensitiveCurve::
SensitivePolygon()
{
  return myPolyP2d;
}


