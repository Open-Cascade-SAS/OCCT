// Created on: 1991-11-25
// Created by: NW,JPB,CAL
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

#ifndef _Visual3d_ContextPick_HeaderFile
#define _Visual3d_ContextPick_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Visual3d_TypeOfOrder.hxx>
class Visual3d_ContextPickDefinitionError;


//! This class allows the creation and update of
//! a pick context for one view of the viewer.
//! A context allows the control of different parameters
//! before the activation of a pick.
//!
//! * the pick aperture.
//! * the depth of pick, the number of sub-structures selected.
//! * the order of picking, the possibility to traverse
//! the pick structures starting from the root
//! or the leaves.
class Visual3d_ContextPick 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a context from default values
  //!
  //! Aperture	: 4.0
  //! Depth	: 10
  //! Order	: TOO_TOPFIRST
  Standard_EXPORT Visual3d_ContextPick();
  
  //! Creates a context with the values defined
  //! Warning: Raises ContextPickDefinitionError if <Depth> or
  //! <Aperture> is a negative value.
  Standard_EXPORT Visual3d_ContextPick(const Standard_Real Aperture, const Standard_Integer Depth, const Visual3d_TypeOfOrder Order);
  
  //! Modifies the size of the pick window.
  //! Category: Methods to modifies the class definition
  //! Warning: Raises ContextPickDefinitionError if <Aperture> is
  //! a negative value.
  Standard_EXPORT void SetAperture (const Standard_Real Aperture);
  
  //! Modifies the pick depth a priori.
  //! Category: Methods to modifies the class definition
  //! Warning: Raises ContextPickDefinitionError if <Depth> is
  //! a negative value.
  Standard_EXPORT void SetDepth (const Standard_Integer Depth);
  
  //! Modifies the order of picking.
  //!
  //! TypeOfOrder : TOO_TOPFIRST
  //! TOO_BOTTOMFIRST
  Standard_EXPORT void SetOrder (const Visual3d_TypeOfOrder Order);
  
  //! Returns the size of the pick window <me>.
  Standard_EXPORT Standard_Real Aperture() const;
  
  //! Returns the effective pick depth of <me>.
  Standard_EXPORT Standard_Integer Depth() const;
  
  //! Returns the order of picking of <me>.
  //!
  //! TypeOfOrder	: TOO_TOPFIRST
  //! TOO_BOTTOMFIRST
  Standard_EXPORT Visual3d_TypeOfOrder Order() const;




protected:





private:



  Standard_Real MyAperture;
  Standard_Integer MyDepth;
  Visual3d_TypeOfOrder MyOrder;


};







#endif // _Visual3d_ContextPick_HeaderFile
