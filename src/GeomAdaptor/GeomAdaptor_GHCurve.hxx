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

#ifndef _GeomAdaptor_GHCurve_HeaderFile
#define _GeomAdaptor_GHCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Adaptor3d_HCurve.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class GeomAdaptor_Curve;
class Adaptor3d_Curve;


class GeomAdaptor_GHCurve;
DEFINE_STANDARD_HANDLE(GeomAdaptor_GHCurve, Adaptor3d_HCurve)


class GeomAdaptor_GHCurve : public Adaptor3d_HCurve
{

public:

  
  //! Creates an empty GenHCurve.
  Standard_EXPORT GeomAdaptor_GHCurve();
  
  //! Creates a GenHCurve from a Curve
  Standard_EXPORT GeomAdaptor_GHCurve(const GeomAdaptor_Curve& C);
  
  //! Sets the field of the GenHCurve.
  Standard_EXPORT void Set (const GeomAdaptor_Curve& C);
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT const Adaptor3d_Curve& Curve() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT Adaptor3d_Curve& GetCurve() Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    GeomAdaptor_Curve& ChangeCurve();




  DEFINE_STANDARD_RTTI_INLINE(GeomAdaptor_GHCurve,Adaptor3d_HCurve)

protected:


  GeomAdaptor_Curve myCurve;


private:




};

#define TheCurve GeomAdaptor_Curve
#define TheCurve_hxx <GeomAdaptor_Curve.hxx>
#define Adaptor3d_GenHCurve GeomAdaptor_GHCurve
#define Adaptor3d_GenHCurve_hxx <GeomAdaptor_GHCurve.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(GeomAdaptor_GHCurve)

#include <Adaptor3d_GenHCurve.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor3d_GenHCurve
#undef Adaptor3d_GenHCurve_hxx
#undef Handle_Adaptor3d_GenHCurve




#endif // _GeomAdaptor_GHCurve_HeaderFile
