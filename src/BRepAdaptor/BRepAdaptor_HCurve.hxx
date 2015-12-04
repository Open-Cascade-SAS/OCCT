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

#ifndef _BRepAdaptor_HCurve_HeaderFile
#define _BRepAdaptor_HCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_HCurve.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class BRepAdaptor_Curve;
class Adaptor3d_Curve;


class BRepAdaptor_HCurve;
DEFINE_STANDARD_HANDLE(BRepAdaptor_HCurve, Adaptor3d_HCurve)


class BRepAdaptor_HCurve : public Adaptor3d_HCurve
{

public:

  
  //! Creates an empty GenHCurve.
  Standard_EXPORT BRepAdaptor_HCurve();
  
  //! Creates a GenHCurve from a Curve
  Standard_EXPORT BRepAdaptor_HCurve(const BRepAdaptor_Curve& C);
  
  //! Sets the field of the GenHCurve.
  Standard_EXPORT void Set (const BRepAdaptor_Curve& C);
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT const Adaptor3d_Curve& Curve() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT Adaptor3d_Curve& GetCurve() Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    BRepAdaptor_Curve& ChangeCurve();




  DEFINE_STANDARD_RTTI_INLINE(BRepAdaptor_HCurve,Adaptor3d_HCurve)

protected:


  BRepAdaptor_Curve myCurve;


private:




};

#define TheCurve BRepAdaptor_Curve
#define TheCurve_hxx <BRepAdaptor_Curve.hxx>
#define Adaptor3d_GenHCurve BRepAdaptor_HCurve
#define Adaptor3d_GenHCurve_hxx <BRepAdaptor_HCurve.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(BRepAdaptor_HCurve)

#include <Adaptor3d_GenHCurve.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor3d_GenHCurve
#undef Adaptor3d_GenHCurve_hxx
#undef Handle_Adaptor3d_GenHCurve




#endif // _BRepAdaptor_HCurve_HeaderFile
