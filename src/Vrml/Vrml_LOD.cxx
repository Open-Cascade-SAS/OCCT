#include <Vrml_LOD.ixx>

Vrml_LOD::Vrml_LOD()
{
  myRange = new TColStd_HArray1OfReal(1,1);
  gp_Vec tmpVec(0,0,0);
  myCenter = tmpVec;
  myRangeFlag = Standard_False;
}

Vrml_LOD::Vrml_LOD(const Handle(TColStd_HArray1OfReal)& aRange,
		   const gp_Vec& aCenter)
{
  myRange = aRange;
  myCenter = aCenter;
  myRangeFlag = Standard_True;
}

 void Vrml_LOD::SetRange(const Handle(TColStd_HArray1OfReal)& aRange) 
{
  myRange = aRange;
  myRangeFlag = Standard_True;
}

 Handle(TColStd_HArray1OfReal) Vrml_LOD::Range() const
{
  return myRange;
}

 void Vrml_LOD::SetCenter(const gp_Vec& aCenter) 
{
  myCenter = aCenter;
}

 gp_Vec Vrml_LOD::Center() const
{
  return myCenter;
}

 Standard_OStream& Vrml_LOD::Print(Standard_OStream& anOStream) const
{
  Standard_Integer i;
  anOStream  << "LOD {" << endl;
  
  if ( myRangeFlag == Standard_True )
    { 
      anOStream  << "    range [" << endl << "\t";
      for ( i = myRange->Lower(); i <= myRange->Upper(); i++ )
	{
	  anOStream << myRange->Value(i);
	  if ( i < myRange->Length() )
	    anOStream  << ',';
        }
      anOStream  << " ]" << endl;
    }

  if ( Abs(myCenter.X() - 0) > 0.0001 || 
      Abs(myCenter.Y() - 0) > 0.0001 || 
      Abs(myCenter.Z() - 0) > 0.0001 ) 
    {
      anOStream  << "    center" << '\t';
      anOStream << myCenter.X() << ' ' << myCenter.Y() << ' ' << myCenter.Z() << endl;
    }
  
  anOStream  << '}' << endl;
  return anOStream;
}
