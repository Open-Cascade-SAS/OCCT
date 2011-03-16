// File:	PPrsStd_AISPresentation.cxx
// Created:	Thu Jul 8 16:40:40 1999
// Author:	srn


#include <PPrsStd_AISPresentation.ixx>


//=======================================================================
//function : PPrsStd_AISPresentation
//purpose  : 
//=======================================================================

PPrsStd_AISPresentation::PPrsStd_AISPresentation()
{}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation::SetDisplayed (const Standard_Boolean B)       
{
  myIsDisplayed = B ;
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean PPrsStd_AISPresentation::IsDisplayed() const 
{
    return myIsDisplayed;
}


//=======================================================================
//function : SetDriverGUID
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetDriverGUID( const Handle(PCollection_HExtendedString)&  guid)
{
  myDriverGUID = guid;
}

//=======================================================================
//function : GetDriverGUID
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PPrsStd_AISPresentation::GetDriverGUID( void ) const
{
  return myDriverGUID;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation::Color() const
{
  return myColor;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetColor(const Standard_Integer C) 
{
  myColor = C;
}

//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation::Transparency() const
{
  return myTransparency;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetTransparency(const Standard_Real T) 
{
  myTransparency = T;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation::Material() const
{
  return myMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetMaterial(const Standard_Integer M) 
{
   myMaterial = M;
}

//=======================================================================
//function : Width 
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation::Width() const
{
  return myWidth;  
}

//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetWidth(const Standard_Real W) 
{
  myWidth = W;
}

