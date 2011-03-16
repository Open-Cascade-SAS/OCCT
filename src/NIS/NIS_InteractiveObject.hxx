// File:      NIS_InteractiveObject.hxx
// Created:   06.07.07 23:26
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_InteractiveObject_HeaderFile
#define NIS_InteractiveObject_HeaderFile

#include <NIS_Drawer.hxx>
#include <Bnd_B3f.hxx>

/**
 * Abstract base type for presentable and selectable object in NIS architecture.
 * @section InteractiveObject_class
 * An InteractiveObject has the attributes:
 * <ul>
 * <li>Integer ID that is unique within the Interactive Context that hosts
 *     this Object. Thsi ID is 0 if the Object is not yet attached to an
 *     InteractiveContext</li>
 * <li>3D Bounding box</li>
 * <li>Presenatble state of the Object: Normal, Hilighted or Transparent.</li>
 * <li>Visibility state (shown/hidden)</li>
 * <li>Transparency level (0 to 1) - for transparent state</li>
 * </ul>
 * Because the class is abstract, it does not define any color, material and
 * other visual aspect - all relevant aspects should be defined in derived
 * classes.<br>
 * Every InteractiveObject type should have an associated NIS_Drawer type; a
 * new instance of this associated drawer is returned by the virtual method
 * DefaultDrawer(). The drawer is responsible for the correct calculation of
 * the presentation in every possible state (normal, hilighted, etc.); usually
 * the associated drawer instance contains all relevant visual aspects.<p>
 * Association with a Drawer instance is performed by method SetDrawer. This
 * method should not be called by any custom code, it is used internally by
 * NIS algorithms (in NIS_InteractiveContext::Display() for instance). If you
 * develop your own InteractiveObject type, you will need to call SetDrawer
 * whenever you change the visual aspect, for example:
 * @code
 * void MyIOClass::SetColor (const Quantity_Color&  theColor);
 * {
 *   Handle(MyIOClassDrawer) aDrawer = new MyIOClassDrawer;
 * // copy the current visual aspects and other attributes to the new Drawer
 *   aDrawer->Assign (GetDrawer());
 * // replace the Drawer
 *   aDrawer->myColor = theColor;
 *   SetDrawer (aDrawer);
 * // optional: redraws the changed InteractiveObject in the views
 *   GetDrawer()->GetContext()->UpdateViews();
 * }
 * @endcode
 * <p>
 * An instance of this class can have an associated value (Attribute) that is
 * stored as a pointer. It can accommodate an integer/float/boolean value or
 * a pointer to some structure. This attribute is NOT automatically destroyed
 * with the InteractiveObject.  
 */

