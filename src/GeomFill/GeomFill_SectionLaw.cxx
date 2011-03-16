// File:	GeomFill_SectionLaw.cxx
// Created:	Fri Nov 21 15:19:07 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomFill_SectionLaw.ixx>

 Standard_Boolean GeomFill_SectionLaw::D1(const Standard_Real,TColgp_Array1OfPnt&,TColgp_Array1OfVec&,TColStd_Array1OfReal&,TColStd_Array1OfReal& ) 
{
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::D1");
  return 0;
}

 Standard_Boolean GeomFill_SectionLaw::D2(const Standard_Real,TColgp_Array1OfPnt& ,TColgp_Array1OfVec&,TColgp_Array1OfVec&,TColStd_Array1OfReal&,TColStd_Array1OfReal&,TColStd_Array1OfReal&) 
{
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::D2");
  return 0;  
}

 Handle(Geom_BSplineSurface) GeomFill_SectionLaw::BSplineSurface() const
{
 Handle(Geom_BSplineSurface) BS;
 BS.Nullify();
 return BS;
}

 void GeomFill_SectionLaw::SetTolerance(const Standard_Real ,const Standard_Real) 
{
  //Ne fait Rien
}

 gp_Pnt GeomFill_SectionLaw::BarycentreOfSurf() const
{

  Standard_NotImplemented::Raise("GeomFill_SectionLaw::BarycentreOfSurf");
    return gp_Pnt(0.0, 0.0, 0.0);
}

 void GeomFill_SectionLaw::GetMinimalWeight(TColStd_Array1OfReal&) const
{
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::GetMinimalWeight");
}

 Standard_Boolean GeomFill_SectionLaw::IsConstant(Standard_Real& Error) const
{
  Error = 0.;
  return Standard_False;
}

 Handle(Geom_Curve)  GeomFill_SectionLaw::ConstantSection() const
{
 Handle(Geom_Curve) C;
 Standard_DomainError::Raise("GeomFill_SectionLaw::ConstantSection");
 return C;
}

 Standard_Boolean GeomFill_SectionLaw::IsConicalLaw(Standard_Real& Error) const
{
  Error = 0.;
  return Standard_False;
}

 Handle(Geom_Curve) GeomFill_SectionLaw::
 CirclSection(const Standard_Real) const
{
 Handle(Geom_Curve) C;
 Standard_DomainError::Raise("GeomFill_SectionLaw::CirclSection");
 return C;
}
