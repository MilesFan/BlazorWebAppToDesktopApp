//In project referrence, add the main DLL of the DotNet application
//In this case, it is "BlazorWebApp.dll".
using namespace System;
extern "C" __declspec(dllexport) void run(int argc, char* argv[])
{
	array<String^>^ args = gcnew array<String^>(argc);
	for (int i = 0; i < argc; i++)
	{
		args[i] = gcnew String(argv[i]);
	}
	BlazorWebApp::Program::Main(args);
}