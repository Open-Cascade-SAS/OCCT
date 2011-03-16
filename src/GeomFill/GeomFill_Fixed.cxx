// File:	GeomFill_Fixed.cxx
// Created:	Wed Dec 10 10:26:36 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomFill_Fixed.ixx>
#include <Precision.hxx>

GeomFill_Fixed::GeomFill_Fixed(const gp_Vec& Tangent,
			       const gp_Vec& Normal)
{
  if (Tangent.IsParallel(Normal, 0.01) ) 
    Standard_ConstructionError::Raise(
      "GeomFill_Fixed : Two parallel vectors !");
  T = Tangent;
  T.Normalize();
  N = Normal;
  N.Normalize();
  B = T ^ N;
  B.Normalize();
}

Handle(GeomFill_TrihedronLaw) GeomFill_Fixed::Copy() const
{
 Handle(GeomFill_Fixed) copy = new (GeomFill_Fixed)(T, N);
 copy->SetCurve(myCurve);
 return copy;
} 

 Standard_Boolean GeomFill_Fixed::D0(const Standard_Real, 
						 gp_Vec& Tangent,
						 gp_Vec& Normal,
						 gp_Vec& BiNormal) 
{
  Tangent = T;
  Normal = N;
  BiNormal = B;

  return Standard_True; 
}

 Standard_Boolean GeomFill_Fixed::D1(const Standard_Real,
						 gp_Vec& Tangent,
						 gp_Vec& DTangent,
						 gp_Vec& Normal,
						 gp_Vec& DNormal,
						 gp_Vec& BiNormal,
						 gp_Vec& DBiNormal) 
{
  Tangent = T;
  Normal = N;
  BiNormal = B;

  gp_Vec V0(0,0,0);
  DTangent = DNormal = DBiNormal = V0;

  return Standard_True; 
}

 Standard_Boolean GeomFill_Fixed::D2(const Standard_Real,
						 gp_Vec& Tangent,
						 gp_Vec& DTangent,
						 gp_Vec& D2Tangent,
						 gp_Vec& Normal,
						 gp_Vec& DNormal,
						 gp_Vec& D2Normal,
						 gp_Vec& BiNormal,
						 gp_Vec& DBiNormal,
						 gp_Vec& D2BiNormal) 
{
  Tangent = T;
  Normal = N;
  BiNormal = B;

  gp_Vec V0(0,0,0);
  DTangent = D2Tangent = V0;
  DNormal = D2Normal = V0;
  DBiNormal = D2BiNormal = V0;

  return Standard_True; 
}

 Standard_Integer GeomFill_Fixed::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

 void GeomFill_Fixed::Intervals(TColStd_Array1OfReal& T,
					    const GeomAbs_Shape) const
{
  T(T.Lower()) = - Precision::Infinite();
  T(T.Upper()) =   Precision::Infinite();
}

 void GeomFill_Fixed::GetAverageLaw(gp_Vec& ATangent,
				    gp_Vec& ANormal,
				    gp_Vec& ABiNormal) 
{
   ATangent = T;
   ANormal = N;
   ABiNormal = B;
}

 Standard_Boolean GeomFill_Fixed::IsConstant() const
{
  return Standard_True;
}
