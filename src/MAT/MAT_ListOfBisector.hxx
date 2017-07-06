// Created on: 1992-09-22
// Created by: Gilles DEBARBOUILLE
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

#ifndef _MAT_ListOfBisector_HeaderFile
#define _MAT_ListOfBisector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class MAT_TListNodeOfListOfBisector;
class MAT_Bisector;


class MAT_ListOfBisector;
DEFINE_STANDARD_HANDLE(MAT_ListOfBisector, Standard_Transient)


class MAT_ListOfBisector : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT_ListOfBisector();
  
  Standard_EXPORT void First();
  
  Standard_EXPORT void Last();
  
  Standard_EXPORT void Init (const Handle(MAT_Bisector)& aniten);
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT void Previous();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT Handle(MAT_Bisector) Current() const;
  
  Standard_EXPORT void Current (const Handle(MAT_Bisector)& anitem) const;
  
  Standard_EXPORT Handle(MAT_Bisector) FirstItem() const;
  
  Standard_EXPORT Handle(MAT_Bisector) LastItem() const;
  
  Standard_EXPORT Handle(MAT_Bisector) PreviousItem() const;
  
  Standard_EXPORT Handle(MAT_Bisector) NextItem() const;
  
    Standard_Integer Number() const;
  
    Standard_Integer Index() const;
  
  Standard_EXPORT Handle(MAT_Bisector) Brackets (const Standard_Integer anindex);
Handle(MAT_Bisector) operator() (const Standard_Integer anindex)
{
  return Brackets(anindex);
}
  
  Standard_EXPORT void Unlink();
  
  Standard_EXPORT void LinkBefore (const Handle(MAT_Bisector)& anitem);
  
  Standard_EXPORT void LinkAfter (const Handle(MAT_Bisector)& anitem);
  
  Standard_EXPORT void FrontAdd (const Handle(MAT_Bisector)& anitem);
  
  Standard_EXPORT void BackAdd (const Handle(MAT_Bisector)& anitem);
  
  Standard_EXPORT void Permute();
  
  Standard_EXPORT void Loop() const;
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT void Dump (const Standard_Integer ashift, const Standard_Integer alevel);




  DEFINE_STANDARD_RTTI_INLINE(MAT_ListOfBisector,Standard_Transient)

protected:




private:


  Handle(MAT_TListNodeOfListOfBisector) thefirstnode;
  Handle(MAT_TListNodeOfListOfBisector) thelastnode;
  Handle(MAT_TListNodeOfListOfBisector) thecurrentnode;
  Standard_Integer thecurrentindex;
  Standard_Integer thenumberofitems;


};

#define Item Handle(MAT_Bisector)
#define Item_hxx <MAT_Bisector.hxx>
#define MAT_TListNode MAT_TListNodeOfListOfBisector
#define MAT_TListNode_hxx <MAT_TListNodeOfListOfBisector.hxx>
#define Handle_MAT_TListNode Handle(MAT_TListNodeOfListOfBisector)
#define MAT_TList MAT_ListOfBisector
#define MAT_TList_hxx <MAT_ListOfBisector.hxx>
#define Handle_MAT_TList Handle(MAT_ListOfBisector)

#include <MAT_TList.lxx>

#undef Item
#undef Item_hxx
#undef MAT_TListNode
#undef MAT_TListNode_hxx
#undef Handle_MAT_TListNode
#undef MAT_TList
#undef MAT_TList_hxx
#undef Handle_MAT_TList




#endif // _MAT_ListOfBisector_HeaderFile
