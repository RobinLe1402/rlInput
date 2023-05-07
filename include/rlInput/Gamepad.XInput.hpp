#pragma once
#ifndef RLINPUT_GAMEPAD_XINPUT
#define RLINPUT_GAMEPAD_XINPUT





// Win32
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#undef WIN32_MEAN_AND_LEAN
#undef NOMINMAX



namespace rlInput
{

	constexpr unsigned char XINPUT_BUTTON_DPAD_UP        = 0;
	constexpr unsigned char XINPUT_BUTTON_DPAD_DOWN      = 1;
	constexpr unsigned char XINPUT_BUTTON_DPAD_LEFT      = 2;
	constexpr unsigned char XINPUT_BUTTON_DPAD_RIGHT     = 3;
	constexpr unsigned char XINPUT_BUTTON_START          = 4;
	constexpr unsigned char XINPUT_BUTTON_BACK           = 5;
	constexpr unsigned char XINPUT_BUTTON_LEFT_SHOULDER  = 6;
	constexpr unsigned char XINPUT_BUTTON_RIGHT_SHOULDER = 7;
	constexpr unsigned char XINPUT_BUTTON_A              = 8;
	constexpr unsigned char XINPUT_BUTTON_B              = 9;
	constexpr unsigned char XINPUT_BUTTON_X              = 10;
	constexpr unsigned char XINPUT_BUTTON_Y              = 11;



	class XInput
	{
	public: // types

		class Gamepad
		{
			friend class XInput;

		public: // types
			
			/// <summary>
			/// The state of a simple button.
			/// </summary>
			struct SimpleButton
			{
				bool bPressed;  // Was the key pressed down?
				bool bDown;     // Is the key currently down?
				bool bReleased; // Was the key released?
			};

			/// <summary>
			/// The state of a trigger button.
			/// </summary>
			struct TriggerButton
			{
				BYTE iState;
				bool bOutsideThreshold;
			};

			/// <summary>
			/// The state of a thumb stick
			/// </summary>
			struct ThumbStick
			{
				SimpleButton oButton;

				SHORT iX;
				SHORT iY;

				bool bXOutsideDeadzone;
				bool bYOutsideDeadzone;
			};


		public: // methods

			/// <summary>
			/// Prepare the internal button info for queries.<para />
			/// Must be called every time an updated state of the gamepad is required.
			/// </summary>
			/// <returns>Was the gamepad present?</returns>
			bool prepare() noexcept;

			/// <summary>
			/// Reset the inner state of the gamepad to "no button down".
			/// </summary>
			void reset() noexcept;



			/// <summary>
			/// Was the gamepad connected at the time of the last call to <c>prepare()</c>?
			/// </summary>
			bool connected() const noexcept { return m_bConnected; }



			/// <summary>
			/// Get the state of a specific button at the time of the last call to <c>prepare()</c>.
			/// </summary>
			/// <param name="iButtonID">one of the <c>XINPUT_BUTTON_[...]</c> constants.</param>
			auto &button(unsigned char iButtonID) const noexcept { return m_oButtons[iButtonID]; }


			/// <summary>
			/// Get the state of the left thumb stick at the time of the last call to
			/// <c>prepare()</c>.
			/// </summary>
			auto &leftThumbStick()  const noexcept { return m_oThumbSticks[0]; }

			/// <summary>
			/// Get the state of the right thumb stick at the time of the last call to
			/// <c>prepare()</c>.
			/// </summary>
			auto &rightThumbStick() const noexcept { return m_oThumbSticks[1]; }


			/// <summary>
			/// Get the state of the left trigger at the time of the last call to
			/// <c>prepare()</c>.
			/// </summary>
			auto &leftTrigger()  const noexcept { return m_oTriggerButtons[0]; }

			/// <summary>
			/// Get the state of the right trigger at the time of the last call to
			/// <c>prepare()</c>.
			/// </summary>
			auto &rightTrigger() const noexcept { return m_oTriggerButtons[1]; }



			/// <summary>
			/// Set the vibration effect.
			/// </summary>
			/// <param name="iLeftVibration">
			/// The speed of the left (low frequency) vibration motor.<para />
			/// Between 0 and 65535.
			/// </param>
			/// <param name="iRightVibration">
			/// The speed of the right (high frequency) vibration motor.<para />
			/// Between 0 and 65535.
			/// </param>
			/// <returns>Could the settings be applied?</returns>
			bool setVibration(WORD iLeftVibration, WORD iRightVibration) noexcept;


			/// <summary>
			/// The current speed of the left (low frequency) vibration motor.
			/// </summary>
			auto leftVibration() const noexcept { return m_iLeftVibration; }

			/// <summary>
			/// The current speed of the right (high frequency) vibration motor.
			/// </summary>
			auto rightVibration() const noexcept { return m_iRightVibration; }


		private: // methods

			Gamepad(unsigned iID); // --> singleton
			~Gamepad() = default;


		private: // variables

			const unsigned m_iID;

			bool m_bConnected = false;

			XINPUT_STATE m_oRawState_Old{};
			XINPUT_STATE m_oRawState_New{};

			SimpleButton  m_oButtons[12];
			ThumbStick    m_oThumbSticks[2];
			TriggerButton m_oTriggerButtons[2];

			WORD m_iLeftVibration  = 0;
			WORD m_iRightVibration = 0;

		};

		using iterator               = Gamepad *;
		using const_iterator         = const Gamepad *;

		using reverse_iterator       = Gamepad *;
		using const_reverse_iterator = const Gamepad *;





	public: // static methods

		static XInput &Instance() noexcept { return s_oInstance; }


	private: // static variables

		static XInput s_oInstance;
		static bool s_bForeground;





	public: // methods

		iterator begin() noexcept { return m_oGamepads; }
		iterator end()   noexcept { return m_oGamepads + 4; }

		const_iterator begin() const noexcept { return m_oGamepads; }
		const_iterator end()   const noexcept { return m_oGamepads + 4; }

		reverse_iterator rbegin() noexcept { return m_oGamepads + 3; }
		reverse_iterator rend()   noexcept { return m_oGamepads - 1; }

		const_reverse_iterator rbegin() const noexcept { return m_oGamepads + 3; }
		const_reverse_iterator rend()   const noexcept { return m_oGamepads - 1; }


		const Gamepad &operator[](unsigned ID) const { return gamepad(ID); }
		Gamepad       &operator[](unsigned ID)       { return gamepad(ID); }



		const Gamepad &gamepad(unsigned ID) const { return m_oGamepads[ID]; }
		Gamepad       &gamepad(unsigned ID)       { return m_oGamepads[ID]; }





		/// <summary>
		/// Prepare the internal button infos of all gamepads for queries.<para />
		/// Must be called every time an updated state of the mouse is required.
		/// </summary>
		void prepare() noexcept;

		/// <summary>
		/// Try to process a Windows message.<para/>
		/// Should be called every time a Windows message is received to keep the state of the
		/// gamepads from getting corrupted.
		/// </summary>
		/// <returns>Has the update changed the state of the gamepads?</returns>
		bool update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
		
		/// <summary>
		/// Reset the inner state of all the gamepads.
		/// </summary>
		void reset() noexcept;



	private: // variables

		Gamepad m_oGamepads[4]{ 0, 1, 2, 3 };

	};

}





#endif // RLINPUT_GAMEPAD_XINPUT