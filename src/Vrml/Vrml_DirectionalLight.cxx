#include <Vrml_DirectionalLight.ixx>

Vrml_DirectionalLight::Vrml_DirectionalLight():
  myOnOff(Standard_True),
  myIntensity(1)
{
  gp_Vec tmpVec(0,0,-1);
  myDirection = tmpVec;

  Quantity_Color tmpColor(1,1,1,Quantity_TOC_RGB);
  myColor = tmpColor;
}

 Vrml_DirectionalLight::Vrml_DirectionalLight(const Standard_Boolean aOnOff, 
					      const Standard_Real aIntensity, 
					      const Quantity_Color& aColor, 
					      const gp_Vec& aDirection)
{
  myOnOff = aOnOff;
  if (aIntensity < 0. || aIntensity > 1.)
    {
      Standard_Failure::Raise("Error : Light intensity must be in the range 0.0 to 1.0, inclusive.");
    }
  myIntensity = aIntensity;
  myColor = aColor;
  myDirection = aDirection;
}

void Vrml_DirectionalLight::SetOnOff(const Standard_Boolean aOnOff)
{
  myOnOff = aOnOff;
}

Standard_Boolean Vrml_DirectionalLight::OnOff() const 
{
  return myOnOff;
}

void Vrml_DirectionalLight::SetIntensity(const Standard_Real aIntensity)
{
  if (aIntensity < 0. || aIntensity > 1.)
    {
      Standard_Failure::Raise("Error : Light intensity must be in the range 0.0 to 1.0, inclusive.");
    }
  myIntensity = aIntensity;
}

Standard_Real Vrml_DirectionalLight::Intensity() const 
{
  return myIntensity;
}

void Vrml_DirectionalLight::SetColor(const Quantity_Color& aColor)
{
  myColor = aColor;
}

Quantity_Color Vrml_DirectionalLight::Color() const 
{
  return  myColor;
}

void Vrml_DirectionalLight::SetDirection(const gp_Vec& aDirection)
{
  myDirection = aDirection;
}

gp_Vec Vrml_DirectionalLight::Direction() const 
{
  return myDirection;
}

Standard_OStream& Vrml_DirectionalLight::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "DirectionalLight {" << endl;

 if ( myOnOff != Standard_True )
   {
    anOStream  << "    on" << "\t\t" << "FALSE" << endl;
//    anOStream << myOnOff << endl;
   }

 if ( Abs(myIntensity - 1) > 0.0001 )
   {
    anOStream  << "    intensity" << '\t';
    anOStream << myIntensity << endl;
   }

 if ( Abs(myColor.Red() - 1) > 0.0001 || 
      Abs(myColor.Green() - 1) > 0.0001 || 
      Abs(myColor.Blue() - 1) > 0.0001 )
   {
    anOStream  << "    color" << '\t';
    anOStream << myColor.Red() << ' ' << myColor.Green() << ' ' << myColor.Blue() << endl;
   }

 if ( Abs(myDirection.X() - 0) > 0.0001 || 
     Abs(myDirection.Y() - 0) > 0.0001 || 
     Abs(myDirection.Z() + 1) > 0.0001 ) 
   {
    anOStream  << "    direction" << '\t';
    anOStream << myDirection.X() << ' ' << myDirection.Y() << ' ' << myDirection.Z() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
