#include "stdafx.h"
#include "MachineView.h"

namespace UiLib{

CMachineNode::CMachineNode()
{
	m_pBtnAdd = NULL;
}

CMachineNode::~CMachineNode()
{

}

void CMachineNode::SetLocation( CMachineTree *pMainTree,CMachineNode *pParentNode )
{
	m_pMainTree = pMainTree;
	m_pParentNode = pParentNode;
	int nChildeCnt = GetCount();
	for(int i = 0 ; i < nChildeCnt ; i++)
	{
		CMachineNode *pChildeNode = (CMachineNode *)GetItemAt(i);
		pChildeNode->SetLocation(m_pMainTree,this);
	}
}

void CMachineNode::AddNode()
{
	CMachineNode *pChildNode = NULL;
	CDialogBuilder dlgBuilder;
	if( !dlgBuilder.GetMarkup()->IsValid())
		pChildNode = static_cast<CMachineNode*>(dlgBuilder.Create(_T("MachineNode.xml"), (UINT)0, this, m_pManager));
	if(!pChildNode)
		return;
	CListContainerElementUI::Add(pChildNode);
	int nHeight = GetFixedHeight();
	SetFixedHeight(nHeight * GetCount()+1);
}

void CMachineNode::RemoveNode( CMachineNode *pChildNode )
{
	CListContainerElementUI::Remove(pChildNode);
}

void CMachineNode::RemoveAllNode()
{
	CListContainerElementUI::RemoveAll();
}

CControlUI* CMachineNode::CreateControl( LPCTSTR pstrClass,CPaintManagerUI* pManager /*= NULL*/ )
{
	if( _tcscmp(pstrClass, _T("MachineNode")) == 0 ) 
	{
		CMachineNode *pNode = new CMachineNode();
		pNode->SetLocation(m_pMainTree,this);
		return pNode;
	}
	return NULL;
}

void CMachineNode::DoInit()
{
	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////

CMachineTree::CMachineTree()
{
	CControlUI::OnInit += MakeDelegate(this,&CMachineTree::OnInit,_T(""));
}

CMachineTree::~CMachineTree(void)
{
}

bool CMachineTree::OnInit(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam)
{
	return true;
}

void CMachineTree::AddItem()
{
	CMachineNode *pItem = NULL;
	CDialogBuilder dlgBuilder;
	if( !dlgBuilder.GetMarkup()->IsValid())
		pItem = static_cast<CMachineNode*>(dlgBuilder.Create(_T("MachineNode.xml"), (UINT)0, this, m_pManager));
	if(!pItem)
		return;
 	pItem->SetOwner(this);
 	CListUI::Add(pItem);
}

void CMachineTree::DeleteItem(int nIndex)
{
	RemoveAt(nIndex);
}

void CMachineTree::DeleteItem(CMachineNode *pListItem)
{
	if(pListItem)
		DeleteItem(GetItemIndex(pListItem));
}

CControlUI* CMachineTree::CreateControl(LPCTSTR pstrClass,CPaintManagerUI* pManager)
{
	if( _tcscmp(pstrClass, _T("MachineNode")) == 0 ) 
	{
		CMachineNode *pNode = new CMachineNode();
		pNode->SetLocation(this,NULL);
		return pNode;
	}
	return NULL;
}

}