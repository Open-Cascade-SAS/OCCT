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
 *     this Object. This ID is 0 if the Object is not yet attached to an
 *     InteractiveContext</li>
 * <li>3D Bounding box</li>
 * <li>Presentable state of the Object: Normal, Hilighted or Transparent.</li>
 * <li>Visibility state (shown/hidden)</li>
 * <li>Selectability (selectable/non-selectable)</li>
 * <li>Transparency level (0 to 1 in 1/1000 steps) - for transparent state</li>
 * </ul>
 * Because the class is abstract, it does not define any color, material and
 * other visual aspect - all relevant aspects should be defined in derived
 * classes and their Drawers.
 *
 * @section nis_interactiveobject_drawer Drawers for NIS_InteractiveObject
 * Every InteractiveObject type should have an associated NIS_Drawer type; a
 * new instance of this associated drawer must be returned by the virtual method
 * DefaultDrawer(). The drawer is responsible for the correct calculation of
 * the presentation in every possible state (normal, hilighted, etc.); usually
 * the associated drawer instance contains all relevant visual aspects.
 * <p>
 * Association with a Drawer instance is performed by method SetDrawer(). This
 * method should not be called by any custom code, it is used internally by
 * NIS algorithms (in NIS_InteractiveContext::Display() for instance). If you
 * develop your own InteractiveObject type, you will need to call SetDrawer
 * whenever you change the visual aspect, for example:
 * @code
 * void MyIOClass::SetColor (const Quantity_Color&  theColor);
 * {
 *   const Handle(MyIOClassDrawer) aDrawer =
 *     static_cast<MyIOClassDrawer*>(DefaultDrawer(0L));
 * // copy the current visual aspects and other attributes to the new Drawer
 *   aDrawer->Assign (GetDrawer());
 * // replace the Drawer
 *   aDrawer->myColor = theColor;
 *   SetDrawer (aDrawer);
 * }
 * @endcode
 * Please keep in mind that with this scheme you should not store the color in
 * MyIOClass type, because it is already stored in its Drawer. 
 *
 * @section nis_interactiveobject_selection Interactive selection
 * Interactive selection is made in class NIS_InteractiveContext, methods
 * selectObjects(). These methods call the virtual API of interactive object,
 * that consists of 3 methods:
 * <ul>
 * <li>Intersect (theAxis, theOver) : find the intersection point with a 3D ray,
 *     the method returns the coordinate of intersection point on the ray.
 *     Parameter theOver provides the tolerance for intersection of thin
 *     geometries (lines, vertices)</li>
 * <li>Intersect (theBox, theTrsf, isFullIn) : check if the interactive object
 *     intersects with a 3D box. Transformation 'theTrf' is the <b>inverse</b>
 *     box transformation, so it is applied to the interactive object rather
 *     than to the 3D box (3D box stays axis-aligned during intersection
 *     test). Parameter IsFullIn defines the condition for the result: if
 *     True then the whole interactive object must be contained inside the box,
 *     otherwise it is sufficient if only a portion (e.g., a point) is inside.
 *     This method is used for interactive rectangle selection.</li>
 * <li>Intersect (thePolygon, theTrsf, isFullIn) : similar to the previous
 *     method, but using a polygonal prism instead of box, for selection by
 *     closed curve or polygon.</li>
 * </ul>
 * 
 * @section nis_interactiveobject_memory Memory management
 * All data used in the scope of NIS_InteractiveObject subtype should be either
 * its explicit fields or pointers to memory managed by a special NIS_Allocator
 * instance that belongs to NIS_InteractiveContext. This is strictly required
 * because NIS_InteractiveContext should completely manage all its objects,
 * meaning that it destroys/reallocates them automatically. To support that,
 * the virtual method Clone() should be correctly defined for every interactive
 * object subtype. Supposing that MyIOClass inherits MyBaseIOBase :
 * @code
 * void MyIOCalss::Clone (const Handle(NCollection_BaseAllocator)& theAlloc,
 *                        Handle(NIS_InteractiveObject)&           theDest) const
 * {
 *   Handle(MyIOClass) aNewObj;
 *   if (theDest.IsNull()) {
 *     aNewObj = new MyIOClass();
 *     theDest = aNewObj;
 *   } else {
 *     aNewObj = reinterpret_cast<MyIOClass*> (theDest.operator->());
 *     aNewObj->myAlloc = theAlloc;
 *   }
 *   MyIOBase::Clone(theAlloc, theDest);
 *   aNewObj->myDataField = myDataField;
 *   memcpy(myNewObj->myDataArray, myDataArray, nBytes);
 *   ...
 * }
 * @endcode
 *
 * @section nis_interactiveobject_attribute Attribute
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
      myBaseType        (NIS_Drawer::Draw_Normal),
      myIsHidden        (Standard_True),
      myIsDynHilighted  (Standard_False),
      myIsUpdateBox     (Standard_True),
      myTransparency    (0),
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
   * has not yet been added to a Context (thus has empty drawer).
   * It is possible to have unassigned myDrawer or a DefaultDrawer as the
   * parameter value (but not both). The Context where we work is taken
   * from theDrawer first, then (if NULL) -- from myDrawer.
   * This method matches theDrawer with the available Drawers in the Context
   * and adds if no match is found.
   * @return
   *   Reference to the finally stored or found Drawer instance inside
   *   the Context. 
   */
  Standard_EXPORT const Handle(NIS_Drawer)&
                          SetDrawer     (const Handle(NIS_Drawer)& theDrawer,
                                         const Standard_Boolean   setUpdated
                                         = Standard_True);

  /**
   * Query the current drawer.
   */
  inline const Handle(NIS_Drawer)&
                          GetDrawer     () const
  { return myDrawer; }

  /**
   * Create a default drawer instance. In the upper-level call (in subclass)
   * it is always called with NULL parameter. Then it should call the same
   * method of the superclass (except for NIS_InteractiveObject superclass type)
   * with the created Drawer instance as parameter.
   * @see NIS_Triangulated as example.
   */
  Standard_EXPORT virtual NIS_Drawer *
                          DefaultDrawer (NIS_Drawer * theDrv) const = 0;

  /**
   * Query a 3D bounding box of the object.
   */
  Standard_EXPORT const Bnd_B3f&
                          GetBox        ();

  /**
   * Query the Transparent state.
   */
  inline Standard_Boolean IsTransparent () const
  { return myTransparency > 0; }

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
  Standard_EXPORT virtual Standard_Boolean
                          IsSelectable  () const;

  /**
   * Set or change the selectable state of the Object.
   * @param isSel
   *   True (default) - the Object will be selectable, False - it will be
   *   ignored by selection/hilighting algorithms.
   */
  Standard_EXPORT virtual void    
                          SetSelectable (const Standard_Boolean isSel
                                         = Standard_True) const; 

  /**
   * Query the Transparency factor.
   */
  inline Standard_ShortReal Transparency  () const
  { return static_cast<Standard_ShortReal>(myTransparency) / MaxTransparency; }

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
   * Create a copy of theObject except its ID.
   * @param theAll
   *   Allocator where the Dest should store its private data.
   * @param theDest
   *   <tt>[in-out]</tt> The target object where the data are copied.
   */
  Standard_EXPORT virtual void
                          Clone (const Handle(NCollection_BaseAllocator)& theAll,
                                 Handle(NIS_InteractiveObject)& theDest) const;

  /**
   * The same as Clone() but also copies the ID.
   */
  Standard_EXPORT void    CloneWithID (const Handle(NCollection_BaseAllocator)&,
                                       Handle(NIS_InteractiveObject)&);

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
   * automatically selected. The specializations should define a more correct behaviour.
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
   * Intersect the InteractiveObject geometry with a selection polygon.
   * The default implementation (in this abstract class) always returns True,
   * signalling that every object pre-selected by its bounding box is
   * automatically selected. The specializations should define a more correct behaviour.
   * The algorithm should transform the InteractiveObject geometry using the
   * parameter theTrf and then reject it with polygon.
   * @param thePolygon
   *   the list of vertices of a free-form closed polygon without
   *   self-intersections. The last point should not coincide with the first
   *   point of the list. Any two neighbor points should not be confused.
   * @param theTrf
   *   Position/Orientation of the polygon. It coincides with theTrfInv that is
   *   passed to NIS_InteractiveContext::selectObjects().
   * @param isFull
   *   True if full inclusion is required (full inside the tested box) for
   *   the positive result, False - if only partial inclusion give a result.
   * @return
   *   True if the InteractiveObject geometry intersects the polygon or is inside it
   */
  Standard_EXPORT virtual Standard_Boolean
       Intersect     (const NCollection_List<gp_XY> &thePolygon,
                      const gp_Trsf                 &theTrf,
                      const Standard_Boolean         isFull) const;

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

  Handle(NIS_Drawer)             myDrawer;
  Standard_Integer              myID;
  NIS_Drawer::DrawType          myDrawType      : 3;
  NIS_Drawer::DrawType          myBaseType      : 3;
  Standard_Boolean              myIsHidden      : 1;
  Standard_Boolean              myIsDynHilighted: 1;

  Standard_Boolean              myIsUpdateBox   : 1;
  unsigned int                  myTransparency  : 10;
  static const unsigned int     MaxTransparency = 1000;

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
