//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "kriging.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TkrigingForm *krigingForm;
//---------------------------------------------------------------------------
__fastcall TkrigingForm::TkrigingForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TkrigingForm::closeBtnClick(TObject *Sender)
{
        krigingForm->Hide();        
}
//---------------------------------------------------------------------------

