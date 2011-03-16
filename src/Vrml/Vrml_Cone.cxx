#include <Vrml_Cone.ixx>

Vrml_Cone::Vrml_Cone(const Vrml_ConeParts aParts,
		     const Standard_Real aBottomRadius,
		     const Standard_Real aHeight)
{
    myParts = aParts;
    myBottomRadius = aBottomRadius;
    myHeight = aHeight;
}

 void Vrml_Cone::SetParts(const Vrml_ConeParts aParts) 
{
    myParts = aParts;
}

 Vrml_ConeParts Vrml_Cone::Parts() const
{
  return myParts;
}

 void Vrml_Cone::SetBottomRadius(const Standard_Real aBottomRadius) 
{
    myBottomRadius = aBottomRadius;
}

 Standard_Real Vrml_Cone::BottomRadius() const
{
  return myBottomRadius;
}

 void Vrml_Cone::SetHeight(const Standard_Real aHeight) 
{
    myHeight = aHeight;
}

 Standard_Real Vrml_Cone::Height() const
{
  return myHeight;
}

 Standard_OStream& Vrml_Cone::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Cone {" << endl;

  switch ( myParts )
    {
     case Vrml_ConeALL: break; // anOStream  << "    parts" << "\t\tALL ";
     case Vrml_ConeSIDES:  anOStream  << "    parts" << "\t\tSIDES" << endl; break;
     case Vrml_ConeBOTTOM: anOStream  << "    parts" << "\t\tBOTTOM" << endl; break; 
    }

 if ( Abs(myBottomRadius - 1 ) > 0.0001 )
   {
    anOStream  << "    bottomRadius" << '\t';
    anOStream << myBottomRadius << endl;
   }

 if ( Abs(myHeight - 2 ) > 0.0001 )
   {
    anOStream  << "    height" << "\t\t";
    anOStream << myHeight << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
