// File:	PPrsStd_AISPresentation.cxx
// Created:	Thu Jul 8 16:40:40 1999
// Author:	srn



// Lastly modified by :
// +---------------------------------------------------------------------------+
// !      ivan ! sauvegarde du mode                      !  $Date: 2002-05-27 18:10:24 $!    %V%-%L%!
// +---------------------------------------------------------------------------+
#include <PPrsStd_AISPresentation_1.ixx>


//=======================================================================
//function : PPrsStd_AISPresentation
//purpose  : 
//=======================================================================

PPrsStd_AISPresentation_1::PPrsStd_AISPresentation_1()
{
  myMode=0;
}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation_1::SetDisplayed (const Standard_Boolean B)       
{
  myIsDisplayed = B ;
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean PPrsStd_AISPresentation_1::IsDisplayed() const 
{
    return myIsDisplayed;
}


//=======================================================================
//function : SetDriverGUID
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetDriverGUID( const Handle(PCollection_HExtendedString)&  guid)
{
  myDriverGUID = guid;
}

//=======================================================================
//function : GetDriverGUID
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PPrsStd_AISPresentation_1::GetDriverGUID( void ) const
{
  return myDriverGUID;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation_1::Color() const
{
  return myColor;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetColor(const Standard_Integer C) 
{
  myColor = C;
}

//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation_1::Transparency() const
{
  return myTransparency;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetTransparency(const Standard_Real T) 
{
  myTransparency = T;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation_1::Material() const
{
  return myMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetMaterial(const Standard_Integer M) 
{
   myMaterial = M;
}

//=======================================================================
//function : Width 
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation_1::Width() const
{
  return myWidth;  
}




//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation_1::SetWidth(const Standard_Real W) 
{
  myWidth = W;
}



//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

Standard_Integer PPrsStd_AISPresentation_1::Mode() const
{
  return myMode;
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation_1::SetMode(const Standard_Integer M) 
{
   myMode = M;
}

// @@SDM: begin


// File history synopsis (creation,modification,correction)
// +---------------------------------------------------------------------------+
// ! Developer !              Comments                   !   Date   ! Version  !
// +-----------!-----------------------------------------!----------!----------+
// !  fontaine ! Creation                                !  $Date: 2002-05-27 18:10:24 $!    %V%-%L%!
// !      ivan ! sauvegarde du mode                      !  $Date: 2002-05-27 18:10:24 $!    %V%-%L%!
// +---------------------------------------------------------------------------+

// @@SDM: end
