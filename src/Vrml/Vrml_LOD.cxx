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
