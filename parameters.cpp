//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "parameters.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TparamForm *paramForm;
//---------------------------------------------------------------------------
__fastcall TparamForm::TparamForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TparamForm::closeBtnClick(TObject *Sender)
{
     //first check if the stuff is ok
     bool ok=true;

     if(StrToFloat(toleranceEdit->Text)>1000000){
          toleranceEdit->Text=FloatToStr(1000000);
          ok=false;
     }
     if(StrToFloat(toleranceEdit->Text)<0.01){
          toleranceEdit->Text=FloatToStr(0.01);
          ok=false;
     }

     if(StrToFloat(tolMaxVelEdit->Text)>10){
          tolMaxVelEdit->Text=FloatToStr(10);
          ok=false;
     }
     if(StrToFloat(tolMaxVelEdit->Text)<0.001){
          tolMaxVelEdit->Text=FloatToStr(0.001);
          ok=false;
     }
     
     if(ok){
        paramForm->Close();
     }
     else{
          
     }
}
//---------------------------------------------------------------------------
