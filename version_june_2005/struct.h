//---------------------------------------------------------------------------
#ifndef structH
#define structH
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
class TstructForm : public TForm
{
__published:	// IDE-managed Components
        TEdit *fileNumEdit;
        TLabel *Label1;
        TEdit *KolConstEdit;
        TLabel *Label2;
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TLineSeries *Series3;
        TLineSeries *Series4;
        TLineSeries *Series5;
        TEdit *stepFileEdit;
        TLabel *Label3;
        TEdit *stepEdit;
        TLabel *Label4;
        TCheckBox *readyCheck;
        TEdit *uEdit;
        TEdit *vEdit;
        TEdit *wEdit;
        TEdit *axEdit;
        TEdit *AyEdit;
        TEdit *AzEdit;
private:	// User declarations
public:		// User declarations
        __fastcall TstructForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TstructForm *structForm;
//---------------------------------------------------------------------------
#endif
