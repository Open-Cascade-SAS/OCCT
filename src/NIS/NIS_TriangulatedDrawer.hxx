// File:      NIS_TriangulatedDrawer.hxx
// Created:   19.07.07 12:05
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_TriangulatedDrawer_HeaderFile
#define NIS_TriangulatedDrawer_HeaderFile

#include <NIS_Drawer.hxx>
#include <Quantity_Color.hxx>

class NIS_Triangulated;

/**
 * Drawer for interactive object type NIS_Triangulated.
 */

class NIS_TriangulatedDrawer : public NIS_Drawer
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  Standard_EXPORT NIS_TriangulatedDrawer(const Quantity_Color theNormal,
                                         const Quantity_Color theHilight
                                                        = Quantity_NOC_GRAY80,
                                         const Quantity_Color theDynHilight
                                                        = Quantity_NOC_CYAN1);

  /**
   * Copy the relevant information from another instance of Drawer.
   * raises exception if theOther has incompatible type (test IsKind).
   */
  Standard_EXPORT virtual void Assign    (const Handle_NIS_Drawer& theOther);

  /**
   * Called before execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void BeforeDraw(const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Called after execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void AfterDraw(const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

  /**
   * Main function: display the given interactive object in the given view.
   */
  Standard_EXPORT virtual void Draw      (const Handle_NIS_InteractiveObject&,
                                          const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsEqual  (const Handle_NIS_Drawer& theOth)const;
private:
  Quantity_Color      myColor[4];
  Standard_ShortReal  myLineWidth;
  Standard_Boolean    myIsDrawPolygons;

  friend class NIS_Triangulated;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_TriangulatedDrawer)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_TriangulatedDrawer, NIS_Drawer)


#endif
