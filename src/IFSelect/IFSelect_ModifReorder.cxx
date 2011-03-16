#include <IFSelect_ModifReorder.ixx>
#include <Interface_ShareTool.hxx>
#include <Interface_EntityIterator.hxx>

    IFSelect_ModifReorder::IFSelect_ModifReorder (const Standard_Boolean rootlast)
    : IFSelect_Modifier (Standard_True)    {  thertl = rootlast;  }

    void  IFSelect_ModifReorder::Perform
  (IFSelect_ContextModif& ctx, const Handle(Interface_InterfaceModel)& target,
   const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC) const
{
  Interface_ShareTool sht (ctx.OriginalGraph());
  Interface_EntityIterator list = sht.All (ctx.OriginalModel(),thertl);
  target->ClearEntities();
  for (list.Start(); list.More(); list.Next())  target->AddEntity (list.Value());
}

TCollection_AsciiString  IFSelect_ModifReorder::Label () const
{
  Standard_CString astr = (Standard_CString ) ( thertl ? "Reorder, Roots last" : "Reorder, Roots first");
  return TCollection_AsciiString( astr ) ;
//    ( thertl ? "Reorder, Roots last" : "Reorder, Roots first");
}
