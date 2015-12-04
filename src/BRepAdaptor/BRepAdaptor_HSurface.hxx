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

#ifndef _BRepAdaptor_HSurface_HeaderFile
#define _BRepAdaptor_HSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <Adaptor3d_HSurface.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class BRepAdaptor_Surface;
class Adaptor3d_Surface;


class BRepAdaptor_HSurface;
DEFINE_STANDARD_HANDLE(BRepAdaptor_HSurface, Adaptor3d_HSurface)


class BRepAdaptor_HSurface : public Adaptor3d_HSurface
{

public:

  
  //! Creates an empty GenHSurface.
  Standard_EXPORT BRepAdaptor_HSurface();
  
  //! Creates a GenHSurface from a Surface.
  Standard_EXPORT BRepAdaptor_HSurface(const BRepAdaptor_Surface& S);
  
  //! Sets the field of the GenHSurface.
  Standard_EXPORT void Set (const BRepAdaptor_Surface& S);
  
  //! Returns a reference to the Surface inside the HSurface.
  //! This is redefined from HSurface, cannot be inline.
  Standard_EXPORT const Adaptor3d_Surface& Surface() const Standard_OVERRIDE;
  
  //! Returns the surface used to create the GenHSurface.
    BRepAdaptor_Surface& ChangeSurface();




  DEFINE_STANDARD_RTTI_INLINE(BRepAdaptor_HSurface,Adaptor3d_HSurface)

protected:


  BRepAdaptor_Surface mySurf;


private:




};

#define TheSurface BRepAdaptor_Surface
#define TheSurface_hxx <BRepAdaptor_Surface.hxx>
#define Adaptor3d_GenHSurface BRepAdaptor_HSurface
#define Adaptor3d_GenHSurface_hxx <BRepAdaptor_HSurface.hxx>
#define Handle_Adaptor3d_GenHSurface Handle(BRepAdaptor_HSurface)

#include <Adaptor3d_GenHSurface.lxx>

#undef TheSurface
#undef TheSurface_hxx
#undef Adaptor3d_GenHSurface
#undef Adaptor3d_GenHSurface_hxx
#undef Handle_Adaptor3d_GenHSurface




#endif // _BRepAdaptor_HSurface_HeaderFile
