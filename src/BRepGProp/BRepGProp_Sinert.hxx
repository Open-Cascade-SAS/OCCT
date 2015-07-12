// Created on: 1991-04-12
// Created by: Michel CHAUVAT
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

#ifndef _BRepGProp_Sinert_HeaderFile
#define _BRepGProp_Sinert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GProp_GProps.hxx>
class BRepGProp_Face;
class gp_Pnt;
class BRepGProp_Domain;



//! Computes the global properties of a face in 3D space.
//! The face 's requirements to evaluate the global properties
//! are defined in the template FaceTool from package GProp.
class BRepGProp_Sinert  : public GProp_GProps
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepGProp_Sinert();
  
  Standard_EXPORT BRepGProp_Sinert(const BRepGProp_Face& S, const gp_Pnt& SLocation);
  

  //! Builds a Sinert to evaluate the global properties of
  //! the face <S>. If isNaturalRestriction is true the domain of S is defined
  //! with the natural bounds, else it defined with an iterator
  //! of Edge from TopoDS (see DomainTool from GProp)
  Standard_EXPORT BRepGProp_Sinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& SLocation);
  
  Standard_EXPORT BRepGProp_Sinert(BRepGProp_Face& S, const gp_Pnt& SLocation, const Standard_Real Eps);
  
  Standard_EXPORT BRepGProp_Sinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& SLocation, const Standard_Real Eps);
  
  Standard_EXPORT void SetLocation (const gp_Pnt& SLocation);
  
  Standard_EXPORT void Perform (const BRepGProp_Face& S);
  
  Standard_EXPORT void Perform (BRepGProp_Face& S, BRepGProp_Domain& D);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, const Standard_Real Eps);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, BRepGProp_Domain& D, const Standard_Real Eps);
  

  //! If previously used method contained Eps parameter
  //! get actual relative error of the computation, else return  1.0.
  Standard_EXPORT Standard_Real GetEpsilon();




protected:





private:



  Standard_Real myEpsilon;


};







#endif // _BRepGProp_Sinert_HeaderFile
