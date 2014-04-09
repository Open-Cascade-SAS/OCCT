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

#ifndef NIS_Drawer_HeaderFile
#define NIS_Drawer_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <NIS_DrawList.hxx>
#include <Bnd_B3f.hxx>

#ifdef WNT
#pragma warning (push)
// Disable warning 4480: nonstandard extension used: specifying underlying type for enum 'enum'
#pragma warning (disable:4480)
#endif

class Handle(NIS_InteractiveObject);
class Handle(NIS_View);
class Handle(NIS_Drawer);
class NIS_InteractiveContext;
class NIS_View;
template <class A> class NCollection_Vector;

/**
 * Abstract Drawer type.
 * Drawer provides the immediate OpenGL drawing for every NIS_InteractiveObject
 * maneged by the given Drawer instance. Each Drawer instance has reciprocal
 * link with a number of NIS_InteractiveObject instances of the same
 * (corresponding to Drawer) type. The idea is to group the drawing for all
 * referred interactive objects using the same pre- and post-treatment like
 * color setting, matrix, polygon offset, line thickness and what not.
 *
 * @section nis_drawer_visualprop Visual properties of Drawer
 * Normally visual properties of any NIS_InteractiveObject are stored in its
 * Drawer instance, but not in an object. For example, if an interactive object
 * has method SetColor() then the color is stored in the corresponding Drawer
 * rather than in the interactive object itself. This scheme avoid useless
 * duplication when a lot of objects have similar properties like color. Please
 * see @see nis_interactiveobject_drawer to learn how this mechanism
 * works from the side of NIS_InteractiveObject.
 *
 * @section nis_drawer_drawing Drawing
 * There are 3 virtual methods to implement OpenGL drawing in this API. They
 * define the drawing cycle that consists of filling the internal OpenGL draw
 * list with commands. This drawing cycle is triggered when the corresponding
 * internal instance of NIS_DrawList has 'IsUpdated' flag (you can set this
 * flag by means of public methods NIS_Drawer::SetUpdated()). 
 * <ul>
 * <li><b>BeforeDraw()</b> : contains all OpenGL commands that define the
 *     common set of visual properties for all managed interactive objects.
 *     This method is called once in the beginning of drawing cycle for the
 *     Drawer instance</li>
 * <li><b>Draw()</b> : all OpenGL commands that are specific to a given
 *     interactive object, usually definition of vertices, triangles, lines,
 *     or their arrays.</li>
 * <li><b>AfterDraw()</b> : commands that revert the status of OpenGL context
 *     to the state before execution of BeforeDraw(). This method is called
 *     once in the end of drawing cycle.</li>
 * </ul>
 * Each of these methods receives NIS_DrawList and DrawType, both identify the
 * OpenGL draw list that should be filled with commands. Based on DrawType
 * you will be able to define different presentation - the most important case
 * is how hilighted (selected) interactive object is presented.
 * <p>
 * For advanced purposes you also can redefine the virtual method redraw(), it
 * is dedicated to higher-level management of draw lists and ordering of
 * their update when necessary.  
 *
 * @section nis_drawer_distinction Distinction of Drawer instances 
 * Every Drawer should define which interactive objects it may manage and
 * which - may not. The same idea could be shaped alternatively: every
 * interactive object should understand to what Drawer it can attach itself.
 * This question is answerd by special virtual method IsEqual() that compares
 * two Drawers of the same type. <b>Two instances of Drawer are equal if they
 * have the same set of visual properties that are implemented in BeforeDraw().
 * </b> The method IsEqual() is the core of Drawer architecture and it must
 * be implemented very carefully for any new type. Particularly, for any
 * derived class the method IsEqual() should first call the same method of
 * its superclass.
 * <p>
 * For the optimal efficiency of OpenGL drawing it is better to keep the size
 * of draw list (i.e., the number of interactive objects in a Drawer instance)
 * not too small and not too big. The latter limitation is entered by the
 * protected field myObjPerDrawer. It is used in method IsEqual() of the base
 * Drawer class: two Drawers are not equal if they are initialized on objects
 * that have too different IDs -- even if all visual properties of these two
 * Drawer instances coincide.
 * <p>
 * @section nis_drawer_cloning Cloning Drawer instances
 * It is possible to clone a Drawer instance with the viryual method Assign().
 * This method copies all visual properties and other important data from the
 * Drawer provided as parameter. Method Clone() also should be very carefully
 * implemented for any new Drawer type, to make sure that all necessary data
 * fields and structures are properly copied.
 */

class NIS_Drawer : public Standard_Transient
{
 public:
#if defined(WNT) && (_MSC_VER >= 1400)
  enum DrawType : unsigned int {
#else
  enum DrawType {
#endif
    Draw_Normal         = 0,
    Draw_Top            = 1,
    Draw_Transparent    = 2,
    Draw_Hilighted      = 3,
    Draw_DynHilighted   = 4
  };

 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  inline NIS_Drawer ()
   : myTransparency     (0.f),
     myIniId            (0),
     myObjPerDrawer     (1024),
     myCtx              (0L)
   {}

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_Drawer ();

  /**
   * Query the Interactive Context.
   */
  inline NIS_InteractiveContext * GetContext () const
  { return myCtx; }

  /**
   * Copy the relevant information from another instance of Drawer.
   * raises exception if theOther has incompatible type (test IsKind).
   */
  Standard_EXPORT virtual void  Assign  (const Handle(NIS_Drawer)& theOther);

