// Created on: 1993-06-03
// Created by: Bruno DUMORTIER
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

#ifndef _Geom2dAdaptor_GHCurve_HeaderFile
#define _Geom2dAdaptor_GHCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Adaptor2d_HCurve2d.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Geom2dAdaptor_Curve;
class Adaptor2d_Curve2d;


class Geom2dAdaptor_GHCurve;
DEFINE_STANDARD_HANDLE(Geom2dAdaptor_GHCurve, Adaptor2d_HCurve2d)


class Geom2dAdaptor_GHCurve : public Adaptor2d_HCurve2d
{

public:

  
  //! Creates an empty GenHCurve2d.
  Standard_EXPORT Geom2dAdaptor_GHCurve();
  
  //! Creates a GenHCurve2d from a Curve
  Standard_EXPORT Geom2dAdaptor_GHCurve(const Geom2dAdaptor_Curve& C);
  
  //! Sets the field of the GenHCurve2d.
  Standard_EXPORT void Set (const Geom2dAdaptor_Curve& C);
  
  //! Returns the curve used to create the GenHCurve2d.
  //! This is redefined from HCurve2d, cannot be inline.
  Standard_EXPORT const Adaptor2d_Curve2d& Curve2d() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    Geom2dAdaptor_Curve& ChangeCurve2d();




  DEFINE_STANDARD_RTTI_INLINE(Geom2dAdaptor_GHCurve,Adaptor2d_HCurve2d)

protected:


  Geom2dAdaptor_Curve myCurve;


private:




};

#define TheCurve Geom2dAdaptor_Curve
#define TheCurve_hxx <Geom2dAdaptor_Curve.hxx>
#define Adaptor2d_GenHCurve2d Geom2dAdaptor_GHCurve
#define Adaptor2d_GenHCurve2d_hxx <Geom2dAdaptor_GHCurve.hxx>
#define Handle_Adaptor2d_GenHCurve2d Handle(Geom2dAdaptor_GHCurve)

#include <Adaptor2d_GenHCurve2d.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor2d_GenHCurve2d
#undef Adaptor2d_GenHCurve2d_hxx
#undef Handle_Adaptor2d_GenHCurve2d




#endif // _Geom2dAdaptor_GHCurve_HeaderFile
