// Created on: 1993-03-19
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Prs3d_Projector_HeaderFile
#define _Prs3d_Projector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HLRAlgo_Projector.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>

class Prs3d_Projector;
DEFINE_STANDARD_HANDLE(Prs3d_Projector, Standard_Transient)

//! A projector object.
//! This object defines the parameters of a view for a
//! visualization algorithm. It is, for example, used by the
//! hidden line removal algorithms.
class Prs3d_Projector : public Standard_Transient
{

public:

  
  Standard_EXPORT Prs3d_Projector(const HLRAlgo_Projector& Pr);
  
  //! Constructs a projector framework from the following parameters
  //! -   Pers is true if the view is a perspective view and
  //! false if it is an axonometric one;
  //! -   Focus is the focal length if a perspective view is defined;
  //! -   DX, DY and DZ are the coordinates of the
  //! projection vector;
  //! -   XAt, YAt and ZAt are the coordinates of the view point;
  //! -   XUp, YUp and ZUp are the coordinates of the
  //! vertical direction vector.
  Standard_EXPORT Prs3d_Projector(const Standard_Boolean Pers, const Standard_Real Focus, const Standard_Real DX, const Standard_Real DY, const Standard_Real DZ, const Standard_Real XAt, const Standard_Real YAt, const Standard_Real ZAt, const Standard_Real XUp, const Standard_Real YUp, const Standard_Real ZUp);
  
  //! Returns a projector object for use in a hidden line removal algorithm.
  Standard_EXPORT HLRAlgo_Projector Projector() const;

  DEFINE_STANDARD_RTTIEXT(Prs3d_Projector,Standard_Transient)

private:

  HLRAlgo_Projector MyProjector;

};

#endif // _Prs3d_Projector_HeaderFile