class NIS_InteractiveObject : public Standard_Transient
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor. Creates an object that is not attached to drawer.
   */
  inline NIS_InteractiveObject ()
    : myID              (0),
      myDrawType        (NIS_Drawer::Draw_Normal),
      myIsHidden        (Standard_True),
      myIsDynHilighted  (Standard_False),
      myIsUpdateBox     (Standard_True),
      myTransparency    (0.f),
      myAttributePtr    (0L)
  {}

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_InteractiveObject ();

  /**
   * Query the ID of the Object in its Context.
   */
  inline Standard_Integer ID            () const
  { return Standard_Integer (myID); }

  /**
   * Query the type of presentation.
   */
  inline NIS_Drawer::DrawType
                          DrawType      () const
  { return myDrawType; }

  /**
   * Replace the drawer. This method must not be called for Object that
   * has not yet been added to a Context (thus has empty drawer).<br>
   * It is possible to have unassigned myDrawer or a DefaultDrawer as the
   * parameter value (but not both). The Context where we work is taken
   * from theDrawer first, then (if NULL) -- from myDrawer.<br>
   * This method matches theDrawer with the available Drawers in the Context
   * and adds if no match is found.
   * @return
   *   Reference to the finally stored or found Drawer instance inside
   *   the Context. 
   */
  Standard_EXPORT const Handle_NIS_Drawer&
                          SetDrawer     (const Handle_NIS_Drawer& theDrawer);

  /**
   * Query the current drawer.
   */
  inline const Handle_NIS_Drawer&
                          GetDrawer     () const
  { return myDrawer; }

  /**
   * Create a default drawer instance.
   */
  Standard_EXPORT virtual Handle_NIS_Drawer
                          DefaultDrawer () const = 0;

  /**
   * Query a 3D bounding box of the object.
   */
  Standard_EXPORT const Bnd_B3f&
                          GetBox        ();

  /**
   * Query the Transparent state.
   */
  inline Standard_Boolean IsTransparent () const
  { return myTransparency > 0.001; }

  /**
   * Query the Hidden state
   */
  inline Standard_Boolean IsHidden      () const
  { return myIsHidden; }

  /**
   * Query the Displayed state - opposite to IsHidden().
   */
  inline Standard_Boolean IsDisplayed   () const
  { return !myIsHidden; }

  /**
   * Query the Dynamic Hilight state
   */
  inline Standard_Boolean IsDynHilighted() const
  { return myIsDynHilighted; }

  /**
   * Query if the Object is selectable.
   */
  Standard_EXPORT Standard_Boolean
                          IsSelectable  () const;

  /**
   * Set or change the selectable state of the Object.
   * @param isSel
   *   True (default) - the Object will be selectable, False - it will be
   *   ignored by selection/hilighting algorithms.
   */
  Standard_EXPORT void    SetSelectable (const Standard_Boolean isSel
                                         = Standard_True) const; 

  /**
   * Query the Transparency factor.
   */
  inline Standard_Real    Transparency  () const
  { return myTransparency; }

  /**
   * Set the Transparency factor.
   */
  Standard_EXPORT void    SetTransparency (const Standard_Real theValue = 0.6);

  /**
   * Present the Object as opaque (Normal draw type).
   */
  inline void             UnsetTransparency ()
  { SetTransparency (0.); }

  /**
   * Intersect the InteractiveObject geometry with a line/ray.
   * @param theAxis
   *   The line or ray in 3D space.
   * @param theOver
   *   Half-thickness of the selecting line.
   * @return
   *   If the return value is more than 0.1*RealLast() then no intersection is
   *   detected. Otherwise returns the coordinate of thePnt on the ray. May be
   *   negative.
   */
  Standard_EXPORT virtual Standard_Real
                          Intersect     (const gp_Ax1&       theAxis,
                                         const Standard_Real theOver)const = 0;

  /**
   * Intersect the InteractiveObject geometry with an oriented box.
   * The default implementation (in this abstract class) always returns True,
   * signalling that every object pre-selected by its bounding box is
   * automatically selected. The specializations should define a more correct
   * behaviour.<br>
   * The algorithm should transform the InteractiveObject geometry using the
   * parameter theTrf and then reject it with box theBox, like:
   * @code
   *    gp_Pnt aPnt = ..... // aPnt is part of our geometry.
   *    if (!theBox.IsOut (aPnt.Transformed(theTrf)))
   *      return Standard_True;
   * @endcode
   * @param theBox
   *   3D box of selection
   * @param theTrf
   *   Position/Orientation of the box. It coincides with theTrfInv that is
   *   passed to NIS_InteractiveObject::selectObjects().
   * @param isFull
   *   True if full inclusion is required (full inside the tested box) for
   *   the positive result, False - if only partial inclusion give a result.
   * @return
   *   True if the InteractiveObject geometry intersects the box or is inside it
   */
  Standard_EXPORT virtual Standard_Boolean
                          Intersect     (const Bnd_B3f&         theBox,
                                         const gp_Trsf&         theTrf,
                                         const Standard_Boolean isFull) const;

  /**
   * Set the pointer to custom (arbitrary) data associated with the Object.
   */
  inline void             SetAttribute  (void * theAttributePtr)
  { myAttributePtr = theAttributePtr; }

  /**
   * Query the associated custom (arbitrary) attribute pointer.
   */
  inline void *           GetAttribute  () const
  { return myAttributePtr; }

 protected:
  // ---------- PROTECTED METHODS ----------

  inline void             setDrawerUpdate  () const
  { myDrawer->SetUpdated (myDrawType); }

  /**
   * Create a 3D bounding box of the object.
   */
  Standard_EXPORT virtual void
                          computeBox       () = 0;

  inline Standard_Boolean isUpdateBox      () const
  { return myIsUpdateBox; }

  inline void             setIsUpdateBox   (const Standard_Boolean isUpdate)
  { myIsUpdateBox = isUpdate; }

 private:
  // ---------- PRIVATE (PROHIBITED) METHODS ----------

  NIS_InteractiveObject             (const NIS_InteractiveObject& theOther);
  NIS_InteractiveObject& operator = (const NIS_InteractiveObject& theOther);

 private:
  // ---------- PRIVATE FIELDS ----------

  Handle_NIS_Drawer             myDrawer;
  Standard_Size                 myID            : 26;
  NIS_Drawer::DrawType          myDrawType      : 2;
  Standard_Boolean              myIsHidden      : 1;
  Standard_Boolean              myIsDynHilighted: 1;

  Standard_Boolean              myIsUpdateBox   : 1;
  Standard_ShortReal            myTransparency;

protected:
  Bnd_B3f                       myBox;
  void                          * myAttributePtr;

  friend class NIS_InteractiveContext;
  friend class NIS_Drawer;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_InteractiveObject)
};

#include <Handle_NIS_InteractiveObject.hxx>

#endif
