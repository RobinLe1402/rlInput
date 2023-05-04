#include <rlInput/Keyboard.hpp>

namespace rlInput
{

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
			pDest->bHeld     =  *pRawNew;
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
			break;

		case WM_KEYUP:
			m_oRawStates_New[wParam] = false;
			break;

		case WM_CHAR:
			if ((!m_bRecordText && m_bRecordingStopped) || !IsWindowUnicode(hWnd))
				return false;

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
				return false;
			}

			if (!IS_LOW_SURROGATE(lParam) ||
				(m_sRecordedText.length() > 0 && IS_HIGH_SURROGATE(*m_sRecordedText.end())))
				m_sRecordedText += (wchar_t)lParam;

			return false;

		default:
			return false;
		}

		return true;
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
