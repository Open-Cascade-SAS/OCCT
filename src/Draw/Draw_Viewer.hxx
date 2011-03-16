// File:	Draw_Viewer.hxx
// Created:	Mon Apr  6 13:12:09 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>


#ifndef Draw_Viewer_HeaderFile
#define Draw_Viewer_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_SequenceOfDrawable3D.hxx>
#include <Draw_Color.hxx>

#ifdef WNT
#include <windows.h>
#if !defined(__Draw_API) && !defined(HAVE_NO_DLL)
# ifdef __Draw_DLL
#  define __Draw_API __declspec( dllexport )
# else
#  define __Draw_API /*__declspec( dllimport )*/
# endif
#endif
#else
#  define __Draw_API  
#endif
const Standard_Integer MAXVIEW  = 30;

class Draw_View;

class Draw_Viewer {

  public :
  __Draw_API Draw_Viewer      ();
  __Draw_API Standard_Boolean DefineColor (const Standard_Integer i,
					   const char* colname);
  __Draw_API void   MakeView    (const Standard_Integer id,
				 const char*   typ,
				 const Standard_Integer X, const Standard_Integer Y, 
				 const Standard_Integer W, const Standard_Integer H);
  // build a view on a given window
#ifdef WNT
  __Draw_API void MakeView    (const Standard_Integer id,
			       const char*   typ,
			       const Standard_Integer X, const Standard_Integer Y,
			       const Standard_Integer W, const Standard_Integer H,
			       HWND win,
			       const Standard_Boolean useBuffer = Standard_False);
#endif
  __Draw_API void   MakeView    (const Standard_Integer id,
				 const char*  typ,
				 const char*  window);
  __Draw_API void   SetTitle    (const Standard_Integer id,
				 const char* name);
  __Draw_API void   ResetView   (const Standard_Integer id);
  __Draw_API void   SetZoom     (const Standard_Integer id,
				 const Standard_Real z);
  __Draw_API void   RotateView  (const Standard_Integer id,
				 const gp_Dir2d&,
				 const Standard_Real);
  __Draw_API void   RotateView  (const Standard_Integer id,
				 const gp_Pnt&,
				 const gp_Dir&,
				 const Standard_Real);
  __Draw_API void   SetFocal    (const Standard_Integer id,
				 const Standard_Real FocalDist);
  __Draw_API char*  GetType     (const Standard_Integer id) const;
  __Draw_API Standard_Real   Zoom        (const Standard_Integer id) const;
  __Draw_API Standard_Real   Focal       (const Standard_Integer id) const;
  __Draw_API void   SetTrsf     (const Standard_Integer id,
				 gp_Trsf& T);
  __Draw_API void   GetTrsf     (const Standard_Integer id,
				 gp_Trsf& T) const;
  __Draw_API void   GetPosSize  (const Standard_Integer id,
				 Standard_Integer& X, Standard_Integer& Y,
				 Standard_Integer& W, Standard_Integer& H);
  __Draw_API Standard_Boolean    Is3D        (const Standard_Integer id) const; 
  __Draw_API void   GetFrame    (const Standard_Integer id,
				 Standard_Integer& xmin, Standard_Integer& ymin,
				 Standard_Integer& xmax, Standard_Integer& ymax);
  __Draw_API void   FitView     (const Standard_Integer id, const Standard_Integer frame);
  __Draw_API void   PanView     (const Standard_Integer id,
				 const Standard_Integer DX, const Standard_Integer DY);
  __Draw_API void   SetPan      (const Standard_Integer id,
				 const Standard_Integer DX, const Standard_Integer DY);
  __Draw_API void   GetPan      (const Standard_Integer id,
				 Standard_Integer& DX, Standard_Integer& DY);
  __Draw_API Standard_Boolean HasView    (const Standard_Integer id) const;
  __Draw_API void   DisplayView (const Standard_Integer id) const;
  __Draw_API void   HideView    (const Standard_Integer id) const;
  __Draw_API void   ClearView   (const Standard_Integer id) const;
  __Draw_API void   RemoveView  (const Standard_Integer id) ;
  __Draw_API void   RepaintView (const Standard_Integer id) const;
#ifdef WNT
  __Draw_API void   ResizeView  (const Standard_Integer id) const;
  __Draw_API void   UpdateView  (const Standard_Integer id, const Standard_Boolean forced = Standard_False) const;
#endif  
  __Draw_API void   ConfigView  (const Standard_Integer id) const;
  __Draw_API void   PostScriptView (const Standard_Integer id,
				    const Standard_Integer VXmin,
				    const Standard_Integer VYmin,
				    const Standard_Integer VXmax,
				    const Standard_Integer VYmax,
				    const Standard_Integer PXmin,
				    const Standard_Integer PYmin,
				    const Standard_Integer PXmax,
				    const Standard_Integer PYmax,
				    ostream& sortie) const;
  __Draw_API void   PostColor(const Standard_Integer icol,
			      const Standard_Integer width,
			      const Standard_Real    gray);
  __Draw_API Standard_Boolean SaveView(const Standard_Integer id, const char* filename);
  __Draw_API void   RepaintAll  () const;
  __Draw_API void   Repaint2D  () const;
  __Draw_API void   Repaint3D  () const;
  __Draw_API unsigned long GetWindow   (const Standard_Integer id) const; 
  __Draw_API void   DeleteView  (const Standard_Integer id);
  __Draw_API void   Clear       ();
  __Draw_API void   Clear2D     ();
  __Draw_API void   Clear3D     ();
  __Draw_API void   Flush       ();
  
  __Draw_API void DrawOnView     (const Standard_Integer id,
				  const Handle(Draw_Drawable3D)& D) const; 
  __Draw_API void HighlightOnView (const Standard_Integer id,
				   const Handle(Draw_Drawable3D)& D,
				   const Draw_ColorKind C = Draw_blanc) const; 
  __Draw_API void AddDrawable    (const Handle(Draw_Drawable3D)& D);
  __Draw_API void RemoveDrawable (const Handle(Draw_Drawable3D)& D);
  __Draw_API Draw_Display MakeDisplay (const Standard_Integer id) const;
  
  __Draw_API void Select (Standard_Integer& id,         // View, -1 if none
			  Standard_Integer& X,          // Pick coordinates
			  Standard_Integer& Y,
			  Standard_Integer& Button,     // Button pressed, 0 if none
			  Standard_Boolean  waitclick = Standard_True
			  );

  __Draw_API Standard_Integer Pick(const Standard_Integer id,   // returns the index (or 0)
				   const Standard_Integer X,
				   const Standard_Integer Y,
				   const Standard_Integer Prec,
				   Handle(Draw_Drawable3D)& D,
				   const Standard_Integer First = 0) const; // search after this drawable
  
  __Draw_API void LastPick(gp_Pnt& P1, gp_Pnt& P2, Standard_Real& Param);
  // returns the extremities and parameter of the last picked segment
  
  __Draw_API ~Draw_Viewer();
  __Draw_API Draw_Viewer& operator<<(const Handle(Draw_Drawable3D)&);
  __Draw_API const Draw_SequenceOfDrawable3D& GetDrawables();

  private :

    Draw_View*                myViews[MAXVIEW];
    Draw_SequenceOfDrawable3D myDrawables;
};


#endif
