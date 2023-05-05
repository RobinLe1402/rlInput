#pragma once
#ifndef RLINPUT_GAMEPAD_DIRECTINPUT
#define RLINPUT_GAMEPAD_DIRECTINPUT





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

			Gamepad(const GUID &guid, HWND hWnd);
			~Gamepad();

			/// <summary>
			/// Prepare the internal button info for queries.<para />
			/// Must be called every time an updated state of the gamepad is required.
			/// </summary>
			/// <returns>Was the gamepad present?</returns>
			bool prepare() noexcept;


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

			const GUID m_oGUID;
			const HWND m_hWnd;

			IDirectInputDevice8 *m_pDevice = nullptr;

			std::wstring m_sInstanceName;
			std::wstring m_sProductName;


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





	public: // methods

		auto &availableControllers() const noexcept { return m_oAvailableControllers; }

		void updateControllerList();

		bool isXInput(const GUID &guid) const noexcept;


	private: // methods

		DirectInput(); // --> singleton
		~DirectInput();


	private: // variables

		std::vector<GamepadMeta> m_oAvailableControllers;
		IDirectInput8 *m_pDirectInput = nullptr;

	};

}





#endif // RLINPUT_GAMEPAD_DIRECTINPUT