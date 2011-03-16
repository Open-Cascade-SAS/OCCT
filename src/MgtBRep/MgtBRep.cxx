// File:	MgtBRep.cxx
// Created:	Mon Jun 14 19:15:15 1993
// Author:	Remi LEQUETTE
//		<rle@zerox>


#include <MgtBRep.ixx>

#include <MgtTopoDS.hxx>
#include <MgtBRep_TranslateTool.hxx>
#include <MgtBRep_TranslateTool1.hxx>

#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>

#include <Geom_Surface.hxx>

#include <BRep_Tool.hxx>

//=======================================================================
//function : Translate
//purpose  : ... a Shape from BRep to PBRep.
//=======================================================================

Handle(PTopoDS_HShape) MgtBRep::Translate
(const TopoDS_Shape&              aShape,
 PTColStd_TransientPersistentMap& aMap,
 const MgtBRep_TriangleMode    aTriMode)
{
  Handle(MgtBRep_TranslateTool) TR = new MgtBRep_TranslateTool(aTriMode);
  return MgtTopoDS::Translate(aShape, TR, aMap);
}

//=======================================================================
//function : Translate
//purpose  : ... a Shape from PBRep to BRep.
//=======================================================================

void MgtBRep::Translate
(const Handle(PTopoDS_HShape)&    aShape,
 PTColStd_PersistentTransientMap& aMap,
 TopoDS_Shape&                    aResult,
 const MgtBRep_TriangleMode    aTriMode)
{
  Handle(MgtBRep_TranslateTool) TR = new MgtBRep_TranslateTool(aTriMode);
  MgtTopoDS::Translate(aShape, TR, aMap, aResult);
}
//=======================================================================
//function : Translate1
//purpose  : ... a Shape from BRep to PBRep.
//=======================================================================

void MgtBRep::Translate1
(const TopoDS_Shape& aShape,
 PTColStd_TransientPersistentMap& aMap,
 PTopoDS_Shape1& aResult,
 const MgtBRep_TriangleMode aTriMode)
{
  Handle(MgtBRep_TranslateTool1) TR = new MgtBRep_TranslateTool1(aTriMode);
  MgtTopoDS::Translate1(aShape, TR, aMap, aResult);
}


//=======================================================================
//function : Translate1
//purpose  : ... a Shape from PBRep to BRep.
//=======================================================================

void MgtBRep::Translate1
(const PTopoDS_Shape1& aShape,
 PTColStd_PersistentTransientMap& aMap,
 TopoDS_Shape& aResult,
 const MgtBRep_TriangleMode aTriMode)
{
  Handle(MgtBRep_TranslateTool1) TR = new MgtBRep_TranslateTool1(aTriMode);
  MgtTopoDS::Translate1(aShape, TR, aMap, aResult);
}
