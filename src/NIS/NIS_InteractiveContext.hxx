// File:      NIS_InteractiveContext.hxx
// Created:   06.07.07 00:18
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_InteractiveContext_HeaderFile
#define NIS_InteractiveContext_HeaderFile

#include <Handle_NIS_InteractiveObject.hxx>
#include <Handle_NIS_View.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <NIS_Drawer.hxx>
#include <NIS_SelectFilter.hxx>

#ifdef WNT
#ifdef GetObject
#undef GetObject
#endif
#endif

class NIS_View;
class Bnd_B3f;

/**
 * InteractiveContext is the central NIS structure that stores and manages
 * all InteractiveObject instances as well as the Drawers for their
 * visualisation.
 * There may be one or more Views referred by an InteractiveContext instance.
 * Also there may be one or more InteractiveContext instances referring the same
 * View. However the latter case is not typical (see NIS_View description).<br>
 * To add or remove a View in a Context, use methods AttachView() and
 * DetachView().
 * <p>The main purpose of class NIS_InteractiveContext is allocation and
 * management of NIS_InteractiveObject instances.
 * <p>An InteractiveObject should be added to the Context by a call to method
 * Display(). After that (not before) it becomes possible to:
 * <ul>
 * <li>change the presentation of the InteractiveObject (e.g., modify the color)
 *     </li>
 * <li>make the InteractiveObject visible or invisible;</li>
 * <li>set Transparency;</li>
 * <li>select InteractiveObject interactively, including the hilighting and
 *     the dynamic hilighting.</li>
 * </ul>
 * All methods managing InteractiveObject instances have the optional parameter
 * 'isUpdateViews'. When it is set to True (default), the modification of the
 * object brings about the immediate update of the presentation (the
 * corresponding Drawer is flagged to recompute presentations). However, for
 * block operations when many InteractiveObject instances are affected it is
 * usually better to delay this recalculation till a definite moment in the end
 * of updates. Then you can set the optional parameter to False and in the end -
 * call the method UpdateViews of the NIS_InteractiveContext.
 * <p>Typical scheme of usage:
 * @code
 *   const Handle(NIS_InteractiveContext) aContext = new NIS_InteractiveContext;
 *   const Handle(NIS_View) aView = new NIS_View(...);
 *   aContext->AttachView (aView);
 *   ....
 *   for (; ;) {
 *     const Handle(NIS_InteractiveObject) anObject = new ...
 *     aContext->Display (anObject, NULL, Standard_False);
 *     ...
 *   }
 *   aContext->UpdateViews();
 * @endcode
 * Additional services provided by InteractiveContext:
 * <ul>
 * <li>Every instance of this class maintains a distinct
 *     NCollection_IncAllocator that can be used by contained
 *     NIS_InteractiveObject and NIS_Drawer instances. For example, each Object
 *     may be assigned a name using the method SetAttribute, like:
 * @code
 *     Standard_Size aLen = strlen(theName)+1;
 *     char * aName = (char *) myContext->Allocator()->Allocate(aLen);
 *     memcpy (aName, theName, aLen);
 *     myObject->SetAttribute(aName);
 * @endcode
 * </li>
 * </ul>
 */

class NIS_InteractiveContext : public Standard_Transient
{
 public:
  typedef enum {
    Mode_NoSelection = 0,       ///< Seelction is totally disabled
    Mode_Normal,                ///< Selected are added to or removed from list
    Mode_Additive,              ///< Selected are added to the list of selected
    Mode_Exclusive              ///< Selected are removed from the list
  } SelectionMode;

 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  Standard_EXPORT NIS_InteractiveContext ();

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_InteractiveContext ();

  ///@name Management of Views
  //@{
  /**
   * Associate this Context with the given View.
   */
  Standard_EXPORT void AttachView (const Handle_NIS_View& theView);

  /**
   * Stop the association of the Context with the given View.
   */
  Standard_EXPORT void DetachView (const Handle_NIS_View& theView);
  //@}

  /**
   * Query the InteractiveObject instance by its ID.
   */
  inline const Handle_NIS_InteractiveObject&
                       GetObject  (const Standard_Integer theID) const
  { return myObjects(theID); }

  /**
   * Query the total number of InteractiveObject instances. This number can be
   * smaller than the greatest object ID, therefore you should not iterate till
   * this number using GetObject; use class NIS_ObjectsIterator instead.
   */
  inline Standard_Integer
                       NbObjects  ()
//  { return myObjects.Length()-1; }
  { return (myMapObjects[0].Extent() + myMapObjects[1].Extent() +
            myMapObjects[2].Extent()); }

