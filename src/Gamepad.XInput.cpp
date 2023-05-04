#include <rlInput/Gamepad.XInput.hpp>

// STL
#include <cmath>

// Win32
#pragma comment(lib, "Xinput.lib")

namespace rlInput
{

	XInput XInput::s_oInstance;



	XInput::Gamepad::Gamepad(unsigned iID) : m_iID(iID) {}

	bool XInput::Gamepad::prepare() noexcept
	{
		m_bConnected = XInputGetState(m_iID, &m_oRawState_New) == ERROR_SUCCESS;
		if (!m_bConnected)
		{
			reset();
			return false;
		}

		if (m_oRawState_New.dwPacketNumber == m_oRawState_Old.dwPacketNumber)
			return true; // no change



		SimpleButton *pButtons[] =
		{
			m_oButtons + 0,
			m_oButtons + 1,
			m_oButtons + 2,
			m_oButtons + 3,
			m_oButtons + 4,
			m_oButtons + 5,
			m_oButtons + 6,
			m_oButtons + 7,
			m_oButtons + 8,
			m_oButtons + 9,
			m_oButtons + 10,
			m_oButtons + 11,

			&m_oThumbSticks[0].oButton,
			&m_oThumbSticks[1].oButton
		};

		constexpr unsigned short iMasks[] =
		{
			XINPUT_GAMEPAD_DPAD_UP,
			XINPUT_GAMEPAD_DPAD_DOWN,
			XINPUT_GAMEPAD_DPAD_LEFT,
			XINPUT_GAMEPAD_DPAD_RIGHT,
			XINPUT_GAMEPAD_START,
			XINPUT_GAMEPAD_BACK,
			XINPUT_GAMEPAD_LEFT_SHOULDER,
			XINPUT_GAMEPAD_RIGHT_SHOULDER,
			XINPUT_GAMEPAD_A,
			XINPUT_GAMEPAD_B,
			XINPUT_GAMEPAD_X,
			XINPUT_GAMEPAD_Y,

			XINPUT_GAMEPAD_LEFT_THUMB,
			XINPUT_GAMEPAD_RIGHT_THUMB
		};

		for (size_t i = 0; i < sizeof(pButtons) / sizeof(pButtons[0]); ++i)
		{
			const bool bOld = m_oRawState_Old.Gamepad.wButtons & iMasks[i];
			const bool bNew = m_oRawState_New.Gamepad.wButtons & iMasks[i];

			pButtons[i]->bPressed  =  bNew && !bOld;
			pButtons[i]->bDown     =  bNew;
			pButtons[i]->bReleased = !bNew && bOld;
		}



		const auto &oGamepad = m_oRawState_New.Gamepad;


		m_oTriggerButtons[0].iState = oGamepad.bLeftTrigger;
		m_oTriggerButtons[0].bOutsideThreshold =
			oGamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		m_oTriggerButtons[1].iState = oGamepad.bRightTrigger;
		m_oTriggerButtons[1].bOutsideThreshold =
			oGamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;


		m_oThumbSticks[0].iX = oGamepad.sThumbLX;
		m_oThumbSticks[0].bXOutsideDeadzone =
			std::abs(oGamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		m_oThumbSticks[0].iY = oGamepad.sThumbLY;
		m_oThumbSticks[0].bYOutsideDeadzone =
			std::abs(oGamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;


		m_oThumbSticks[1].iX = oGamepad.sThumbRX;
		m_oThumbSticks[1].bXOutsideDeadzone =
			std::abs(oGamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		m_oThumbSticks[1].iY = oGamepad.sThumbRY;
		m_oThumbSticks[1].bYOutsideDeadzone =
			std::abs(oGamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;



		m_oRawState_Old = m_oRawState_New;
		return true;
	}

	void XInput::Gamepad::reset() noexcept
	{
		m_oRawState_Old = {};
		m_oRawState_New = {};

		memset(m_oButtons,        0, sizeof(m_oButtons));
		memset(m_oThumbSticks,    0, sizeof(m_oThumbSticks));
		memset(m_oTriggerButtons, 0, sizeof(m_oTriggerButtons));

		if (!setVibration(0, 0))
		{
			m_iLeftVibration  = 0;
			m_iRightVibration = 0;
		}
	}

	bool XInput::Gamepad::setVibration(WORD iLeftVibration, WORD iRightVibration) noexcept
	{
		XINPUT_VIBRATION oVib =
		{
			.wLeftMotorSpeed  = iLeftVibration,
			.wRightMotorSpeed = iRightVibration
		};

		bool bResult = XInputSetState(m_iID, &oVib) == ERROR_SUCCESS;

		if (bResult)
		{
			m_iLeftVibration  = iLeftVibration;
			m_iRightVibration = iRightVibration;
		}

		return bResult;
	}





	void XInput::prepare() noexcept
	{
		for (auto &o : m_oGamepads)
		{
			o.prepare();
		}
	}

	bool XInput::update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (uMsg)
		{
		case WM_KILLFOCUS:
			for (auto &o : m_oGamepads)
			{
				o.reset();
			}
			break;

		default:
			return false;
		}

		return true;
	}

}
