#pragma once
#ifndef RLINPUT_GAMEPAD_DIRECTINPUT
#define RLINPUT_GAMEPAD_DIRECTINPUT





// STL
#include <memory>
#include <set>
#include <string>
#include <vector>

// Win32
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#undef WIN32_MEAN_AND_LEAN
#undef NOMINMAX
#include <dinput.h>



namespace rlInput
{

	constexpr unsigned char DINPUT_AXIS_X  = 0;
	constexpr unsigned char DINPUT_AXIS_Y  = 1;
	constexpr unsigned char DINPUT_AXIS_Z  = 1;
	constexpr unsigned char DINPUT_AXIS_RX = 2;
	constexpr unsigned char DINPUT_AXIS_RY = 3;
	constexpr unsigned char DINPUT_AXIS_RZ = 1;

	constexpr LONG DINPUT_AXISPOS_MIN    = 0;
	constexpr LONG DINPUT_AXISPOS_CENTER = 32767;
	constexpr LONG DINPUT_AXISPOS_MAX    = 65535;





	class DirectInput final
	{
	public: // types

		struct GamepadMeta
		{
			GUID guidInstance;
			GUID guidProduct;

			std::wstring sInstanceName;
			std::wstring sProductName;
		};

		class Gamepad final
		{
		public: // types

			struct Button
			{
				bool bPressed;  // Was the key pressed down?
				bool bDown;     // Is the key currently down?
				bool bReleased; // Was the key released?
			};

			using Axis = LONG;


		public: // methods

			Gamepad(const GamepadMeta &oMeta, HWND hWnd);
			~Gamepad();

			/// <summary>
			/// Prepare the internal button info for queries.<para />
			/// Must be called every time an updated state of the gamepad is required.<para />
			/// You can alternatively call the <c>DirectInput::prepare()</c> method to prepare the
			/// input of <b>all</b> <c>Gamepad</c> instances.<para />
			/// Please call <b>either</b> this function <b>or</b> the <c>Gamepad::prepare()</c>
			/// function in your game loop, but <b>not both</b> as this would result in an
			/// inaccurate state.
			/// </summary>
			/// <returns>Was the gamepad present?</returns>
			bool prepare() noexcept;

			/// <summary>
			/// Reset the inner state of the gamepad to "no button down".
			/// </summary>
			void reset() noexcept;





			/// <summary>
			/// The instance GUID of the gamepad.
			/// </summary>
			const GUID &guidInstance() const noexcept { return m_oGuidInstance; }

			/// <summary>
			/// The product GUID of the gamepad.
			/// </summary>
			const GUID &guidProduct() const noexcept { return m_oGuidProduct; }

			/// <summary>
			/// The display name of the gamepad.
			/// </summary>
			const std::wstring &instanceName() const noexcept { return m_sInstanceName; }

			/// <summary>
			/// The display name of the type of gamepad.
			/// </summary>
			const std::wstring &productName()  const noexcept { return m_sProductName; }


			/// <summary>
			/// Was the gamepad present at the time of the last call to <c>prepare()</c>?
			/// </summary>
			bool connected() const noexcept { return m_bConnected; }

			/// <summary>
			/// The states of the buttons at the time of the last call to <c>prepare()</c>.
			/// </summary>
			auto &buttons() const noexcept { return m_oButtons; }

			/// <summary>
			/// The axes count given by the device.<para />
			/// There's no standard implementation of which axes are active at what count, so the
			/// programmer will have to decide which axes values to actually evaluate.
			/// </summary>
			auto axesCount() const noexcept { return m_iAxesCount; }

			/// <summary>
			/// The states of the axes at the time of the last call to <c>prepare()</c>.<para/>
			/// Use the <c>DINPUT_AXIS_[...]</c> constants for the indexes. Also, you can use the
			/// <c>DINPUT_AXISPOS_[...]</c> constants for reference on the values.
			/// </summary>
			auto &axes() const noexcept { return m_oAxes; }


		private: // variables

			const GUID m_oGuidInstance;
			const GUID m_oGuidProduct;
			const std::wstring m_sInstanceName;
			const std::wstring m_sProductName;
			const HWND m_hWnd;

			IDirectInputDevice8 *m_pDevice = nullptr;


			bool m_bConnected = false;
			std::unique_ptr<bool[]> m_oRawStates_Old;
			std::vector<Button> m_oButtons;

			unsigned m_iAxesCount = 0;
			std::vector<Axis> m_oAxes;
		};





	public: // static methods

		static DirectInput &Instance() noexcept { return s_oInstance; }


	private: // static variables

		static DirectInput s_oInstance;
		static bool s_bForeground;
		static bool s_bInstanceValid;





	public: // methods

		/// <summary>
		/// Prepare the internal button infos of all controllers for queries.<para />
		/// Can be called when an updated state of all the gamepads is required.<para/>
		/// Please call <b>either</b> this function <b>or</b> the <c>Gamepad::prepare()</c>
		/// function in your game loop, but <b>not both</b> as this would result in an inaccurate
		/// state.
		/// </summary>
		void prepare() noexcept;

		/// <summary>
		/// Try to process a Windows message.<para/>
		/// Should be called every time a Windows message is received.
		/// </summary>
		void update(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

		/// <summary>
		/// Reset the inner state of all the gamepads.
		/// </summary>
		void reset() noexcept;





		auto &availableControllers() const noexcept { return m_oAvailableControllers; }

		void updateControllerList();

		bool isXInput(const GUID &guidProduct) const noexcept;


	private: // methods

		DirectInput(); // --> singleton
		~DirectInput();


	private: // variables

		std::vector<GamepadMeta> m_oAvailableControllers;
		IDirectInput8 *m_pDirectInput = nullptr;

		std::set<Gamepad *> m_oGamepadInstances;

	};

}





#endif // RLINPUT_GAMEPAD_DIRECTINPUT