// File:	ShapeAnalysis_FreeBoundData.cxx
// Created:	Tue Aug 25 12:42:28 1998
// Author:	Pavel DURANDIN
//		<pdn@nnov.matra-dtv.fr>


#include <ShapeAnalysis_FreeBoundData.ixx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : ShapeAnalysis_FreeBoundData
//purpose  : Empty constructor
//=======================================================================

ShapeAnalysis_FreeBoundData::ShapeAnalysis_FreeBoundData()
{
  myNotches = new TopTools_HSequenceOfShape();
  Clear();
}

//=======================================================================
//function : ShapeAnalysis_FreeBoundData
//purpose  : Creates object with contour given in the form of TopoDS_Wire
//=======================================================================

ShapeAnalysis_FreeBoundData::ShapeAnalysis_FreeBoundData(const TopoDS_Wire& freebound)
{
  myNotches = new TopTools_HSequenceOfShape();
  Clear();
  SetFreeBound(freebound);
}

//=======================================================================
//function : Clear
//purpose  : Clears all properties of the contour. Contour bound itself is not cleared.
//=======================================================================

void ShapeAnalysis_FreeBoundData::Clear()
{
  myArea = -1;
  myPerimeter = -1;
  myRatio = -1;
  myWidth = -1;
  myNotches->Clear();
  myNotchesParams.Clear();
}

//=======================================================================
//function : AddNotch
//purpose  : Adds notch on free bound with its maximum width
//=======================================================================

void ShapeAnalysis_FreeBoundData::AddNotch(const TopoDS_Wire& notch,const Standard_Real width)
{
  if (myNotchesParams.IsBound(notch)) return;
  myNotches->Append(notch);
  myNotchesParams.Bind(notch, width);
}


//=======================================================================
//function : NotchWidth
//purpose  : Returns maximum width of notch specified by its rank number
//    	     on the contour
//=======================================================================

Standard_Real ShapeAnalysis_FreeBoundData::NotchWidth(const Standard_Integer index) const
{
  TopoDS_Wire wire = TopoDS::Wire(myNotches -> Value(index));
  return myNotchesParams.Find(wire);
}

//=======================================================================
//function : NotchWidth
//purpose  : Returns maximum width of notch specified as TopoDS_Wire
//    	     on the contour
//=======================================================================

Standard_Real ShapeAnalysis_FreeBoundData::NotchWidth(const TopoDS_Wire& notch) const
{
  return myNotchesParams.Find(notch);
}
