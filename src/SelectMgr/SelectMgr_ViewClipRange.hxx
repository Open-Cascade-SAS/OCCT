// Created on: 2015-12-15
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_ViewClipRange_HeaderFile
#define _SelectMgr_ViewClipRange_HeaderFile

#include <Standard_TypeDef.hxx>

//! Class for handling depth clipping range.
//! It is used to perform checks in case if global (for the whole view)
//! clipping planes are defined inside of SelectMgr_RectangularFrustum
//! class methods.
class SelectMgr_ViewClipRange
{
public:
  //! Creates new empty view clip range
  SelectMgr_ViewClipRange()
  {
    Clear();
  }

  //! Sets boundaries and validates view clipping range
  void Set (const Standard_Real theDepthMin, const Standard_Real theDepthMax)
  {
    myMin = theDepthMin;
    myMax = theDepthMax;
    myIsValid = Standard_True;
  }

  //! Returns true if clip range is set and depth of each matched
  //! primitive must be tested for satisfying the defined interval
  Standard_Boolean IsValid() const
  {
    return myIsValid;
  }

  //! Returns the upper bound of valid depth range
  Standard_Real MaxDepth() const
  {
    return myMax;
  }

  //! Returns the lower bound of valid depth range
  Standard_Real MinDepth() const
  {
    return myMin;
  }

  //! Invalidates view clipping range
  void Clear()
  {
    myIsValid = Standard_False;
  }

private:
  Standard_Real    myMin;      //!< Lower bound of valid depth range
  Standard_Real    myMax;      //!< Upper bound of valid depth range
  Standard_Boolean myIsValid;  //!< The flag is set to true when range boundaries are set and depth check must be performed
};

#endif // _SelectMgr_ViewClipRange_HeaderFile
