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
#include <TopoDS_Shape.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
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
                                 const double Angle);

  Standard_EXPORT void SetOptions(const BRepFill_TransitionStyle Style    = BRepFill_Right,
                                  const double            AngleMin = 0.01,
                                  const double            AngleMax = 3.0);

  Standard_EXPORT void SetDraft(const bool IsInternal = false);

  Standard_EXPORT void Perform(const double LengthMax);

  Standard_EXPORT void Perform(const occ::handle<Geom_Surface>& Surface,
                               const bool      KeepInsideSurface = true);

  Standard_EXPORT void Perform(const TopoDS_Shape&    StopShape,
                               const bool KeepOutSide = true);

  Standard_EXPORT bool IsDone() const;

  //! Returns the draft surface
  //! To have the complete shape
  //! you have to use the Shape() methode.
  Standard_EXPORT TopoDS_Shell Shell() const;

  //! Returns the list of shapes generated from the
  //! shape <S>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Generated(const TopoDS_Shape& S);

  Standard_EXPORT TopoDS_Shape Shape() const;

private:
  Standard_EXPORT void Init(const occ::handle<Geom_Surface>& Surf,
                            const double         Length,
                            const Bnd_Box&              Box);

  Standard_EXPORT void BuildShell(const occ::handle<Geom_Surface>& Surf,
                                  const bool      KeepOutSide = false);

  Standard_EXPORT bool Fuse(const TopoDS_Shape& S, const bool KeepOutSide);

  Standard_EXPORT bool Sewing();

  gp_Dir                          myDir;
  double                   myAngle;
  double                   angmin;
  double                   angmax;
  double                   myTol;
  occ::handle<BRepFill_DraftLaw>       myLoc;
  occ::handle<BRepFill_SectionLaw>     mySec;
  occ::handle<NCollection_HArray2<TopoDS_Shape>> mySections;
  occ::handle<NCollection_HArray2<TopoDS_Shape>> myFaces;
  NCollection_List<TopoDS_Shape>            myGenerated;
  TopoDS_Shape                    myShape;
  TopoDS_Shape                    myTop;
  TopoDS_Shell                    myShell;
  TopoDS_Wire                     myWire;
  GeomAbs_Shape                   myCont;
  BRepFill_TransitionStyle        myStyle;
  bool                IsInternal;
  bool                myDone;
};

#endif // _BRepFill_Draft_HeaderFile
