// ToDo: Callback on device removal?
// https://stackoverflow.com/a/16528504
// RegisterDeviceNotification

#include <rlInput/Gamepad.DirectInput.hpp>

// Win32
#include <wbemidl.h>
#include <oleauto.h>
#pragma comment(lib, "Dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

namespace rlInput
{

	namespace
	{

		bool IsGameController(DWORD dwDevType)
		{
			switch ((BYTE)dwDevType)
			{
			case DI8DEVTYPE_GAMEPAD:
			case DI8DEVTYPE_JOYSTICK:
				return true;

			default:
				return false;
			}
		}

		BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
		{
			// check if regular game controller
			if (!IsGameController(lpddi->dwDevType))
				return DIENUM_CONTINUE; // no regular game controller



			using Meta = DirectInput::GamepadMeta;

			auto &oDest = *reinterpret_cast<std::vector<Meta> *>(pvRef);

			try
			{
				oDest.push_back(Meta
				{
					.guidInstance = lpddi->guidInstance,
					.sInstanceName = lpddi->tszInstanceName,
					.sProductName  = lpddi->tszProductName
				});
			}
			catch (...)
			{
				return DIENUM_STOP;
			}

			return DIENUM_CONTINUE;
		}

	}





	DirectInput::Gamepad::Gamepad(const GUID &guid, HWND hWnd) :
		m_oGUID(guid), m_hWnd(hWnd), m_oAxes(6)
	{
		const auto pDirectInput = DirectInput::s_oInstance.m_pDirectInput;
		
		if (pDirectInput->CreateDevice(guid, &m_pDevice, NULL) != DI_OK)
			throw std::exception("Failed to initialize DirectInput device");

		bool bAcquired = false;
		const auto hrSetDataFormat = m_pDevice->SetDataFormat(&c_dfDIJoystick);
		switch (hrSetDataFormat)
		{
		case DIERR_ACQUIRED:
			bAcquired = true;
			[[fallthrough]];
		case DI_OK:
			break;

		default:
			goto lbError;
		}

		if (m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
			goto lbError;



		{
			// get device strings

			DIPROPSTRING dips{};
			dips.diph.dwSize       = sizeof(dips);
			dips.diph.dwHeaderSize = sizeof(dips.diph);

			auto hrProp = m_pDevice->GetProperty(DIPROP_INSTANCENAME, &dips.diph);
			if (hrProp == DI_OK || hrProp == S_FALSE)
				m_sInstanceName = dips.wsz;
			hrProp = m_pDevice->GetProperty(DIPROP_PRODUCTNAME, &dips.diph);
			if (hrProp == DI_OK || hrProp == S_FALSE)
				m_sProductName = dips.wsz;



			DIDEVCAPS didc{ sizeof(didc) };
			if (m_pDevice->GetCapabilities(&didc) != DI_OK)
				goto lbError;

			if (!IsGameController(didc.dwDevType))
			{
				m_pDevice->Release();
				throw std::exception("DirectInput device was not a game controller");
			}

			m_bConnected = didc.dwFlags & DIDC_ATTACHED;
			m_oButtons.resize(didc.dwButtons < 32 ? didc.dwButtons : 32);
			m_iAxesCount = didc.dwAxes;

			m_oRawStates_Old = std::make_unique<bool[]>(m_oButtons.size());
			memset(m_oRawStates_Old.get(), 0, m_oButtons.size() * sizeof(bool));
		}


		return;

	lbError:
		m_pDevice->Release();
		throw std::exception("Failed to initialize DirectInput device");
	}

	DirectInput::Gamepad::~Gamepad() { m_pDevice->Release(); }

	bool DirectInput::Gamepad::prepare() noexcept
	{
		auto hr = m_pDevice->Poll();

		switch (hr)
		{
		case DI_OK:
		case DI_NOEFFECT:
			break;

		case DIERR_INPUTLOST:
		case DIERR_NOTACQUIRED:
			hr = m_pDevice->Acquire();

			if (hr != DI_OK && hr != S_FALSE)
			{
				m_bConnected = (hr != DIERR_INPUTLOST);
				return false;
			}
		}

		DIJOYSTATE oState{};
		if (m_pDevice->GetDeviceState(sizeof(oState), &oState) != DI_OK)
			return false;

		auto pOld = m_oRawStates_Old.get();
		auto pNew  = m_oButtons.data();
		for (size_t iButton = 0; iButton < m_oButtons.size(); ++iButton)
		{
			const bool bNew = oState.rgbButtons[iButton] & 0x80;

			pNew->bPressed  =  bNew && !*pOld;
			pNew->bDown     =  bNew;
			pNew->bReleased = !bNew && *pOld;

			*pOld = bNew;
			++pOld;
			++pNew;
		}

		m_oAxes[DINPUT_AXIS_X]  = oState.lX;
		m_oAxes[DINPUT_AXIS_Y]  = oState.lY;
		m_oAxes[DINPUT_AXIS_Z]  = oState.lZ;
		m_oAxes[DINPUT_AXIS_RX] = oState.lRx;
		m_oAxes[DINPUT_AXIS_RY] = oState.lRy;
		m_oAxes[DINPUT_AXIS_RZ] = oState.lRz;

		return true;
	}

	void DirectInput::Gamepad::reset() noexcept
	{
		m_bConnected = false;
		memset(m_oRawStates_Old.get(), 0, m_oButtons.size() * sizeof(bool));
		memset(m_oButtons.data(),      0, m_oButtons.size() * sizeof(Button));
		memset(m_oAxes.data(),         0, m_oAxes.size()    * sizeof(Axis));
	}





	DirectInput DirectInput::s_oInstance;

	void DirectInput::updateControllerList()
	{
		m_oAvailableControllers.clear();
		const auto hr = m_pDirectInput->EnumDevices(
			DI8DEVCLASS_GAMECTRL,     // dwDevType
			DIEnumDevicesCallback,    // lpCallback
			&m_oAvailableControllers, // pvRef
			DIEDFL_ATTACHEDONLY       // dwFlags
		);

		if (hr != DI_OK)
			throw std::exception("DirectInput: Call to EnumDevices failed");
	}

	bool DirectInput::isXInput(const GUID &guid) const noexcept
	{
		// SOURCE CODE BY MICROSOFT
		// https://learn.microsoft.com/en-us/windows/win32/xinput/xinput-and-directinput#xinput-and-directinput-side-by-side

		IWbemLocator*           pIWbemLocator   = nullptr;
		IEnumWbemClassObject*   pEnumDevices    = nullptr;
		IWbemClassObject*       pDevices[20]    = {};
		IWbemServices*          pIWbemServices  = nullptr;
		BSTR                    bstrNamespace   = nullptr;
		BSTR                    bstrDeviceID    = nullptr;
		BSTR                    bstrClassName   = nullptr;
		bool                    bIsXinputDevice = false;

		// CoInit if needed
		HRESULT hr = CoInitialize(nullptr);
		bool bCleanupCOM = SUCCEEDED(hr);

		// So we can call VariantClear() later, even if we never had a successful
		// IWbemClassObject::Get().
		VARIANT var ={};
		VariantInit(&var);

		// Create WMI
		hr = CoCreateInstance(__uuidof(WbemLocator),
			nullptr,
			CLSCTX_INPROC_SERVER,
			__uuidof(IWbemLocator),
			(LPVOID*)&pIWbemLocator);
		if (FAILED(hr) || pIWbemLocator == nullptr)
			goto LCleanup;

		bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");
		if (bstrNamespace == nullptr)
			goto LCleanup;
		bstrClassName = SysAllocString(L"Win32_PNPEntity");
		if (bstrClassName == nullptr)
			goto LCleanup;
		bstrDeviceID = SysAllocString(L"DeviceID");
		if (bstrDeviceID == nullptr)
			goto LCleanup;

		// Connect to WMI 
		hr = pIWbemLocator->ConnectServer(bstrNamespace, nullptr, nullptr, 0L,
			0L, nullptr, nullptr, &pIWbemServices);
		if (FAILED(hr) || pIWbemServices == nullptr)
			goto LCleanup;

		// Switch security level to IMPERSONATE. 
		hr = CoSetProxyBlanket(pIWbemServices,
			RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
			RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
			nullptr, EOAC_NONE);
		if (FAILED(hr))
			goto LCleanup;

		hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &pEnumDevices);
		if (FAILED(hr) || pEnumDevices == nullptr)
			goto LCleanup;

		// Loop over all devices.
		for (;;)
		{
			ULONG uReturned = 0;
			hr = pEnumDevices->Next(10000, _countof(pDevices), pDevices, &uReturned);
			if (FAILED(hr))
				goto LCleanup;
			if (uReturned == 0)
				break;

			for (size_t iDevice = 0; iDevice < uReturned; ++iDevice)
			{
				// For each device, get its device ID.
				hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, nullptr, nullptr);
				if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
				{
					// Check if the device ID contains "IG_".  If it does, then it's an XInput
					// device.
					// This information can not be found from DirectInput.
					if (wcsstr(var.bstrVal, L"IG_"))
					{
						// If it does, then get the VID/PID from var.bstrVal
						DWORD dwPid = 0, dwVid = 0;
						WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
						if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
							dwVid = 0;
						WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
						if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
							dwPid = 0;

						// Compare the VID/PID to the DInput device
						DWORD dwVidPid = MAKELONG(dwVid, dwPid);
						if (dwVidPid == guid.Data1)
						{
							bIsXinputDevice = true;
							goto LCleanup;
						}
					}
				}
				VariantClear(&var);
				SAFE_RELEASE(pDevices[iDevice]);
			}
		}

	LCleanup:
		VariantClear(&var);

		if (bstrNamespace)
			SysFreeString(bstrNamespace);
		if (bstrDeviceID)
			SysFreeString(bstrDeviceID);
		if (bstrClassName)
			SysFreeString(bstrClassName);

		for (size_t iDevice = 0; iDevice < _countof(pDevices); ++iDevice)
			SAFE_RELEASE(pDevices[iDevice]);

		SAFE_RELEASE(pEnumDevices);
		SAFE_RELEASE(pIWbemLocator);
		SAFE_RELEASE(pIWbemServices);

		if (bCleanupCOM)
			CoUninitialize();

		return bIsXinputDevice;
	}

	DirectInput::DirectInput()
	{
		const auto hr =
			DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8,
				reinterpret_cast<LPVOID *>(&m_pDirectInput), NULL);

		if (hr != DI_OK)
			throw std::exception("Error initializing DirectInput");

		updateControllerList();
	}

	DirectInput::~DirectInput()
	{
		m_pDirectInput->Release();
	}

}
