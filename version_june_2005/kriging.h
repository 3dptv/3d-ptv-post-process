//---------------------------------------------------------------------------
#ifndef krigingH
#define krigingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//---------------------------------------------------------------------------
class TkrigingForm : public TForm
{
__published:	// IDE-managed Components
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TCheckBox *readyCheckbox;
        TEdit *maxiEdit;
        TLabel *Label1;
        TEdit *stepEdit;
        TLabel *Label2;
        TButton *closeBtn;
        void __fastcall closeBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TkrigingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TkrigingForm *krigingForm;
//---------------------------------------------------------------------------
#endif
