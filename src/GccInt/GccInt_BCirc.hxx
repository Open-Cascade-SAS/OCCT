// Created on: 1991-10-04
// Created by: Remi GILET
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

#ifndef _GccInt_BCirc_HeaderFile
#define _GccInt_BCirc_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Circ2d.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_IType.hxx>
class gp_Circ2d;


class GccInt_BCirc;
DEFINE_STANDARD_HANDLE(GccInt_BCirc, GccInt_Bisec)

//! Describes a circle as a bisecting curve between two 2D
//! geometric objects (such as circles or points).
class GccInt_BCirc : public GccInt_Bisec
{

public:

  
  //! Constructs a bisecting curve whose geometry is the 2D circle Circ.
  Standard_EXPORT GccInt_BCirc(const gp_Circ2d& Circ);
  
  //! Returns a 2D circle which is the geometry of this bisecting curve.
  Standard_EXPORT virtual gp_Circ2d Circle() const Standard_OVERRIDE;
  
  //! Returns GccInt_Cir, which is the type of any GccInt_BCirc bisecting curve.
  Standard_EXPORT GccInt_IType ArcType() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GccInt_BCirc,GccInt_Bisec)

protected:




private:


  gp_Circ2d cir;


};







#endif // _GccInt_BCirc_HeaderFile
