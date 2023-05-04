#include <rlInput/Gamepad.XInput.hpp>
#include <rlInput/Keyboard.hpp>
#include <rlInput/Mouse.hpp>

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void testInputs(HWND hWnd);

int WINAPI WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR     szCmdLine,
	_In_     int       iCmdShow)
{
	// ignored parameters
	(void)hPrevInstance;
	(void)szCmdLine;

	auto &keyboard = rlInput::Keyboard::Instance();
	keyboard.startTextRecording();


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
	while (true)
	{
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				goto lbEnd;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		testInputs(hWnd);
	}
lbEnd:

	DestroyCursor(wc.hCursor);
	return 0;
}

void testInputs(HWND hWnd)
{
	auto &xinput   = rlInput::XInput::Instance();
	auto &keyboard = rlInput::Keyboard::Instance();
	auto &mouse    = rlInput::Mouse::Instance();

	using rlInput::Keyboard;
	using rlInput::Mouse;

	xinput.prepare();
	keyboard.prepare();
	mouse.prepare();

	if (xinput.gamepad(0).connected())
	{
		if (xinput.gamepad(0).button(rlInput::XINPUT_BUTTON_START).bPressed)
			MessageBoxA(NULL, "[Start] was pressed.", "Info",
				MB_ICONINFORMATION | MB_APPLMODAL);

		SetWindowTextA(hWnd, std::to_string(xinput.gamepad(0).leftTrigger().iState).c_str());
	}

	const auto iModKeys = keyboard.modifierKeys();
	if (iModKeys & Keyboard::ModKey_Control)
	{
		std::wstring sPressedKeys;

		for (unsigned char c = 'A'; c <= 'Z'; ++c)
		{
			if (keyboard[c].bPressed)
			{
				if (!sPressedKeys.empty())
					sPressedKeys += L" + ";
				sPressedKeys += c;
			}
		}

		if (!sPressedKeys.empty())
		{
			if (iModKeys & Keyboard::ModKey_Alt)
				sPressedKeys.insert(0, L"Alt + ");
			if (iModKeys & Keyboard::ModKey_Shift)
				sPressedKeys.insert(0, L"Shift + ");
			if (iModKeys & Keyboard::ModKey_Control)
				sPressedKeys.insert(0, L"Control + ");

			if (!keyboard.recordedText().empty())
			{
				sPressedKeys += L"\nInput text: \"";
				sPressedKeys += keyboard.recordedText();
				sPressedKeys += L"\"";

				keyboard.clearRecordedText();
			}

			MessageBoxW(NULL, sPressedKeys.c_str(), L"Key combination pressed",
				MB_ICONINFORMATION | MB_APPLMODAL);
		}
	}

	if (mouse.leftButton().bClicked)
	{
		MessageBoxA(NULL, (std::string("Mouse clicked at (") +
			std::to_string(mouse.x()) +
			", " +
			std::to_string(mouse.y()) +
			")").c_str(), "Info",
			MB_ICONINFORMATION | MB_APPLMODAL);
	}
}


LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto &xinput   = rlInput::XInput::Instance();
	auto &keyboard = rlInput::Keyboard::Instance();
	auto &mouse    = rlInput::Mouse::Instance();

	xinput.update(hWnd, uMsg, wParam, lParam);
	keyboard.update(hWnd, uMsg, wParam, lParam);
	mouse.update(hWnd, uMsg, wParam, lParam);


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
