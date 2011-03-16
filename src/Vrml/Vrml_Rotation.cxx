#include <Vrml_Rotation.ixx>

Vrml_Rotation::Vrml_Rotation()
{
  Vrml_SFRotation tmpR(0,0,1,0);
  myRotation = tmpR;
}

Vrml_Rotation::Vrml_Rotation(const Vrml_SFRotation& aRotation)
{
  myRotation = aRotation;
}

 void Vrml_Rotation::SetRotation(const Vrml_SFRotation& aRotation) 
{
  myRotation = aRotation;
}

 Vrml_SFRotation Vrml_Rotation::Rotation() const
{
  return myRotation;
}

 Standard_OStream& Vrml_Rotation::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Rotation {" << endl;

 if ( Abs(myRotation.RotationX() - 0) > 0.0001 || 
     Abs(myRotation.RotationY() - 0) > 0.0001 || 
     Abs(myRotation.RotationZ() - 1) > 0.0001 ||
     Abs(myRotation.Angle() - 0) > 0.0001 ) 
   {
    anOStream  << "    rotation" << '\t';
    anOStream << myRotation.RotationX() << ' ' << myRotation.RotationY() << ' ';
    anOStream << myRotation.RotationZ() << ' ' << myRotation.Angle() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
