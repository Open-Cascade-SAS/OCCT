// File:	QADBMReflex_OCC749Prs.cxx
// Created:	Mon Oct  7 15:01:41 2002
// Author:	QA Admin
//		<qa@russox>


#include <QADBMReflex_OCC749Prs.ixx>

//=======================================================================
//function : QADBMReflex_OCC749Prs
//purpose  : 
//=======================================================================

QADBMReflex_OCC749Prs::QADBMReflex_OCC749Prs( const Standard_Boolean Reflection, 
					     const Quantity_Color& InteriorColor,
					     const Quantity_Color& EdgeColor,
					     const Quantity_Color& EdgeColor2,
					     const Standard_Integer XCount,
					     const Standard_Integer YCount,
					     const Standard_Integer BoxSize,
					     const Graphic3d_MaterialAspect& MaterialAspect,
					     const Standard_Boolean Material,
					     const Standard_Boolean Timer ) :
       myReflection( Reflection ),
       myInteriorColor( InteriorColor ),
       myEdgeColor( EdgeColor ),
       myEdgeColor2( EdgeColor2 ),
       myXCount( XCount ),
       myYCount( YCount ),
       myBoxSize( BoxSize ),
       myMaterialAspect( MaterialAspect ),
       myMaterial( Material ),
       myTimer( Timer )
{
  SetHilightMode(0);
  SetSelectionMode(0);
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
					     const Standard_Integer aMode)
{
}

//=======================================================================
//function : SetReflection
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetReflection( const Standard_Boolean Reflection )
{
  myReflection = Reflection;
}
    
//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetColor(const Quantity_Color& InteriorColor,
				     const Quantity_Color& EdgeColor )
{
  myInteriorColor = InteriorColor;
  myEdgeColor = EdgeColor;
}
    
//=======================================================================
//function : SetEdgeColor2
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetEdgeColor2(const Quantity_Color& EdgeColor2 )
{
  myEdgeColor2 = EdgeColor2;
}

//=======================================================================
//function : SetXYCount
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetXYCount(const Standard_Integer XCount,
				       const Standard_Integer YCount)
{
  myXCount = XCount;
  myYCount = YCount;
}

//=======================================================================
//function : SetBoxSize
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetBoxSize(const Standard_Integer BoxSize)
{
  myBoxSize = BoxSize;
}


//=======================================================================
//function : SetMaterialAspect
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetMaterialAspect(const Graphic3d_MaterialAspect& MaterialAspect)
{
  myMaterialAspect = MaterialAspect;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetMaterial( const Standard_Boolean Material)
{
  myMaterial = Material;
}

//=======================================================================
//function : SetTimer
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749Prs::SetTimer( const Standard_Boolean Timer)
{
  myTimer = Timer;
}

//=======================================================================
//function : GetReflection
//purpose  : 
//=======================================================================

Standard_Boolean QADBMReflex_OCC749Prs::GetReflection() const
{
  return myReflection;
}

//=======================================================================
//function : GetInteriorColor
//purpose  : 
//=======================================================================

Quantity_Color QADBMReflex_OCC749Prs::GetInteriorColor() const
{
  return myInteriorColor;
}

//=======================================================================
//function : GetEdgeColor
//purpose  : 
//=======================================================================

Quantity_Color QADBMReflex_OCC749Prs::GetEdgeColor() const
{
  return myEdgeColor;
}

//=======================================================================
//function : GetEdgeColor2
//purpose  : 
//=======================================================================

Quantity_Color QADBMReflex_OCC749Prs::GetEdgeColor2() const
{
  return myEdgeColor2;
}

//=======================================================================
//function : GetXCount
//purpose  : 
//=======================================================================

Standard_Integer QADBMReflex_OCC749Prs::GetXCount() const
{
  return myXCount;
}

//=======================================================================
//function : GetYCount
//purpose  : 
//=======================================================================

Standard_Integer QADBMReflex_OCC749Prs::GetYCount() const
{
  return myYCount;
}

//=======================================================================
//function : GetBoxSize
//purpose  : 
//=======================================================================

Standard_Integer QADBMReflex_OCC749Prs::GetBoxSize() const
{
  return myBoxSize;
}

//=======================================================================
//function : GetMaterialAspect
//purpose  : 
//=======================================================================

Graphic3d_MaterialAspect QADBMReflex_OCC749Prs::GetMaterialAspect() const
{
  return myMaterialAspect;
}

//=======================================================================
//function : GetMaterial
//purpose  : 
//=======================================================================

Standard_Boolean QADBMReflex_OCC749Prs::GetMaterial() const
{
  return myMaterial;
}

//=======================================================================
//function : GetTimer
//purpose  : 
//=======================================================================

Standard_Boolean QADBMReflex_OCC749Prs::GetTimer() const
{
  return myTimer;
}
