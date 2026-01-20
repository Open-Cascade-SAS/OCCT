// Created on: 1997-02-06
// Created by: Olga PILLOT
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//=================================================================================================

inline BRepFeat_Form::BRepFeat_Form()
    : myFuse(false),
      myModify(false),
      myPerfSelection(BRepFeat_NoSelection),
      myJustGluer(false),
      myJustFeat(false),
      mySbOK(false),
      mySkOK(false),
      myGSOK(false),
      mySFOK(false),
      mySUOK(false),
      myGFOK(false),
      myPSOK(false),
      myStatusError(BRepFeat_NotInitialized)

{
}

//=================================================================================================

inline void BRepFeat_Form::BasisShapeValid()
{
  mySbOK = true;
}

//=================================================================================================

inline void BRepFeat_Form::PerfSelectionValid()
{
  myPSOK = true;
}

//=================================================================================================

inline void BRepFeat_Form::GeneratedShapeValid()
{
  myGSOK = true;
}

//=================================================================================================

inline void BRepFeat_Form::ShapeFromValid()
{
  mySFOK = true;
}

//=================================================================================================

inline void BRepFeat_Form::ShapeUntilValid()
{
  mySUOK = true;
}

//=================================================================================================

inline void BRepFeat_Form::GluedFacesValid()
{
  myGFOK = true;
}

//=================================================================================================

inline void BRepFeat_Form::SketchFaceValid()
{
  mySkOK = true;
}
