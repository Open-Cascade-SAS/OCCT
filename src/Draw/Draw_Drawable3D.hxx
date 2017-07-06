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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Draw_Display;


class Draw_Drawable3D;
DEFINE_STANDARD_HANDLE(Draw_Drawable3D, Standard_Transient)


class Draw_Drawable3D : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const = 0;
  
  //! Returs True if the pick is outside the box
  Standard_EXPORT virtual Standard_Boolean PickReject (const Standard_Real X, const Standard_Real Y, const Standard_Real Prec) const;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const;
  
  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const;
  
  //! Is a 3D object. (Default True).
  Standard_EXPORT virtual Standard_Boolean Is3D() const;
  
  Standard_EXPORT void SetBounds (const Standard_Real xmin, const Standard_Real xmax, const Standard_Real ymin, const Standard_Real ymax);
  
  Standard_EXPORT void Bounds (Standard_Real& xmin, Standard_Real& xmax, Standard_Real& ymin, Standard_Real& ymax) const;
  
    Standard_Boolean Visible() const;
  
    void Visible (const Standard_Boolean V);
  
    Standard_Boolean Protected() const;
  
    void Protected (const Standard_Boolean P);
  
    Standard_CString Name() const;
  
    virtual void Name (const Standard_CString N);




  DEFINE_STANDARD_RTTIEXT(Draw_Drawable3D,Standard_Transient)

protected:

  
  Standard_EXPORT Draw_Drawable3D();



private:


  Standard_Real myXmin;
  Standard_Real myXmax;
  Standard_Real myYmin;
  Standard_Real myYmax;
  Standard_Boolean isVisible;
  Standard_Boolean isProtected;
  Standard_CString myName;


};


#include <Draw_Drawable3D.lxx>





#endif // _Draw_Drawable3D_HeaderFile
