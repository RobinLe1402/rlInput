#include <rlInput/Keyboard.hpp>

namespace rlInput
{

	Keyboard Keyboard::s_oInstance;



	void Keyboard::pressedKeys(std::vector<unsigned char> &oDest) const noexcept
	{
		oDest.clear();
		for (unsigned i = 0; i < 256; ++i)
		{
			if (m_upStates[i].bPressed)
				oDest.push_back(i);
		}
	}

	Keyboard::ModKeys Keyboard::modifierKeys() const noexcept
	{
		return ModKeys(
			(m_upStates[VK_MENU   ].bDown ? ModKey_Alt     : 0) |
			(m_upStates[VK_CONTROL].bDown ? ModKey_Control : 0) |
			(m_upStates[VK_SHIFT  ].bDown ? ModKey_Shift   : 0)
		);
	}

	Keyboard::Keyboard()
	{
		memset(m_upStates.get(), 0, sizeof(Key) * 256);
	}

	void Keyboard::prepare() noexcept
	{
		Key        *pDest   = m_upStates.get();
		const bool *pRawOld = m_oRawStates_Old;
		const bool *pRawNew = m_oRawStates_New;

		for (unsigned i = 0; i < 256; ++i)
		{
			pDest->bPressed  =  *pRawNew && !*pRawOld;
			pDest->bDown     =  *pRawNew;
			pDest->bReleased = !*pRawNew &&  *pRawOld;

			++pDest;
			++pRawOld;
			++pRawNew;
		}

		memcpy_s(
			m_oRawStates_Old, sizeof(m_oRawStates_Old),
			m_oRawStates_New, sizeof(m_oRawStates_New)
		);
	}

	bool Keyboard::update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			m_oRawStates_New[wParam] = true;
			return true;

		case WM_KEYUP:
			m_oRawStates_New[wParam] = false;
			return true;



		case WM_IME_STARTCOMPOSITION:
			return true; // call to DefWndProc will lead to "Default IME Window" popping up



		case WM_KILLFOCUS:
			// When the window loses focus while a button is being held,
			// the internal state of this class is corrupted.
			reset();
			break;



		case WM_CHAR:
			if ((!m_bRecordText && m_bRecordingStopped) || !IsWindowUnicode(hWnd))
				break;

			if (!m_bRecordText)
			{
				if (!m_sRecordedText.empty() && IS_HIGH_SURROGATE(*m_sRecordedText.end()))
				{
					if (IS_LOW_SURROGATE(wParam))
						m_sRecordedText += (wchar_t)wParam;
					else
						m_sRecordedText.pop_back();
				}

				m_bRecordingStopped = true;
				break;
			}

			if (wParam <= 0x7F && iscntrl((int)wParam))
			{
				switch (wParam)
				{
				case VK_RETURN:
					m_sRecordedText += '\n';
					break;
				case VK_TAB:
					m_sRecordedText += '\t';
					break;
				}

				break;
			}

			if (m_oRawStates_New[VK_MENU] || m_oRawStates_New[VK_CONTROL])
				break; // Control key/Alt/Ctrl keypresses are ignored

			if (!IS_LOW_SURROGATE(wParam) ||
				(m_sRecordedText.length() > 0 && IS_HIGH_SURROGATE(*m_sRecordedText.rbegin())))
				m_sRecordedText += (wchar_t)wParam;

			break;
		}



		return false;
	}

	void Keyboard::reset() noexcept
	{
		memset(m_oRawStates_Old, 0, sizeof(m_oRawStates_Old));
		memset(m_oRawStates_New, 0, sizeof(m_oRawStates_New));
	}

}
