// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PPoly_Triangle.ixx>

//=======================================================================
//function : PPoly_Triangle::PPoly_Triangle
//purpose  : 
//=======================================================================

PPoly_Triangle::PPoly_Triangle(const Standard_Integer N1, 
			       const Standard_Integer N2, 
			       const Standard_Integer N3)
{
  myNodes[0] = N1;
  myNodes[1] = N2;
  myNodes[2] = N3;  
}

//=======================================================================
//function : PPoly_Triangle::Set
//purpose  : 
//=======================================================================

void PPoly_Triangle::Set(const Standard_Integer N1, 
			 const Standard_Integer N2, 
			 const Standard_Integer N3)
{
  myNodes[0] = N1;
  myNodes[1] = N2;
  myNodes[2] = N3;  
}

//=======================================================================
//function : PPoly_Triangle::Get
//purpose  : 
//=======================================================================

void PPoly_Triangle::Get(Standard_Integer& N1, 
			 Standard_Integer& N2, 
			 Standard_Integer& N3) const 
{
  N1 = myNodes[0];
  N2 = myNodes[1];
  N3 = myNodes[2];  
}

