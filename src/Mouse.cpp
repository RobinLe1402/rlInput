#include <rlInput/Mouse.hpp>

// Win32
#include <windowsx.h>

namespace rlInput
{

	namespace
	{
		constexpr size_t iRL_MOUSE_L = 0;
		constexpr size_t iRL_MOUSE_R = 1;
		constexpr size_t iRL_MOUSE_M = 2;
	}



	Mouse Mouse::s_oInstance;



	void Mouse::prepare() noexcept
	{
		Button *pDest     = m_oStates;
		bool   *pRawOld   = m_oRawStates_Old;
		bool   *pRawNew   = m_oRawStates_New;
		bool   *pDblClick = m_oDoubleClicked;

		for (unsigned i = 0; i < 3; ++i)
		{
			pDest->bClicked       =  *pRawNew && !*pRawOld;
			pDest->bDoubleClicked =  *pDblClick;
			pDest->bDown          =  *pRawNew;
			pDest->bReleased      = !*pRawNew &&  *pRawOld;

			++pDest;
			++pRawOld;
			++pRawNew;
			++pDblClick;
		}

		m_iCachedClientX       = m_iClientX;
		m_iCachedClientY       = m_iClientY;
		m_bCachedOnClient      = m_bOnClient;
		m_iCachedWheelRotation = m_iWheelRotation;

		memcpy_s(
			m_oRawStates_Old, sizeof(m_oRawStates_Old),
			m_oRawStates_New, sizeof(m_oRawStates_New)
		);
		memset(m_oDoubleClicked, 0, sizeof(m_oDoubleClicked));
		m_iWheelRotation = 0;
	}

	bool Mouse::update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			m_bOnClient = true;
			m_iClientX  = GET_X_LPARAM(lParam);
			m_iClientY  = GET_Y_LPARAM(lParam);

			if (!m_bTracking) // enable mouse tracking
			{
				TRACKMOUSEEVENT tme{};
				tme.cbSize    = sizeof(tme);
				tme.hwndTrack = hWnd;
				tme.dwFlags   = TME_LEAVE;

				TrackMouseEvent(&tme);

				m_bTracking = true;
			}

			return true;



		case WM_MOUSELEAVE:
			m_bOnClient = false;
			m_bTracking = false; // must re-call TrackMouseEvent
			return true;



			// left
		case WM_LBUTTONDOWN:
			m_oRawStates_New[iRL_MOUSE_L] = true;
			beginCapture(hWnd);
			return true;
		case WM_LBUTTONUP:
			m_oRawStates_New[iRL_MOUSE_L] = false;
			endCapture();
			return true;
		case WM_LBUTTONDBLCLK:
			m_oDoubleClicked[iRL_MOUSE_L] = true;
			return true;



			// right
		case WM_RBUTTONDOWN:
			m_oRawStates_New[iRL_MOUSE_R] = true;
			beginCapture(hWnd);
			return true;
		case WM_RBUTTONUP:
			m_oRawStates_New[iRL_MOUSE_R] = false;
			endCapture();
			return true;
		case WM_RBUTTONDBLCLK:
			m_oDoubleClicked[iRL_MOUSE_R] = true;
			return true;



			// middle
		case WM_MBUTTONDOWN:
			m_oRawStates_New[iRL_MOUSE_M] = true;
			beginCapture(hWnd);
			return true;
		case WM_MBUTTONUP:
			m_oRawStates_New[iRL_MOUSE_M] = false;
			endCapture();
			return true;
		case WM_MBUTTONDBLCLK:
			m_oDoubleClicked[iRL_MOUSE_M] = true;
			return true;



			// wheel
		case WM_MOUSEWHEEL:
			m_iWheelRotation += GET_WHEEL_DELTA_WPARAM(wParam);
			return true;



		case WM_KILLFOCUS:
			reset();
			break;
		}



		return false;
	}

	void Mouse::reset() noexcept
	{
		memset(m_oRawStates_Old, 0, sizeof(m_oRawStates_Old));
		memset(m_oRawStates_New, 0, sizeof(m_oRawStates_New));
	}

	void Mouse::beginCapture(HWND hWnd)
	{
		if (++m_iCaptureCount == 1)
			SetCapture(hWnd);
	}

	void Mouse::endCapture()
	{
		if (m_iCaptureCount == 0)
			return;

		if (--m_iCaptureCount == 0)
			ReleaseCapture();
	}

}
