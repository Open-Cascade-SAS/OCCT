// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Vrml_PointSet.hxx>

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
 anOStream  << "PointSet {\n";
 if ( myStartIndex != 0 || myNumPoints !=-1 )
  {
    if ( myStartIndex != 0)
      {
	anOStream  << "    startIndex\t";
	anOStream << myStartIndex << "\n";
      }
    if ( myNumPoints != 0)
      {
	anOStream  << "    numPoints\t";
	anOStream << myNumPoints << "\n";
      }
  }
 anOStream  << "}\n";
 return anOStream;
}

