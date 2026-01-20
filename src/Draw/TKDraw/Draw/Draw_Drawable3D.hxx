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

class Draw_Drawable3D : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Draw_Drawable3D, Standard_Transient)
public:
  //! Function type for restoring drawable from stream.
  typedef occ::handle<Draw_Drawable3D> (*FactoryFunction_t)(Standard_IStream& theStream);

  //! Register factory for restoring drawable from stream (opposite to Draw_Drawable3D::Save()).
  //! @param[in] theType  class name
  //! @param[in] theFactory  factory function
  Standard_EXPORT static void RegisterFactory(const char* const   theType,
                                              const FactoryFunction_t& theFactory);

  //! Restore drawable from stream (opposite to Draw_Drawable3D::Save()).
  //! @param[in] theType  class name
  //! @param[in] theStream  input stream
  //! @return restored drawable or NULL if factory is undefined for specified class
  Standard_EXPORT static occ::handle<Draw_Drawable3D> Restore(const char* const theType,
                                                         Standard_IStream&      theStream);

//! @def Draw_Drawable3D_FACTORY
//! Auxiliary macros defining Draw_Drawable3D restoration API to sub-class.
#define Draw_Drawable3D_FACTORY                                                                    \
  static void RegisterFactory() { Draw_Drawable3D::RegisterFactory(get_type_name(), &Restore); }   \
  Standard_EXPORT static occ::handle<Draw_Drawable3D> Restore(Standard_IStream& theStream);

public:
  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const = 0;

  //! Returns True if the pick is outside the box
  Standard_EXPORT virtual bool PickReject(const double X,
                                                      const double Y,
                                                      const double Prec) const;

  //! For variable copy.
  Standard_EXPORT virtual occ::handle<Draw_Drawable3D> Copy() const;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const;

  //! Save drawable into stream; default implementation raises Standard_NotImplemented exception.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const;

  //! Is a 3D object. (Default True).
  virtual bool Is3D() const { return true; }

  //! Return TRUE if object can be displayed.
  virtual bool IsDisplayable() const { return true; }

  void SetBounds(const double theXMin,
                 const double theXMax,
                 const double theYMin,
                 const double theYMax)
  {
    myXmin = theXMin;
    myXmax = theXMax;
    myYmin = theYMin;
    myYmax = theYMax;
  }

  void Bounds(double& theXMin,
              double& theXMax,
              double& theYMin,
              double& theYMax) const
  {
    theXMin = myXmin;
    theXMax = myXmax;
    theYMin = myYmin;
    theYMax = myYmax;
  }

  bool Visible() const { return isVisible; }

  void Visible(const bool V) { isVisible = V; }

  bool Protected() const { return isProtected; }

  void Protected(const bool P) { isProtected = P; }

  const char* Name() const { return myName; }

  virtual void Name(const char* const N) { myName = N; }

protected:
  Standard_EXPORT Draw_Drawable3D();

private:
  double    myXmin;
  double    myXmax;
  double    myYmin;
  double    myYmax;
  const char* myName;
  bool isVisible;
  bool isProtected;
};

#endif // _Draw_Drawable3D_HeaderFile
