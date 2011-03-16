// File:      NIS_View.hxx
// Created:   06.07.07 10:08
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_View_HeaderFile
#define NIS_View_HeaderFile

#include <Handle_NIS_InteractiveObject.hxx>
#include <V3d_OrthographicView.hxx>
#include <Standard_DefineHandle.hxx>
#include <NCollection_List.hxx>

class NIS_InteractiveContext;
class gp_Ax1;

/**
 * Manager of a single window with OpenGL context, used by one or more
 * NIS_InteractiveContext instances.
 * This class inherits V3d_OrthograpicView therefore its instances can be used
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

class NIS_View : public V3d_OrthographicView
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Attach the view to the window.
   */
  Standard_EXPORT NIS_View (const Handle_V3d_Viewer&    theViewer,
                            const Handle_Aspect_Window& theWindow = NULL);

  /**
   * Attach the view to the window.
   */
  Standard_EXPORT void SetWindow(const Handle_Aspect_Window &theWindow);

  /**
   * Zoom the view to fit to visible objects size and positions.
   */
  Standard_EXPORT void FitAll3d ();

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
  Standard_EXPORT void  DynamicUnhilight(const Handle_NIS_InteractiveObject&);

  /**
   * Set or unset the selected (hilighted) state of the object that is under
   * the coordinates theX, theY.
   * @param theX
   *   X coordinate of the view window
   * @param theX
   *   X coordinate of the view window
   * @param isForceMult
   *   True if the effect of multi-Selection should be forced (e.g., when Shift
   *   is pressed).
   */
  Standard_EXPORT void  Select          (const Standard_Integer theX,
                                         const Standard_Integer theY,
                                         const Standard_Boolean isForceMult
                                                         = Standard_False);

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
   */
  Standard_EXPORT void  Select          (const Standard_Integer  theXmin,
                                         const Standard_Integer  theYmin,
                                         const Standard_Integer  theXmax,
                                         const Standard_Integer  theYmax,
                                         const Standard_Boolean  isForceMult
                                                         = Standard_False,
                                         const Standard_Boolean isFullyIncluded
                                                         = Standard_False);

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
  Standard_EXPORT Handle_NIS_InteractiveObject
                        Pick            (const Standard_Integer theX,
                                         const Standard_Integer theY) const;

  /**
   * Interactive selection by mouse click. Selection itself is performed in each
   * Interactive Context attached to this View, accordingto the local parameters
   * (mode, filter) of selection in each context. 
   * @param theAxis
   *   3D axis for objects selection
   * @param theOver
   *   Overlap for the selecting axis
   * @param isOnlySelectable
   *   If False, any displayed object can be picked, otherwise only selectable
   *   ones.
   * @return
   *   the selected object picked by the minimal intersection distance among
   *   all contexts attached to this View.
   */
  Standard_EXPORT Handle_NIS_InteractiveObject
                        Pick    (const gp_Ax1&          theAxis,
                                 const Standard_Real    theOver,
                                 const Standard_Boolean isOnlySelectable) const;

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
  Handle_NIS_InteractiveObject                     myDynHilighted;

  friend class NIS_InteractiveContext;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_View)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
//DEFINE_STANDARD_HANDLE (NIS_View, V3d_OrthographicView)

#include <Handle_NIS_View.hxx>

#endif