  /**
   * Query the total number of Drawers instances.
   */
  inline Standard_Integer
                       NbDrawers  ()
  { return myDrawers.Size(); }

  /**
   * Query the memory allocator associated with InteractiveContext instance.
   */ 
  inline const Handle_NCollection_IncAllocator&
                        Allocator  () const
  { return myAllocator; }

  // ================ BEGIN Mangement of Objects ================
  ///@name Management of Objects
  //@{

  /**
   * Make the given interactive object visible in the current context.
   * If the object is not yet added to this context, it is added. Therefore
   * this method should follow the creation of an InteractiveObject instance
   * before it can be displayed.
   * @param theObj
   *   Interactive object instance.
   * @param theDrawer
   *   If this parameter is NULL, the default drawer is used for theObj, defined
   *   by the object type. Otherwise the given Drawer (must be present in this
   *   context) is used for theObj. Use the parameter to change the presentation
   *   of theObj.
   * @param isUpdateViews
   *   If True, the drawer receives isUpdate flag, then it will recompute
   *   the presentations when Redraw event happens. You can leave the parameter
   *   to False if you have to make a number of similar calls, then you would
   *   call UpdateViews() in the end.
   */
  Standard_EXPORT void Display    (const Handle_NIS_InteractiveObject& theObj,
                                   const Handle_NIS_Drawer& theDrawer = NULL,
                                   const Standard_Boolean isUpdateViews
                                                        = Standard_True);

  /**
   * Make the given object invisible in the current InteractiveContext.
   * @param theObj
   *   Interactive object instance. Must be already added to this context.
   * @param isUpdateViews
   *   If True, the drawer receives isUpdate flag, then it will recompute
   *   the presentations when Redraw event happens. You can leave the parameter
   *   to False if you have to make a number of similar calls, then you would
   *   call UpdateViews() in the end.
   */   
  Standard_EXPORT void Erase      (const Handle_NIS_InteractiveObject& theObj,
                                   const Standard_Boolean isUpdateViews
                                                        = Standard_True);

  /**
   * Remove the given object from its Interactive context.
   * @param theObj
   *   Interactive object instance. Must be already added to this context.
   * @param isUpdateViews
   *   If True, the drawer receives isUpdate flag, then it will recompute
   *   the presentations when Redraw event happens. You can leave the parameter
   *   to False if you have to make a number of similar calls, then you would
   *   call UpdateViews() in the end.
   */   
  Standard_EXPORT void Remove     (const Handle_NIS_InteractiveObject& theObj,
                                   const Standard_Boolean isUpdateViews
                                                        = Standard_True);

  /**
   * Make all stored InteractiveObject instances visible, equivalent to
   * calling method Display() for all contained objects.
   * @param isUpdateViews
   *   If True, the drawer receives isUpdate flag, then it will recompute
   *   the presentations when Redraw event happens. You can leave the parameter
   *   to False if you have to make a number of similar calls, then you would
   *   call UpdateViews() in the end.
   */
  Standard_EXPORT void DisplayAll (const Standard_Boolean isUpdateViews
                                                        = Standard_True);

  /**
   * Make all stored InteractiveObject instances invisible, equivalent to
   * calling method Erase() for all contained objects.
   * @param isUpdateViews
   *   If True, the drawer receives isUpdate flag, then it will recompute
   *   the presentations when Redraw event happens. You can leave the parameter
   *   to False if you have to make a number of similar calls, then you would
   *   call UpdateViews() in the end.
   */
  Standard_EXPORT void EraseAll   (const Standard_Boolean isUpdateViews
                                                        = Standard_True);

  /**
   * Clean the context of its contained objects. Drawers are not destroyed
   * however all presentations should become empty.
   * @param isUpdateViews
   *   If True, the drawer receives isUpdate flag, then it will recompute
   *   the presentations when Redraw event happens. You can leave the parameter
   *   to False if you have to make a number of similar calls, then you would
   *   call UpdateViews() in the end.
   */
  Standard_EXPORT void RemoveAll  (const Standard_Boolean isUpdateViews
                                                        = Standard_True);

  /**
   * This method signal that the presenation should be refreshed in all
   * Drawers and in all Views.
   */
  Standard_EXPORT void UpdateViews ();

