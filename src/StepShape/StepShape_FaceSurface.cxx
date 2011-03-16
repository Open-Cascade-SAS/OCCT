#include <StepShape_FaceSurface.ixx>


StepShape_FaceSurface::StepShape_FaceSurface ()  {}

void StepShape_FaceSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFaceBound)& aBounds)
{

	StepShape_Face::Init(aName, aBounds);
}

void StepShape_FaceSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFaceBound)& aBounds,
	const Handle(StepGeom_Surface)& aFaceGeometry,
	const Standard_Boolean aSameSense)
{
	// --- classe own fields ---
	faceGeometry = aFaceGeometry;
	sameSense = aSameSense;
	// --- classe inherited fields ---
	StepShape_Face::Init(aName, aBounds);
}


void StepShape_FaceSurface::SetFaceGeometry(const Handle(StepGeom_Surface)& aFaceGeometry)
{
	faceGeometry = aFaceGeometry;
}

Handle(StepGeom_Surface) StepShape_FaceSurface::FaceGeometry() const
{
	return faceGeometry;
}

void StepShape_FaceSurface::SetSameSense(const Standard_Boolean aSameSense)
{
	sameSense = aSameSense;
}

Standard_Boolean StepShape_FaceSurface::SameSense() const
{
	return sameSense;
}
