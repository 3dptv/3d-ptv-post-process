//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "traj.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TtrajForm *trajForm;
//---------------------------------------------------------------------------
__fastcall TtrajForm::TtrajForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TtrajForm::Button1Click(TObject *Sender)
{
     startPointEdit->Text=IntToStr(StrToInt(startPointEdit->Text)+1);
     startPointEdit->Refresh();        
}
//---------------------------------------------------------------------------





void __fastcall TtrajForm::startFileEditChange(TObject *Sender)
{
      changedFlag->Checked=true;
}
//---------------------------------------------------------------------------

