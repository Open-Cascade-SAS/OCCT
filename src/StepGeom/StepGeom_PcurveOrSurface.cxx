
#include <StepGeom_PcurveOrSurface.ixx>
#include <Interface_Macros.hxx>

StepGeom_PcurveOrSurface::StepGeom_PcurveOrSurface () {  }

Standard_Integer StepGeom_PcurveOrSurface::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Pcurve))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Surface))) return 2;
	return 0;
}

Handle(StepGeom_Pcurve) StepGeom_PcurveOrSurface::Pcurve () const
{
	return GetCasted(StepGeom_Pcurve,Value());
}

Handle(StepGeom_Surface) StepGeom_PcurveOrSurface::Surface () const
{
	return GetCasted(StepGeom_Surface,Value());
}
