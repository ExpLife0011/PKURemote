// PKURemoteDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CPKURemoteDlg 对话框
class CPKURemoteDlg : public CDialog
{
// 构造
public:
	CPKURemoteDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPKURemoteDlg();

// 对话框数据
	enum { IDD = IDD_PKUREMOTE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	NOTIFYICONDATA m_nid;
	CStatic m_staticReadMe;
	CStatic m_staticState;
	CButton m_btnStart;
	CButton m_btnStop;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	void AddTray();
	void RemoveTray();
};
