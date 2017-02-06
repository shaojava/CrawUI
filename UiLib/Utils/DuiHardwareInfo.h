#ifndef DuiHardwareInfo_h__
#define DuiHardwareInfo_h__

namespace UiLib
{
	//////////////////////////////////////////////////////////////////////////
	//显示器相关信息
	class UILIB_API DuiMonitor
	{
	public:
		static DWORD GetMonitorCount(bool _bReCheck = false);
		static DWORD GetMainMonitorIndex();
		static POINTL GetMonitorXY(DWORD _MonitorIndex = 1);
		static SIZE  GetMonitorSize(DWORD _MonitorIndex = 1);
		static DISPLAY_DEVICE& GetMonitorDisplayDevice(DWORD _MonitorIndex = 1);
		static DEVMODE& GetMonitorDevMode(DWORD _MonitorIndex = 1);
	};
}
#endif // DuiHardwareInfo_h__
