// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_Face.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESSolid_Face::IGESSolid_Face ()    {  }


    void  IGESSolid_Face::Init
  (const Handle(IGESData_IGESEntity)&     aSurface,
   const Standard_Boolean                 OuterLoopFlag,
   const Handle(IGESSolid_HArray1OfLoop)& Loops)
{
  if (Loops->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESSolid_Face : Init");
  theSurface   = aSurface;
  hasOuterLoop = OuterLoopFlag;
  theLoops     = Loops;
  InitTypeAndForm(510,1);
}

    Handle(IGESData_IGESEntity)  IGESSolid_Face::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESSolid_Face::NbLoops () const
{
  return theLoops->Length();
}

    Standard_Boolean  IGESSolid_Face::HasOuterLoop () const
{
  return hasOuterLoop;
}

    Handle(IGESSolid_Loop)  IGESSolid_Face::Loop (const Standard_Integer Index) const
{
  return theLoops->Value(Index);
}

