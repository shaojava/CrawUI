#pragma once

#define UPDATE_FRAME_TIMER 666

typedef std::vector<Image *> ImageFrames;

class CFrameIamgeUI : public CControlUI
{
public:
	CFrameIamgeUI(void);
	~CFrameIamgeUI(void);
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	
	void DoEvent(TEventUI &event);
	void DoPaint(const RECT& rcPaint);
	void DoPaintWithOutClip(const RECT& rcPaint);

	void StartPlay(int nElapse);

	bool AddFrame(Image *pImage,int nIndex = -1);
	void ClearFrame();
public:
	int m_nElapse;
	int m_nPlayIndex;
	ImageFrames m_ImgFrames;
};

