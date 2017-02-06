#pragma once

namespace UiLib
{
class CMachineTree;
class CMachineNode : public CListContainerElementUI, public IDialogBuilderCallback
{
public:
	CMachineNode();
	~CMachineNode();
	void SetLocation(CMachineTree *pMainTree,CMachineNode *pParentNode);
	void AddNode();
	void RemoveNode(CMachineNode *pChildNode);
	void RemoveAllNode();
	void DoInit();

	CControlUI* CreateControl(LPCTSTR pstrClass,CPaintManagerUI* pManager = NULL);
protected:
	CMachineTree *m_pMainTree;
	CMachineNode *m_pParentNode;
	CButtonUI *m_pBtnAdd;
};

class CMachineTree : public CListUI , public IDialogBuilderCallback
{
public:
	CMachineTree();
	~CMachineTree(void);
	bool OnInit(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);

	void AddItem();
	void DeleteItem(int nIndex);
	void DeleteItem(CMachineNode *pListItem);
	
	CControlUI* CreateControl(LPCTSTR pstrClass,CPaintManagerUI* pManager = NULL);
};

}