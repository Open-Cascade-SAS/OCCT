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

#ifndef _BRepFill_Draft_HeaderFile
#define _BRepFill_Draft_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir.hxx>
#include <Standard_Real.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <GeomAbs_Shape.hxx>
#include <BRepFill_TransitionStyle.hxx>
#include <Standard_Boolean.hxx>
class BRepFill_DraftLaw;
class BRepFill_SectionLaw;
class Geom_Surface;
class Bnd_Box;

class BRepFill_Draft
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_Draft(const TopoDS_Shape& Shape,
                                 const gp_Dir&       Dir,
                                 const Standard_Real Angle);

  Standard_EXPORT void SetOptions(const BRepFill_TransitionStyle Style    = BRepFill_Right,
                                  const Standard_Real            AngleMin = 0.01,
                                  const Standard_Real            AngleMax = 3.0);

  Standard_EXPORT void SetDraft(const Standard_Boolean IsInternal = Standard_False);

  Standard_EXPORT void Perform(const Standard_Real LengthMax);

  Standard_EXPORT void Perform(const Handle(Geom_Surface)& Surface,
                               const Standard_Boolean      KeepInsideSurface = Standard_True);

  Standard_EXPORT void Perform(const TopoDS_Shape&    StopShape,
                               const Standard_Boolean KeepOutSide = Standard_True);

  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the draft surface
  //! To have the complete shape
  //! you have to use the Shape() methode.
  Standard_EXPORT TopoDS_Shell Shell() const;

  //! Returns the  list   of shapes generated   from the
  //! shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Generated(const TopoDS_Shape& S);

  Standard_EXPORT TopoDS_Shape Shape() const;

protected:
private:
  Standard_EXPORT void Init(const Handle(Geom_Surface)& Surf,
                            const Standard_Real         Length,
                            const Bnd_Box&              Box);

  Standard_EXPORT void BuildShell(const Handle(Geom_Surface)& Surf,
                                  const Standard_Boolean      KeepOutSide = Standard_False);

  Standard_EXPORT Standard_Boolean Fuse(const TopoDS_Shape& S, const Standard_Boolean KeepOutSide);

  Standard_EXPORT Standard_Boolean Sewing();

  gp_Dir                          myDir;
  Standard_Real                   myAngle;
  Standard_Real                   angmin;
  Standard_Real                   angmax;
  Standard_Real                   myTol;
  Handle(BRepFill_DraftLaw)       myLoc;
  Handle(BRepFill_SectionLaw)     mySec;
  Handle(TopTools_HArray2OfShape) mySections;
  Handle(TopTools_HArray2OfShape) myFaces;
  TopTools_ListOfShape            myGenerated;
  TopoDS_Shape                    myShape;
  TopoDS_Shape                    myTop;
  TopoDS_Shell                    myShell;
  TopoDS_Wire                     myWire;
  GeomAbs_Shape                   myCont;
  BRepFill_TransitionStyle        myStyle;
  Standard_Boolean                IsInternal;
  Standard_Boolean                myDone;
};

#endif // _BRepFill_Draft_HeaderFile
