// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Draw_Drawable3D_HeaderFile
#define _Draw_Drawable3D_HeaderFile

#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <Standard_IStream.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>

class Draw_Display;

DEFINE_STANDARD_HANDLE(Draw_Drawable3D, Standard_Transient)

class Draw_Drawable3D : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Draw_Drawable3D, Standard_Transient)
public:

  //! Function type for restoring drawable from stream.
  typedef Handle(Draw_Drawable3D)(*FactoryFunction_t)(Standard_IStream& theStream);

  //! Register factory for restoring drawable from stream (opposite to Draw_Drawable3D::Save()).
  //! @param theType [in] class name
  //! @param theFactory [in] factory function
  Standard_EXPORT static void RegisterFactory (const Standard_CString theType,
                                               const FactoryFunction_t& theFactory);

  //! Restore drawable from stream (opposite to Draw_Drawable3D::Save()).
  //! @param theType [in] class name
  //! @param theStream [in] input stream
  //! @return restored drawable or NULL if factory is undefined for specified class
  Standard_EXPORT static Handle(Draw_Drawable3D) Restore (const Standard_CString theType,
                                                          Standard_IStream& theStream);

  //! @def Draw_Drawable3D_FACTORY
  //! Auxiliary macros defining Draw_Drawable3D restoration API to sub-class.
  #define Draw_Drawable3D_FACTORY \
    static void RegisterFactory() { Draw_Drawable3D::RegisterFactory (get_type_name(), &Restore); } \
    Standard_EXPORT static Handle(Draw_Drawable3D) Restore (Standard_IStream& theStream);

public:

  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const = 0;
  
  //! Returns True if the pick is outside the box
  Standard_EXPORT virtual Standard_Boolean PickReject (const Standard_Real X, const Standard_Real Y, const Standard_Real Prec) const;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const;

  //! Save drawable into stream; default implementation raises Standard_NotImplemented exception.
  Standard_EXPORT virtual void Save (Standard_OStream& theStream) const;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const;
  
  //! Is a 3D object. (Default True).
  virtual bool Is3D() const { return true; }

  //! Return TRUE if object can be displayed.
  virtual bool IsDisplayable() const { return true; }

  void SetBounds (const Standard_Real theXMin, const Standard_Real theXMax,
                  const Standard_Real theYMin, const Standard_Real theYMax)
  {
    myXmin = theXMin;
    myXmax = theXMax;
    myYmin = theYMin;
    myYmax = theYMax;
  }

  void Bounds (Standard_Real& theXMin, Standard_Real& theXMax,
               Standard_Real& theYMin, Standard_Real& theYMax) const
  {
    theXMin = myXmin;
    theXMax = myXmax;
    theYMin = myYmin;
    theYMax = myYmax;
  }

  Standard_Boolean Visible() const { return isVisible; }

  void Visible (const Standard_Boolean V) { isVisible = V; }

  Standard_Boolean Protected() const { return isProtected; }

  void Protected (const Standard_Boolean P) { isProtected = P; }

  Standard_CString Name() const { return myName; }

  virtual void Name (const Standard_CString N) { myName = N; }

protected:

  Standard_EXPORT Draw_Drawable3D();

private:

  Standard_Real myXmin;
  Standard_Real myXmax;
  Standard_Real myYmin;
  Standard_Real myYmax;
  Standard_CString myName;
  Standard_Boolean isVisible;
  Standard_Boolean isProtected;

};

#endif // _Draw_Drawable3D_HeaderFile
