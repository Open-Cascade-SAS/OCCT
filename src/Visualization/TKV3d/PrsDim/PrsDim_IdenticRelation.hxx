// Created on: 1997-03-03
// Created by: Jean-Pierre COMBE
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

#ifndef _PrsDim_IdenticRelation_HeaderFile
#define _PrsDim_IdenticRelation_HeaderFile

#include <PrsDim_Relation.hxx>
#include <gp_Pnt.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>

class TopoDS_Shape;
class Geom_Plane;
class Geom_Line;
class Geom_Circle;
class Geom_Ellipse;
class TopoDS_Wire;
class TopoDS_Vertex;
class gp_Dir;

//! Constructs a constraint by a relation of identity
//! between two or more datums figuring in shape
//! Interactive Objects.
class PrsDim_IdenticRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_IdenticRelation, PrsDim_Relation)
public:
  //! Initializes the relation of identity between the two
  //! entities, FirstShape and SecondShape. The plane
  //! aPlane is initialized in case a visual reference is
  //! needed to show identity.
  Standard_EXPORT PrsDim_IdenticRelation(const TopoDS_Shape&            FirstShape,
                                         const TopoDS_Shape&            SecondShape,
                                         const occ::handle<Geom_Plane>& aPlane);

  bool HasUsers() const { return !myUsers.IsEmpty(); }

  const NCollection_List<occ::handle<Standard_Transient>>& Users() const { return myUsers; }

  void AddUser(const occ::handle<Standard_Transient>& theUser) { myUsers.Append(theUser); }

  void ClearUsers() { myUsers.Clear(); }

  //! Returns true if the interactive object is movable.
  bool IsMovable() const override { return true; }

private:
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                               const occ::handle<Prs3d_Presentation>&         thePrs,
                               const int                                      theMode) override;

  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                        const int                               theMode) override;

  Standard_EXPORT void ComputeOneEdgeOVertexPresentation(
    const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeTwoEdgesPresentation(
    const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeTwoLinesPresentation(
    const occ::handle<Prs3d_Presentation>& aPresentation,
    const occ::handle<Geom_Line>&          aLin,
    gp_Pnt&                                Pnt1On1,
    gp_Pnt&                                Pnt2On1,
    gp_Pnt&                                Pnt1On2,
    gp_Pnt&                                Pnt2On2,
    const bool                             isInf1,
    const bool                             isInf2);

  Standard_EXPORT void ComputeTwoCirclesPresentation(
    const occ::handle<Prs3d_Presentation>& aPresentation,
    const occ::handle<Geom_Circle>&        aCircle,
    const gp_Pnt&                          Pnt1On1,
    const gp_Pnt&                          Pnt2On1,
    const gp_Pnt&                          Pnt1On2,
    const gp_Pnt&                          Pnt2On2);

  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of automatic presentation
  Standard_EXPORT void ComputeAutoArcPresentation(const occ::handle<Geom_Circle>& aCircle,
                                                  const gp_Pnt&                   firstp,
                                                  const gp_Pnt&                   lastp,
                                                  const bool                      isstatic = false);

  //! Computes the presentation of the identic constraint
  //! between 2 circles in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoCircPresentation(const occ::handle<Geom_Circle>& aCircle);

  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoArcPresentation(const occ::handle<Geom_Circle>& aCircle,
                                                     const gp_Pnt&                   pntfirst,
                                                     const gp_Pnt&                   pntlast);

  Standard_EXPORT void ComputeTwoEllipsesPresentation(const occ::handle<Prs3d_Presentation>& aPrs,
                                                      const occ::handle<Geom_Ellipse>&       anEll,
                                                      const gp_Pnt& Pnt1On1,
                                                      const gp_Pnt& Pnt2On1,
                                                      const gp_Pnt& Pnt1On2,
                                                      const gp_Pnt& Pnt2On2);

  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of automatic presentation
  Standard_EXPORT void ComputeAutoArcPresentation(const occ::handle<Geom_Ellipse>& theEll,
                                                  const gp_Pnt&                    firstp,
                                                  const gp_Pnt&                    lastp,
                                                  const bool isstatic = false);

  //! Computes the presentation of the identic constraint
  //! between 2 ellipses in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoElipsPresentation(const occ::handle<Geom_Ellipse>& theEll);

  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoArcPresentation(const occ::handle<Geom_Ellipse>& theEll,
                                                     const gp_Pnt&                    pntfirst,
                                                     const gp_Pnt&                    pntlast);

  Standard_EXPORT void ComputeTwoVerticesPresentation(
    const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT double ComputeSegSize() const;

  Standard_EXPORT bool ComputeDirection(const TopoDS_Wire&   aWire,
                                        const TopoDS_Vertex& aVertex,
                                        gp_Dir&              aDir) const;

  Standard_EXPORT gp_Dir ComputeLineDirection(const occ::handle<Geom_Line>& aLin,
                                              const gp_Pnt&                 anExtremity) const;

  Standard_EXPORT gp_Dir ComputeCircleDirection(const occ::handle<Geom_Circle>& aCirc,
                                                const TopoDS_Vertex& ConnectedVertex) const;

private:
  NCollection_List<occ::handle<Standard_Transient>> myUsers;
  bool                                              isCircle;
  gp_Pnt                                            myFAttach;
  gp_Pnt                                            mySAttach;
  gp_Pnt                                            myCenter;
};

#endif // _PrsDim_IdenticRelation_HeaderFile
