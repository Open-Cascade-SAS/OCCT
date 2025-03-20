// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDimen_AngularDimension_HeaderFile
#define _IGESDimen_AngularDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_WitnessLine;
class IGESDimen_LeaderArrow;
class gp_Pnt2d;

class IGESDimen_AngularDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_AngularDimension, IGESData_IGESEntity)

//! defines AngularDimension, Type <202> Form <0>
//! in package IGESDimen
//! Used to dimension angles
class IGESDimen_AngularDimension : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_AngularDimension();

  //! This method is used to set the fields of the class
  //! AngularDimension
  //! - aNote         : General Note Entity
  //! - aLine         : First Witness Line Entity or Null
  //! Handle
  //! - anotherLine   : Second Witness Line Entity or Null
  //! Handle
  //! - aVertex       : Coordinates of vertex point
  //! - aRadius       : Radius of leader arcs
  //! - aLeader       : First Leader Entity
  //! - anotherLeader : Second Leader Entity
  Standard_EXPORT void Init(const Handle(IGESDimen_GeneralNote)& aNote,
                            const Handle(IGESDimen_WitnessLine)& aLine,
                            const Handle(IGESDimen_WitnessLine)& anotherLine,
                            const gp_XY&                         aVertex,
                            const Standard_Real                  aRadius,
                            const Handle(IGESDimen_LeaderArrow)& aLeader,
                            const Handle(IGESDimen_LeaderArrow)& anotherLeader);

  //! returns the General Note Entity of the Dimension.
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;

  //! returns False if theFirstWitnessLine is Null Handle.
  Standard_EXPORT Standard_Boolean HasFirstWitnessLine() const;

  //! returns the First Witness Line Entity or Null Handle.
  Standard_EXPORT Handle(IGESDimen_WitnessLine) FirstWitnessLine() const;

  //! returns False if theSecondWitnessLine is Null Handle.
  Standard_EXPORT Standard_Boolean HasSecondWitnessLine() const;

  //! returns the Second Witness Line Entity or Null Handle.
  Standard_EXPORT Handle(IGESDimen_WitnessLine) SecondWitnessLine() const;

  //! returns the coordinates of the Vertex point as Pnt2d from gp.
  Standard_EXPORT gp_Pnt2d Vertex() const;

  //! returns the coordinates of the Vertex point as Pnt2d from gp
  //! after Transformation. (Z = 0.0 for Transformation)
  Standard_EXPORT gp_Pnt2d TransformedVertex() const;

  //! returns the Radius of the Leader arcs.
  Standard_EXPORT Standard_Real Radius() const;

  //! returns the First Leader Entity.
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) FirstLeader() const;

  //! returns the Second Leader Entity.
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) SecondLeader() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_AngularDimension, IGESData_IGESEntity)

protected:
private:
  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_WitnessLine) theFirstWitnessLine;
  Handle(IGESDimen_WitnessLine) theSecondWitnessLine;
  gp_XY                         theVertex;
  Standard_Real                 theRadius;
  Handle(IGESDimen_LeaderArrow) theFirstLeader;
  Handle(IGESDimen_LeaderArrow) theSecondLeader;
};

#endif // _IGESDimen_AngularDimension_HeaderFile
