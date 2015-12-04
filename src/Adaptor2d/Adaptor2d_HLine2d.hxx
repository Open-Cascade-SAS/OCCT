// Created on: 1992-10-08
// Created by: Isabelle GRIGNON
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

#ifndef _Adaptor2d_HLine2d_HeaderFile
#define _Adaptor2d_HLine2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Adaptor2d_Line2d.hxx>
#include <Adaptor2d_HCurve2d.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Adaptor2d_Line2d;
class Adaptor2d_Curve2d;


class Adaptor2d_HLine2d;
DEFINE_STANDARD_HANDLE(Adaptor2d_HLine2d, Adaptor2d_HCurve2d)


class Adaptor2d_HLine2d : public Adaptor2d_HCurve2d
{

public:

  
  //! Creates an empty GenHCurve2d.
  Standard_EXPORT Adaptor2d_HLine2d();
  
  //! Creates a GenHCurve2d from a Curve
  Standard_EXPORT Adaptor2d_HLine2d(const Adaptor2d_Line2d& C);
  
  //! Sets the field of the GenHCurve2d.
  Standard_EXPORT void Set (const Adaptor2d_Line2d& C);
  
  //! Returns the curve used to create the GenHCurve2d.
  //! This is redefined from HCurve2d, cannot be inline.
  Standard_EXPORT const Adaptor2d_Curve2d& Curve2d() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    Adaptor2d_Line2d& ChangeCurve2d();




  DEFINE_STANDARD_RTTI_INLINE(Adaptor2d_HLine2d,Adaptor2d_HCurve2d)

protected:


  Adaptor2d_Line2d myCurve;


private:




};

#define TheCurve Adaptor2d_Line2d
#define TheCurve_hxx <Adaptor2d_Line2d.hxx>
#define Adaptor2d_GenHCurve2d Adaptor2d_HLine2d
#define Adaptor2d_GenHCurve2d_hxx <Adaptor2d_HLine2d.hxx>
#define Handle_Adaptor2d_GenHCurve2d Handle(Adaptor2d_HLine2d)

#include <Adaptor2d_GenHCurve2d.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor2d_GenHCurve2d
#undef Adaptor2d_GenHCurve2d_hxx
#undef Handle_Adaptor2d_GenHCurve2d




#endif // _Adaptor2d_HLine2d_HeaderFile
