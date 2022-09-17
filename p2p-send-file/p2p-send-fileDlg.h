
// p2p-send-fileDlg.h: 头文件
//

#pragma once

// visit https://www.kkuai.com to get latest lib and header file
#include "kkp2p_sdk.h"

// Cp2psendfileDlg 对话框
class Cp2psendfileDlg : public CDialog
{
// 构造
public:
	Cp2psendfileDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_P2PSENDFILE_DIALOG };
#endif

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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_loginDomain;
	CString m_strDomain;

	CEdit m_loginPort;
	unsigned short m_usLoginPort;

	CComboBox m_connectMode;

	CEdit m_lanPort;
	unsigned short m_usLanPort;

	CEdit m_peerId;
	CString m_strPeerId;

	CEdit m_connectType;

	CEdit m_fileName;
	CString m_strFileName;

	CEdit m_statInfo;
	CButton m_fileButton;
	CButton m_startButton;

	kkp2p_engine_conf_t m_engineConf;
public:
	void BeginWork();
};
