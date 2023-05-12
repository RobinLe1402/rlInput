#pragma once
#ifndef RLINPUT_GAMEPAD
#define RLINPUT_GAMEPAD





// STL
#include <memory>
#include <string>
#include <vector>

// Win32
#include <GameInput.h>



namespace rlInput
{

	class GameInput final
	{
	public: // types

		class Gamepad final
		{
			friend class GameInput;


		public: // methods

			~Gamepad() = default;

			bool connected() const noexcept { return m_bConnected; }


		private: // methods

			Gamepad(IGameInputDevice *pDevice);


		private: // variables

			IGameInputDevice *m_pDevice = nullptr;
			bool m_bConnected = true;

			std::wstring m_sDisplayName;
			std::wstring m_sDeviceStrings;

		};





	public: // methods

		static GameInput &Instance() noexcept { return s_oInstance; }


	private: // static variables

		static GameInput s_oInstance;


	private: // static methods

		static void DeleteGameInput(IGameInput *p) { p->Release(); }

		static void DeviceCallback(
			 GameInputCallbackToken callbackToken,
			 void* context,
			 IGameInputDevice* device,
			 uint64_t timestamp,
			 GameInputDeviceStatus currentStatus,
			 GameInputDeviceStatus previousStatus
		);





	public: // methods

		void updateDeviceList();

		auto &devices() noexcept { return m_oGamepads; }


	private: // methods

		GameInput(); // --> singleton
		~GameInput() = default;


	private: // variables

		std::unique_ptr<IGameInput, decltype(&DeleteGameInput)> m_upGameInput;
		std::vector<Gamepad> m_oGamepads;

	};

}





#endif // RLINPUT_GAMEPAD