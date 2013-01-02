//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "graph.h"
//#include "main.h"
//#include "parameters.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TgraphForm *graphForm;
//---------------------------------------------------------------------------
__fastcall TgraphForm::TgraphForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TgraphForm::Button1Click(TObject *Sender)
{
     graphForm->Close();        
}
//---------------------------------------------------------------------------
