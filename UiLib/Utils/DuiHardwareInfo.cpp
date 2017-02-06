#include "StdAfx.h"
#include "DuiHardwareInfo.h"

namespace UiLib
{
	//************************************
	// 函数名称: GetMonitorCount
	// 返回类型: DWORD
	// 参数信息: bool _bReCheck
	// 函数说明: 
	//************************************
	DWORD DuiMonitor::GetMonitorCount(bool _bReCheck/* = false*/)
	{
		static DWORD mMonitorCount = 0;

		if(mMonitorCount && !_bReCheck)
			return mMonitorCount;

		DEVMODE mDevMode;
		memset(&mDevMode,0, sizeof(mDevMode));
		DISPLAY_DEVICE mDisplayDevice;
		DWORD mCheckCount = 0;

		while(true)
		{
			memset(&mDisplayDevice,0, sizeof(mDisplayDevice));
			mDisplayDevice.cb = sizeof(DISPLAY_DEVICE);
			if(EnumDisplayDevices(NULL, mCheckCount, &mDisplayDevice,0) == FALSE)
				break;

			if(EnumDisplaySettings(mDisplayDevice.DeviceName,ENUM_CURRENT_SETTINGS,&mDevMode))
				mMonitorCount++;

			mCheckCount++;
		}

		return mMonitorCount;
	}

	//************************************
	// 函数名称: GetMainMonitorIndex
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD DuiMonitor::GetMainMonitorIndex()
	{
		DWORD nMonitorCount = GetMonitorCount();
		for(DWORD nMonitorIndex = 1;nMonitorIndex <= nMonitorCount;nMonitorIndex++)
		{
			POINTL nPoint = GetMonitorXY(nMonitorIndex);
			if(0 == nPoint.x && 0 == nPoint.y)
				return nMonitorIndex;
		}
		return 1;
	}

	//************************************
	// 函数名称: GetMonitorXY
	// 返回类型: POINTL
	// 参数信息: DWORD _MonitorIndex
	// 函数说明: 
	//************************************
	POINTL DuiMonitor::GetMonitorXY( DWORD _MonitorIndex /*= 1*/ )
	{
		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return POINTL();

		DEVMODE& mDevMode = GetMonitorDevMode(_MonitorIndex);
		if(!mDevMode.dmDriverVersion)
			return POINTL();

		return mDevMode.dmPosition;
	}

	//************************************
	// 函数名称: GetMonitorSize
	// 返回类型: SIZE
	// 参数信息: DWORD _MonitorIndex
	// 函数说明: 
	//************************************
	SIZE DuiMonitor::GetMonitorSize( DWORD _MonitorIndex /*= 1*/ )
	{
		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return SIZE();

		DEVMODE& mDevMode = GetMonitorDevMode(_MonitorIndex);
		if(!mDevMode.dmDriverVersion)
			return SIZE();
		
		SIZE mSize = { mDevMode.dmPelsWidth, mDevMode.dmPelsHeight};
		return mSize;
	}

	//************************************
	// 函数名称: GetMonitorDisplayDevice
	// 返回类型: DISPLAY_DEVICE&
	// 参数信息: DWORD _MonitorIndex
	// 函数说明: 
	//************************************
	DISPLAY_DEVICE& DuiMonitor::GetMonitorDisplayDevice( DWORD _MonitorIndex /*= 1*/ )
	{
		static DISPLAY_DEVICE mDisplayDevice;
		memset(&mDisplayDevice,0, sizeof(mDisplayDevice));

		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return mDisplayDevice;

		DEVMODE mDevMode;
		memset(&mDevMode,0, sizeof(mDevMode));
		DWORD mCheckCount = 0,mMonitorCount = 0;

		while(mMonitorCount != _MonitorIndex)
		{
			mDisplayDevice.cb = sizeof(DISPLAY_DEVICE);
			if(EnumDisplayDevices(NULL, mCheckCount, &mDisplayDevice,0) == FALSE)
				break;

			if(EnumDisplaySettings(mDisplayDevice.DeviceName,ENUM_CURRENT_SETTINGS,&mDevMode))
				mMonitorCount++;
			
			mCheckCount++;
		}

		return mDisplayDevice;
	}

	//************************************
	// 函数名称: GetMonitorDevMode
	// 返回类型: DEVMODE&
	// 参数信息: DWORD _MonitorIndex
	// 函数说明: 
	//************************************
	DEVMODE& DuiMonitor::GetMonitorDevMode( DWORD _MonitorIndex /*= 1*/ )
	{
		static DEVMODE mDevMode;
		memset(&mDevMode,0, sizeof(mDevMode));

		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return mDevMode;

		mDevMode.dmSize = sizeof(mDevMode);

		DISPLAY_DEVICE& mDisplayDevice = GetMonitorDisplayDevice(_MonitorIndex);
		mDisplayDevice.cb = sizeof(DISPLAY_DEVICE);

		if(EnumDisplaySettings(mDisplayDevice.DeviceName,ENUM_CURRENT_SETTINGS,&mDevMode))
			return mDevMode;

		memset(&mDevMode,0, sizeof(mDevMode));
		return mDevMode;
	}

}