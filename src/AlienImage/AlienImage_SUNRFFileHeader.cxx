// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
 
