// Created on: 1991-05-16
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _IntAna_Int3Pln_HeaderFile
#define _IntAna_Int3Pln_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
class StdFail_NotDone;
class Standard_DomainError;
class gp_Pln;
class gp_Pnt;


//! Intersection between 3 planes. The algorithm searches
//! for an intersection point. If two of the planes are
//! parallel or identical, IsEmpty returns TRUE.
class IntAna_Int3Pln 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntAna_Int3Pln();
  
  //! Determination of the intersection point between
  //! 3 planes.
  Standard_EXPORT IntAna_Int3Pln(const gp_Pln& P1, const gp_Pln& P2, const gp_Pln& P3);
  
  //! Determination of the intersection point between
  //! 3 planes.
  Standard_EXPORT void Perform (const gp_Pln& P1, const gp_Pln& P2, const gp_Pln& P3);
  
  //! Returns True if the computation was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns TRUE if there is no intersection POINT.
  //! If 2 planes are identical or parallel, IsEmpty
  //! will return TRUE.
    Standard_Boolean IsEmpty() const;
  
  //! Returns the intersection point.
    const gp_Pnt& Value() const;




protected:





private:



  Standard_Boolean done;
  Standard_Boolean empt;
  gp_Pnt pnt;


};


#include <IntAna_Int3Pln.lxx>





#endif // _IntAna_Int3Pln_HeaderFile
