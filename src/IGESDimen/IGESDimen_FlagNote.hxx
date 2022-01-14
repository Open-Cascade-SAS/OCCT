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

#ifndef _IGESDimen_FlagNote_HeaderFile
#define _IGESDimen_FlagNote_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESDimen_GeneralNote;
class gp_Pnt;
class IGESDimen_LeaderArrow;


class IGESDimen_FlagNote;
DEFINE_STANDARD_HANDLE(IGESDimen_FlagNote, IGESData_IGESEntity)

//! defines FlagNote, Type <208> Form <0>
//! in package IGESDimen
//! Is label information formatted in different ways
class IGESDimen_FlagNote : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_FlagNote();
  
  //! This method is used to set the fields of the class
  //! FlagNote
  //! - leftCorner  : Lower left corner of the Flag
  //! - anAngle     : Rotation angle in radians
  //! - aNote       : General Note Entity
  //! - someLeaders : Leader Entities
  Standard_EXPORT void Init (const gp_XYZ& leftCorner, const Standard_Real anAngle, const Handle(IGESDimen_GeneralNote)& aNote, const Handle(IGESDimen_HArray1OfLeaderArrow)& someLeaders);
  
  //! returns Lower Left coordinate of Flag as Pnt from package gp
  Standard_EXPORT gp_Pnt LowerLeftCorner() const;
  
  //! returns Lower Left coordinate of Flag as Pnt from package gp
  //! after Transformation.
  Standard_EXPORT gp_Pnt TransformedLowerLeftCorner() const;
  
  //! returns Rotation angle in radians
  Standard_EXPORT Standard_Real Angle() const;
  
  //! returns General Note Entity
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  //! returns number of Arrows (Leaders) or zero
  Standard_EXPORT Standard_Integer NbLeaders() const;
  
  //! returns Leader Entity
  //! raises exception if Index <= 0 or Index > NbLeaders()
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) Leader (const Standard_Integer Index) const;
  
  //! returns Height computed by the formula :
  //! Height = 2 * CH   where CH is from theNote
  Standard_EXPORT Standard_Real Height() const;
  
  //! returns the Character Height (from General Note)
  Standard_EXPORT Standard_Real CharacterHeight() const;
  
  //! returns Length computed by the formula :
  //! Length = TW + 0.4*CH  where CH is from theNote
  //! and TW is from theNote
  Standard_EXPORT Standard_Real Length() const;
  
  //! returns the Text Width (from General Note)
  Standard_EXPORT Standard_Real TextWidth() const;
  
  //! returns TipLength computed by the formula :
  //! TipLength = 0.5 * H / tan 35(deg)  where H is Height()
  Standard_EXPORT Standard_Real TipLength() const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_FlagNote,IGESData_IGESEntity)

protected:




private:


  gp_XYZ theLowerLeftcorner;
  Standard_Real theAngle;
  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_HArray1OfLeaderArrow) theLeaders;


};







#endif // _IGESDimen_FlagNote_HeaderFile
