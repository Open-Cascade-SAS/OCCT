//--------------------------------------------------------------------
//
//  File Name : IGESSolid_Face.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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

