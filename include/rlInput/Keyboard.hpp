#pragma once
#ifndef RLINPUT_KEYBOARD
#define RLINPUT_KEYBOARD





// STL
#include <memory>
#include <string>
#include <vector>

// Win32
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#undef WIN32_MEAN_AND_LEAN
#undef NOMINMAX



namespace rlInput
{

	class Keyboard
	{
	public: // types

		/// <summary>
		/// The state of a single key.
		/// </summary>
		struct Key
		{
			bool bPressed;  // Was the key pressed down?
			bool bDown;     // Is the key currently down?
			bool bReleased; // Was the key released?
		};

		/// <summary>
		/// State of the modifier keys.<para/>
		/// Consists of the <c>ModKey_[...]</c> flags.
		/// </summary>
		using ModKeys = unsigned char;

		static constexpr ModKeys ModKey_Alt     = 0x01;
		static constexpr ModKeys ModKey_Control = 0x02;
		static constexpr ModKeys ModKey_Shift   = 0x04;





	public: // static methods

		static Keyboard &Instance() { return s_oInstance; }


	private: // static variables

		static Keyboard s_oInstance;





	public: // methods

		const Key &operator[](unsigned char index) const noexcept { return key(index); }

		/// <summary>
		/// Get the state of a key at the time of the last call to <c>prepare()</c>.
		/// </summary>
		const Key &key(unsigned char index) const noexcept { return m_upStates[index]; }

		/// <summary>
		/// Get all keys that have been pressed between the previous and current call to
		/// <c>prepare()</c>.
		/// </summary>
		/// <param name="oDest">
		/// The virtual key codes of the newly pressed keys.<para/>
		/// The previous values are cleared before anything is written.<para/>
		/// After the call, the values are guaranteed to be in ascending order by binary value.
		/// </param>
		void pressedKeys(std::vector<unsigned char> &oDest) const;

		/// <summary>
		/// Get the state of the modifier keys at the time of the last call to <c>prepare()</c>.
		/// </summary>
		ModKeys modifierKeys() const;

		/// <summary>
		/// Prepare the internal key info for queries.<para />
		/// Must be called every time an updated state of the keys is required.
		/// </summary>
		void prepare();

		/// <summary>
		/// Try to process a Windows message.<para/>
		/// Should be called every time a Windows message is received.
		/// </summary>
		/// <returns>
		/// Was the message keyboard-input related?<para />
		/// If the return value is <c>TRUE</c>, <c>DefWindowProc</c> doesn't have to be called.
		/// <para/>
		/// Please note that if you still decide to call DefWndProc with the
		/// <c>WM_IME_STARTCOMPOSITION</c> message, an ugly default Windows IME popup window will
		/// appear.
		/// </returns>
		bool update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

		/// <summary>
		/// Reset the inner state of the keyboard to "no button down".
		/// </summary>
		void reset() noexcept;



		/// <summary>
		/// Start recording text input.<para/>
		/// Text is recorded only if the window sending messages uses Unicode
		/// (<c>RegisterClassW</c> was used).
		/// </summary>
		void startTextRecording() noexcept { m_bRecordText = true; }
		/// <summary>
		/// End recording text input.
		/// </summary>
		void endTextRecording() noexcept { m_bRecordText = m_bRecordingStopped = false; }

		/// <summary>
		/// Is text currently being recorded?
		/// </summary>
		bool recordingText() const noexcept { return m_bRecordText; }

		/// <summary>
		/// The text recorded via <c>startTextRecording()</c> up until now.<para/>
		/// New text input will always be appended.
		/// You can call <c>clearRecordedText()</c> to empty this string.
		/// </summary>
		auto &recordedText() const noexcept { return m_sRecordedText; }

		/// <summary>
		/// Clear the text recorded via <c>startTextRecording()</c> up until now.
		/// </summary>
		void clearRecordedText() noexcept { m_sRecordedText.clear(); }


	private: // methods

		Keyboard(); // --> singleton
		~Keyboard() = default;


	private: // variables

		std::unique_ptr<Key[]> m_upStates = std::make_unique<Key[]>(256);
		bool m_oRawStates_Old[256]{};
		bool m_oRawStates_New[256]{};

		bool m_bRecordText       = false;
		bool m_bRecordingStopped = true;
		std::wstring m_sRecordedText;

	};

}





#endif // RLINPUT_KEYBOARD