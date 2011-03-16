// File:	Approx_SweepFunction.cxx
// Created:	Wed Jun 25 16:54:26 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <Approx_SweepFunction.ixx>

// Standard_Boolean Approx_SweepFunction::D1(const Standard_Real Param,const Standard_Real First,const Standard_Real Last,TColgp_Array1OfPnt& Poles,TColgp_Array1OfVec& DPoles,TColgp_Array1OfPnt2d& Poles2d,TColgp_Array1OfVec2d& DPoles2d,TColStd_Array1OfReal& Weigths,TColStd_Array1OfReal& DWeigths) 
 Standard_Boolean Approx_SweepFunction::D1(const Standard_Real ,const Standard_Real ,const Standard_Real ,TColgp_Array1OfPnt& ,TColgp_Array1OfVec& ,TColgp_Array1OfPnt2d& ,TColgp_Array1OfVec2d& ,TColStd_Array1OfReal& ,TColStd_Array1OfReal& ) 
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::D1");
 return Standard_False;
}

// Standard_Boolean Approx_SweepFunction::D2(const Standard_Real Param,const Standard_Real First,const Standard_Real Last,TColgp_Array1OfPnt& Poles,TColgp_Array1OfVec& DPoles,TColgp_Array1OfVec& D2Poles,TColgp_Array1OfPnt2d& Poles2d,TColgp_Array1OfVec2d& DPoles2d,TColgp_Array1OfVec2d& D2Poles2d,TColStd_Array1OfReal& Weigths,TColStd_Array1OfReal& DWeigths,TColStd_Array1OfReal& D2Weigths) 
 Standard_Boolean Approx_SweepFunction::D2(const Standard_Real ,const Standard_Real ,const Standard_Real ,TColgp_Array1OfPnt& ,TColgp_Array1OfVec& ,TColgp_Array1OfVec& ,TColgp_Array1OfPnt2d& ,TColgp_Array1OfVec2d& ,TColgp_Array1OfVec2d& ,TColStd_Array1OfReal& ,TColStd_Array1OfReal& ,TColStd_Array1OfReal& ) 
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::D2");
 return Standard_False;
}

// void Approx_SweepFunction::Resolution(const Standard_Integer Index,const Standard_Real Tol,Standard_Real& TolU,Standard_Real& TolV) const
 void Approx_SweepFunction::Resolution(const Standard_Integer ,const Standard_Real ,Standard_Real& ,Standard_Real& ) const
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::Resolution");  
}

 gp_Pnt Approx_SweepFunction::BarycentreOfSurf() const
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::BarycentreOfSurf");
 return gp_Pnt(0.,0.,0.);
}

 Standard_Real Approx_SweepFunction::MaximalSection() const
{
  Standard_NotImplemented::Raise("Approx_SweepFunction::MaximalSection()");
  return 0;
}

// void Approx_SweepFunction::GetMinimalWeight(TColStd_Array1OfReal& Weigths) const
 void Approx_SweepFunction::GetMinimalWeight(TColStd_Array1OfReal& ) const
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::GetMinimalWeight");
}
