#include <StepBasic_Product.ixx>


StepBasic_Product::StepBasic_Product ()  {}

void StepBasic_Product::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_HArray1OfProductContext)& aFrameOfReference)
{
	// --- classe own fields ---
	id = aId;
	name = aName;
	description = aDescription;
	frameOfReference = aFrameOfReference;
}


void StepBasic_Product::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_Product::Id() const
{
	return id;
}

void StepBasic_Product::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_Product::Name() const
{
	return name;
}

void StepBasic_Product::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_Product::Description() const
{
	return description;
}

void StepBasic_Product::SetFrameOfReference(const Handle(StepBasic_HArray1OfProductContext)& aFrameOfReference)
{
	frameOfReference = aFrameOfReference;
}

Handle(StepBasic_HArray1OfProductContext) StepBasic_Product::FrameOfReference() const
{
	return frameOfReference;
}

Handle(StepBasic_ProductContext) StepBasic_Product::FrameOfReferenceValue(const Standard_Integer num) const
{
	return frameOfReference->Value(num);
}

Standard_Integer StepBasic_Product::NbFrameOfReference () const
{
	if (frameOfReference.IsNull()) return 0;
	return frameOfReference->Length();
}
