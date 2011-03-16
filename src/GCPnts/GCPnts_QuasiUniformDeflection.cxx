#include <GCPnts_QuasiUniformDeflection.ixx>
#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>
//
// mask the return of a Adaptor2d_Curve2d as a gp_Pnt 
//
static gp_Pnt Value(const Adaptor3d_Curve & C,
		    const Standard_Real Parameter) 
{
  return C.Value(Parameter) ;
}
static gp_Pnt Value(const Adaptor2d_Curve2d & C,
		    const Standard_Real Parameter) 
{
  gp_Pnt aPoint ;
  gp_Pnt2d a2dPoint(C.Value(Parameter));
  aPoint.SetCoord(a2dPoint.X(),a2dPoint.Y(),0.);
  return aPoint ;
}

static void D1(const Adaptor3d_Curve & C,
	       const Standard_Real Parameter,
	       gp_Pnt& P,
	       gp_Vec& V) 
{
  C.D1(Parameter,P,V);
}

static void D1(const Adaptor2d_Curve2d & C,
	       const Standard_Real Parameter,
	       gp_Pnt& P,
	       gp_Vec& V) 
{
  gp_Pnt2d a2dPoint;
  gp_Vec2d a2dVec;
  C.D1(Parameter,a2dPoint,a2dVec);
  P.SetCoord(a2dPoint.X(),a2dPoint.Y(),0.);
  V.SetCoord(a2dVec.X(),a2dVec.Y(),0.);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt GCPnts_QuasiUniformDeflection::Value
                      (const Standard_Integer Index) const
{ 
  StdFail_NotDone_Raise_if(!myDone, 
			 "GCPnts_QuasiUniformAbscissa::Parameter()");
  return myPoints.Value(Index) ;
}
//=======================================================================
//function : GCPnts_QuasiUniformDeflection
//purpose  : 
//=======================================================================

GCPnts_QuasiUniformDeflection::GCPnts_QuasiUniformDeflection ()
:myDone(Standard_False),myCont(GeomAbs_C1)
{
} 

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#define TheCurve                 Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)

#include <GCPnts_QuasiUniformDeflection.gxx>

#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#define TheCurve                 Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)

#include <GCPnts_QuasiUniformDeflection.gxx>






