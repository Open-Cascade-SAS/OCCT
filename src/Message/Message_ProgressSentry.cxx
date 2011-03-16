#include <Message_ProgressSentry.ixx>
  
//=======================================================================
//function : Message_ProgressSentry
//purpose  : 
//=======================================================================

Message_ProgressSentry::Message_ProgressSentry (const Handle(Message_ProgressIndicator) &progress,
                                                  const Standard_CString name,
                                                  const Standard_Real min, 
                                                  const Standard_Real max, 
                                                  const Standard_Real step,
                                                  const Standard_Boolean isInf,
                                                  const Standard_Real newScopeSpan) :
       myProgress(progress), myActive(!progress.IsNull())
{
  if ( ! myActive ) return;
  progress->SetName ( name );
  progress->SetScale ( min, max, step, isInf );
  progress->NewScope ( newScopeSpan >0 ? newScopeSpan : step );
}

//=======================================================================
//function : Message_ProgressSentry
//purpose  : 
//=======================================================================

Message_ProgressSentry::Message_ProgressSentry (const Handle(Message_ProgressIndicator) &progress,
                                                  const Handle(TCollection_HAsciiString) &name,
                                                  const Standard_Real min, 
                                                  const Standard_Real max, 
                                                  const Standard_Real step,
                                                  const Standard_Boolean isInf,
                                                  const Standard_Real newScopeSpan) :
       myProgress(progress), myActive(!progress.IsNull())
{
  if ( ! myActive ) return;
  progress->SetName ( name );
  progress->SetScale ( min, max, step, isInf );
  progress->NewScope ( newScopeSpan >0 ? newScopeSpan : step );
}
