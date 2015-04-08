//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("mainProj.res");
USEFORM("main.cpp", mainForm);
USEFORM("parameters.cpp", paramForm);
USEFORM("graph.cpp", graphForm);
USEFORM("xuap.cpp", xuapForm);
USEFORM("traj.cpp", trajForm);
USEFORM("struct.cpp", structForm);
USEFORM("accuracy.cpp", accuracyForm);
USEFORM("structRepr.cpp", structReprForm);
USEFORM("kriging.cpp", krigingForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "";
                 Application->CreateForm(__classid(TmainForm), &mainForm);
                 Application->CreateForm(__classid(TparamForm), &paramForm);
                 Application->CreateForm(__classid(TgraphForm), &graphForm);
                 Application->CreateForm(__classid(TxuapForm), &xuapForm);
                 Application->CreateForm(__classid(TtrajForm), &trajForm);
                 Application->CreateForm(__classid(TstructForm), &structForm);
                 Application->CreateForm(__classid(TaccuracyForm), &accuracyForm);
                 Application->CreateForm(__classid(TstructReprForm), &structReprForm);
                 Application->CreateForm(__classid(TkrigingForm), &krigingForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
