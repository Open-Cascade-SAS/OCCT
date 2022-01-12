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

#ifndef _gce_MakeElips_HeaderFile
#define _gce_MakeElips_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Elips.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;


//! This class implements the following algorithms used to
//! create an ellipse from gp.
//!
//! * Create an ellipse from its center, and two points:
//! one on the ciconference giving the major radius, the
//! other giving the value of the small radius.
class gce_MakeElips  : public gce_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! The major radius of the ellipse is on the "XAxis" and the
  //! minor radius is on the "YAxis" of the ellipse. The "XAxis"
  //! is defined with the "XDirection" of A2 and the "YAxis" is
  //! defined with the "YDirection" of A2.
  //! Warnings :
  //! It is not forbidden to create an ellipse with
  //! MajorRadius = MinorRadius.
  Standard_EXPORT gce_MakeElips(const gp_Ax2& A2, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Make an ellipse with its center and two points.
  //! Warning
  //! The MakeElips class does not prevent the
  //! construction of an ellipse where the MajorRadius is
  //! equal to the MinorRadius.
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_InvertRadius if MajorRadius is less than MinorRadius;
  //! -   gce_NegativeRadius if MinorRadius is less than 0.0;
  //! -   gce_NullAxis if the points S1 and Center are coincident; or
  //! -   gce_InvertAxis if:
  //! -   the major radius computed with Center and S1
  //! is less than the minor radius computed with Center, S1 and S2, or
  //! -   Center, S1 and S2 are collinear.
  Standard_EXPORT gce_MakeElips(const gp_Pnt& S1, const gp_Pnt& S2, const gp_Pnt& Center);
  
  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  Standard_EXPORT const gp_Elips& Value() const;
  
  Standard_EXPORT const gp_Elips& Operator() const;
Standard_EXPORT operator gp_Elips() const;




protected:





private:



  gp_Elips TheElips;


};







#endif // _gce_MakeElips_HeaderFile
