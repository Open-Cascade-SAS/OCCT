// File:	BRepBuilderAPI_GTransform.cxx
// Created:	Mon Dec 30 17:12:14 1996
// Author:	Stagiaire Mary FABIEN
//		<fbi@zozox.paris1.matra-dtv.fr>


#include <BRepBuilderAPI_GTransform.ixx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepTools_GTrsfModification.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <gp.hxx>

#include <TopTools_ListOfShape.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
//=======================================================================
//function : BRepBuilderAPI_GTransform
//purpose  : 
//=======================================================================

BRepBuilderAPI_GTransform::BRepBuilderAPI_GTransform (const gp_GTrsf& T) :
  myGTrsf(T)
{
  myModification = new BRepTools_GTrsfModification(T);
}


//=======================================================================
//function : BRepBuilderAPI_GTransform
//purpose  : 
//=======================================================================

BRepBuilderAPI_GTransform::BRepBuilderAPI_GTransform (const TopoDS_Shape& S,
					const gp_GTrsf& T,
					const Standard_Boolean Copy) :
  myGTrsf(T)
{
  myModification = new BRepTools_GTrsfModification(T);
  Perform(S,Copy);
}



//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_GTransform::Perform(const TopoDS_Shape& S,
				 const Standard_Boolean Copy)
{
  BRepBuilderAPI_NurbsConvert nc;
  nc.Perform(S, Copy);
  myHist.Add(S,nc);
  TopoDS_Shape Slocal = nc.Shape();
  Handle(BRepTools_GTrsfModification) theModif =
    Handle(BRepTools_GTrsfModification)::DownCast(myModification);
  theModif->GTrsf() = myGTrsf;
  DoModif(Slocal,myModification);
//  myHist.Filter (Shape());
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_GTransform::Modified
  (const TopoDS_Shape& F)
{
  myGenerated.Clear();
  const TopTools_DataMapOfShapeListOfShape& M = myHist.Modification();
  if (M.IsBound(F)) { 
    TopTools_ListOfShape Li;
    TopTools_ListIteratorOfListOfShape itL(M(F));
    for (;itL.More();itL.Next())
      Li.Assign(BRepBuilderAPI_ModifyShape::Modified(itL.Value()));
  }
  return myGenerated;
}


//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepBuilderAPI_GTransform::ModifiedShape
  (const TopoDS_Shape& S) const
{
  const TopTools_DataMapOfShapeListOfShape &aMapModif = myHist.Modification();
  TopoDS_Shape                              aShape    = S;

  if (aMapModif.IsBound(S)) {
    const TopTools_ListOfShape &aListModShape = aMapModif(S);
    Standard_Integer            aNbShapes     = aListModShape.Extent();

    if (aNbShapes > 0)
      aShape = aListModShape.First();
  }

  return BRepBuilderAPI_ModifyShape::ModifiedShape(aShape);
}

