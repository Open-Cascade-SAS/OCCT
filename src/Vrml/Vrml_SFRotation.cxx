#include <Vrml_SFRotation.ixx>

Vrml_SFRotation::Vrml_SFRotation()
{
}

Vrml_SFRotation::Vrml_SFRotation( const Standard_Real aRotationX, 
				  const Standard_Real aRotationY, 
				  const Standard_Real aRotationZ, 
				  const Standard_Real anAngle )
{
    myRotationX = aRotationX;
    myRotationY = aRotationY;
    myRotationZ = aRotationZ;
    myAngle     = anAngle;
}

void Vrml_SFRotation::SetRotationX(const Standard_Real aRotationX)
{
    myRotationX = aRotationX;
}

Standard_Real Vrml_SFRotation::RotationX() const 
{
  return myRotationX;
}

void Vrml_SFRotation::SetRotationY(const Standard_Real aRotationY)
{
    myRotationY = aRotationY;
}

Standard_Real Vrml_SFRotation::RotationY() const 
{
  return myRotationY;
}

void Vrml_SFRotation::SetRotationZ(const Standard_Real aRotationZ)
{
    myRotationZ = aRotationZ;
}

Standard_Real Vrml_SFRotation::RotationZ() const 
{
  return myRotationZ;
}

void Vrml_SFRotation::SetAngle(const Standard_Real anAngle)
{
    myAngle = anAngle;
}

Standard_Real Vrml_SFRotation::Angle() const 
{
  return myAngle;
}
