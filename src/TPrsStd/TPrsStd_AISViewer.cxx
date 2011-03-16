// File:	TPrsStd_AISViewer.cxx
// Created:	Wed Sep 30 09:30:28 1998
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <TPrsStd_AISViewer.ixx>
#include <TDF_Label.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TPrsStd_AISViewer::GetID () 
{  
  static Standard_GUID TPrsStd_AISViewerID("04fb4d05-5690-11d1-8940-080009dc3333");
  return TPrsStd_AISViewerID;
}
//=======================================================================
//function : New
//purpose  : 
//=======================================================================

Handle(TPrsStd_AISViewer) TPrsStd_AISViewer::New (const TDF_Label& acces, 
						  const Handle(V3d_Viewer)& viewer)
{   
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(),V)) { 
    Standard_DomainError::Raise("already done");
  }
  V = new TPrsStd_AISViewer();
  V->SetInteractiveContext (new AIS_InteractiveContext(viewer));
  acces.Root().AddAttribute(V);
  return V; 
}

//=======================================================================
//function : New
//purpose  : 
//=======================================================================
Handle(TPrsStd_AISViewer) TPrsStd_AISViewer::New (const TDF_Label& acces, 
						  const Handle(AIS_InteractiveContext)& IC) 
{   
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(),V)) { 
    Standard_DomainError::Raise("already done");
  }
  V = new TPrsStd_AISViewer();
  V->SetInteractiveContext (IC);
  acces.Root().AddAttribute(V);
  return V; 
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISViewer::Find (const TDF_Label& acces, 
					  Handle(TPrsStd_AISViewer)& V) 
{ 
  return (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(),V)); 
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISViewer::Find (const TDF_Label& acces, 
					  Handle(AIS_InteractiveContext)& IC)
{ 
  Handle(TPrsStd_AISViewer) V; 
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(),V)) { 
    IC = V->GetInteractiveContext();
    return Standard_True; 
  }
  return Standard_False; 
} 

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Standard_Boolean TPrsStd_AISViewer::Find (const TDF_Label& acces, 
					  Handle(V3d_Viewer)& VIEW) 
{ 
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(),V)) { 
    VIEW = V->GetInteractiveContext()->CurrentViewer();
    return Standard_True; 
  }
  return Standard_False; 
}

//=======================================================================
//function : Has
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISViewer::Has (const TDF_Label& acces) 
{ 
  return (acces.Root().IsAttribute(TPrsStd_AISViewer::GetID())); 
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
void TPrsStd_AISViewer::Update (const TDF_Label& acces)  
{ 
  Handle(TPrsStd_AISViewer) viewer; 
  if (Find (acces,viewer)) viewer->Update(); 
}


//=======================================================================
//function : TPrsStd_AISViewer
//purpose  : 
//=======================================================================

TPrsStd_AISViewer::TPrsStd_AISViewer()
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void TPrsStd_AISViewer::Update () const
{
  myInteractiveContext->UpdateCurrentViewer();  
}
 
//=======================================================================
//function : SetInteractiveContext
//purpose  : 
//=======================================================================

void TPrsStd_AISViewer::SetInteractiveContext(const Handle(AIS_InteractiveContext)& ctx) 
{
  myInteractiveContext = ctx;
}

//=======================================================================
//function : GetInteractiveContext
//purpose  : 
//=======================================================================

Handle(AIS_InteractiveContext) TPrsStd_AISViewer::GetInteractiveContext() const
{
  return myInteractiveContext;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TPrsStd_AISViewer::ID() const { return GetID(); }


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TPrsStd_AISViewer::Restore(const Handle(TDF_Attribute)& with) 
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TPrsStd_AISViewer::NewEmpty() const
{
  return new  TPrsStd_AISViewer();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TPrsStd_AISViewer::Paste (const Handle(TDF_Attribute)& into,
			     const Handle(TDF_RelocationTable)& RT) const
{
}


