#include <Vrml_Translation.ixx>

Vrml_Translation::Vrml_Translation()
{
  gp_Vec tmpV(0,0,0);
  myTranslation = tmpV;
}

Vrml_Translation::Vrml_Translation(const gp_Vec& aTranslation)
{
  myTranslation = aTranslation;
}

 void Vrml_Translation::SetTranslation(const gp_Vec& aTranslation) 
{
  myTranslation = aTranslation;
}

 gp_Vec Vrml_Translation::Translation() const
{
  return myTranslation;
}

 Standard_OStream& Vrml_Translation::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Translation {" << endl;

 if ( Abs(myTranslation.X() - 0) > 0.0001 || 
     Abs(myTranslation.Y() - 0) > 0.0001 || 
     Abs(myTranslation.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    translation" << '\t';
    anOStream << myTranslation.X() << ' ' << myTranslation.Y() << ' ' << myTranslation.Z() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
