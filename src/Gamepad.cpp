#include "rlInput/Gamepad.hpp"

#include <locale>

#pragma comment(lib, "GameInput.lib")


namespace rlInput
{

	namespace
	{
		std::wstring GameInputStringToWString(const GameInputString *o)
		{
			if (o == nullptr)
				return L"";

			std::wstring sResult(o->sizeInBytes, L'\0');
			MultiByteToWideChar(CP_UTF8, 0, o->data, o->sizeInBytes, sResult.data(),
				sResult.size() + 1);
			sResult.shrink_to_fit();
			return sResult;
		}
	}



	GameInput GameInput::s_oInstance;



	GameInput::Gamepad::Gamepad(IGameInputDevice *pDevice) : m_pDevice(pDevice)
	{
		auto gamepadInfo = pDevice->GetDeviceInfo();
		m_sDisplayName   = GameInputStringToWString(gamepadInfo->displayName);
		m_sDeviceStrings = GameInputStringToWString(gamepadInfo->deviceStrings);
	}

	void GameInput::DeviceCallback(
			 GameInputCallbackToken callbackToken,
			 void* context,
			 IGameInputDevice* device,
			 uint64_t timestamp,
			 GameInputDeviceStatus currentStatus,
			 GameInputDeviceStatus previousStatus
	)
	{
		auto &oDest = *reinterpret_cast<std::vector<GameInput::Gamepad> *>(context);

		oDest.push_back(device);
	}

	void GameInput::updateDeviceList()
	{
		m_oGamepads.clear();

		GameInputCallbackToken oToken;

		if (FAILED(m_upGameInput->RegisterDeviceCallback(nullptr,
			GameInputKindController | GameInputKindGamepad | GameInputKindArcadeStick,
			GameInputDeviceConnected, GameInputBlockingEnumeration, &m_oGamepads,
			DeviceCallback, &oToken)))
			throw std::exception("rlInput: RegisterDeviceCallback failed");

		m_upGameInput->UnregisterCallback(oToken, 0);
	}

	GameInput::GameInput() : m_upGameInput(
		std::unique_ptr<IGameInput, decltype(&DeleteGameInput)>(nullptr, &DeleteGameInput))
	{
		IGameInput *pGameInput;
		if (FAILED(GameInputCreate(&pGameInput)))
			throw std::exception("rlInput::GameInput: GameInputCreate() failed");

		m_upGameInput.reset(pGameInput);

		updateDeviceList();
	}

}
