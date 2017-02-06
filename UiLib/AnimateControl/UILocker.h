#pragma once

namespace UiLib
{
	class CLockerUI;

	class UILIB_API CLockerItemUI : public CVerticalLayoutUI
	{
	public:
		CLockerItemUI();
		~CLockerItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetOwner(CLockerUI *pLocker);

		void SetTrayShrinkSize(int nTrayHeight);
		int GetTrayShrinkSize();
		int GetTrayExpandSize();

		void DoEvent(TEventUI& event);

		void Expand(bool bExpand = true);
		bool IsExpand();
	protected:
		int m_nTrayShrinkHeight;
		CLockerUI *m_pOwner;
		bool m_bExpand;
	};

	/////////////////////////////////////////////////////////////////////
	//////////////////

	class UILIB_API CLockerUI : public CVerticalLayoutUI
	{
	public:
		struct ItemHeightInfo
		{
			CDuiString szTag;
			float fHeight;
			CLockerItemUI *pItem;
			ItemHeightInfo()
			{
				szTag.Empty();
				fHeight = 0.0f;
				pItem = NULL;
			}
		};

		typedef vector<ItemHeightInfo *> ItemInfos;
	public:
		CLockerUI(void);
		~CLockerUI(void);

		LPCTSTR GetClass() const;
		void DoInit();
		bool Add(CControlUI* pControl);
		void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

		void ExpandItem(CLockerItemUI *pItem);
		void ShrinkItem(CLockerItemUI *pItem);
		void UpdateItemHeightInfo();

		bool OnAnimateStep(TimeEvent *pEvent);

		void SetPos(RECT rc);
	protected:
		ItemInfos m_ItemInfos;
		CTimeLineTween *m_pExpandTime;
	};

}
