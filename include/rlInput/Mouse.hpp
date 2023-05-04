#pragma once
#ifndef RLINPUT_MOUSE
#define RLINPUT_MOUSE





// Win32
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#undef WIN32_MEAN_AND_LEAN
#undef NOMINMAX



namespace rlInput
{

	class Mouse
	{
	public: // types

		/// <summary>
		/// The state of a single mouse button.
		/// </summary>
		struct Button
		{
			bool bClicked;       // Was the button pressed down?
			bool bDoubleClicked; // Was the button double clicked?
			bool bDown;          // Is the button currently down?
			bool bReleased;      // Was the button released?
		};





	public: // static methods

		static Mouse &Instance() { return s_oInstance; }


	private: // static variables

		static Mouse s_oInstance;





	public: // methods

		/// <summary>
		/// Prepare the internal button info for queries.<para />
		/// Must be called every time an updated state of the mouse is required.
		/// </summary>
		void prepare() noexcept;

		/// <summary>
		/// Try to process a Windows message.<para/>
		/// Should be called every time a Windows message is received.
		/// </summary>
		/// <returns>
		/// Was the message mouse-input related?<para />
		/// If the return value is <c>TRUE</c>, <c>DefWindowProc</c> doesn't have to be called.
		/// </returns>
		bool update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

		/// <summary>
		/// Reset the inner state of the mouse to "no button down".
		/// </summary>
		void reset() noexcept;





		/// <summary>
		/// Get the state of the left mouse button at the time of the last call to <c>prepare()</c>.
		/// </summary>
		const Button &leftButton()   const noexcept { return m_oStates[0]; }

		/// <summary>
		/// Get the state of the right mouse button at the time of the last call to
		/// <c>prepare()</c>.
		/// </summary>
		const Button &rightButton()  const noexcept { return m_oStates[1]; }

		/// <summary>
		/// Get the state of the middle mouse button at the time of the last call to
		/// <c>prepare()</c>.
		/// </summary>
		const Button &middleButton() const noexcept { return m_oStates[2]; }



		/// <summary>
		/// Get the horizontal client position of the cursor at the time of the last call to
		/// <c>prepare()</c>.<para />
		/// Only valid if <c>onClient()</c> returns <c>TRUE</c>.
		/// </summary>
		auto x() const noexcept { return m_iCachedClientX; }
		/// <summary>
		/// Get the vertical client position of the cursor at the time of the last call to
		/// <c>prepare()</c>.<para />
		/// Only valid if <c>onClient()</c> returns <c>TRUE</c>.
		/// </summary>
		auto y() const noexcept { return m_iCachedClientY; }



		/// <summary>
		/// Was the mouse on the client when the last call to <c>prepare()</c> occured?<para/>
		/// Please note that this function returns <c>TRUE</c> if a mouse button was held down while
		/// the mouse left the window. In this case, this function only returns <c>FALSE</c> after
		/// all these mouse buttons were released.
		/// </summary>
		bool onClient() const noexcept { return m_bCachedOnClient; }

		/// <summary>
		/// The mouse wheel delta, in multiples or divisions of <c>WHEEL_DELTA</c>, at the time of
		/// the last call to <c>prepare()</c>.
		/// </summary>
		auto weelRotation() const noexcept { return m_iCachedWheelRotation; }


	private: // methods

		Mouse()  = default; // --> singleton
		~Mouse() = default;

		void beginCapture(HWND hWnd);
		void endCapture();


	private: // variables

		Button m_oStates[3]{};
		bool m_oRawStates_Old[3]{};
		bool m_oRawStates_New[3]{};
		bool m_oDoubleClicked[3]{};

		int m_iClientX = 0;
		int m_iClientY = 0;
		bool m_bOnClient = false;
		int  m_iWheelRotation = 0;

		bool m_bTracking = false;
		unsigned m_iCaptureCount = 0;

		int  m_iCachedClientX       = 0;
		int  m_iCachedClientY       = 0;
		bool m_bCachedOnClient      = false;
		int  m_iCachedWheelRotation = 0;

	};

}





#endif // RLINPUT_MOUSE