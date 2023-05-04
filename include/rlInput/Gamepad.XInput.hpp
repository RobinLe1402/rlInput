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

	static constexpr unsigned char XINPUT_BUTTON_DPAD_UP        = 0;
	static constexpr unsigned char XINPUT_BUTTON_DPAD_DOWN      = 1;
	static constexpr unsigned char XINPUT_BUTTON_DPAD_LEFT      = 2;
	static constexpr unsigned char XINPUT_BUTTON_DPAD_RIGHT     = 3;
	static constexpr unsigned char XINPUT_BUTTON_START          = 4;
	static constexpr unsigned char XINPUT_BUTTON_BACK           = 5;
	static constexpr unsigned char XINPUT_BUTTON_LEFT_SHOULDER  = 6;
	static constexpr unsigned char XINPUT_BUTTON_RIGHT_SHOULDER = 7;
	static constexpr unsigned char XINPUT_BUTTON_A              = 8;
	static constexpr unsigned char XINPUT_BUTTON_B              = 9;
	static constexpr unsigned char XINPUT_BUTTON_X              = 10;
	static constexpr unsigned char XINPUT_BUTTON_Y              = 11;



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
			/// <returns>Was the controller present?</returns>
			bool prepare() noexcept;

			/// <summary>
			/// Reset the inner state of the controller to "no button down".
			/// </summary>
			void reset() noexcept;



			/// <summary>
			/// Was the gamepad connected at the time of the last call to <c>prepare()</c>?
			/// </summary>
			bool connected() const noexcept { return m_bConnected; }



			auto &button(unsigned char iButtonID) const noexcept { return m_oButtons[iButtonID]; }

			auto &leftThumbStick()  const noexcept { return m_oThumbSticks[0]; }
			auto &rightThumbStick() const noexcept { return m_oThumbSticks[1]; }

			auto &leftTrigger()  const noexcept { return m_oTriggerButtons[0]; }
			auto &rightTrigger() const noexcept { return m_oTriggerButtons[1]; }


			bool setVibration(WORD iLeftVibration, WORD iRightVibration) noexcept;

			auto leftVibration() const noexcept { return m_iLeftVibration; }
			auto rightVibration() const noexcept { return m_iRightVibration; }


		private: // methods

			Gamepad(unsigned iID);
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
		/// Prepare the internal button info of all gamepads for queries.<para />
		/// Must be called every time an updated state of the mouse is required.
		/// </summary>
		void prepare() noexcept;

		/// <summary>
		/// Try to process a Windows message.<para/>
		/// Should be called every time a Windows message is received to keep the state of the
		/// gamepads from getting corrupted.
		/// </summary>
		/// <returns>Has the update changed the state of the controllers?</returns>
		bool update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;


	private: // variables

		Gamepad m_oGamepads[4]{ 0, 1, 2, 3 };

	};

}





#endif // RLINPUT_GAMEPAD_XINPUT