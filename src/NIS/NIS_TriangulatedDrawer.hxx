// Created on: 2007-07-19
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
  Standard_EXPORT virtual void Assign    (const Handle(NIS_Drawer)& theOther);

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
  Standard_EXPORT virtual void Draw      (const Handle(NIS_InteractiveObject)&,
                                          const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsEqual  (const Handle(NIS_Drawer)& theOth)const;

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
