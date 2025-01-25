// Created on: 2021-07-20
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Poly_TriangulationParameters_HeaderFile
#define _Poly_TriangulationParameters_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_DefineHandle.hxx>

//! Represents initial set of parameters triangulation is built for.
class Poly_TriangulationParameters : public Standard_Transient
{
public:
  //! Constructor.
  //! Initializes object with the given parameters.
  //! @param theDeflection linear deflection
  //! @param theAngle angular deflection
  //! @param theMinSize minimum size
  Poly_TriangulationParameters(const Standard_Real theDeflection = -1.,
                               const Standard_Real theAngle      = -1.,
                               const Standard_Real theMinSize    = -1.)
      : myDeflection(theDeflection),
        myAngle(theAngle),
        myMinSize(theMinSize)
  {
  }

  //! Destructor.
  virtual ~Poly_TriangulationParameters() {}

  //! Returns true if linear deflection is defined.
  Standard_Boolean HasDeflection() const { return !(myDeflection < 0.); }

  //! Returns true if angular deflection is defined.
  Standard_Boolean HasAngle() const { return !(myAngle < 0.); }

  //! Returns true if minimum size is defined.
  Standard_Boolean HasMinSize() const { return !(myMinSize < 0.); }

  //! Returns linear deflection or -1 if undefined.
  Standard_Real Deflection() const { return myDeflection; }

  //! Returns angular deflection or -1 if undefined.
  Standard_Real Angle() const { return myAngle; }

  //! Returns minimum size or -1 if undefined.
  Standard_Real MinSize() const { return myMinSize; }

  DEFINE_STANDARD_RTTIEXT(Poly_TriangulationParameters, Standard_Transient)

private:
  Standard_Real myDeflection;
  Standard_Real myAngle;
  Standard_Real myMinSize;
};

DEFINE_STANDARD_HANDLE(Poly_TriangulationParameters, Standard_Transient)

#endif
