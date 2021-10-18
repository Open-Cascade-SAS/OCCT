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

#ifndef _GccInt_BLine_HeaderFile
#define _GccInt_BLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Lin2d.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_IType.hxx>


class GccInt_BLine;
DEFINE_STANDARD_HANDLE(GccInt_BLine, GccInt_Bisec)

//! Describes a line as a bisecting curve between two 2D
//! geometric objects (such as lines, circles or points).
class GccInt_BLine : public GccInt_Bisec
{

public:

  
  //! Constructs a bisecting line whose geometry is the 2D line Line.
  Standard_EXPORT GccInt_BLine(const gp_Lin2d& Line);
  
  //! Returns a 2D line which is the geometry of this bisecting line.
  Standard_EXPORT virtual gp_Lin2d Line() const Standard_OVERRIDE;
  
  //! Returns GccInt_Lin, which is the type of any GccInt_BLine bisecting line.
  Standard_EXPORT GccInt_IType ArcType() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GccInt_BLine,GccInt_Bisec)

protected:




private:


  gp_Lin2d lin;


};







#endif // _GccInt_BLine_HeaderFile
