// File:	BRepFill_CurveConstraint.cxx
// Created:	Fri Oct 31 10:54:42 1997
// Author:	Joelle CHAUVET
//		<jct@sgi64>


#include <BRepFill_CurveConstraint.ixx>
#include <GeomPlate_CurveConstraint.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <ProjLib_ProjectOnPlane.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAPI.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRep_Tool.hxx>

#include <Geom2dAdaptor_HCurve.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <ProjLib_ProjectedCurve.hxx>
//---------------------------------------------------------
//         Constructeurs avec courbe sur surface
//---------------------------------------------------------
BRepFill_CurveConstraint :: BRepFill_CurveConstraint (const Handle(Adaptor3d_HCurveOnSurface)& Boundary,
						const Standard_Integer Tang,
						const Standard_Integer NPt,
						const Standard_Real TolDist,
						const Standard_Real TolAng,
						const Standard_Real TolCurv
)
{ 
  myFrontiere=Boundary;
  myTolDist=TolDist;
  myTolAng=TolAng;
  myTolCurv=TolCurv;
  GeomLProp_SLProps SLP(2,TolDist);
  myLProp=SLP;
  myOrder=Tang;
  if ((Tang<-1)||(Tang>2))
    Standard_Failure::Raise("BRepFill : The continuity is not G0 G1 or G2"); 
  myNbPoints=NPt;
myConstG0=Standard_True;
myConstG1=Standard_True;
myConstG2=Standard_True;
 if (myFrontiere.IsNull())
    Standard_Failure::Raise("BRepFill_CurveConstraint : Curve must be on a Surface"); 
  Handle(Geom_Surface) Surf;
  Handle(GeomAdaptor_HSurface) GS1;
  GS1 = Handle(GeomAdaptor_HSurface)::DownCast(myFrontiere->ChangeCurve().GetSurface());
  if (!GS1.IsNull()) {
    Surf=GS1->ChangeSurface().Surface();
  }
  else {
    Handle(BRepAdaptor_HSurface) BS1;
    BS1=Handle(BRepAdaptor_HSurface)::DownCast(myFrontiere->ChangeCurve().GetSurface());
    Surf = BRep_Tool::Surface(BS1->ChangeSurface().Face());
  }
  myLProp.SetSurface(Surf);
}

//---------------------------------------------------------
//    Constructeurs avec courbe 3d (pour continuite G0 G-1)
//---------------------------------------------------------
BRepFill_CurveConstraint :: BRepFill_CurveConstraint (const Handle(Adaptor3d_HCurve)& Boundary,
						const Standard_Integer Tang, 
						const Standard_Integer NPt,
						const Standard_Real TolDist) 
: GeomPlate_CurveConstraint(Boundary,Tang,NPt,TolDist)

{ 
}





