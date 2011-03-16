// File:	ProjLib_Projector.cxx
// Created:	Tue Aug 24 12:05:40 1993
// Author:	Bruno DUMORTIER
//		<dub@topsn3>

//  Modified by skv - Wed Aug 11 15:45:58 2004 OCC6272


#include <ProjLib_Projector.ixx>

#include <ElCLib.hxx>


//=======================================================================
//function : ProjLib_Projector
//purpose  : 
//=======================================================================

ProjLib_Projector::ProjLib_Projector()
{}


//=======================================================================
void ProjLib_Projector::Delete()
{}



//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_Projector::IsDone() const
{
  return isDone;
}


//=======================================================================
//function : Done
//purpose  : 
//=======================================================================

void ProjLib_Projector::Done()
{
  isDone = Standard_True;
}


//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType  ProjLib_Projector::GetType() const 
{
  return myType;
}


//=======================================================================
//function : SetType
//purpose  : 
//=======================================================================

void ProjLib_Projector::SetType( const GeomAbs_CurveType Type)
{
  myType = Type;
}


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_Projector::IsPeriodic() const 
{
  return myIsPeriodic;
}


//=======================================================================
//function : SetPeriodic
//purpose  : 
//=======================================================================

void ProjLib_Projector::SetPeriodic()
{
  myIsPeriodic = Standard_True;
}


//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

const gp_Lin2d&  ProjLib_Projector::Line()const 
{
  if (myType != GeomAbs_Line)
    Standard_NoSuchObject::Raise("ProjLib_Projector::Line");
  return myLin;
}


//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

const gp_Circ2d&  ProjLib_Projector::Circle()const 
{
  if (myType != GeomAbs_Circle)
    Standard_NoSuchObject::Raise("ProjLib_Projector::Circle");
  return myCirc;
}


//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

const gp_Elips2d&  ProjLib_Projector::Ellipse()const 
{
  if (myType != GeomAbs_Ellipse)
    Standard_NoSuchObject::Raise("ProjLib_Projector::Ellipse");
  return myElips;
}


//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

const gp_Hypr2d&  ProjLib_Projector::Hyperbola()const 
{
  if (myType != GeomAbs_Hyperbola)
    Standard_NoSuchObject::Raise("ProjLib_Projector::Hyperbola");
  return myHypr;
}


//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

const gp_Parab2d&  ProjLib_Projector::Parabola()const 
{
  if (myType != GeomAbs_Parabola)
    Standard_NoSuchObject::Raise("ProjLib_Projector::Parabola");
  return myParab;
}




//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Projector::Project(const gp_Lin& )
{
  myType = GeomAbs_OtherCurve;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Projector::Project(const gp_Circ& )
{
  myType = GeomAbs_OtherCurve;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Projector::Project(const gp_Elips& )
{
  myType = GeomAbs_OtherCurve;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Projector::Project(const gp_Parab& )
{
  myType = GeomAbs_OtherCurve;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Projector::Project(const gp_Hypr& )
{
  myType = GeomAbs_OtherCurve;
}


//=======================================================================
//function : UFrame
//purpose  : 
//=======================================================================

void ProjLib_Projector::UFrame(const Standard_Real CFirst, 
//			       const Standard_Real CLast, 
			       const Standard_Real , 
			       const Standard_Real UFirst, 
			       const Standard_Real Period)
{
  if (myType == GeomAbs_Line) {
//  Modified by skv - Wed Aug 11 15:45:58 2004 OCC6272 Begin
//     if ( myLin.Direction().Y() == 0.) {
//  Modified by skv - Wed Aug 11 15:45:58 2004 OCC6272 End
//      gp_Pnt2d PFirst, PLast;
    gp_Pnt2d PFirst;
    PFirst = ElCLib::Value(CFirst,myLin);
    //PLast  = ElCLib::Value(CLast ,myLin);
    //Standard_Real U = Min( PFirst.X(), PLast.X());
    Standard_Real U = PFirst.X();
    Standard_Real NewU = ElCLib::InPeriod(U,UFirst, UFirst + Period);
    myLin.Translate(gp_Vec2d(NewU-U,0.));
  }
}

//=======================================================================
//function : VFrame
//purpose  : 
//=======================================================================

void ProjLib_Projector::VFrame(const Standard_Real CFirst,
//			       const Standard_Real CLast, 
			       const Standard_Real , 
			       const Standard_Real VFirst, 
			       const Standard_Real Period)
{
  if (myType == GeomAbs_Line) {
//  Modified by skv - Wed Aug 11 15:45:58 2004 OCC6272 Begin
//     if ( myLin.Direction().X() == 0.) {
//  Modified by skv - Wed Aug 11 15:45:58 2004 OCC6272 End
//      gp_Pnt2d PFirst, PLast;
    gp_Pnt2d PFirst;
    PFirst = ElCLib::Value(CFirst,myLin);
    //PLast  = ElCLib::Value(CLast ,myLin);
    //Standard_Real V = Min( PFirst.Y(), PLast.Y());
    Standard_Real V = PFirst.Y();
    Standard_Real NewV = ElCLib::InPeriod(V,VFirst, VFirst + Period);
    myLin.Translate(gp_Vec2d(0.,NewV-V));
  }
}

//=======================================================================
//function : SetBezier
//purpose  : 
//=======================================================================

void  ProjLib_Projector::SetBezier(const Handle(Geom2d_BezierCurve)&  C) 
{
 myBezier = C ;
}
//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom2d_BezierCurve)ProjLib_Projector::Bezier() const 
{ 
  return myBezier ;
}
//=======================================================================
//function : SetBSpline
//purpose  : 
//=======================================================================

void  ProjLib_Projector::SetBSpline(const Handle(Geom2d_BSplineCurve)&  C) 
{
 myBSpline = C ;
}
//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) ProjLib_Projector::BSpline()  const 
{ 
 return myBSpline ;
}
