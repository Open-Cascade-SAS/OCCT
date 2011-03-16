#include <StepGeom_CartesianPoint.ixx>


StepGeom_CartesianPoint::StepGeom_CartesianPoint ()  {}

void StepGeom_CartesianPoint::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CartesianPoint::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TColStd_HArray1OfReal)& aCoordinates)
{
	// --- classe own fields ---
  nbcoord = aCoordinates->Length();
  coords[0] = aCoordinates->Value(1);
  coords[1] = aCoordinates->Value(2);
  coords[2] = aCoordinates->Value(3);
//	coordinates = aCoordinates;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CartesianPoint::Init2D(
	const Handle(TCollection_HAsciiString)& aName,
        const Standard_Real X, const Standard_Real Y)
{
  nbcoord = 2;
  coords[0] = X;
  coords[1] = Y;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CartesianPoint::Init3D(
	const Handle(TCollection_HAsciiString)& aName,
        const Standard_Real X, const Standard_Real Y, const Standard_Real Z)
{
  nbcoord = 3;
  coords[0] = X;
  coords[1] = Y;
  coords[2] = Z;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CartesianPoint::SetCoordinates(const Handle(TColStd_HArray1OfReal)& aCoordinates)
{
  nbcoord = aCoordinates->Length();
  coords[0] = aCoordinates->Value(1);
  coords[1] = aCoordinates->Value(2);
  coords[2] = aCoordinates->Value(3);
//	coordinates = aCoordinates;
}

Handle(TColStd_HArray1OfReal) StepGeom_CartesianPoint::Coordinates() const
{
  Handle(TColStd_HArray1OfReal)  coordinates = new TColStd_HArray1OfReal(1,nbcoord);
  coordinates->SetValue(1,coords[0]);
  coordinates->SetValue(2,coords[1]);
  coordinates->SetValue(3,coords[2]);
	return coordinates;
}

Standard_Real StepGeom_CartesianPoint::CoordinatesValue(const Standard_Integer num) const
{
  return coords[num-1];
//	return coordinates->Value(num);
}

Standard_Integer StepGeom_CartesianPoint::NbCoordinates () const
{
  return nbcoord;
//	return coordinates->Length();
}
