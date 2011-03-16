#include <Vrml_Sphere.ixx>
Vrml_Sphere::Vrml_Sphere(const Standard_Real aRadius)
{
  myRadius = aRadius;
}

 void Vrml_Sphere::SetRadius(const Standard_Real aRadius) 
{
  myRadius = aRadius;
}

 Standard_Real Vrml_Sphere::Radius() const
{
  return myRadius; 
}

 Standard_OStream& Vrml_Sphere::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Sphere {" << endl;

 if ( Abs(myRadius - 1) > 0.0001 )
   {
    anOStream  << "    radius" << '\t';
    anOStream << myRadius << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