  /**
   * Create a3D bounding box of drawn objects.
   * @param pView
   *   In not NULL, only objects visible in this view are taken.
   */
  Standard_EXPORT virtual const Bnd_B3f&
                                GetBox  (const NIS_View * pView = 0L) const;

  /**
   * Mark all draw lists for update
   */
  Standard_EXPORT void          SetUpdated (const DrawType theType) const;

  Standard_EXPORT void          SetUpdated (const DrawType theType1,
                                            const DrawType theType2) const;

  Standard_EXPORT void          SetUpdated (const DrawType theType1,
                                            const DrawType theType2,
                                            const DrawType theType3) const;

  Standard_EXPORT void          SetUpdated (const DrawType theType1,
                                            const DrawType theType2,
                                            const DrawType theType3,
                                            const DrawType theType4) const;

  /**
   * Switch on/off the dynamic hilight of the given object in the
   * given view.
   * @param isHilighted
   *   True if the object should be hilighted, False - if not hilighted
   * @param theObj
   *   Object instance that should be hilighted or unhilighted.
   * @param theView
   *   View where the status of object must be changed. If NULL, the object
   *   is hilighted/unhilighted in all views.
   */
  Standard_EXPORT void          SetDynamicHilighted
                                   (const Standard_Boolean      isHilighted,
                                    const Handle(NIS_InteractiveObject)& theObj,
                                    const Handle(NIS_View)&      theView = 0L);

  /**
   * Hash value, for Map interface.
   */ 
  Standard_EXPORT virtual Standard_Integer
                                HashCode(const Standard_Integer theN) const;

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsEqual (const Handle(NIS_Drawer)& theOth) const;

  /**
   * Obtain the iterator of IDs of associated objects.
   */
  inline TColStd_MapIteratorOfPackedMapOfInteger
                                ObjectIterator   () const
  { return TColStd_MapIteratorOfPackedMapOfInteger (myMapID); }

  /**
   * Query associated draw lists.
   */
  inline NCollection_List<NIS_DrawList *>
                                GetLists() const
  { return myLists; }
 
protected:
  /**
   * Called to add draw list IDs to ex-list Ids of view. These draw lists are
   * eventually released in the callback function, before anything is displayed
   */
  Standard_EXPORT void UpdateExListId   (const Handle(NIS_View)& theView) const;

  // ---------- PROTECTED METHODS ----------

  /**
   * Called before execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void  BeforeDraw
                                        (const DrawType         theType,
                                         const NIS_DrawList&    theDrawList );

  /**
   * Called after execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void  AfterDraw
                                        (const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

  /**
   * Main function: display the given interactive object in the given view.
   */
  Standard_EXPORT virtual void  Draw    (const Handle(NIS_InteractiveObject)&,
                                         const DrawType         theType,
                                         const NIS_DrawList&    theDrawList)= 0;

  Standard_EXPORT virtual void  redraw  (const DrawType         theType,
                                         const Handle(NIS_View)& theView);

  Standard_EXPORT void  addObject       (const NIS_InteractiveObject * theObj,
                                         const Standard_Boolean isShareList,
                                         const Standard_Boolean isUpVws);

  Standard_EXPORT void  removeObject    (const NIS_InteractiveObject * theObj,
                                         const Standard_Boolean        isUpVws);

  Standard_EXPORT virtual NIS_DrawList*
                        createDefaultList (const Handle(NIS_View)&) const;

 protected:
  //! Get the number of interactive objects in this drawer
  inline Standard_Integer      NObjects() const
  { return myMapID.Extent(); }

 private:
  // ---------- PRIVATE (PROHIBITED) METHODS ----------

  NIS_Drawer             (const NIS_Drawer& theOther);
  NIS_Drawer& operator = (const NIS_Drawer& theOther);

  // ---------- PRIVATE METHODS ----------
  void                  prepareList (const NIS_Drawer::DrawType theType,
                                     const NIS_DrawList&        theDrawLst,
                                     const TColStd_PackedMapOfInteger& mapObj);

 protected:
// ---------- PROTECTED FIELDS ----------  
  NCollection_List<NIS_DrawList*>       myLists;
  Standard_ShortReal                    myTransparency;  
  //! ID of the initializing InteractiveObject. It is never changed, can be
  //! used to compute hash code of the Drawer instance.
  Standard_Integer                      myIniId;
  //! Maximal range of IDs of objects in one drawer. Limits the size of
  //! draw lists. Can be initialized only in constructor (default 1024). It is
  //! strictly prohibited to change this value outside the constructor.
  Standard_Integer                      myObjPerDrawer;

 private:
  // ---------- PRIVATE FIELDS ----------

  NIS_InteractiveContext                * myCtx;
  TColStd_PackedMapOfInteger            myMapID;
  Bnd_B3f                               myBox;

  friend class NIS_InteractiveContext;
  friend class NIS_InteractiveObject;
  friend class NIS_View;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Drawer)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Drawer, Standard_Transient)

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

inline Standard_Integer HashCode (const Handle(NIS_Drawer)& theDrawer,
                                  const Standard_Integer   theN)
{ return theDrawer.IsNull() ? 0 : theDrawer->HashCode (theN); }

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

inline Standard_Boolean IsEqual  (const Handle(NIS_Drawer)& theDrawer1,
                                  const Handle(NIS_Drawer)& theDrawer2)
{ return theDrawer1.IsNull()? Standard_False: theDrawer1->IsEqual(theDrawer2); }

#ifdef WNT
#pragma warning (pop)
#endif

#endif
