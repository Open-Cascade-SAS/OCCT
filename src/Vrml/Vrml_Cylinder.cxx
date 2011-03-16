#include <Vrml_Cylinder.ixx>

Vrml_Cylinder::Vrml_Cylinder(const Vrml_CylinderParts aParts,
			     const Standard_Real aRadius,
			     const Standard_Real aHeight)
{
    myParts = aParts;   
    myRadius = aRadius;
    myHeight = aHeight;
}

 void Vrml_Cylinder::SetParts(const Vrml_CylinderParts aParts) 
{
    myParts = aParts;   
}

 Vrml_CylinderParts Vrml_Cylinder::Parts() const
{
  return myParts;
}

 void Vrml_Cylinder::SetRadius(const Standard_Real aRadius) 
{
    myRadius = aRadius;
}

 Standard_Real Vrml_Cylinder::Radius() const
{
  return myRadius;
}

 void Vrml_Cylinder::SetHeight(const Standard_Real aHeight) 
{
    myHeight = aHeight;
}

 Standard_Real Vrml_Cylinder::Height() const
{
  return myHeight;
}

 Standard_OStream& Vrml_Cylinder::Print(Standard_OStream& anOStream) const
{
  anOStream  << "Cylinder {" << endl;

  switch ( myParts )
    {
     case Vrml_CylinderALL: break; // anOStream  << "\tparts" << "\tALL ";
     case Vrml_CylinderSIDES:  anOStream  << "    parts" << "\tSIDES" << endl; break;
     case Vrml_CylinderTOP:    anOStream  << "    parts" << "\tTOP" << endl; break; 
     case Vrml_CylinderBOTTOM: anOStream  << "    parts" << "\tBOTTOM" << endl; break; 
    }

 if ( Abs(myRadius - 1) > 0.0001 )
   {
    anOStream  << "    radius" << '\t';
    anOStream << myRadius << endl;
   }

 if ( Abs(myHeight - 2) > 0.0001 )
   {
    anOStream  << "    height" << '\t';
    anOStream << myHeight << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;

}
