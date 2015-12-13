// PKURemoteDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CPKURemoteDlg �Ի���
class CPKURemoteDlg : public CDialog
{
// ����
public:
	CPKURemoteDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CPKURemoteDlg();

// �Ի�������
	enum { IDD = IDD_PKUREMOTE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
