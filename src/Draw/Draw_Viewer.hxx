// Created on: 1992-04-06
// Created by: Remi LEQUETTE
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

#ifndef Draw_Viewer_HeaderFile
#define Draw_Viewer_HeaderFile

#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <Draw_SequenceOfDrawable3D.hxx>
#include <Draw_Color.hxx>

#ifdef _WIN32
#include <windows.h>
#endif

const Standard_Integer MAXVIEW  = 30;

class Draw_View;

class Draw_Viewer {

  public :
  Standard_EXPORT Draw_Viewer      ();
  Standard_EXPORT Standard_Boolean DefineColor (const Standard_Integer i,
					   const char* colname);
  Standard_EXPORT void   MakeView    (const Standard_Integer id,
				 const char*   typ,
				 const Standard_Integer X, const Standard_Integer Y, 
				 const Standard_Integer W, const Standard_Integer H);
  // build a view on a given window
#ifdef _WIN32
  Standard_EXPORT void MakeView    (const Standard_Integer id,
			       const char*   typ,
			       const Standard_Integer X, const Standard_Integer Y,
			       const Standard_Integer W, const Standard_Integer H,
			       HWND win,
			       const Standard_Boolean useBuffer = Standard_False);
#endif
  Standard_EXPORT void   MakeView    (const Standard_Integer id,
				 const char*  typ,
				 const char*  window);
  Standard_EXPORT void   SetTitle    (const Standard_Integer id,
				 const char* name);
  Standard_EXPORT void   ResetView   (const Standard_Integer id);
  Standard_EXPORT void   SetZoom     (const Standard_Integer id,
				 const Standard_Real z);
  Standard_EXPORT void   RotateView  (const Standard_Integer id,
				 const gp_Dir2d&,
				 const Standard_Real);
  Standard_EXPORT void   RotateView  (const Standard_Integer id,
				 const gp_Pnt&,
				 const gp_Dir&,
				 const Standard_Real);
  Standard_EXPORT void   SetFocal    (const Standard_Integer id,
				 const Standard_Real FocalDist);
  Standard_EXPORT char*  GetType     (const Standard_Integer id) const;
  Standard_EXPORT Standard_Real   Zoom        (const Standard_Integer id) const;
  Standard_EXPORT Standard_Real   Focal       (const Standard_Integer id) const;
  Standard_EXPORT void   SetTrsf     (const Standard_Integer id,
				 gp_Trsf& T);
  Standard_EXPORT void   GetTrsf     (const Standard_Integer id,
				 gp_Trsf& T) const;
  Standard_EXPORT void   GetPosSize  (const Standard_Integer id,
				 Standard_Integer& X, Standard_Integer& Y,
				 Standard_Integer& W, Standard_Integer& H);
  Standard_EXPORT Standard_Boolean    Is3D        (const Standard_Integer id) const; 
  Standard_EXPORT void   GetFrame    (const Standard_Integer id,
				 Standard_Integer& xmin, Standard_Integer& ymin,
				 Standard_Integer& xmax, Standard_Integer& ymax);
  Standard_EXPORT void   FitView     (const Standard_Integer id, const Standard_Integer frame);
  Standard_EXPORT void   PanView     (const Standard_Integer id,
				 const Standard_Integer DX, const Standard_Integer DY);
  Standard_EXPORT void   SetPan      (const Standard_Integer id,
				 const Standard_Integer DX, const Standard_Integer DY);
  Standard_EXPORT void   GetPan      (const Standard_Integer id,
				 Standard_Integer& DX, Standard_Integer& DY);
  Standard_EXPORT Standard_Boolean HasView    (const Standard_Integer id) const;
  Standard_EXPORT void   DisplayView (const Standard_Integer id) const;
  Standard_EXPORT void   HideView    (const Standard_Integer id) const;
  Standard_EXPORT void   ClearView   (const Standard_Integer id) const;
  Standard_EXPORT void   RemoveView  (const Standard_Integer id) ;
  Standard_EXPORT void   RepaintView (const Standard_Integer id) const;
#ifdef _WIN32
  Standard_EXPORT void   ResizeView  (const Standard_Integer id) const;
  Standard_EXPORT void   UpdateView  (const Standard_Integer id, const Standard_Boolean forced = Standard_False) const;
#endif  
  Standard_EXPORT void   ConfigView  (const Standard_Integer id) const;
  Standard_EXPORT void   PostScriptView (const Standard_Integer id,
				    const Standard_Integer VXmin,
				    const Standard_Integer VYmin,
				    const Standard_Integer VXmax,
				    const Standard_Integer VYmax,
				    const Standard_Integer PXmin,
				    const Standard_Integer PYmin,
				    const Standard_Integer PXmax,
				    const Standard_Integer PYmax,
				    std::ostream& sortie) const;
  Standard_EXPORT void   PostColor(const Standard_Integer icol,
			      const Standard_Integer width,
			      const Standard_Real    gray);
  Standard_EXPORT Standard_Boolean SaveView(const Standard_Integer id, const char* filename);
  Standard_EXPORT void   RepaintAll  () const;
  Standard_EXPORT void   Repaint2D  () const;
  Standard_EXPORT void   Repaint3D  () const;
  Standard_EXPORT void   DeleteView  (const Standard_Integer id);
  Standard_EXPORT void   Clear       ();
  Standard_EXPORT void   Clear2D     ();
  Standard_EXPORT void   Clear3D     ();
  Standard_EXPORT void   Flush       ();
  
  Standard_EXPORT void DrawOnView     (const Standard_Integer id,
				  const Handle(Draw_Drawable3D)& D) const; 
  Standard_EXPORT void HighlightOnView (const Standard_Integer id,
				   const Handle(Draw_Drawable3D)& D,
				   const Draw_ColorKind C = Draw_blanc) const; 
  Standard_EXPORT void AddDrawable    (const Handle(Draw_Drawable3D)& D);
  Standard_EXPORT void RemoveDrawable (const Handle(Draw_Drawable3D)& D);
  Standard_EXPORT Draw_Display MakeDisplay (const Standard_Integer id) const;
  
  Standard_EXPORT void Select (Standard_Integer& id,         // View, -1 if none
			  Standard_Integer& X,          // Pick coordinates
			  Standard_Integer& Y,
			  Standard_Integer& Button,     // Button pressed, 0 if none
			  Standard_Boolean  waitclick = Standard_True
			  );

  Standard_EXPORT Standard_Integer Pick(const Standard_Integer id,   // returns the index (or 0)
				   const Standard_Integer X,
				   const Standard_Integer Y,
				   const Standard_Integer Prec,
				   Handle(Draw_Drawable3D)& D,
				   const Standard_Integer First = 0) const; // search after this drawable
  
  Standard_EXPORT void LastPick(gp_Pnt& P1, gp_Pnt& P2, Standard_Real& Param);
  // returns the extremities and parameter of the last picked segment
  
  Standard_EXPORT ~Draw_Viewer();
  Standard_EXPORT Draw_Viewer& operator<<(const Handle(Draw_Drawable3D)&);
  Standard_EXPORT const Draw_SequenceOfDrawable3D& GetDrawables();

  private :

    Draw_View*                myViews[MAXVIEW];
    Draw_SequenceOfDrawable3D myDrawables;
};


#endif
