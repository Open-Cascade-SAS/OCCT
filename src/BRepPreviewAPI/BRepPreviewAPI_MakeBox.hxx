// Created on: 2020-01-31
// Created by: Svetlana SHUTINA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _BRepPreviewAPI_MakeBox_HeaderFile
#define _BRepPreviewAPI_MakeBox_HeaderFile

#include <BRepPrimAPI_MakeBox.hxx>

//! Builds a valid box, if points fulfill the conditions of a valid box.
//! And allows to build a preview, otherwise.
//! There are 4 cases:
//! 1 - preview can be a vertex if thin box in all directions is a point;
//! 2 - preview can be an edge if thin box in two directions is a point;
//! 3 - preview can be a rectangular face if thin box in only one direction is a point;
//! 4 - preview can be a valid box if point values fulfill the conditions of a valid box.

class BRepPreviewAPI_MakeBox : public BRepPrimAPI_MakeBox
{
public:

  //! Constructor
  BRepPreviewAPI_MakeBox() {}

  //! Creates a preview depending on point values.
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

private:

  //! Create a vertex if thin box in all directions is a point.
  void makeVertex (const gp_Pnt& thePoint);

  //! Create an edge if thin box in two directions is a point.
  void makeEdge (const gp_Pnt& thePoint1, const gp_Pnt& thePoint2);

  //! Create a rectangular face if the box is thin in one direction only.
  //! @param thePnt1 the first point for a rectangular face
  //! @param thePnt2 the second point for a rectangular face
  //! @param thePnt3 the third point for a rectangular face
  //! @param thePnt4 the fourth point for a rectangular face
  void makeRectangle (const gp_Pnt& thePnt1, const gp_Pnt& thePnt2,
                      const gp_Pnt& thePnt3, const gp_Pnt& thePnt4);

};

#endif
