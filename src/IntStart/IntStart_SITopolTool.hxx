// Created on: 1994-03-30
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IntStart_SITopolTool_HeaderFile
#define _IntStart_SITopolTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Real.hxx>
class gp_Pnt2d;


class IntStart_SITopolTool;
DEFINE_STANDARD_HANDLE(IntStart_SITopolTool, Standard_Transient)

//! template class for a topological tool.
//! This tool is linked with the surface on which
//! the classification has to be made.
class IntStart_SITopolTool : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual TopAbs_State Classify (const gp_Pnt2d& P, const Standard_Real Tol) = 0;




  DEFINE_STANDARD_RTTIEXT(IntStart_SITopolTool,Standard_Transient)

protected:




private:




};







#endif // _IntStart_SITopolTool_HeaderFile
