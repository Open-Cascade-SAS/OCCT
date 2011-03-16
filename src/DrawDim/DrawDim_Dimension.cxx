// File:	DrawDim_Dimension.cxx
// Created:	Mon Apr 21 15:30:02 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <DrawDim_Dimension.ixx>
#include <Standard_DomainError.hxx>
#include <Draw_Interpretor.hxx>
#include <TCollection_AsciiString.hxx>


//=======================================================================
//function : DrawDim_Dimension
//purpose  : 
//=======================================================================

DrawDim_Dimension::DrawDim_Dimension()
     : is_valued(Standard_False),
       myTextColor(Draw_blanc)
{
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void DrawDim_Dimension::SetValue (const Standard_Real avalue)
{
  is_valued = Standard_True;
  myValue = avalue;
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

Standard_Real DrawDim_Dimension::GetValue() const 
{
  if (!is_valued) Standard_DomainError::Raise();
  return myValue;
}

//=======================================================================
//function : IsValued
//purpose  : 
//=======================================================================

Standard_Boolean DrawDim_Dimension::IsValued() const 
{
  return is_valued;
}

//=======================================================================
//function : TextColor
//purpose  : 
//=======================================================================

Draw_Color DrawDim_Dimension::TextColor() const
{
  return myTextColor;
}

//=======================================================================
//function : TextColor
//purpose  : 
//=======================================================================

void DrawDim_Dimension::TextColor(const Draw_Color& C)
{
   myTextColor = C;
}

//=======================================================================
//function : DrawText
//purpose  : 
//=======================================================================

void DrawDim_Dimension::DrawText(const gp_Pnt& P, Draw_Display& D) const
{
  TCollection_AsciiString t = Name();
  if (is_valued) {
    t+="=";
    Standard_Integer l = t.Length();
    t+= myValue;
    for (Standard_Integer i = l; i <= t.Length(); i++) {
      if (t.Value(i) == '.') { t.Trunc(i+2); break; }
    }
  }

  D.SetColor(myTextColor);
  D.DrawString(P,t.ToCString());
}
