
#include <StepGeom_CurveOnSurface.ixx>
#include <Interface_Macros.hxx>

StepGeom_CurveOnSurface::StepGeom_CurveOnSurface () {  }

Standard_Integer StepGeom_CurveOnSurface::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Pcurve))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_SurfaceCurve))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_CompositeCurveOnSurface))) return 3;
	return 0;
}

Handle(StepGeom_Pcurve) StepGeom_CurveOnSurface::Pcurve () const
{
	return GetCasted(StepGeom_Pcurve,Value());
}

Handle(StepGeom_SurfaceCurve) StepGeom_CurveOnSurface::SurfaceCurve () const
{
	return GetCasted(StepGeom_SurfaceCurve,Value());
}

Handle(StepGeom_CompositeCurveOnSurface) StepGeom_CurveOnSurface::CompositeCurveOnSurface () const
{
	return GetCasted(StepGeom_CompositeCurveOnSurface,Value());
}
