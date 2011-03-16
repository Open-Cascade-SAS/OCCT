// File:	Law_Constant.cxx
// Created:	Fri Mar 29 10:29:35 1996
// Author:	Laurent BOURESCHE
//		<lbo@phylox>

// pmn -> 17/01/1996 added : Continuity, (Nb)Interals, D2, Trim

#include <Law_Constant.ixx>

//=======================================================================
//function : Law_Constant
//purpose  : 
//=======================================================================

Law_Constant::Law_Constant()
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Law_Constant::Set(const Standard_Real Radius, 
		       const Standard_Real PFirst, 
		       const Standard_Real PLast)
{
  radius = Radius;
  first  = PFirst;
  last   = PLast;
}
//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Law_Constant::Continuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================
//Standard_Integer Law_Constant::NbIntervals(const GeomAbs_Shape S) const 
Standard_Integer Law_Constant::NbIntervals(const GeomAbs_Shape ) const 
{
   return 1;
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================
void Law_Constant::Intervals(TColStd_Array1OfReal& T,
//                             const GeomAbs_Shape S) const 
                             const GeomAbs_Shape ) const 
{
  T.SetValue(T.Lower(), first);
  T.SetValue(T.Upper(), last);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Law_Constant::Value(const Standard_Real)
{
  return radius;
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Law_Constant::D1(const Standard_Real, 
		      Standard_Real& F, 
		      Standard_Real& D)
{
  F = radius;
  D = 0.;
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Law_Constant::D2(const Standard_Real, 
		      Standard_Real& F, 
		      Standard_Real& D,
		      Standard_Real& D2)
{
  F = radius;
  D = D2 = 0.;
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Law_Function) Law_Constant::Trim(const Standard_Real PFirst, 
				      const Standard_Real PLast, 
//				      const Standard_Real Tol) const 
				      const Standard_Real ) const 
{
  Handle(Law_Constant) l = new (Law_Constant)();
  l->Set(radius, PFirst, PLast);
  return l;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Law_Constant::Bounds(Standard_Real& PFirst, 
			  Standard_Real& PLast)
{
  PFirst = first;
  PLast  = last;
}
