#pragma once
#ifndef RLINPUT_KEYBOARD
#define RLINPUT_KEYBOARD





// STL
#include <memory>
#include <string>

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

		struct Key
		{
			bool bPressed;
			bool bHeld;
			bool bReleased;
		};





	public: // static methods

		static Keyboard &Instance() { return s_oInstance; }


	private: // static variables

		static Keyboard s_oInstance;





	public: // methods

		const Key &operator[](unsigned char index) const { return m_upStates[index]; }

		/// <summary>
		/// Prepare the internal key info for queries.<para />
		/// Must be called every time an updated state of the keys is required.
		/// </summary>
		void prepare();

		/// <summary>
		/// Try to process a Windows message.<para/>
		/// Should be called every time a Windows message is received.
		/// </summary>
		/// <returns>Was the message keyboard-input related?</returns>
		bool update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

		/// <summary>
		/// Reset the inner state of the keyboard to "no button down".<para />
		/// Also stops text recording and clears the previously recorded text.
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