  /**
   * Find the bounding box of all Objects displayed (visible) in the given View.
   * @param theBox
   *   <tt>[out]</tt> Bounding box, updated (not reinitialized!) by the object
   *   boxes.
   * @param theView
   *   View instance where the objects are displayed.
   */
  Standard_EXPORT void GetBox     (Bnd_B3f&         theBox,
                                   const NIS_View * theView) const;

  //@}
  // ================ END Mangement of Objects ================

  // ================ BEGIN Selection API ================
  ///@name Selection API
  //@{

  /**
   * Query the current selection filter. Use the method SetFilter to install it.
   * By default returns a NULL handle.
   */
  inline const Handle_NIS_SelectFilter&
                        GetFilter   () const
  { return mySelectFilter; }

  /**
   * Install a selection filter.
   */
  inline void           SetFilter   (const Handle_NIS_SelectFilter& theFilter)
  { mySelectFilter = theFilter; }

  /**
   * Query the current selection mode.
   */
  inline SelectionMode  GetSelectionMode () const
  { return mySelectionMode; }

  /**
   * Set the selection mode.
   */
  inline void           SetSelectionMode (const SelectionMode theMode)
  { mySelectionMode = theMode; }

  /**
   * Set or unset the selected state of the object, also changing its
   * hilight status.<br>
   * If mySelectionMode == Mode_NoSelection this method does nothing (returns
   * False always).<br>
   * If the given object is NULL (e.g., if the mouse was clicked on empty area),
   * then the current selection is cleared (modes Normal and Additive only).<br>
   * The selection algorithm with respect to the given object is defined by
   * the current selection mode :
   * <ul>
   * <li>Mode_Normal    - the selection state is toggled</li>
   * <li>Mode_Additive  - the object is always added to the selection</li>
   * <li>Mode_Exclusive - the object is always removed from the selection</li>
   * </ul>
   * This method does not update the views.
   * @param O
   *   Object to be selected or deselected
   * @param isMultiple
   *   If True, then the objects are not automatically deselected.
   * @return
   *   True if the selection status has been changed, False if nothing changed
   */
  Standard_EXPORT Standard_Boolean
                        ProcessSelection(const Handle_NIS_InteractiveObject& O,
                                         const Standard_Boolean     isMultiple
                                            = Standard_False);

  /**
   * Process the selection of multiple objects. Equivalent to the other
   * ProcessSelection method, on a set of objects. Particularly, the current
   * selection mode is respected.
   * @param map
   *   Container of IDs of objects to be processed
   * @param isMultiple
   *   If True, then the objects are not automatically deselected.
   */
  Standard_EXPORT void  ProcessSelection(const TColStd_PackedMapOfInteger& map,
                                         const Standard_Boolean     isMultiple
                                            = Standard_False);

  /**
   * Set or unset the selected state of the object, also changing its
   * hilight status.<br>
   * This method does not update the views.
   * @param theObj
   *   Object to be selected or deselected
   * @param isSelected
   *   true if the object should be selected, False - if deselected.
   * @return
   *   True if the selection status has been changed, False if noithing changed
   */
  Standard_EXPORT Standard_Boolean
                        SetSelected (const Handle_NIS_InteractiveObject& theObj,
                                     const Standard_Boolean isSelected
                                                = Standard_True);

  /**
   * Set the selection. Previously selected objects are deselected if they
   * are not included in the given map.
   * @param map
   *   Container of IDs of objects to be selected
   * @param isAdded
   *   If True, the given IDs are added to the current selection (nothing is
   *   deselected). If False (default) - the previous selection is forgotten.
   */
  Standard_EXPORT void  SetSelected     (const TColStd_PackedMapOfInteger& map,
                                         const Standard_Boolean isAdded
                                                = Standard_False);

  /**
   * Query if the given object is selected.
   */
  Standard_EXPORT Standard_Boolean
                        IsSelected  (const Handle_NIS_InteractiveObject& theOb);

  /**
   * Reset all previous selection.
   */
  Standard_EXPORT void  ClearSelected   ();

  /**
   * Query the set of selected objects.
   * @return
   *   Map of integer IDs of objects.
   */
  inline const TColStd_PackedMapOfInteger&
                        GetSelected     ()
  { return myMapObjects[NIS_Drawer::Draw_Hilighted]; }

