#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

typedef void(__cdecl* run_fn)(int argc, char* argv[]);

void FreeArgv(int argc, char** argv) {
	if (argv == NULL) return;
	for (int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
}

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
	int argc = 0;
	char** argv = NULL;
	LPWSTR* szArglistW = NULL;
	char exePath[MAX_PATH];

	GetModuleFileNameA(NULL, exePath, MAX_PATH);

	szArglistW = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (szArglistW == NULL) {
		MessageBoxA(NULL, "Failed to parse command line arguments", "Error", MB_ICONERROR);
		return -1;
	}

	argv = (char**)malloc(argc * sizeof(char*));
	if (argv == NULL) {
		LocalFree(szArglistW);
		return -1;
	}

	argv[0] = _strdup(exePath);

	for (int i = 1; i < argc; i++) {
		int len = WideCharToMultiByte(CP_ACP, 0, szArglistW[i], -1, NULL, 0, NULL, NULL);
		argv[i] = (char*)malloc(len);
		if (argv[i]) {
			WideCharToMultiByte(CP_ACP, 0, szArglistW[i], -1, argv[i], len, NULL, NULL);
		}
	}

	LocalFree(szArglistW);

	int result = main(argc, argv);

	FreeArgv(argc, argv);

	return result;
}