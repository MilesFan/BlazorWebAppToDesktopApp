#include <stdio.h>
#include <windows.h>

typedef void(__cdecl* run_fn)(int argc, char* argv[]);

void* LoadSymbol(HMODULE module, const char* name)
{
	FARPROC proc = GetProcAddress(module, name);
	if (proc == NULL)
	{
		printf("GetProcAddress failed for symbol: %s\n", name);
		exit(EXIT_FAILURE);
	}

	return proc;
}

int main(int argc, char* argv[])
{
	HMODULE module = LoadLibraryW(L"DesktopAppCLRWrapper.dll");
	if (module == NULL)
	{
		perror("LoadLibraryW failed");
		return EXIT_FAILURE;
	}
	run_fn run = LoadSymbol(module, "run");
	run(argc, argv);
	return 0;
}

int WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	main(0, NULL);
}