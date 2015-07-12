// Created on: 1991-07-15
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

#ifndef _Draw_Display_HeaderFile
#define _Draw_Display_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_MarkerShape.hxx>
#include <Standard_CString.hxx>
class Draw_Color;
class gp_Pnt;
class gp_Pnt2d;
class gp_Circ;
class gp_Circ2d;


//! Use to  draw in a 3d or a 2d view.
//!
//! * The  3d methods draw in the 3d  system,  in a 2d
//! view the drawing is projected on X,Y.
//!
//! * The 2d methods draw in the projection plane.
//!
//! * To draw in screen coordinates the length must be
//! divided by the zoom.
class Draw_Display 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Draw_Display();
  
  //! Following drawings will use this color.
  Standard_EXPORT void SetColor (const Draw_Color& col) const;
  
  //! Set the drawing mode, 3 = copy, 6 = xor
  Standard_EXPORT void SetMode (const Standard_Integer M) const;
  
  Standard_EXPORT void Flush() const;
  
  Standard_EXPORT void MoveTo (const gp_Pnt& pt);
  
  Standard_EXPORT void DrawTo (const gp_Pnt& pt);
  
  Standard_EXPORT void MoveTo (const gp_Pnt2d& pt);
  
  Standard_EXPORT void DrawTo (const gp_Pnt2d& pt);
  
  Standard_EXPORT void Draw (const gp_Pnt& p1, const gp_Pnt& p2);
  
  Standard_EXPORT void Draw (const gp_Pnt2d& p1, const gp_Pnt2d& p2);
  
  //! Draw  a  circle   <C> from  angle <A1>   to  <A2>
  //! (Radians). if ModifyWithZoom  = 0,  then
  //! rayon of circle is convert to Integer.
  Standard_EXPORT void Draw (const gp_Circ& C, const Standard_Real A1, const Standard_Real A2, const Standard_Boolean ModifyWithZoom = Standard_True);
  
  //! Draw  a 2D circle  <C>  from angle   <A1> to <A2>
  //! (Radians). if ModifyWithZoom  = 0,  then
  //! rayon of circle is convert to Integer.
  Standard_EXPORT void Draw (const gp_Circ2d& C, const Standard_Real A1, const Standard_Real A2, const Standard_Boolean ModifyWithZoom = Standard_True);
  
  Standard_EXPORT void DrawMarker (const gp_Pnt& pt, const Draw_MarkerShape S, const Standard_Integer Size = 5);
  
  Standard_EXPORT void DrawMarker (const gp_Pnt2d& pt, const Draw_MarkerShape S, const Standard_Integer Size = 5);
  
  Standard_EXPORT void DrawMarker (const gp_Pnt& pt, const Draw_MarkerShape S, const Standard_Real Size);
  
  Standard_EXPORT void DrawMarker (const gp_Pnt2d& pt, const Draw_MarkerShape S, const Standard_Real Size);
  
  Standard_EXPORT void DrawString (const gp_Pnt& pt, const Standard_CString S);
  
  Standard_EXPORT void DrawString (const gp_Pnt2d& pt, const Standard_CString S);
  
  Standard_EXPORT void DrawString (const gp_Pnt& pt, const Standard_CString S, const Standard_Real moveX, const Standard_Real moveY);
  
  Standard_EXPORT void DrawString (const gp_Pnt2d& pt, const Standard_CString S, const Standard_Real moveX, const Standard_Real moveY);
  
  //! Returns the 2D projection of a 3D point.
  Standard_EXPORT gp_Pnt2d Project (const gp_Pnt& pt) const;
  
  //! Returns the 2D projection of a 3D point.
  Standard_EXPORT void Project (const gp_Pnt& pt, gp_Pnt2d& pt2d) const;
  
  //! Returns the current Zoom value.
  Standard_EXPORT Standard_Real Zoom() const;
  
  //! Returns the   identifier  of the  view   where the
  //! display is drawing.
  Standard_EXPORT Standard_Integer ViewId() const;
  
  //! Returs  True  if  the  last   drawing   operations
  //! generated  a pick hit.  When HasPicked is True the
  //! Drawing should be resumed.
  //!
  //! This function is used to shorten the  drawing when
  //! picking and to save the picked sub-parts.
  Standard_EXPORT Standard_Boolean HasPicked() const;




protected:





private:





};







#endif // _Draw_Display_HeaderFile