  /**
   * Define objects that can be selected by no means (isSelectable = false),
   * or make the objects selectable (isSelectable = true).
   * @param IDs
   *   Container of IDs of objects to be set as selectable or not selectable
   * @param isSelectable
   *   If False, the given IDs are made non-selectable by SetSelected methods,
   *   If True, the given IDs are made selectable.
   */
  Standard_EXPORT void  SetSelectable   (const TColStd_PackedMapOfInteger& IDs,
                                         const Standard_Boolean isSelectable);

  /**
   * Return True if the object can be selected (processing by SetSelected
   * methods), or False if can not be.
   * @return
   *   Selectable state of the object.
   */
  inline Standard_Boolean IsSelectable  (const Standard_Integer objID) const
  { return (myMapNonSelectableObjects.Contains( objID ) == Standard_False); }


  //@}
  // ====== END Selection API ================

 protected:
  // ---------- PROTECTED METHODS ----------

  Standard_EXPORT void redraw           (const Handle_NIS_View&     theView,
                                         const NIS_Drawer::DrawType theType);

  /*Standard_EXPORT void prepareList      (const NIS_Drawer::DrawType theType,
                                         const NIS_DrawList&        theView,
                                         NIS_Drawer *               theDrawer);
  */
  /**
   * Detect the object selected by the given ray.
   * @param theSel
   *   <tt>[out]</tt> The selected object that has the lowest ray distance.
   * @param theAxis
   *   Selection ray
   * @param theOver
   *   Thickness of the selecting ray
   * @param isOnlySelectable
   *   If False, any displayed object can be picked, otherwise only selectable
   *   ones.
   * @return
   *   The ray distance of the intersection point between the ray and theSel. 
   */
  Standard_EXPORT Standard_Real
                       selectObject     (Handle_NIS_InteractiveObject& theSel,
                                         const gp_Ax1&                 theAxis,
                                         const Standard_Real           theOver,
                                         const Standard_Boolean isOnlySelectable
                                         = Standard_True) const;

  /**
   * Build a list of objects that are inside or touched by an oriented box.
   * @param mapObj
   *   <tt>[out]</tt> Container of object IDs, updated by detected objects.
   * @param theBox
   *   3D box of selection
   * @param theTrf
   *   Position/Orientation of the box (for box-box intersections)
   * @param theTrfInv
   *   Inverted Position/Orientation of the box (for box-object intersections)
   * @param isFullyIn
   *   True if only those objects are processed that are fully inside the
   *   selection rectangle. False if objects fully or partially included in
   *   the rectangle are processed. 
   * @return
   *   True if at least one object was selected.
   */
  Standard_EXPORT Standard_Boolean
                        selectObjects   (TColStd_PackedMapOfInteger& mapObj,
                                         const Bnd_B3f&        theBox,
                                         const gp_Trsf&        theTrf,
                                         const gp_Trsf&        theTrfInv,
                                         const Standard_Boolean isFullyIn)const;

 private:
  // ---------- PRIVATE FIELDS ----------

  /**
   * Container of InteractiveObject instances.
   */ 
  NCollection_Vector <Handle_NIS_InteractiveObject> myObjects;

  /**
   * List of Views.
   */
  NCollection_List   <Handle_NIS_View>              myViews;

  /**
   * Container of Drawers. There should be one or more Drawers for each type of
   * contained InteractiveObject.
   */
  NCollection_Map    <Handle_NIS_Drawer>            myDrawers;

  /**
   * Three maps indicating the state of contained objects:
   *  - #0 - normally presented objects
   *  - #1 - hilighted objects (i.e., selected)
   *  - #2 - transparent objects
   * <br>Each object can have only one entry in these maps.
   */
  TColStd_PackedMapOfInteger                        myMapObjects[3];

  /**
   * Objects contained in this map are ignored by SetSelected methods,
   * these objects are not selectable.
   */
  TColStd_PackedMapOfInteger                        myMapNonSelectableObjects;

  /**
   * Instance of selection filter used for interactive selections.
   */
  Handle_NIS_SelectFilter                           mySelectFilter;

  /**
   * Current selection.
   */
  //  TColStd_PackedMapOfInteger                    mySelection;

  /**
   * Current mode of selection.
   */
  SelectionMode                                     mySelectionMode;

  /**
   * Allocator for arbitrary data associated with objects and drawers.
   */
  Handle_NCollection_IncAllocator                   myAllocator;

  friend class NIS_View;
  friend class NIS_Drawer;
  friend class NIS_InteractiveObject;
  friend class NIS_ObjectsIterator;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_InteractiveContext)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_InteractiveContext, Standard_Transient)


#endif
