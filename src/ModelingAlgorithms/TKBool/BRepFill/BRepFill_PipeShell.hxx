// Created on: 1998-07-22
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_PipeShell_HeaderFile
#define _BRepFill_PipeShell_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepFill_Section.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <GeomFill_Trihedron.hxx>
#include <BRepFill_TransitionStyle.hxx>
#include <GeomFill_PipeError.hxx>
#include <Standard_Transient.hxx>
#include <BRepFill_TypeOfContact.hxx>
class Law_Function;
class BRepFill_LocationLaw;
class BRepFill_SectionLaw;
class gp_Ax2;
class gp_Dir;
class TopoDS_Vertex;
class gp_Trsf;
class BRepFill_Sweep;

//! Computes a topological shell using some wires
//! (spines and profiles) and displacement option
//! Perform general sweeping construction
class BRepFill_PipeShell : public Standard_Transient
{

public:
  //! Set an sweep's mode
  //! If no mode are set, the mode used in MakePipe is used
  Standard_EXPORT BRepFill_PipeShell(const TopoDS_Wire& Spine);

  //! Set an Frenet or an CorrectedFrenet trihedron
  //! to perform the sweeping
  Standard_EXPORT void Set(const bool Frenet = false);

  //! Set a Discrete trihedron to perform the sweeping
  Standard_EXPORT void SetDiscrete();

  //! Set an fixed trihedron to perform the sweeping
  //! all sections will be parallel.
  Standard_EXPORT void Set(const gp_Ax2& Axe);

  //! Set an fixed BiNormal direction to perform
  //! the sweeping
  Standard_EXPORT void Set(const gp_Dir& BiNormal);

  //! Set support to the spine to define the BiNormal
  //! at the spine, like the normal the surfaces.
  //! Warning: To be effective, Each edge of the <spine> must
  //! have an representation on one face of<SpineSupport>
  Standard_EXPORT bool Set(const TopoDS_Shape& SpineSupport);

  //! Set an auxiliary spine to define the Normal
  //! For each Point of the Spine P, an Point Q is evaluated
  //! on <AuxiliarySpine>
  //! If <CurvilinearEquivalence>
  //! Q split <AuxiliarySpine> with the same length ratio
  //! than P split <Spline>.
  //! Else the plan define by P and the tangent to the <Spine>
  //! intersect <AuxiliarySpine> in Q.
  //! If <KeepContact> equals BRepFill_NoContact: The Normal is defined
  //! by the vector PQ.
  //! If <KeepContact> equals BRepFill_Contact: The Normal is defined to
  //! achieve that the sweeped section is in contact to the
  //! auxiliarySpine. The width of section is constant all along the path.
  //! In other words, the auxiliary spine lies on the swept surface,
  //! but not necessarily is a boundary of this surface. However,
  //! the auxiliary spine has to be close enough to the main spine
  //! to provide intersection with any section all along the path.
  //! If <KeepContact> equals BRepFill_ContactOnBorder: The auxiliary spine
  //! becomes a boundary of the swept surface and the width of section varies
  //! along the path.
  Standard_EXPORT void Set(const TopoDS_Wire&           AuxiliarySpine,
                           const bool                   CurvilinearEquivalence = true,
                           const BRepFill_TypeOfContact KeepContact = BRepFill_NoContact);

  //! Define the maximum V degree of resulting surface
  Standard_EXPORT void SetMaxDegree(const int NewMaxDegree);

  //! Define the maximum number of spans in V-direction
  //! on resulting surface
  Standard_EXPORT void SetMaxSegments(const int NewMaxSegments);

  //! Set the flag that indicates attempt to approximate
  //! a C1-continuous surface if a swept surface proved
  //! to be C0.
  //! Give section to sweep.
  //! Possibilities are:
  //! - Give one or several profile
  //! - Give one profile and an homotetic law.
  //! - Automatic compute of correspondence between profile, and section on the sweeped shape
  //! - correspondence between profile, and section on the sweeped shape defined by a vertex of the
  //! spine
  Standard_EXPORT void SetForceApproxC1(const bool ForceApproxC1);

  //! Sets the build history flag.
  //! If set to True, the pipe shell will store the history of the sections
  //! and the spine, which can be used for further modifications or analysis.
  inline void SetIsBuildHistory(const bool theIsBuildHistory)
  {
    myIsBuildHistory = theIsBuildHistory;
  }

  //! Returns the build history flag.
  //! If True, the pipe shell stores the history of the sections and the spine.
  inline bool IsBuildHistory() const { return myIsBuildHistory; }

  //! Set an section. The correspondence with the spine, will be automatically performed.
  Standard_EXPORT void Add(const TopoDS_Shape& Profile,
                           const bool          WithContact    = false,
                           const bool          WithCorrection = false);

