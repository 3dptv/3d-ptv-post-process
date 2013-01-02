//---------------------------------------------------------------------------
#ifndef parametersH
#define parametersH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TparamForm : public TForm
{
__published:	// IDE-managed Components
        TEdit *toleranceEdit;
        TLabel *Label36;
        TEdit *tolMaxVelEdit;
        TLabel *Label37;
        void __fastcall closeBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TparamForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TparamForm *paramForm;
//---------------------------------------------------------------------------
#endif
