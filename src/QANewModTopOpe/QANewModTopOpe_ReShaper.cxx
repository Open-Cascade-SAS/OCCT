// File:	QANewModTopOpe_ReShaper.cxx
// Created:	Thu Feb  7 12:37:08 2002
// Author:	Igor FEOKTISTOV <ifv@nnov.matra-dtv.fr>
// Copyright:	 SAMTECH S.A. 2002

const static char sccsid[] = "@(#) QANewModTopOpe_ReShaper.cxx 4.0-1, 04/28/03@(#)";

// Lastly modified by :
// +---------------------------------------------------------------------------+
// !       ifv ! Creation                                ! 7-02-2002! 3.0-00-2!
// +---------------------------------------------------------------------------+


#include <QANewModTopOpe_ReShaper.ixx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>

QANewModTopOpe_ReShaper::QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape):
       myInitShape(TheInitialShape)
{

}
QANewModTopOpe_ReShaper::QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape, 
				       const TopTools_MapOfShape& TheMap):
       myInitShape(TheInitialShape)
{
  myMap.Assign(TheMap);
}

QANewModTopOpe_ReShaper::QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape,
				       const Handle(TopTools_HSequenceOfShape)& TheShapeToBeRemoved):
       myInitShape(TheInitialShape)
{

  Standard_Integer i, n = TheShapeToBeRemoved->Length();
  for(i = 1; i <= n; i++) {
    myMap.Add(TheShapeToBeRemoved->Value(i));
  }

}

void QANewModTopOpe_ReShaper::Remove(const TopoDS_Shape& TheS) 
{
  myMap.Add(TheS);
}

void QANewModTopOpe_ReShaper::Perform() 
{
  
  BRepBuilderAPI_Copy aCopier;

  if(myMap.IsEmpty()) {
    
    aCopier.Perform(myInitShape);
    myResult = aCopier.Shape();
    return;

  }

  BRep_Builder aBB;
  myResult.Nullify();
  aBB.MakeCompound(TopoDS::Compound(myResult));
  TopoDS_Iterator anIter(myInitShape);

  for(; anIter.More(); anIter.Next()) {

    const TopoDS_Shape& aS = anIter.Value();
    
    if(myMap.Contains(aS)) continue;

    if(aS.ShapeType() != TopAbs_COMPOUND) {
      aCopier.Perform(aS);
      aBB.Add(myResult, aCopier.Shape());
    }
    else {
      Handle(QANewModTopOpe_ReShaper) aR = new QANewModTopOpe_ReShaper(aS, myMap);
      aR->Perform();
      const TopoDS_Shape& aSp = aR->GetResult();
      if(aSp.ShapeType() == TopAbs_COMPOUND) {
	TopoDS_Iterator anIt(aSp);
	if(anIt.More()) {
	  aBB.Add(myResult, aSp);
	}
      }
      else {
	aBB.Add(myResult, aSp);
      }
    }

  }

  if(myResult.ShapeType() == TopAbs_COMPOUND) {
    anIter.Initialize(myResult);
    if(anIter.More()) {
      const TopoDS_Shape& aSp = anIter.Value();
      anIter.Next();
      if(!anIter.More()) myResult = aSp;
    }
  }
  
}

const TopoDS_Shape& QANewModTopOpe_ReShaper::GetResult() const
{
  return myResult;
}

void QANewModTopOpe_ReShaper::Clear() 
{
  myMap.Clear();
  myResult.Nullify();
}

// @@SDM: begin

// Copyright SAMTECH ..........................................Version    3.0-00
// Lastly modified by : ifv                                    Date :  7-02-2002

// File history synopsis (creation,modification,correction)
// +---------------------------------------------------------------------------+
// ! Developer !              Comments                   !   Date   ! Version  !
// +-----------!-----------------------------------------!----------!----------+
// !       ifv ! Creation                                ! 7-02-2002! 3.0-00-2!
// +---------------------------------------------------------------------------+

// @@SDM: end
