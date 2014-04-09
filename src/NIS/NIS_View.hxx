// Created on: 2007-07-06
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef NIS_View_HeaderFile
#define NIS_View_HeaderFile

#include <Handle_NIS_InteractiveObject.hxx>
#include <V3d_View.hxx>
#include <Standard_DefineHandle.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>
#include <Bnd_B3f.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <gp_XY.hxx>

class NIS_InteractiveContext;
class gp_Ax1;

/**
 * Manager of a single window with OpenGL context, used by one or more
 * NIS_InteractiveContext instances.
 * This class inherits V3d_View therefore its instances can be used
 * to display any object that is normally handled by Graphic3d/V3d/AIS classes.
 * Also the standard view operations: Pan, Rotate, Zoom, mouse API, etc. are
 * supported due to this inheritance.<p>
 * The new services added by this class is the visualisation of
 * NIS_InteractiveObject (i.e., accommodation of NIS_InteractiveContext
 * instances). This visualisation is done by the method MyCallback that is
 * called just before redrawing the scene by Graphic3d/OpenGl drivers.<p>
 * The View instance can be understood as a collector of several interactive
 * contexts that exist independently (do not interfere) though they output
 * into the same OpenGL context. The contexts are:
 * <ul>
 *  <li>AIS_InteractiveContext associated with the view by means of V3d_Viewer
 *  instance (the traditional AIS architecture);</li>
 *  <li>1st associated NIS_InteractiveContext</li>
 *  <li>2nd associated NIS_InteractiveContext</li>
 *  <li>...</li>
 * </ul>
 * The list of associated NIS_InteractiveContext can be modified by methods
 * AddContext and RemoveContext.
 */

class NIS_View : public V3d_View
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Attach the view to the window.
   */
  Standard_EXPORT NIS_View (const Handle(V3d_Viewer)&    theViewer,
                            const Handle(Aspect_Window)& theWindow = NULL);

  /**
   * Attach the view to the window.
   */
  Standard_EXPORT void SetWindow(const Handle(Aspect_Window) &theWindow);

  /**
   * Indicate whether to draw hilighted objects on top of all other ones
   */
  inline void SetHilightOnTop(const Standard_Boolean theTop = Standard_True)
  { myIsTopHilight = theTop; }

  /**
   * Indicate whether to hilight selected object dynamically
   * By default dynamic hilight works on all objects independently on its
   * selected/non-selected state.
   * This behaviour differs from the behaviour of AIS interactive context,
   * that doesn't hilight dynamically (on mouse movements) selected objects. 
   * In the case both context are used in the same view the behaviour of both
   * context can be made consistent by setting this flag to False
   */
  inline void SetDynHilightSelected     (const Standard_Boolean
                                         theHilight = Standard_True)
  { myDoHilightSelected = theHilight; }

  /**
   * Zoom the view to fit to visible objects size and positions.
   * @param theCoef
   *   Relative margin in both X and Y dimensions. For example, value 1.0
   *   will fit to twice the actual size.
   * @return
   *   True if operation performed, False if failed (most likely because of
   *   very big actual scale)
   */
  Standard_EXPORT Standard_Boolean FitAll3d (const Quantity_Coefficient theCoef
                                             = 0.01);

  /**
   * Gets bounding box covering objects displayed in viewer.
   */
  Standard_EXPORT Bnd_B3f GetBndBox() const;

  /**
   * Gets bounding box covering objects displayed in viewer.
   * If operation is fails when Xmax < Xmin abd Ymax < Ymin
   */
  Standard_EXPORT void GetBndBox(Standard_Integer& theXMin,
                                 Standard_Integer& theXMax, 
                                 Standard_Integer& theYMin,
                                 Standard_Integer& theYMax ) const;

