// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _ExtremaPC_POnCurv_HeaderFile
#define _ExtremaPC_POnCurv_HeaderFile

#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point on curve with parameter.
//!
//! This class represents a point on a curve identified by its
//! parameter value and 3D coordinates. It is used to store
//! extremum points found during point-curve distance computation.
class ExtremaPC_POnCurv
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized point.
  ExtremaPC_POnCurv()
      : myU(0.0)
  {
  }

  //! Constructor with parameter and point.
  //! @param theU parameter value on curve
  //! @param theP 3D point on curve
  ExtremaPC_POnCurv(double theU, const gp_Pnt& theP)
      : myU(theU),
        myP(theP)
  {
  }

  //! Sets the parameter and point values.
  //! @param theU parameter value on curve
  //! @param theP 3D point on curve
  void SetValues(double theU, const gp_Pnt& theP)
  {
    myU = theU;
    myP = theP;
  }

  //! Returns the parameter value on curve.
  double Parameter() const { return myU; }

  //! Returns the 3D point on curve.
  const gp_Pnt& Value() const { return myP; }

private:
  double myU; //!< Parameter value on curve
  gp_Pnt myP; //!< 3D point on curve
};

#endif // _ExtremaPC_POnCurv_HeaderFile
