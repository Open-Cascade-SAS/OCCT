#include <AlienImage_SGIRGBFileHeader.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_SGIRGBFileHeader)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("AlienImage_SGIRGBFileHeader", sizeof (AlienImage_SGIRGBFileHeader));
  return _atype;
}

Standard_Boolean  operator == ( const AlienImage_SGIRGBFileHeader& AnObject,
				const AlienImage_SGIRGBFileHeader& AnotherObject )

{ Standard_Boolean _result = Standard_True; 

 return _result;
}

//============================================================================
//==== ShallowDump : Writes a CString value.
//============================================================================
void ShallowDump (const AlienImage_SGIRGBFileHeader& AnObject, Standard_OStream& s)

{
	s << "AlienImage_SGIRGBFileHeader\n" ;
}

ostream& operator << ( ostream& s,  const AlienImage_SGIRGBFileHeader& c )

{
  return( s 	<< "AlienImage_SGIRGBFileHeader " );
}
 
