// Created on: 1992-08-26
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _gce_MakeLin_HeaderFile
#define _gce_MakeLin_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Lin.hxx>
#include <gce_Root.hxx>
class gp_Ax1;
class gp_Pnt;
class gp_Dir;


//! This class implements the following algorithms used
//! to create a Lin from gp.
//! * Create a Lin parallel to another and passing
//! through a point.
//! * Create a Lin passing through 2 points.
//! * Create a lin from its axis (Ax1 from gp).
//! * Create a lin from a point and a direction.
class gce_MakeLin  : public gce_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a line located along the axis A1.
  Standard_EXPORT gce_MakeLin(const gp_Ax1& A1);
  

  //! <P> is the location point (origin) of the line and
  //! <V> is the direction of the line.
  Standard_EXPORT gce_MakeLin(const gp_Pnt& P, const gp_Dir& V);
  
  //! Make a Lin from gp <TheLin> parallel to another
  //! Lin <Lin> and passing through a Pnt <Point>.
  Standard_EXPORT gce_MakeLin(const gp_Lin& Lin, const gp_Pnt& Point);
  
  //! Make a Lin from gp <TheLin> passing through 2
  //! Pnt <P1>,<P2>.
  //! It returns false if <p1> and <P2> are confused.
  Standard_EXPORT gce_MakeLin(const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Returns the constructed line.
  //! Exceptions StdFail_NotDone is raised if no line is constructed.
  Standard_EXPORT const gp_Lin& Value() const;
  
  Standard_EXPORT const gp_Lin& Operator() const;
Standard_EXPORT operator gp_Lin() const;




protected:





private:



  gp_Lin TheLin;


};







#endif // _gce_MakeLin_HeaderFile
