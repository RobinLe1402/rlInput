#include <rlInput/Keyboard.hpp>

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR     szCmdLine,
	_In_     int       iCmdShow)
{
	// ignored parameters
	(void)hPrevInstance;
	(void)szCmdLine;


	constexpr wchar_t szCLASSNAME[] = L"TestWindowClass";

	WNDCLASSW wc{};
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance     = hInstance;
	wc.lpfnWndProc   = WindowProc;
	wc.lpszClassName = szCLASSNAME;

	if (!RegisterClassW(&wc))
	{
		MessageBoxA(NULL, "RegisterClassW failed.", NULL, MB_ICONERROR | MB_SYSTEMMODAL);

		DestroyCursor(wc.hCursor);
		return 1;
	}

	HWND hWnd = CreateWindowW(szCLASSNAME, L"Hello rlInput!", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	
	MSG msg{};
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	DestroyCursor(wc.hCursor);
	return 0;
}


LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto &keyboard = rlInput::Keyboard::Instance();

	using rlInput::Keyboard;

	if (keyboard.update(hWnd, uMsg, wParam, lParam))
	{
		keyboard.prepare();

		const auto iModKeys = keyboard.modifierKeys();
		if (iModKeys & Keyboard::ModKey_Control)
		{
			std::string sPressedKeys;

			for (unsigned char c = 'A'; c <= 'Z'; ++c)
			{
				if (keyboard[c].bPressed)
				{
					if (!sPressedKeys.empty())
						sPressedKeys += " + ";
					sPressedKeys += c;
				}
			}

			if (!sPressedKeys.empty())
			{
				if (iModKeys & Keyboard::ModKey_Alt)
					sPressedKeys.insert(0, "Alt + ");
				if (iModKeys & Keyboard::ModKey_Shift)
					sPressedKeys.insert(0, "Shift + ");
				if (iModKeys & Keyboard::ModKey_Control)
					sPressedKeys.insert(0, "Control + ");

				MessageBoxA(hWnd, sPressedKeys.c_str(), "Key combination pressed",
					MB_ICONINFORMATION | MB_APPLMODAL);
			}
		}
	}


	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
