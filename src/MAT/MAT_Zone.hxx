// Created on: 1993-05-27
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _MAT_Zone_HeaderFile
#define _MAT_Zone_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MAT_SequenceOfArc.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <MAT_Side.hxx>
class MAT_BasicElt;
class MAT_Arc;
class MAT_Node;


class MAT_Zone;
DEFINE_STANDARD_HANDLE(MAT_Zone, Standard_Transient)


//! Definition of Zone of Proximity of a BasicElt :
//! ----------------------------------------------
//! A Zone of proximity is the set of the points which are
//! more near from the BasicElt than any other.
class MAT_Zone : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT_Zone();
  
  //! Compute the frontier of the Zone of proximity.
  Standard_EXPORT MAT_Zone(const Handle(MAT_BasicElt)& aBasicElt);
  
  //! Compute the frontier of the Zone of proximity.
  Standard_EXPORT void Perform (const Handle(MAT_BasicElt)& aBasicElt);
  
  //! Return the number Of Arcs On the frontier of <me>.
  Standard_EXPORT Standard_Integer NumberOfArcs() const;
  
  //! Return the  Arc number <Index>  on the frontier.
  //! of  <me>.
  Standard_EXPORT Handle(MAT_Arc) ArcOnFrontier (const Standard_Integer Index) const;
  
  //! Return TRUE if <me> is not empty .
  Standard_EXPORT Standard_Boolean NoEmptyZone() const;
  
  //! Return TRUE if <me> is Limited.
  Standard_EXPORT Standard_Boolean Limited() const;




  DEFINE_STANDARD_RTTIEXT(MAT_Zone,Standard_Transient)

protected:




private:

  
  Standard_EXPORT Handle(MAT_Node) NodeForTurn (const Handle(MAT_Arc)& anArc, const Handle(MAT_BasicElt)& aBasicElt, const MAT_Side aSide) const;

  MAT_SequenceOfArc frontier;
  Standard_Boolean limited;


};







#endif // _MAT_Zone_HeaderFile
