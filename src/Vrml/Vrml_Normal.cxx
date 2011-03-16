#include <Vrml_Normal.ixx>

 Vrml_Normal::Vrml_Normal(const Handle(TColgp_HArray1OfVec)& aVector)
{
 myVector = aVector;
}

 Vrml_Normal::Vrml_Normal()
{
 gp_Vec Tmp_Vec;
 myVector = new TColgp_HArray1OfVec (1,1);
 
 Tmp_Vec.SetX(0); Tmp_Vec.SetY(0); Tmp_Vec.SetZ(1);
 myVector->SetValue(1,Tmp_Vec);
}

void Vrml_Normal::SetVector(const Handle(TColgp_HArray1OfVec)& aVector)
{
 myVector = aVector;
}

Handle(TColgp_HArray1OfVec) Vrml_Normal::Vector() const 
{
 return myVector;
}

Standard_OStream& Vrml_Normal::Print(Standard_OStream& anOStream) const 
{
 Standard_Integer i;

 anOStream  << "Normal {" << endl;
 i = myVector->Lower();
 if ( myVector->Length() == 1 && 
     Abs(myVector->Value(i).X() - 0) < 0.0001 && 
     Abs(myVector->Value(i).Y() - 0) < 0.0001 && 
     Abs(myVector->Value(i).Z() - 1) < 0.0001 )
   {
    anOStream  << '}' << endl;
    return anOStream;
   }
 else 
  {
  anOStream  << "    vector [\n\t";
   for ( i = myVector->Lower(); i <= myVector->Upper(); i++ )
     {
	 anOStream << myVector->Value(i).X() << ' ' << myVector->Value(i).Y() << ' ' << myVector->Value(i).Z();
      if ( i < myVector->Length() )
	 anOStream  << ',' << endl << '\t';
     }
    anOStream << " ]" << endl;
  }  
  anOStream  << '}' << endl;

 return anOStream;
}

