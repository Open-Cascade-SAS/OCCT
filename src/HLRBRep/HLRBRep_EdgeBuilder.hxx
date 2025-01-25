// Created on: 1997-04-17
// Created by: Christophe MARION
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

#ifndef _HLRBRep_EdgeBuilder_HeaderFile
#define _HLRBRep_EdgeBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_State.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
class HLRBRep_AreaLimit;
class HLRBRep_VertexList;
class HLRAlgo_Intersection;

class HLRBRep_EdgeBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates  an   EdgeBuilder    algorithm.    <VList>
  //! describes   the edge    and  the    interferences.
  //! AreaLimits   are   created  from   the   vertices.
  //! Builds(IN) is automatically called.
  Standard_EXPORT HLRBRep_EdgeBuilder(HLRBRep_VertexList& VList);

  //! Initialize an iteration on the areas.
  Standard_EXPORT void InitAreas();

  //! Set the current area to the next area.
  Standard_EXPORT void NextArea();

  //! Set the current area to the previous area.
  Standard_EXPORT void PreviousArea();

  //! Returns True if there is a current area.
  Standard_EXPORT Standard_Boolean HasArea() const;

  //! Returns the state of the current area.
  Standard_EXPORT TopAbs_State AreaState() const;

  //! Returns the edge state of the current area.
  Standard_EXPORT TopAbs_State AreaEdgeState() const;

  //! Returns the  AreaLimit beginning the current area.
  //! This is a NULL handle when the area is infinite on
  //! the left.
  Standard_EXPORT Handle(HLRBRep_AreaLimit) LeftLimit() const;

  //! Returns the  AreaLimit   ending  the current area.
  //! This is a NULL handle when the area is infinite on
  //! the right.
  Standard_EXPORT Handle(HLRBRep_AreaLimit) RightLimit() const;

  //! Reinitialize  the results  iteration  to the parts
  //! with State <ToBuild>. If this method is not called
  //! after construction the default is <ToBuild> = IN.
  Standard_EXPORT void Builds(const TopAbs_State ToBuild);

  //! Returns True if there are more new edges to build.
  Standard_EXPORT Standard_Boolean MoreEdges() const;

  //! Proceeds  to  the  next  edge to  build.  Skip all
  //! remaining vertices on the current edge.
  Standard_EXPORT void NextEdge();

  //! True if there are more vertices in the current new
  //! edge.
  Standard_EXPORT Standard_Boolean MoreVertices() const;

  //! Proceeds to the next vertex of the current edge.
  Standard_EXPORT void NextVertex();

  //! Returns the current vertex of the current edge.
  Standard_EXPORT const HLRAlgo_Intersection& Current() const;

  //! Returns True if the  current vertex comes from the
  //! boundary of the edge.
  Standard_EXPORT Standard_Boolean IsBoundary() const;

  //! Returns  True if    the  current  vertex  was   an
  //! interference.
  Standard_EXPORT Standard_Boolean IsInterference() const;

  //! Returns the new orientation of the current vertex.
  Standard_EXPORT TopAbs_Orientation Orientation() const;

  Standard_EXPORT void Destroy();

  ~HLRBRep_EdgeBuilder() { Destroy(); }

protected:
private:
  TopAbs_State              toBuild;
  Handle(HLRBRep_AreaLimit) myLimits;
  Handle(HLRBRep_AreaLimit) left;
  Handle(HLRBRep_AreaLimit) right;
  Standard_Integer          current;
};

#endif // _HLRBRep_EdgeBuilder_HeaderFile
