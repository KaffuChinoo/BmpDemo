
// BMP2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "BMP2.h"
#include "BMP2Dlg.h"
#include "afxdialogex.h"
#include "pack.h"
#include "unpack.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBMP2Dlg 对话框



CBMP2Dlg::CBMP2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BMP2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBMP2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILEPATH, m_editFilePath);
	DDX_Control(pDX, IDC_BUTTON_PACK, m_btnPack);
	DDX_Control(pDX, IDC_BUTTON_UNPACK, m_btnUnpack);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_btnFile);
}

BEGIN_MESSAGE_MAP(CBMP2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CBMP2Dlg::OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_PACK, &CBMP2Dlg::OnBnClickedButtonPack)
	ON_BN_CLICKED(IDC_BUTTON_UNPACK, &CBMP2Dlg::OnBnClickedButtonUnpack)
END_MESSAGE_MAP()


// CBMP2Dlg 消息处理程序

BOOL CBMP2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBMP2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBMP2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBMP2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBMP2Dlg::OnBnClickedButtonFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("BMP Files (*.bmp)|*.bmp||"), NULL);
	if (fileDlg.DoModal() == IDOK)
	{
		CString filePathName = fileDlg.GetPathName();
		// 将所选文件的路径添加到 CEdit 控件中
		m_editFilePath.SetWindowText(filePathName);
	}
}


void CBMP2Dlg::OnBnClickedButtonPack()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFilePath;
	m_editFilePath.GetWindowText(strFilePath);
	std::string s = CT2A(strFilePath);//将文件路径转换为const char*类型
	const char* file = s.c_str();
	bmpPack(file);
	MessageBox(_T("压缩完成，文件名 packed.bmp"), _T("提示"), MB_OK);
}


void CBMP2Dlg::OnBnClickedButtonUnpack()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFilePath;
	m_editFilePath.GetWindowText(strFilePath);
	std::string s = CT2A(strFilePath);//将文件路径转换为const char*类型
	const char* file = s.c_str();
	bmpUnpack(file);
	MessageBox(_T("解压完成，文件名 unpack.bmp"), _T("提示"), MB_OK);
}
