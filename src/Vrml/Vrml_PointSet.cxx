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

#include <Vrml_PointSet.ixx>

Vrml_PointSet::Vrml_PointSet(const Standard_Integer aStartIndex, 
			      const Standard_Integer aNumPoints)
{
 myStartIndex = aStartIndex;
 myNumPoints  = aNumPoints;
}

void Vrml_PointSet::SetStartIndex(const Standard_Integer aStartIndex)
{
 myStartIndex = aStartIndex;
}

Standard_Integer Vrml_PointSet::StartIndex() const 
{
 return myStartIndex;
}

void Vrml_PointSet::SetNumPoints(const Standard_Integer aNumPoints)
{
 myNumPoints  = aNumPoints;
}

Standard_Integer Vrml_PointSet::NumPoints() const 
{
 return myNumPoints;
}

Standard_OStream& Vrml_PointSet::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "PointSet {" << endl;
 if ( myStartIndex != 0 || myNumPoints !=-1 )
  {
    if ( myStartIndex != 0)
      {
	anOStream  << "    startIndex" << '\t';
	anOStream << myStartIndex << endl;
      }
    if ( myNumPoints != 0)
      {
	anOStream  << "    numPoints" << '\t';
	anOStream << myNumPoints << endl;
      }
  }
 anOStream  << '}' << endl;
 return anOStream;
}

