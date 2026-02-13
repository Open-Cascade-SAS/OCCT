// Created on: 1992-05-27
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRep_TFace_HeaderFile
#define _BRep_TFace_HeaderFile

#include <Standard.hxx>

#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_TFace.hxx>
class Geom_Surface;
class TopoDS_TShape;

//! The Tface from BRep is based on the TFace from
//! TopoDS. The TFace contains:
//!
//! * A surface, a tolerance and a Location.
//!
//! * A NaturalRestriction flag, when this flag is
//! True the boundary of the face is known to be the
//! parametric space (Umin, UMax, VMin, VMax).
//!
//! * An optional list of triangulations. If there are any
//! triangulations the surface can be absent.
//!
//! The Location is used for the Surface.
//!
//! The triangulation is in the same reference system
//! than the TFace. A point on mySurface must be
//! transformed with myLocation, but not a point on
//! the triangulation.
//!
//! The Surface may be shared by different TFaces but
//! not the Triangulation, because the Triangulation
//! may be modified by the edges.
class BRep_TFace : public TopoDS_TFace
{

public:
  //! Creates an empty TFace.
  Standard_EXPORT BRep_TFace();

  //! Returns face surface.
  const occ::handle<Geom_Surface>& Surface() const { return mySurface; }

  //! Sets surface for this face.
  Standard_EXPORT void Surface(const occ::handle<Geom_Surface>& theSurface);

  //! Returns the face location.
  const TopLoc_Location& Location() const { return myLocation; }

  //! Sets the location for this face.
  void Location(const TopLoc_Location& theLocation) { myLocation = theLocation; }

  //! Returns the face tolerance.
  double Tolerance() const { return myTolerance; }

  //! Sets the tolerance for this face.
  Standard_EXPORT void Tolerance(const double theTolerance);

  //! Returns TRUE if the face surface is a plane.
  bool IsPlane() const { return myIsPlane; }

  //! Returns TRUE if the boundary of this face is known to be the parametric space (Umin, UMax,
  //! VMin, VMax).
  bool NaturalRestriction() const { return myNaturalRestriction; }

  //! Sets the flag that is TRUE if the boundary of this face is known to be the parametric space.
  void NaturalRestriction(const bool theRestriction) { myNaturalRestriction = theRestriction; }

  //! Returns the triangulation of this face according to the mesh purpose.
  //! @param[in] thePurpose a mesh purpose to find appropriate triangulation (NONE by default).
  //! @return an active triangulation in case of NONE purpose,
  //!         the first triangulation appropriate for the input purpose,
  //!         just the first triangulation if none matching other criteria and input purpose is
  //!         AnyFallback or null handle if there is no any suitable triangulation.
  Standard_EXPORT const occ::handle<Poly_Triangulation>& Triangulation(
    const Poly_MeshPurpose thePurpose = Poly_MeshPurpose_NONE) const;

  //! Sets input triangulation for this face.
  //! @param[in] theTriangulation  triangulation to be set
  //! @param[in] theToReset  flag to reset triangulations list to new list with only one input
  //! triangulation. If theTriangulation is NULL internal list of triangulations will be cleared and
  //! active triangulation will be nullified. If theToReset is TRUE internal list of triangulations
  //! will be reset to new list with only one input triangulation that will be active. Else if input
  //! triangulation is contained in internal triangulations list it will be made active,
  //!      else the active triangulation will be replaced to input one.
  Standard_EXPORT void Triangulation(const occ::handle<Poly_Triangulation>& theTriangulation,
                                     const bool                             theToReset = true);

  //! Returns a copy of the TShape with no sub-shapes.
  //! The new Face has no triangulation.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

public:
  //! Returns the list of available face triangulations.
  const NCollection_List<occ::handle<Poly_Triangulation>>& Triangulations() const
  {
    return myTriangulations;
  }

  //! Sets input list of triangulations and currently active triangulation for this face.
  //! If list is empty internal list of triangulations will be cleared and active triangulation will
  //! be nullified. Else this list will be saved and the input active triangulation be saved as
  //! active. Use NULL active triangulation to set the first triangulation in list as active. Note:
  //! the method throws exception if there is any NULL triangulation in input list or
  //!       if this list doesn't contain input active triangulation.
  Standard_EXPORT void Triangulations(
    const NCollection_List<occ::handle<Poly_Triangulation>>& theTriangulations,
    const occ::handle<Poly_Triangulation>&                   theActiveTriangulation);

  //! Returns number of available face triangulations.
  int NbTriangulations() const { return myTriangulations.Size(); }

  //! Returns current active triangulation.
  const occ::handle<Poly_Triangulation>& ActiveTriangulation() const
  {
    return myActiveTriangulation;
  }

  DEFINE_STANDARD_RTTIEXT(BRep_TFace, TopoDS_TFace)

private:
  NCollection_List<occ::handle<Poly_Triangulation>> myTriangulations;
  occ::handle<Poly_Triangulation>                   myActiveTriangulation;
  occ::handle<Geom_Surface>                         mySurface;
  TopLoc_Location                                   myLocation;
  double                                            myTolerance;
  bool                                              myNaturalRestriction;
  bool                                              myIsPlane;
};

#endif // _BRep_TFace_HeaderFile
