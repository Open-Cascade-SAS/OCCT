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

#ifndef _GccInt_BPoint_HeaderFile
#define _GccInt_BPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt2d.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_IType.hxx>


class GccInt_BPoint;
DEFINE_STANDARD_HANDLE(GccInt_BPoint, GccInt_Bisec)

//! Describes a point as a bisecting object between two 2D geometric objects.
class GccInt_BPoint : public GccInt_Bisec
{

public:

  
  //! Constructs a bisecting object whose geometry is the 2D point Point.
  Standard_EXPORT GccInt_BPoint(const gp_Pnt2d& Point);
  
  //! Returns a 2D point which is the geometry of this bisecting object.
  Standard_EXPORT virtual gp_Pnt2d Point() const Standard_OVERRIDE;
  
  //! Returns GccInt_Pnt, which is the type of any GccInt_BPoint bisecting object.
  Standard_EXPORT GccInt_IType ArcType() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GccInt_BPoint,GccInt_Bisec)

protected:




private:


  gp_Pnt2d pnt;


};







#endif // _GccInt_BPoint_HeaderFile