  //! Set an section. The correspondence with the spine, is given by Location.
  Standard_EXPORT void Add(const TopoDS_Shape&  Profile,
                           const TopoDS_Vertex& Location,
                           const bool           WithContact    = false,
                           const bool           WithCorrection = false);

  //! Set an section and an homotetic law.
  //! The homotetie's centers is given by point on the <Spine>.
  Standard_EXPORT void SetLaw(const TopoDS_Shape&              Profile,
                              const occ::handle<Law_Function>& L,
                              const bool                       WithContact    = false,
                              const bool                       WithCorrection = false);

  //! Set an section and an homotetic law.
  //! The homotetie center is given by point on the <Spine>
  Standard_EXPORT void SetLaw(const TopoDS_Shape&              Profile,
                              const occ::handle<Law_Function>& L,
                              const TopoDS_Vertex&             Location,
                              const bool                       WithContact    = false,
                              const bool                       WithCorrection = false);

  //! Delete an section.
  Standard_EXPORT void DeleteProfile(const TopoDS_Shape& Profile);

  //! Say if <me> is ready to build the shape
  //! return False if <me> do not have section definition
  Standard_EXPORT bool IsReady() const;

  //! Get a status, when Simulate or Build failed.
  Standard_EXPORT GeomFill_PipeError GetStatus() const;

  Standard_EXPORT void SetTolerance(const double Tol3d      = 1.0e-4,
                                    const double BoundTol   = 1.0e-4,
                                    const double TolAngular = 1.0e-2);

  //! Set the Transition Mode to manage discontinuities
  //! on the sweep.
  Standard_EXPORT void SetTransition(const BRepFill_TransitionStyle Mode   = BRepFill_Modified,
                                     const double                   Angmin = 1.0e-2,
                                     const double                   Angmax = 6.0);

  //! Perform simulation of the sweep:
  //! Some Section are returned.
  Standard_EXPORT void Simulate(const int                       NumberOfSection,
                                NCollection_List<TopoDS_Shape>& Sections);

  //! Builds the resulting shape (redefined from MakeShape).
  Standard_EXPORT bool Build();

  //! Transform the sweeping Shell in Solid.
  //! If the section are not closed returns False
  Standard_EXPORT bool MakeSolid();

  //! Returns the result Shape.
  Standard_EXPORT const TopoDS_Shape& Shape() const;

  Standard_EXPORT double ErrorOnSurface() const;

  //! Returns the TopoDS Shape of the bottom of the sweep.
  Standard_EXPORT const TopoDS_Shape& FirstShape() const;

  //! Returns the TopoDS Shape of the top of the sweep.
  Standard_EXPORT const TopoDS_Shape& LastShape() const;

  //! Returns the list of original profiles
  void Profiles(NCollection_List<TopoDS_Shape>& theProfiles)
  {
    for (int i = 1; i <= mySeq.Length(); ++i)
      theProfiles.Append(mySeq(i).OriginalShape());
  }

  //! Returns the spine
  const TopoDS_Wire& Spine() { return mySpine; }

  //! Returns the list of shapes generated from the
  //! shape <S>.
  Standard_EXPORT void Generated(const TopoDS_Shape& S, NCollection_List<TopoDS_Shape>& L);

  DEFINE_STANDARD_RTTIEXT(BRepFill_PipeShell, Standard_Transient)

private:
  Standard_EXPORT void Prepare();

  Standard_EXPORT void Place(const BRepFill_Section& Sec,
                             TopoDS_Wire&            W,
                             gp_Trsf&                Trsf,
                             double&                 param);

  Standard_EXPORT void ResetLoc();

  Standard_EXPORT void BuildHistory(const BRepFill_Sweep& theSweep);

  TopoDS_Wire                            mySpine;
  TopoDS_Shape                           myFirst;
  TopoDS_Shape                           myLast;
  TopoDS_Shape                           myShape;
  NCollection_Sequence<BRepFill_Section> mySeq;
  NCollection_Sequence<TopoDS_Shape>     WSeq;
  NCollection_Sequence<int>              myIndOfSec;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myEdgeNewEdges;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                 myGenMap;
  double                                         myTol3d;
  double                                         myBoundTol;
  double                                         myTolAngular;
  double                                         angmin;
  double                                         angmax;
  int                                            myMaxDegree;
  int                                            myMaxSegments;
  bool                                           myForceApproxC1;
  occ::handle<Law_Function>                      myLaw;
  bool                                           myIsAutomaticLaw;
  occ::handle<BRepFill_LocationLaw>              myLocation;
  occ::handle<BRepFill_SectionLaw>               mySection;
  occ::handle<NCollection_HArray2<TopoDS_Shape>> myFaces;
  GeomFill_Trihedron                             myTrihedron;
  BRepFill_TransitionStyle                       myTransition;
  GeomFill_PipeError                             myStatus;
  double                                         myErrorOnSurf;
  bool                                           myIsBuildHistory;
};

#endif // _BRepFill_PipeShell_HeaderFile
