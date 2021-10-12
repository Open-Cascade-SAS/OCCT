// Created on: 1994-04-18
// Created by: Modelistation
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Draw_Segment2D_HeaderFile
#define _Draw_Segment2D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt2d.hxx>
#include <Draw_Color.hxx>
#include <Draw_Drawable2D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Draw_Display;


class Draw_Segment2D;
DEFINE_STANDARD_HANDLE(Draw_Segment2D, Draw_Drawable2D)


class Draw_Segment2D : public Draw_Drawable2D
{

public:

  
  Standard_EXPORT Draw_Segment2D(const gp_Pnt2d& p1, const gp_Pnt2d& p2, const Draw_Color& col);
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Pnt2d& First() const;
  
  Standard_EXPORT void First (const gp_Pnt2d& P);
  
  Standard_EXPORT const gp_Pnt2d& Last() const;
  
  Standard_EXPORT void Last (const gp_Pnt2d& P);
  
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Draw_Segment2D,Draw_Drawable2D)

protected:




private:


  gp_Pnt2d myFirst;
  gp_Pnt2d myLast;
  Draw_Color myColor;


};







#endif // _Draw_Segment2D_HeaderFile
