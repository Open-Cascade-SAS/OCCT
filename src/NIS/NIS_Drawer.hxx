// File:      NIS_Drawer.hxx
// Created:   06.07.07 21:10
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_Drawer_HeaderFile
#define NIS_Drawer_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <NIS_DrawList.hxx>
#include <Bnd_B3f.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4480)
#endif

class Handle_NIS_InteractiveObject;
class Handle_NIS_View;
class Handle_NIS_Drawer;
class NIS_InteractiveContext;
class NIS_View;
template <class A> class NCollection_Vector;

/**
 * Abstract Drawer type
 */

class NIS_Drawer : public Standard_Transient
{
 public:
#if (_MSC_VER < 1400)
   enum DrawType {
#else
   enum DrawType : unsigned int {
#endif
    Draw_Normal         = 0,
    Draw_Transparent    = 1,
    Draw_Hilighted      = 2,
    Draw_DynHilighted   = 3
  };

 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  inline NIS_Drawer () : myCtx (0L) {}

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
  Standard_EXPORT virtual void  Assign  (const Handle_NIS_Drawer& theOther);

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
                                    const Handle_NIS_InteractiveObject& theObj,
                                    const Handle_NIS_View&      theView = 0L);

  /**
   * Hash value, for Map interface.
   */ 
  Standard_EXPORT virtual Standard_Integer
                                HashCode(const Standard_Integer theN) const;

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsEqual (const Handle_NIS_Drawer& theOth) const;

  /**
   * Obtain the iterator of IDs of associated objects.
   */
  inline TColStd_MapIteratorOfPackedMapOfInteger
                                ObjectIterator   () const
  { return TColStd_MapIteratorOfPackedMapOfInteger (myMapID); }

protected:
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
  Standard_EXPORT virtual void  Draw    (const Handle_NIS_InteractiveObject&,
                                         const DrawType         theType,
                                         const NIS_DrawList&    theDrawList)= 0;

  Standard_EXPORT virtual void  redraw  (const DrawType         theType,
                                         const Handle_NIS_View& theView);

  Standard_EXPORT void  addObject       (const NIS_InteractiveObject * theObj,
                                         const Standard_Boolean        isUpVws);

  Standard_EXPORT void  removeObject    (const NIS_InteractiveObject * theObj,
                                         const Standard_Boolean        isUpVws);

  Standard_EXPORT virtual NIS_DrawList*
                        createDefaultList (const Handle_NIS_View&) const;

 private:
  // ---------- PRIVATE (PROHIBITED) METHODS ----------

  NIS_Drawer             (const NIS_Drawer& theOther);
  NIS_Drawer& operator = (const NIS_Drawer& theOther);

  // ---------- PRIVATE METHODS ----------
  void                  prepareList     (const NIS_Drawer::DrawType theType,
                                         const NIS_DrawList&        theDrawLst);

 protected:
// ---------- PROTECTED FIELDS ----------  
  NCollection_List<NIS_DrawList*>       myLists;

 private:
  // ---------- PRIVATE FIELDS ----------

  NIS_InteractiveContext                * myCtx;
  TColStd_PackedMapOfInteger            myMapID;
  Bnd_B3f                               myBox;

  friend class NIS_InteractiveContext;
  friend class NIS_InteractiveObject;

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

inline Standard_Integer HashCode (const Handle_NIS_Drawer& theDrawer,
                                  const Standard_Integer   theN)
{ return theDrawer.IsNull() ? 0 : theDrawer->HashCode (theN); }

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

inline Standard_Boolean IsEqual  (const Handle_NIS_Drawer& theDrawer1,
                                  const Handle_NIS_Drawer& theDrawer2)
{ return theDrawer1.IsNull()? Standard_False: theDrawer1->IsEqual(theDrawer2); }

#ifdef WNT
#pragma warning (pop)
#endif

#endif