//   /**
//    * Destructor.
//    */
//   Standard_EXPORT virtual ~NIS_View ();

  /**
   * Set or unset the dynamically hilighted object, depending on the
   * given mouse coordinates.
   */
  Standard_EXPORT void  DynamicHilight  (const Standard_Integer theX,
                                         const Standard_Integer theY);

  /**
   * Unhilights the hilighted object if it coincides with the given
   * object instance.
   */
  Standard_EXPORT void  DynamicUnhilight(const Handle(NIS_InteractiveObject)&);

  /**
   * Unhilights the currently hilighted object.
   */
  inline void  DynamicUnhilight()  { DynamicUnhilight(myDynHilighted); }

  /**
   * Set or unset the selected (hilighted) state of the object that is under
   * the coordinates theX, theY.
   * @param theX
   *   X coordinate of the view window
   * @param theY
   *   X coordinate of the view window
   * @param isForceMult
   *   True if the effect of multi-Selection should be forced (e.g., when Shift
   *   is pressed).
   * @param theRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void  Select          (const Standard_Integer theX,
                                         const Standard_Integer theY,
                                         const Standard_Boolean isForceMult
                                                         = Standard_False,
                                         const Standard_Boolean theRedraw
                                                         = Standard_True);

  /**
   * Set or unset the selected (hilighted) state of the objects that are
   * intersected by 2D box in the view
   * @param theXmin
   *   left X coordinate of the view window
   * @param theYmin
   *   lower Y coordinate of the view window
   * @param theXmax
   *   right X coordinate of the view window
   * @param theYmax
   *   upper Y coordinate of the view window
   * @param isForceMult
   *   True if the effect of multi-Selection should be forced (e.g., when Shift
   *   is pressed).
   * @param isFullyIncluded
   *   True if only those objects are processed that are fully inside the
   *   selection rectangle. False if objects fully or partially included in
   *   the rectangle are processed. 
   * @param theRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void  Select          (const Standard_Integer  theXmin,
                                         const Standard_Integer  theYmin,
                                         const Standard_Integer  theXmax,
                                         const Standard_Integer  theYmax,
                                         const Standard_Boolean  isForceMult
                                                         = Standard_False,
                                         const Standard_Boolean isFullyIncluded
                                                         = Standard_False,
                                         const Standard_Boolean theRedraw
                                                         = Standard_True);

  /**
   * Set or unset the selected (hilighted) state of the objects that are
   * intersected by 2D polygon in the view
   * @param thePolygon
   *   defines the vertices of a free-form closed polygon without
   *   self-intersections. The last point should not coincide with the first
   *   point of the list. Points are interpreted as X and Y integer coordinates
   *   of the view window. Any two neighbor points should not be confused.
   * @param isForceMult
   *   True if the effect of multi-Selection should be forced (e.g., when Shift
   *   is pressed).
   * @param isFullyIncluded
   *   True if only those objects are processed that are fully inside the
   *   selection rectangle. False if objects fully or partially included in
   *   the rectangle are processed. 
   * @param theRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void  Select   (const NCollection_List<gp_XY> &thePolygon,
                                  const Standard_Boolean         isForceMult
                                                         = Standard_False,
                                  const Standard_Boolean         isFullyIncluded
                                                         = Standard_False,
                                  const Standard_Boolean theRedraw
                                                         = Standard_True);

  /**
   * Interactive selection by mouse click. Selection itself is performed in each
   * Interactive Context attached to this View, accordingto the local parameters
   * (mode, filter) of selection in each context. 
   * @param theX
   *   X coordinate of mouse pointer in the window.
   * @param theY
   *   Y coordinate of mouse pointer in the window.
   * @return
   *   the selected object picked by the minimal intersection distance among
   *   all contexts attached to this View.
   */
  Standard_EXPORT Handle(NIS_InteractiveObject)
                        Pick            (const Standard_Integer theX,
                                         const Standard_Integer theY);

  /**
   * Interactive selection by mouse click. Selection itself is performed in each
   * Interactive Context attached to this View, accordingto the local parameters
   * (mode, filter) of selection in each context. 
   * @param theAxis
   *   3D axis for objects selection
   * @param theOver
   *   Overlap for the selecting axis
   * @param isOnlySel
   *   If False, any displayed object can be picked, otherwise only selectable
   *   ones.
   * @return
   *   the selected object picked by the minimal intersection distance among
   *   all contexts attached to this View.
   */
  Standard_EXPORT Handle(NIS_InteractiveObject)
                        Pick            (const gp_Ax1&          theAxis,
                                         const Standard_Real    theOver,
                                         const Standard_Boolean isOnlySel);

  /**
   * Gets all objects detected by last call of Pick() method
   */
  inline NCollection_Vector<NIS_InteractiveObject *> GetDetected() const
  { return myDetected; }

  /**
   * Obtain the IDs of ex-lists.
   */
  inline TColStd_PackedMapOfInteger& GetExListId ()
  { return myExListId; }

 protected:
  // ---------- PROTECTED METHODS ----------

  static int MyCallback (Aspect_Drawable                /* Window ID */,
                         void*                          /* user data */, 
                         Aspect_GraphicCallbackStruct*  /* call data */);

  /**
   * Register an interactive context for this view.
   */
  Standard_EXPORT void  AddContext      (NIS_InteractiveContext *);

  /**
   * Detach an interactive context from this view.
   */
  Standard_EXPORT void  RemoveContext   (NIS_InteractiveContext *);

 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List<NIS_InteractiveContext *>       myContexts;
  Handle(NIS_InteractiveObject)                     myDynHilighted;
  Standard_Boolean                                 myIsTopHilight      : 1;
  Standard_Boolean                                 myDoHilightSelected : 1;
  NCollection_Vector<NIS_InteractiveObject *>      myDetected;
  TColStd_PackedMapOfInteger                       myExListId;

  friend class NIS_InteractiveContext;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_View)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
//DEFINE_STANDARD_HANDLE (NIS_View, V3d_OrthographicView)

#include <Handle_NIS_View.hxx>

#endif
