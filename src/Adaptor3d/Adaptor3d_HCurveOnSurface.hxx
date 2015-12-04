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

#ifndef _Adaptor3d_HCurveOnSurface_HeaderFile
#define _Adaptor3d_HCurveOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Adaptor3d_CurveOnSurface;
class Adaptor3d_Curve;


class Adaptor3d_HCurveOnSurface;
DEFINE_STANDARD_HANDLE(Adaptor3d_HCurveOnSurface, Adaptor3d_HCurve)


class Adaptor3d_HCurveOnSurface : public Adaptor3d_HCurve
{

public:

  
  //! Creates an empty GenHCurve.
  Standard_EXPORT Adaptor3d_HCurveOnSurface();
  
  //! Creates a GenHCurve from a Curve
  Standard_EXPORT Adaptor3d_HCurveOnSurface(const Adaptor3d_CurveOnSurface& C);
  
  //! Sets the field of the GenHCurve.
  Standard_EXPORT void Set (const Adaptor3d_CurveOnSurface& C);
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT const Adaptor3d_Curve& Curve() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT Adaptor3d_Curve& GetCurve() Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    Adaptor3d_CurveOnSurface& ChangeCurve();




  DEFINE_STANDARD_RTTI_INLINE(Adaptor3d_HCurveOnSurface,Adaptor3d_HCurve)

protected:


  Adaptor3d_CurveOnSurface myCurve;


private:




};

#define TheCurve Adaptor3d_CurveOnSurface
#define TheCurve_hxx <Adaptor3d_CurveOnSurface.hxx>
#define Adaptor3d_GenHCurve Adaptor3d_HCurveOnSurface
#define Adaptor3d_GenHCurve_hxx <Adaptor3d_HCurveOnSurface.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(Adaptor3d_HCurveOnSurface)

#include <Adaptor3d_GenHCurve.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor3d_GenHCurve
#undef Adaptor3d_GenHCurve_hxx
#undef Handle_Adaptor3d_GenHCurve




#endif // _Adaptor3d_HCurveOnSurface_HeaderFile
