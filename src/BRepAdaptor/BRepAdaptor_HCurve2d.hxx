// Created on: 1993-02-19
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepAdaptor_HCurve2d_HeaderFile
#define _BRepAdaptor_HCurve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepAdaptor_Curve2d.hxx>
#include <Adaptor2d_HCurve2d.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class BRepAdaptor_Curve2d;
class Adaptor2d_Curve2d;


class BRepAdaptor_HCurve2d;
DEFINE_STANDARD_HANDLE(BRepAdaptor_HCurve2d, Adaptor2d_HCurve2d)


class BRepAdaptor_HCurve2d : public Adaptor2d_HCurve2d
{

public:

  
  //! Creates an empty GenHCurve2d.
  Standard_EXPORT BRepAdaptor_HCurve2d();
  
  //! Creates a GenHCurve2d from a Curve
  Standard_EXPORT BRepAdaptor_HCurve2d(const BRepAdaptor_Curve2d& C);
  
  //! Sets the field of the GenHCurve2d.
  Standard_EXPORT void Set (const BRepAdaptor_Curve2d& C);
  
  //! Returns the curve used to create the GenHCurve2d.
  //! This is redefined from HCurve2d, cannot be inline.
  Standard_EXPORT const Adaptor2d_Curve2d& Curve2d() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    BRepAdaptor_Curve2d& ChangeCurve2d();




  DEFINE_STANDARD_RTTI_INLINE(BRepAdaptor_HCurve2d,Adaptor2d_HCurve2d)

protected:


  BRepAdaptor_Curve2d myCurve;


private:




};

#define TheCurve BRepAdaptor_Curve2d
#define TheCurve_hxx <BRepAdaptor_Curve2d.hxx>
#define Adaptor2d_GenHCurve2d BRepAdaptor_HCurve2d
#define Adaptor2d_GenHCurve2d_hxx <BRepAdaptor_HCurve2d.hxx>
#define Handle_Adaptor2d_GenHCurve2d Handle(BRepAdaptor_HCurve2d)

#include <Adaptor2d_GenHCurve2d.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor2d_GenHCurve2d
#undef Adaptor2d_GenHCurve2d_hxx
#undef Handle_Adaptor2d_GenHCurve2d




#endif // _BRepAdaptor_HCurve2d_HeaderFile
