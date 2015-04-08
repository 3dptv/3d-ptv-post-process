//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "xuap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TxuapForm *xuapForm;
//---------------------------------------------------------------------------
__fastcall TxuapForm::TxuapForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TxuapForm::closeBtnClick(TObject *Sender)
{
     xuapForm->Close();        
}
//---------------------------------------------------------------------------
void __fastcall TxuapForm::UpDown1Changing(TObject *Sender,
      bool &AllowChange)
{
      startPointEdit->Text=IntToStr(UpDown1->Position);        
}
//---------------------------------------------------------------------------

