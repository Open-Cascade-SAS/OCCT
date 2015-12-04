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
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Quantity_Length.hxx>
class HLRAlgo_Projector;


class Prs3d_Projector;
DEFINE_STANDARD_HANDLE(Prs3d_Projector, MMgt_TShared)

//! A projector object.
//! This object defines the parameters of a view for a
//! visualization algorithm. It is, for example, used by the
//! hidden line removal algorithms.
class Prs3d_Projector : public MMgt_TShared
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
  Standard_EXPORT Prs3d_Projector(const Standard_Boolean Pers, const Quantity_Length Focus, const Quantity_Length DX, const Quantity_Length DY, const Quantity_Length DZ, const Quantity_Length XAt, const Quantity_Length YAt, const Quantity_Length ZAt, const Quantity_Length XUp, const Quantity_Length YUp, const Quantity_Length ZUp);
  
  //! Returns a projector object for use in a hidden line removal algorithm.
  Standard_EXPORT HLRAlgo_Projector Projector() const;




  DEFINE_STANDARD_RTTIEXT(Prs3d_Projector,MMgt_TShared)

protected:




private:


  HLRAlgo_Projector MyProjector;


};







#endif // _Prs3d_Projector_HeaderFile
