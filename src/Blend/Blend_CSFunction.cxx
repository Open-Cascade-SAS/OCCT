#include <Blend_CSFunction.ixx>
#include <Standard_NotImplemented.hxx>

Standard_Integer Blend_CSFunction::NbVariables () const
{
  return 3;
}

const gp_Pnt& Blend_CSFunction::Pnt1() const 
{
  return PointOnC();
}

const gp_Pnt& Blend_CSFunction::Pnt2() const 
{
  return PointOnS();
}

Standard_Boolean Blend_CSFunction::Section(const Blend_Point& P, 
					   TColgp_Array1OfPnt& Poles, 
					   TColgp_Array1OfVec& DPoles, 
					   TColgp_Array1OfVec& D2Poles, 
					   TColgp_Array1OfPnt2d& Poles2d, 
					   TColgp_Array1OfVec2d& DPoles2d, 
					   TColgp_Array1OfVec2d& D2Poles2d, 
					   TColStd_Array1OfReal& Weigths, 
					   TColStd_Array1OfReal& DWeigths, 
					   TColStd_Array1OfReal& D2Weigths)
{
  return Standard_False;
}

Standard_Real Blend_CSFunction::GetMinimalDistance() const
{
  Standard_NotImplemented::Raise("Blend_CSFunction::GetMinimalDistance");
  return RealLast();
}
