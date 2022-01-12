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

#ifndef _GccInt_BHyper_HeaderFile
#define _GccInt_BHyper_HeaderFile

#include <Standard.hxx>

#include <gp_Hypr2d.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_IType.hxx>


class GccInt_BHyper;
DEFINE_STANDARD_HANDLE(GccInt_BHyper, GccInt_Bisec)

//! Describes a hyperbola as a bisecting curve between two
//! 2D geometric objects (such as circles or points).
class GccInt_BHyper : public GccInt_Bisec
{

public:

  

  //! Constructs a bisecting curve whose geometry is the 2D hyperbola Hyper.
  Standard_EXPORT GccInt_BHyper(const gp_Hypr2d& Hyper);
  
  //! Returns a 2D hyperbola which is the geometry of this bisecting curve.
  Standard_EXPORT virtual gp_Hypr2d Hyperbola() const Standard_OVERRIDE;
  
  //! Returns GccInt_Hpr, which is the type of any GccInt_BHyper bisecting curve.
  Standard_EXPORT GccInt_IType ArcType() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GccInt_BHyper,GccInt_Bisec)

protected:




private:


  gp_Hypr2d hyp;


};







#endif // _GccInt_BHyper_HeaderFile
