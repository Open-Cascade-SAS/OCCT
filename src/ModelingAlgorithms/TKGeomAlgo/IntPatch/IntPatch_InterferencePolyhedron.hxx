// Created on: 1992-09-29
// Created by: Didier PIFFAULT
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

#ifndef _IntPatch_InterferencePolyhedron_HeaderFile
#define _IntPatch_InterferencePolyhedron_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <Intf_Interference.hxx>
class IntPatch_Polyhedron;
class Intf_TangentZone;

//! Computes the  interference between two polyhedra or the
//! self interference of a polyhedron. Points of intersection,
//! polylines  of intersection and zones of tangence.
class IntPatch_InterferencePolyhedron : public Intf_Interference
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty interference of Polyhedron.
  Standard_EXPORT IntPatch_InterferencePolyhedron();

  //! Constructs  and computes  an  interference between  the two
  //! Polyhedra.
  Standard_EXPORT IntPatch_InterferencePolyhedron(const IntPatch_Polyhedron& Obje1,
                                                  const IntPatch_Polyhedron& Obje2);

  //! Constructs  and  computes   the self   interference  of   a
  //! Polyhedron.
  Standard_EXPORT IntPatch_InterferencePolyhedron(const IntPatch_Polyhedron& Obje);

  //! Computes the interference between the two Polyhedra.
  Standard_EXPORT void Perform(const IntPatch_Polyhedron& Obje1, const IntPatch_Polyhedron& Obje2);

  //! Computes the self interference of a Polyhedron.
  Standard_EXPORT void Perform(const IntPatch_Polyhedron& Obje);

protected:
private:
  Standard_EXPORT void Interference(const IntPatch_Polyhedron& Obje1);

  //! Compares the bounding volumes between the facets of <Obje1>
  //! and the facets of <Obje2> and intersects the facets when the
  //! bounding volumes have a common part.
  Standard_EXPORT void Interference(const IntPatch_Polyhedron& Obje1,
                                    const IntPatch_Polyhedron& Obje2);

  //! Computes  the intersection between    the  facet <Tri1>  of
  //! <FirstPol> and the facet <Tri2> of <SecondPol>.
  Standard_EXPORT void Intersect(const Standard_Integer     TriF,
                                 const IntPatch_Polyhedron& Obje1,
                                 const Standard_Integer     TriS,
                                 const IntPatch_Polyhedron& Obje2);

  //! Computes the  zone of tangence between the  facet <Tri1> of
  //! <FirstPol> and the facet <Tri2> of <SecondPol>.
  Standard_EXPORT Standard_Boolean TangentZoneValue(Intf_TangentZone&          TheTZ,
                                                    const IntPatch_Polyhedron& Obje1,
                                                    const Standard_Integer     Tri1,
                                                    const IntPatch_Polyhedron& Obje2,
                                                    const Standard_Integer     Tri2) const;

  Standard_EXPORT void CoupleCharacteristics(const IntPatch_Polyhedron& FirstPol,
                                             const IntPatch_Polyhedron& SeconPol);

  Standard_Integer OI[3];
  Standard_Integer TI[3];
  Standard_Real    dpOpT[3][3];
  Standard_Real    dpOeT[3][3];
  Standard_Real    deOpT[3][3];
  gp_XYZ           voo[3];
  gp_XYZ           vtt[3];
  Standard_Real    Incidence;
};

#endif // _IntPatch_InterferencePolyhedron_HeaderFile
