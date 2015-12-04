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

#ifndef _GeomAdaptor_GHSurface_HeaderFile
#define _GeomAdaptor_GHSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Adaptor3d_HSurface.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class GeomAdaptor_Surface;
class Adaptor3d_Surface;


class GeomAdaptor_GHSurface;
DEFINE_STANDARD_HANDLE(GeomAdaptor_GHSurface, Adaptor3d_HSurface)


class GeomAdaptor_GHSurface : public Adaptor3d_HSurface
{

public:

  
  //! Creates an empty GenHSurface.
  Standard_EXPORT GeomAdaptor_GHSurface();
  
  //! Creates a GenHSurface from a Surface.
  Standard_EXPORT GeomAdaptor_GHSurface(const GeomAdaptor_Surface& S);
  
  //! Sets the field of the GenHSurface.
  Standard_EXPORT void Set (const GeomAdaptor_Surface& S);
  
  //! Returns a reference to the Surface inside the HSurface.
  //! This is redefined from HSurface, cannot be inline.
  Standard_EXPORT const Adaptor3d_Surface& Surface() const Standard_OVERRIDE;
  
  //! Returns the surface used to create the GenHSurface.
    GeomAdaptor_Surface& ChangeSurface();




  DEFINE_STANDARD_RTTI_INLINE(GeomAdaptor_GHSurface,Adaptor3d_HSurface)

protected:


  GeomAdaptor_Surface mySurf;


private:




};

#define TheSurface GeomAdaptor_Surface
#define TheSurface_hxx <GeomAdaptor_Surface.hxx>
#define Adaptor3d_GenHSurface GeomAdaptor_GHSurface
#define Adaptor3d_GenHSurface_hxx <GeomAdaptor_GHSurface.hxx>
#define Handle_Adaptor3d_GenHSurface Handle(GeomAdaptor_GHSurface)

#include <Adaptor3d_GenHSurface.lxx>

#undef TheSurface
#undef TheSurface_hxx
#undef Adaptor3d_GenHSurface
#undef Adaptor3d_GenHSurface_hxx
#undef Handle_Adaptor3d_GenHSurface




#endif // _GeomAdaptor_GHSurface_HeaderFile
