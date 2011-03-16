#include <StepShape_Block.ixx>


StepShape_Block::StepShape_Block ()  {}

void StepShape_Block::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Block::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aX,
	const Standard_Real aY,
	const Standard_Real aZ)
{
	// --- classe own fields ---
	position = aPosition;
	x = aX;
	y = aY;
	z = aZ;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Block::SetPosition(const Handle(StepGeom_Axis2Placement3d)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis2Placement3d) StepShape_Block::Position() const
{
	return position;
}

void StepShape_Block::SetX(const Standard_Real aX)
{
	x = aX;
}

Standard_Real StepShape_Block::X() const
{
	return x;
}

void StepShape_Block::SetY(const Standard_Real aY)
{
	y = aY;
}

Standard_Real StepShape_Block::Y() const
{
	return y;
}

void StepShape_Block::SetZ(const Standard_Real aZ)
{
	z = aZ;
}

Standard_Real StepShape_Block::Z() const
{
	return z;
}
