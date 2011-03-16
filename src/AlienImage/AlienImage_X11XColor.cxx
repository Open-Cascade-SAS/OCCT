#include <AlienImage_X11XColor.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_X11XColor)
{
  static Handle(Standard_Type) _atype = new Standard_Type ("AlienImage_X11XColor", sizeof (AlienImage_X11XColor));
  return _atype;
}

Standard_Boolean  operator == ( const AlienImage_X11XColor& AnObject,
				const AlienImage_X11XColor& AnotherObject )

{ Standard_Boolean _result = Standard_True; 

  _result = _result && (AnObject.pixel==AnotherObject.pixel) ; 
  _result = _result && (AnObject.red  ==AnotherObject.red) ; 
  _result = _result && (AnObject.green==AnotherObject.green) ; 
  _result = _result && (AnObject.blue ==AnotherObject.blue) ; 
  _result = _result && (AnObject.flags==AnotherObject.flags) ; 
  
 return _result;
}

//============================================================================
//==== ShallowDump : Writes a CString value.
//============================================================================
void ShallowDump (const AlienImage_X11XColor& AnObject, Standard_OStream& s)

{
	s << "AlienImage_X11XColor\n" ;
	s << "\tpixel :" << AnObject.pixel << "\n";
	s << "\tred/green/blue :" << AnObject.red << "/"
				  << AnObject.green << "/"
				  << AnObject.blue<< "\n";
	s << "\tflags :" << AnObject.flags << "\n" << flush ;
}

ostream& operator << ( ostream& s,  const AlienImage_X11XColor& c )

{
  return( s 	<< "("	<< c.pixel 
		   	<< ",(" << c.red << "," 
				<< c.green <<"," 
				<< c.blue
			<< ")," 
			<< hex << c.flags
		<< ")" ) ;
}
 
