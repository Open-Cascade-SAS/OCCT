// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshTools_EdgeTessellator_HeaderFile
#define _IMeshTools_EdgeTessellator_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

class gp_Pnt;

//! Interface class providing API for edge tessellation tools.
class IMeshTools_CurveTessellator : public Standard_Transient
{
public:

  //! Destructor.
  virtual ~IMeshTools_CurveTessellator()
  {
  }

  //! Returns number of tessellation points.
  Standard_EXPORT virtual Standard_Integer PointsNb () const = 0;

  //! Returns parameters of solution with the given index.
  //! @param theIndex index of tessellation point.
  //! @param thePoint tessellation point.
  //! @param theParameter parameters on PCurve corresponded to the solution.
  //! @return True in case of valid result, false elewhere.
  Standard_EXPORT virtual Standard_Boolean Value (
    const Standard_Integer theIndex,
    gp_Pnt&                thePoint,
    Standard_Real&         theParameter) const = 0;

  DEFINE_STANDARD_RTTIEXT(IMeshTools_CurveTessellator, Standard_Transient)

protected:

  //! Constructor.
  IMeshTools_CurveTessellator()
  {
  }
};

#endif