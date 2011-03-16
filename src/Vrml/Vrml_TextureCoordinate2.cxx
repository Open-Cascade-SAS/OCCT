#include <Vrml_TextureCoordinate2.ixx>

Vrml_TextureCoordinate2::Vrml_TextureCoordinate2()
{
  gp_Vec2d tmpVec(0,0);
  myPoint = new TColgp_HArray1OfVec2d(1,1,tmpVec);
}

Vrml_TextureCoordinate2::Vrml_TextureCoordinate2(const Handle(TColgp_HArray1OfVec2d)& aPoint)
{
  myPoint = aPoint;
}

 void Vrml_TextureCoordinate2::SetPoint(const Handle(TColgp_HArray1OfVec2d)& aPoint) 
{
  myPoint = aPoint;
}

 Handle(TColgp_HArray1OfVec2d) Vrml_TextureCoordinate2::Point() const
{
  return myPoint;
}

 Standard_OStream& Vrml_TextureCoordinate2::Print(Standard_OStream& anOStream) const
{
 Standard_Integer i;
 anOStream  << "TextureCoordinate2 {" << endl;

 if ( myPoint->Length() != 1 || Abs(myPoint->Value(myPoint->Lower()).X() - 0) > 0.0001 || 
                                Abs(myPoint->Value(myPoint->Lower()).Y() - 0) > 0.0001 )
  { 
   anOStream  << "    point [" << endl << '\t';
    for ( i = myPoint->Lower(); i <= myPoint->Upper(); i++ )
	{
	 anOStream << myPoint->Value(i).X() << ' ' << myPoint->Value(i).Y();

         if ( i < myPoint->Length() )
	    anOStream  << ',' << endl << '\t';
	}
    anOStream  << " ]" << endl;
  }
  anOStream  << '}' << endl;
 return anOStream;
}
