
// BMP2Dlg.h: 头文件
//

#pragma once


// CBMP2Dlg 对话框
class CBMP2Dlg : public CDialogEx
{
// 构造
public:
	CBMP2Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BMP2_DIALOG };
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
	DECLARE_MESSAGE_MAP()
public:

	CEdit m_editFilePath;
	CButton m_btnPack;
	CButton m_btnUnpack;
	CButton m_btnFile;
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonPack();
	afx_msg void OnBnClickedButtonUnpack();
};
