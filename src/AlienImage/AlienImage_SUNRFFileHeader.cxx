#include <AlienImage_SUNRFFileHeader.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_SUNRFFileHeader)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("AlienImage_SUNRFFileHeader", sizeof (AlienImage_SUNRFFileHeader));
  return _atype;
}

Standard_Boolean  operator == ( const AlienImage_SUNRFFileHeader& AnObject,
				const AlienImage_SUNRFFileHeader& AnotherObject )

{ Standard_Boolean _result = Standard_True; 

 return _result;
}

//============================================================================
//==== ShallowDump : Writes a CString value.
//============================================================================
void ShallowDump (const AlienImage_SUNRFFileHeader& AnObject, Standard_OStream& s)

{
	s << AnObject ;
}

ostream& operator << ( ostream& s,  const AlienImage_SUNRFFileHeader& c )

{
  return( s 	<< "AlienImage_SUNRFFileHeader :" 
		<< "\n\tmagic    :" << c.ras_magic 
		<< "\n\twidth    :" << c.ras_width
		<< "\n\theight   :" << c.ras_height
		<< "\n\tdepth    :" << c.ras_depth
		<< "\n\tlength   :" << c.ras_length
		<< "\n\tmaptype  :" << c.ras_maptype
		<< "\n\tmaplength:" << c.ras_maplength << endl << flush );
}
 
