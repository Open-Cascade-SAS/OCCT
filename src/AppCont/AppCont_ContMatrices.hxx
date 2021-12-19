// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef AppCont_ContMatrices_HeaderFile
#define AppCont_ContMatrices_HeaderFile
#include <math_Vector.hxx>

// Stockage des matrices de resolution d approxiamtion continue.
// La classe est limitee a 26 pour MMatrix et InvMMatrix, (pas de contraintes)
// pour IBPMatrix (contraintes de passage aux extremites) 
// et IBTMatrix (contraintes de tangence aux extremites).


void InvMMatrix(const Standard_Integer classe, math_Matrix& M);

void MMatrix(const Standard_Integer classe, math_Matrix& M);

void IBPMatrix(const Standard_Integer classe, math_Matrix& M);

void IBTMatrix(const Standard_Integer classe, math_Matrix& M);

void VBernstein(const Standard_Integer classe, 
		const Standard_Integer nbpoints,
		math_Matrix&           M);

#endif
