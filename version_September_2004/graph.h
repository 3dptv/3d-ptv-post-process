//---------------------------------------------------------------------------
#ifndef graphH
#define graphH
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
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TgraphForm : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TLineSeries *Series3;
        TChart *Chart2;
        TLineSeries *Series4;
        TEdit *fileNum2Edit;
        TLabel *Label1;
        TChart *Chart3;
        TLineSeries *Series5;
        TLineSeries *Series6;
        TLineSeries *Series7;
        TChart *Chart6;
        TLineSeries *Series14;
        TLineSeries *Series15;
        TLineSeries *Series16;
        TLineSeries *Series17;
        TLineSeries *Series18;
        TLineSeries *Series19;
        TLineSeries *Series20;
        TLineSeries *Series21;
        TLineSeries *Series22;
        TLineSeries *Series23;
        TEdit *stepFileEdit;
        TLabel *Label2;
        TEdit *stepEdit;
        TLabel *Label3;
        TCheckBox *readyCheck;
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TgraphForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TgraphForm *graphForm;
//---------------------------------------------------------------------------
#endif
