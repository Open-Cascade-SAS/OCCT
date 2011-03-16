#include <ISession2D_SensitiveCurve.ixx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <gp_Pnt.hxx>
#include <Bnd_Box2d.hxx>
#include <SelectBasics_BasicTool.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <SelectBasics_ListOfBox2d.hxx>


ISession2D_SensitiveCurve::ISession2D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,const Handle(Geom2d_Curve)& C,const Standard_Real CDeflect,const Standard_Integer MaxRect)
     :Select2D_SensitiveEntity(OwnerId),
       myCDeflect(CDeflect),
       myMaxRect(MaxRect),
       myCurve(C)
{
  Compute();
}

 void ISession2D_SensitiveCurve::Compute() 
{
  Geom2dAdaptor_Curve Curve (myCurve);
  Standard_Real ADeflect = 180; //Angular deflection
  
  GCPnts_TangentialDeflection PointsOnCurve;
  PointsOnCurve.Initialize (Curve, ADeflect, myCDeflect,myMaxRect,1.0e-9); 
  
  
  myPolyP2d = new TColgp_HArray1OfPnt2d(1,PointsOnCurve.NbPoints());
  
  gp_Pnt P; 
  for (Standard_Integer i=1; i<=PointsOnCurve.NbPoints();i++) 
    {
      P = PointsOnCurve.Value (i);
      myPolyP2d->SetValue(i,gp_Pnt2d(P.X(),P.Y()));
    }
}

 void ISession2D_SensitiveCurve::Areas(SelectBasics_ListOfBox2d& aSeq) 
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
      aSeq.Append(abox);
    }
  Bnd_Box2d abox;
  abox.Set(myPolyP2d->Value(CurrentPoint));
  for(Standard_Integer j=CurrentPoint;j<=myPolyP2d->Length()-1;j++)
    {
      CurrentPoint++;
      abox.Add(myPolyP2d->Value(CurrentPoint));
    }
  aSeq.Append(abox);
}

 Standard_Boolean ISession2D_SensitiveCurve::Matches(const Standard_Real XMin,const Standard_Real YMin,const Standard_Real XMax,const Standard_Real YMax,const Standard_Real aTol) 
{
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);

  for(Standard_Integer j=1;j<=myPolyP2d->Length()-1;j++)
  {
    if(BoundBox.IsOut(myPolyP2d->Value(j))) return Standard_False;
  }
  return Standard_True;
}

 Standard_Boolean ISession2D_SensitiveCurve::Matches(const Standard_Real X,const Standard_Real Y,const Standard_Real aTol,Standard_Real& DMin) 
{
  // VERY VERY IMPORTANT : set the selector sensibility !!! ( it's aTol !! )
  
  Standard_Integer Rank = 0;
  Standard_Boolean Result =  SelectBasics_BasicTool::MatchPolyg2d(myPolyP2d->Array1(),
								  X,Y,
								  aTol,
								  DMin, 
								  Rank);
  

  return Result;
}

 Handle(TColgp_HArray1OfPnt2d) ISession2D_SensitiveCurve::SensitivePolygon() 
{
  return myPolyP2d;
}

