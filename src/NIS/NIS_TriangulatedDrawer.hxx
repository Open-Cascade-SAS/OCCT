// Created on: 2007-07-19
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

protected:
  /**
   * If myPolygonAsLineLoop is true then draw polygons of the object
   * in the mode GL_LINE_LOOP instead of GL_POLYGON in the case if no filling
   * was requested. This will eliminate the bug with Intel integrated graphic
   * cards (e.g. 945G Express) for the sake of polygon offset functionality.
   */
  Standard_Boolean    myPolygonAsLineLoop;

  Quantity_Color      myColor[5];
  Standard_ShortReal  myLineWidth;
  Standard_Boolean    myIsDrawPolygons;
  unsigned int        myPolygonType;

  friend class NIS_Triangulated;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_TriangulatedDrawer)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_TriangulatedDrawer, NIS_Drawer)


#endif
