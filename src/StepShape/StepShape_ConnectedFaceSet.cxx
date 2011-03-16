#include <StepShape_ConnectedFaceSet.ixx>


StepShape_ConnectedFaceSet::StepShape_ConnectedFaceSet ()  {}

void StepShape_ConnectedFaceSet::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_ConnectedFaceSet::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{
	// --- classe own fields ---
	cfsFaces = aCfsFaces;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_ConnectedFaceSet::SetCfsFaces(const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{
	cfsFaces = aCfsFaces;
}

Handle(StepShape_HArray1OfFace) StepShape_ConnectedFaceSet::CfsFaces() const
{
	return cfsFaces;
}

Handle(StepShape_Face) StepShape_ConnectedFaceSet::CfsFacesValue(const Standard_Integer num) const
{
	return cfsFaces->Value(num);
}

Standard_Integer StepShape_ConnectedFaceSet::NbCfsFaces () const
{
	if (cfsFaces.IsNull()) return 0;
	return cfsFaces->Length();
}
