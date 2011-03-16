#include <MoniTool_SignText.ixx>

TCollection_AsciiString  MoniTool_SignText::TextAlone
  (const Handle(Standard_Transient)& ent) const
{
  Handle(Standard_Transient) nulctx;  // no context
  TCollection_AsciiString atext = Text (ent,nulctx);
  if (atext.Length() == 0) {
    if (ent.IsNull()) atext.AssignCat ("(NULL)");
    else              atext.AssignCat (ent->DynamicType()->Name());
  }
  return atext;
}
