#include <rlInput/Keyboard.hpp>

namespace rlInput
{

	Keyboard Keyboard::s_oInstance;



	void Keyboard::pressedKeys(std::vector<unsigned char> &oDest) const
	{
		oDest.clear();
		for (unsigned i = 0; i < 256; ++i)
		{
			if (m_upStates[i].bPressed)
				oDest.push_back(i);
		}
	}

	Keyboard::ModKeys Keyboard::modifierKeys() const
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

	void Keyboard::prepare()
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
					if (IS_LOW_SURROGATE(lParam))
						m_sRecordedText += (wchar_t)lParam;
					else
						m_sRecordedText.pop_back();
				}

				m_bRecordingStopped = true;
				break;
			}

			if (!IS_LOW_SURROGATE(lParam) ||
				(m_sRecordedText.length() > 0 && IS_HIGH_SURROGATE(*m_sRecordedText.end())))
				m_sRecordedText += (wchar_t)lParam;

			break;
		}



		return false;
	}

	void Keyboard::reset() noexcept
	{
		memset(m_oRawStates_Old, 0, sizeof(m_oRawStates_Old));
		memset(m_oRawStates_New, 0, sizeof(m_oRawStates_New));

		m_bRecordText       = false;
		m_bRecordingStopped = true;
		m_sRecordedText.clear();
	}

}
