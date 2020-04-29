
// MFCApplication1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include <tchar.h>

#include "cryptlib.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "hex.h"
#include "md5.h"
#include "sha.h"
#include "crc.h"
#include "files.h"
#include <string>

#include <iostream>
#include <shlwapi.h>

#include<iostream>
#include<Windows.h>
#include<TlHelp32.h>

#include<afxsock.h>

#include "stdafx.h"
#include <Windows.h>
#include "folder_dialog.h"
#include "stdafx.h"

#include "winsock.h"
#include "stdio.h"
#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib")


using namespace std;
//using namespace BaseFunc;

#pragma comment(lib,"Shlwapi.lib") 
#pragma comment(lib, "cryptlib.lib")
using namespace CryptoPP;

#pragma warning(disable:4996)

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


// CMFCApplication1Dlg 对话框
CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
	, mEdit1(_T(""))
	, mEditFolderName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	if (MDEBUG) {
		CString strdebug = getWorkDir();
		strdebug.Replace(_T('\\'), _T('/'));

		pszFileName = strdebug + _T("/../x64/release/cfg.txt");
		cstrListFile = strdebug + _T("/../x64/release/Filelist.txt");// keep all files in the setting floder
		cstrDetctedFile = strdebug + _T("/../x64/release/Detctedlist.txt");// keep all files detected results
		cstrIListFile = strdebug + _T("/../x64/release/IFilelist.txt");// files to identify
		cstrGalleryFile = strdebug + _T("/../x64/release/Gallerylist.txt");// pictures for identify database
		cstrIdentifiedFile = strdebug + _T("/../x64/release/Identifiedlist.txt");// pictures for identify results
		cstrChangeFile = strdebug + _T("/../x64/release/Changelist.txt");// file names which has been changed
		cstrDarkexeName = strdebug + _T("/../x64/release/darknet_no_gpu.exe");
		cstrSearchexeName = strdebug + _T("/../x64/release/search.exe");
	}

	cstrWorkDir = getWorkDir();

	pszFileName = cstrWorkDir + _T("/") + pszFileName;
	cstrListFile = cstrWorkDir + _T("/") + cstrListFile;
	cstrDetctedFile = cstrWorkDir + _T("/") + cstrDetctedFile;
	cstrIListFile = cstrWorkDir + _T("/") + cstrIListFile;
	cstrGalleryFile = cstrWorkDir + _T("/") + cstrGalleryFile;
	cstrIdentifiedFile = cstrWorkDir + _T("/") + cstrIdentifiedFile;
	cstrChangeFile = cstrWorkDir + _T("/") + cstrChangeFile;



	strAllFile = _T("");
	ldFileCnt = 0;
	ldPathCnt = 0;

	// create or read setfile
	if (!cfgRead(pszFileName)) {
		cfgDefault(pszFileName);
		if (!cfgRead(pszFileName)) {
			//exit process
		}
		else {
			mEditFolderName = CFGStr[0];
		}
	}
	else {
		mEditFolderName = CFGStr[0];
	}

	//AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(cstrStatichead);

	
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_button1);
	DDX_Text(pDX, IDC_EDIT1, mEdit1);
	DDX_Text(pDX, IDC_EDIT2, mEditFolderName);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButtonIdentify)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication1Dlg::OnBnClickedButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication1Dlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCApplication1Dlg::OnBnClickedButtonIdentifyDefaul)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CMFCApplication1Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMFCApplication1Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFCApplication1Dlg::OnBnClickedChooseFolder)
	ON_BN_CLICKED(IDC_BUTTON7, &CMFCApplication1Dlg::OnBnClickedDetectedFile)
	ON_BN_CLICKED(IDC_BUTTON8, &CMFCApplication1Dlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CMFCApplication1Dlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CMFCApplication1Dlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &CMFCApplication1Dlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON11, &CMFCApplication1Dlg::OnBnClickedButton11)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	// 初始化父进程 pipe
	my_PipeInit();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCApplication1Dlg::OnPaint()
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
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication1Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	// 文件当前目录
	TCHAR Buffer[MAX_PATH];
	DWORD dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
	CString strCurrent(Buffer);
	CString strtime, strdir;

	CString mstr;

	//获取系统时间
	CTime tm;
	m_button1.EnableWindow(false);

	clock_t start, finish;
	double  duration;
	
	start = clock();
	

	strdir = mEditFolderName;// "e:";

	CFile fileList;
	if (!fileList.Open(cstrListFile, CFile::modeCreate | CFile::modeWrite))// create a empty file Filelist.txt
	{
		TRACE(_T("File could not be opened\n"));
	}

	if (strdir == _T("")) {
		MessageBox(_T("设置文件夹先"), NULL, MB_OK);
		m_button1.EnableWindow(true);
		return;
	}
	TravelFolder(strdir, &fileList);

	fileList.Close();

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("%f seconds\n", duration);
	mstr.Format(_T("run time :%f "),duration);
	TRACE(mstr);
	//AfxMessageBox(_T("finish"));
	m_button1.EnableWindow(true);
}
unsigned long ulcont = 0;
void CMFCApplication1Dlg::TravelFolder(CString strDir, CFile* fp)
{
	CFileFind filefind;                                         //声明CFileFind类型变量
	CFile file;
	Weak::MD5 hash;
	CString strWildpath = strDir + _T("/*.*");     //所有jpg文件都列出。

	if (filefind.FindFile(strWildpath, 0))                    //开始检索文件
	{
		BOOL bRet = TRUE;

		while (bRet)
		{
			bRet = filefind.FindNextFile();                 //枚举一个文件            

			if (filefind.IsDots())                                 //如果是. 或 .. 做下一个
			{
				continue;
			}

			// 文件名 begin
			CString strFileName = filefind.GetFileName();
			CString strLastName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1);
			// 文件名 end

			// 文件修改时间 begin
			FILETIME   filetime;
			FILETIME   localtime;
			SYSTEMTIME systemtime;
			
			filefind.GetLastWriteTime(&filetime);

			FileTimeToLocalFileTime(&filetime, &localtime); //换成本地时间

			FileTimeToSystemTime(&localtime, &systemtime);  //换成系统时间格式

			CString strTime = _T("");
			strTime.Format(_T("%04d%02d%02d%02d%02d%02d"),
				systemtime.wYear, systemtime.wMonth, systemtime.wDay,
				systemtime.wHour, systemtime.wMinute, systemtime.wSecond);

			CString strlen;
			strlen.Format(_T("%ull"), filefind.GetLength());

			CString strFilePath = (LPCTSTR)filefind.GetFilePath();

			CString strwrite;
			//strwrite = strFileName + _T("\x09") + strDir + _T("\x09") + strlen + _T("\x09");
			strwrite = strDir + _T("\\") + strFileName;

			// 文件修改时间 end            
			if (!filefind.IsDirectory())                          //不是子目录，把文件名打印出来
			{
				//filter: jpg only;
				CString strLastName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1).MakeLower();
				if (strLastName.Find(_T("jpg")) == -1)
					continue;
				/*
				int haslen = hash.DigestSize();
				memset(buf, 0, sizeof(buf));//初始化内存，防止读出字符末尾出现乱码
				memset(digest, 0, sizeof(digest));//初始化内存，防止读出字符末尾出现乱码
				try
				{
					if (!file.Open(strFilePath, CFile::modeRead))
					{
						CString str; 
						str.Format(_T("没有文件! %s\n"), strFilePath);
						TRACE(str);
						return;
					}
					
					//结构体格式读取
					int rdlen = 0;
					while(1)
					{
						rdlen = file.Read(buf, sizeof(buf));
						if (rdlen)
						{
							hash.Update((const BYTE*)buf,rdlen);
						}
						if (rdlen < sizeof(buf))
						{		
							hash.Final((BYTE*)&digest[0]);
							break;
						}
					} 
					file.Close();
				}
				catch (CFileException *e)
				{
					CString str;
					str.Format(_T("读取数据失败的原因是:%d ,%s"), e->m_cause, strFilePath);
					TRACE("str");
					file.Abort();
					e->Delete();
				}

				CString tempstr;
				//tempstr.Format(_T("%s"),digest);
				tempstr = charToHexCString(digest, sizeof(digest));
				strwrite += tempstr;
				*/
				strwrite += _T("\r\n");
				int len = CStringA(strwrite).GetLength();
				strwrite.Replace(_T('\\'), _T('/'));

				USES_CONVERSION;
				char* cp = T2A(strwrite);

				fp->Write(cp, len);
				
			}
			else                                                   //如果是子目录，递归调用该函数
			{
				CString strNewDir = strDir + CString(_T("/")) + filefind.GetFileName();
				ldPathCnt++;

				TravelFolder(strNewDir,fp);//递归调用该函数打印子目录里的文件
			}
		}
		filefind.Close();
	}
}

CString CMFCApplication1Dlg::charToHexCString(BYTE *ca, int len)
{
	CString str, tem;
	str = "";
	for (int i = 0; i < 16; i++)
	{
		tem.Format(_T("%02X"), ca[i]);
		str += tem;
	}
	return str;
}

void CMFCApplication1Dlg::my_PipeInit()
{
	if (!my_CreatePipe()) return;
	//LPWSTR exe_path = _T("D:/imageManage/seetaface/src/SeetaFace2-master/build/bin/Release/search.exe");
	//CString exe_path = _T("D:/imageManage/MFCApplication1/x64/darknet_no_gpu.exe");
	/*
	if (!my_CreateProcess(cstrDarkexeName)) {
		CloseHandle(m_hPipeWriteParentDetect);	//创建进程失败，关闭读写句柄
		CloseHandle(m_hPipeReadParentDetect);
		CloseHandle(m_hPipeWriteChildDetect);	//创建进程失败，关闭读写句柄
		CloseHandle(m_hPipeReadChildDetect);
		return;
	}
	*/
	pRecvParam.sock = m_hPipeReadParentDetect;
	pRecvParam.ptr = this;
	pRecvParam.hw = m_hPipeWriteChildDetect;

	LPVOID *plp = (LPVOID *)&pRecvParam;
	RECVPAR *lp = (RECVPAR *)plp;
	m_threadrun = 1;
	m_hThread = CreateThread(NULL, 0, Pipe_Listen, (LPVOID)&pRecvParam, 0, NULL);	//创建socket的发送线程
}

void CMFCApplication1Dlg::SetSecurity_attr(SECURITY_ATTRIBUTES& ps)
{
	//这里必须将 bInheritHandle 设置为 TRUE，
	//从而使得子进程可以继承父进程创建的匿名管道的句柄
	ps.bInheritHandle = TRUE;
	ps.lpSecurityDescriptor = NULL;
	ps.nLength = sizeof(SECURITY_ATTRIBUTES);
}

void CMFCApplication1Dlg::SetStartInfo(STARTUPINFO& si)
{
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESTDHANDLES;//STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
									  //子进程的标准输入句柄为父进程管道的读数据句柄
	si.hStdInput = m_hPipeReadChildDetect;
	//子进程的标准输出句柄为父进程管道的写数据句柄
	si.hStdOutput = m_hPipeWriteParentDetect;
	//子进程的标准错误处理句柄和父进程的标准错误处理句柄一致
	si.hStdError = HANDLE(STD_ERROR_HANDLE);
}

bool CMFCApplication1Dlg::my_CreatePipe()
{
	SECURITY_ATTRIBUTES sa;
	SetSecurity_attr(sa);
	if (!CreatePipe(&m_hPipeReadParentDetect, &m_hPipeWriteParentDetect, &sa, 0))
	{
		MessageBox(_T("create anonymous pipe failed"));
		return false;
	}
	if (!CreatePipe(&m_hPipeReadChildDetect, &m_hPipeWriteChildDetect, &sa, 0))
	{
		MessageBox(_T("create anonymous pipe failed"));
		return false;
	}
	pipeflg = 1;
	return true;
}

void CMFCApplication1Dlg::my_SendDataPipe(CString data)
{
	DWORD data_write;

	USES_CONVERSION;
	char* cp = T2A(data);

	//写入数据
	if (!pipeflg) return;
	if (!WriteFile(m_hPipeWriteChildDetect, (LPCTSTR)cp, (UINT)strlen(cp), &data_write, NULL))
	{
		//MessageBox(_T("client send data:"));
	}
}

bool CMFCApplication1Dlg::my_CreateProcess(CString epath)
{
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si;
	SetStartInfo(si);
	epath.Replace(_T('\\'), _T('/'));
	CString wpath = epath.Left(epath.ReverseFind(_T('/')));
	CString pname = epath.Right(epath.GetLength() - epath.ReverseFind(_T('/')));
	int pid=0;
	while (pid = getpid(pname)) {
		closepid(pid);
	}

	if (proFlg) {
		ColseChildProcess();
		proFlg = 0;
	}
	//BOOL kk = CreateProcess(epath, NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, wpath, &si, &pi);
	CString cstrdir = getWorkDir();
	cstrdir.Replace(_T('\\'), _T('/'));
	BOOL kk = CreateProcess(cstrWorkDir+_T("/")+epath, NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, cstrWorkDir, &si, &pi);
	if (!kk)
	{
		MessageBox(_T("create process failed: \r\n") + epath );
		return false;
	}
	//由于结构体pi中的数据已经使用不到了，将其资源释放
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	proFlg = 1;
	return true;
}

DWORD WINAPI CMFCApplication1Dlg::Pipe_Listen(LPVOID lpParameter)
{
	RECVPAR *lp =(RECVPAR *)lpParameter;
	CMFCApplication1Dlg *pthis = lp->ptr;
	HANDLE PipeRead = lp->sock;
	HANDLE* hw = &lp->hw;
	CHAR data[2048] = { 0 };
	DWORD data_read;
	CString cstrProgress;
	int npos;
	wchar_t wca[4096 + 1] = { 0 };
	wchar_t *wp = wca;

	while (pthis->m_threadrun==1)
	{
		memset(data, 0, sizeof(data));
		if (!ReadFile((HANDLE)(PipeRead), data, sizeof(data), &data_read, NULL))
			continue;
		time_t finish;
		time(&finish);

		CString str,strd; 
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->GetWindowText(str);

		//USES_CONVERSION;
		//CString strd = A2T(data);
		memset(wca, 0, sizeof(wca));
		int unicodefilelen = MultiByteToWideChar(CP_ACP, 0, data, (UINT)strlen(data), NULL, 0);//把ansi文件长度转为Unicode文件长度
		MultiByteToWideChar(CP_ACP, 0, data, (UINT)strlen(data), wp, (int)unicodefilelen);//把读到的文件转成宽字符
		wp[unicodefilelen] = '\0';

		strd = wp;
		str += _T("C|") + strd;
		//str += _T("C|") + wp;
		
		if (strd.Find(_T("1467359740360")) != -1) {
			str += _T(" 1467359740360");
		}


		if (strd.Find(_T("detect finished")) != -1) {
			if (pthis->autoFlg) {
				pthis->OnBnClickedDetectedFile();// detectetd file
				pthis->OnBnClickedButtonIdentify();
			}
		}
		if (strd.Find(_T("Identify finished")) != -1) {
			if (pthis->autoFlg) { 
				pthis->OnBnClickedButton9();
				pthis->autoFlg = 0;
			}
		}
		if (strd.Find(_T("exit process")) != -1) {
			//pthis->OnBnClickedDetectedFile();// detectetd file
			pthis->proFlg = 0;
		}
		npos = strd.Find(_T("LoaDing image: "));
		if (npos != -1) {
			pthis->clockCurrent = clock();

			
			int nCount = strd.Replace(_T("LoaDing image: "), _T("LoaDing image: "));
			if ((pthis->fileGrow + nCount) <= pthis->fileCount)
				pthis->fileGrow += nCount;

			long needTime = (pthis->clockCurrent - pthis->clockStart) * (pthis->fileCount - pthis->fileGrow) / pthis->fileGrow / CLOCKS_PER_SEC;

			cstrProgress.Format(_T("%s  %d/%d 【%02d:%02d:%02d】 %s")
								, pthis->cstrStatichead.GetBuffer(), pthis->fileGrow, pthis->fileCount
								,needTime/3600,(needTime%3600)/60,(needTime%60), strd.Right(strd.GetLength() - npos - 15).GetBuffer());
			cstrProgress.Replace(pthis->mEditFolderName, _T("\x7e"));
			AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(cstrProgress);
			//AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC)->SetWindowText(strd);

		}
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	pthis->m_threadrun = 2;
	ExitThread(0);
	//return data_read;
}


void CMFCApplication1Dlg::OnBnClickedButtonIdentify()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	cstrStatichead = _T(" 面部识别: ");

	fileCount = cuntFiles(cstrIListFile);
	fileGrow = 0;
	clockStart = clock();
	str.Format(_T("%s %d/%d "), cstrStatichead.GetBuffer(0), fileGrow,fileCount);
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(str);

	getGalleryFileList(cstrGalleryFile, NULL);
	
	creatIdentifyProcess();

	str = _T("IdenTifY ") + cstrGalleryFile + _T(" ") + cstrIListFile + _T(" ") + cstrIdentifiedFile;
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
}


void CMFCApplication1Dlg::OnBnClickedButtonDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	cstrStatichead = _T("目标检测: ");

	fileCount = cuntFiles(cstrListFile);
	fileGrow = 0;
	clockStart = clock();

	str.Format(_T("%s %d/%d "), cstrStatichead.GetBuffer(), fileGrow, fileCount);
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(str);



	createDetectProcess();

	str = _T("DetecT ") + cstrListFile + _T(" ") + cstrDetctedFile; // send the command and detect filet and detected file
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
}


void CMFCApplication1Dlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str = "QUIT";
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
}


void CMFCApplication1Dlg::OnBnClickedButtonIdentifyDefaul()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str = "test";
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
}

void CMFCApplication1Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//ColseChildProcess();	
	dopreexit();
	CDialogEx::OnClose();
}


void CMFCApplication1Dlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//ColseChildProcess();	
	dopreexit();
	CDialogEx::OnOK();
}


void CMFCApplication1Dlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//ColseChildProcess(); 
	dopreexit();
	CDialogEx::OnCancel();
}


int CMFCApplication1Dlg::ColseChildProcess()
{
	CString str;
	str = "QUIT";
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
	return 0;
}



unsigned CMFCApplication1Dlg::selFile(CString &strFile, const CString &strExt, bool bOpen)
{
	CString strDir = _T("D:\\imageManage");//这里通过strFile解析目录,CFileDialog会自动记住
	CString filename = _T("hi.txt");  //通过strFile解析文件名
	CString filter = strExt + _T("文件 (*.") + strExt + _T(")|*.") + strExt + _T("||");
	CString ext = _T(".") + strExt;


	CFileDialog dlg(((bool)bOpen, _T("*..*"), filename, OFN_READONLY | OFN_OVERWRITEPROMPT, filter, NULL));
	
	dlg.GetOFN().lpstrInitialDir = strFile;// 默认目录

	if (dlg.DoModal())
	{
		strFile = dlg.GetPathName();
		return IDOK;
	}

	return IDCANCEL;
}

void CMFCApplication1Dlg::OnBnClickedChooseFolder()
{
	CString path;
	//selFile(path, NULL, 1);
	CString pathSelected;
	CString dpath;
	if(mEditFolderName==_T(""))
		dpath = getWorkDir();
	else {
		dpath = mEditFolderName ;
		dpath.Replace(_T("/"), _T("\\"));
	}

	CFolderDialog dlg(&pathSelected,dpath);
	if (dlg.DoModal() == IDOK)
	{
		mEditFolderName = pathSelected;
		mEditFolderName.Replace(_T("\\"), _T("/"));
		cfgRead(pszFileName);
		CFGStr[0] = mEditFolderName;
		cfgWrite(pszFileName, CFGStr);
	}
	else
		AfxMessageBox(_T("无效的目录，请重新选择"));
	UpdateData(false);
}

int CMFCApplication1Dlg::cfgRead(CString fstr)
{
	CString rstr;
	CStdioFile fs;
	CFileException e;
	
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");
	
	if (!fs.Open(fstr, CFile::modeRead, &e)) {
		return 0;
	}
	else {
		int maxcnt = M_MAXCNT;
		//CString showstr;

		for (int i = 0; i < maxcnt; i++) {
			CFGStr[i] = "";
			if (fs.ReadString(rstr)) {
				CFGStr[i] = rstr;
				//showstr.Append(CFGStr[i]+_T("\r\n"));
			}
		}
	}
	fs.Close();
	setlocale(LC_CTYPE, old_locale); //还原语言区域的设置 
	free(old_locale);//还原区域设定

	return 1;
}


int CMFCApplication1Dlg::cfgWrite(CString fstr, CString *wstr)
{
	CFile f;
	CFileException e;

	if (!f.Open(fstr, CFile::modeCreate | CFile::modeWrite, &e))// creat a empty file to write
	{
		TRACE(_T("File could not be  created %d\n"), e.m_cause);
		return 0;
	}
	for (int i = 0; i < 5; i++) {

		USES_CONVERSION;
		char* cp = T2A(wstr[i]+_T("\r\n"));

		f.Write(cp, (UINT)strlen(cp));
	}
	return 0;
}


int CMFCApplication1Dlg::cfgDefault(CString fstr)
{
	CFile f;
	CFileException e;

	if (!f.Open(fstr, CFile::modeCreate | CFile::modeWrite, &e))
	{
		TRACE(_T("File could not be opened %d\n"), e.m_cause);
	}
	else {
		CString dstr = { _T("") };
		dstr = getWorkDir() + _T("\r\n") + _T("2\r\n") + _T("3\r\n") + _T("4\r\n") + _T("5");

		USES_CONVERSION;
		char* cp = T2A(dstr);

		f.Write(cp, (UINT)strlen(cp));
	}

	return 0;
}


CString CMFCApplication1Dlg::getWorkDir()
{
	TCHAR pFileName[MAX_PATH];
	int nPos = GetCurrentDirectory(MAX_PATH, pFileName);

	CString csFullPath(pFileName);
	if (nPos < 0)
		return NULL;
	else
		return csFullPath;	
}


void CMFCApplication1Dlg::OnBnClickedDetectedFile()// detected file
{
	doListToChange(cstrDetctedFile, cstrIListFile);	
}


int CMFCApplication1Dlg::DoChangeFileNamebak(CString cstrChFileList,int fdelete)
{
	CStdioFile fs;// chang list file write
	CFile fw;// chang list file write
	CStringArray cstrArdbuf;
	CFileException e;
	int rdlen = 0;

	char rdbuf[512];
	char trdbuf[512];


	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	if (!fs.Open(cstrChFileList,CFile::modeRead, &e)) {// open changelist file
		TRACE(_T("open change list file  failed\r\n"));
		return 0;
	}
	else {
		CString rstr;


		if (fs.GetLength() < 11) {
			TRACE(_T("file not long enough to do file change\r\n"));
			fs.Close();
			return 0;
		}

		if (!fw.Open(cstrChangeFile, CFile::modeCreate |CFile::modeNoTruncate | CFile::modeReadWrite, &e)) {// creat a empty file to write,"IListfile.txt"
			TRACE(_T("open changlist file failed\r\n"));
			return 0;
		}
		//fw.SeekToEnd();

		CString rcstr;
		
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		rcstr.Format(_T("%s m:%04d-%02d-%02d %02d:%02d:%02d c:%04d-%02d-%02d %02d:%02d:%02d\r\n")
			, cstrChangestart.GetBuffer(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
			, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);


		CString fstr, rfstr;

		while (1) {
			if (fs.ReadString(rstr)) {
				fstr = "";
				rfstr = "";

				CStringArray strResult;

				CString strGap = _T("\x09");
				int nPos = rstr.Find(strGap);

				CString strLeft = _T("");
				int cnt = 0;
				while (0 <= nPos)
				{
					strLeft = rstr.Left(nPos);
					if (!strLeft.IsEmpty())
						strResult.Add(strLeft);
					rstr = rstr.Right(rstr.GetLength() - nPos - 1);
					nPos = rstr.Find(strGap);
					if (nPos == -1) {
						strResult.Add(rstr);
						break;
					}
				}

				cnt = (int)strResult.GetSize();
				if (cnt==2) {// got gap "\x09"
					fstr = strResult.GetAt(0);
					rfstr = strResult.GetAt(1);
					if (rfstr != fstr) {

						BOOL FLAG = PathFileExists(fstr);
						if (!FLAG)
						{
							//AfxMessageBox(L"不存在该文件:"+fstr);							
						}
						else {
							CFile::Rename(fstr, rfstr);// there is no return value to know wether fail or success
							USES_CONVERSION;
							char *cp = T2A(rcstr + fstr + _T("\x09") + rfstr + _T("\r\n"));
							// read the same size of writing data to a buf,and after finish new writeing ,then write the buf data to file
							
							if (strlen(cp) < 512) {
								memset(rdbuf, 0, sizeof(rdbuf));
								rdlen = (UINT)strlen(cp);
								//rdlen = 0;
								rdlen = fw.Read(rdbuf, rdlen);
								rdlen = (UINT)strlen(rdbuf);
								if (rdlen > 0) {
									cstrArdbuf.Add(A2T(rdbuf));
									fw.Seek(fw.GetPosition() - rdlen, CFile::begin);
								}
							}
							fw.Write(cp, (UINT)strlen(cp));
							rcstr = "";
						}
					}
				}
				else { // not detected
				}
			}
			else {
				break;
			}
		}
		

		if (rcstr.GetAllocLength() == 0) {
			USES_CONVERSION;
			char *cp = T2A(cstrChangeend + _T("\r\n"));
			if (strlen(cp) < 512) {
				memset(rdbuf, 0, sizeof(rdbuf));
				rdlen = 0;
				rdlen = fw.Read(rdbuf, (UINT)strlen(cp));
				if (rdlen > 0) {
					cstrArdbuf.Add(A2T(rdbuf));
					fw.Seek(fw.GetPosition() - rdlen, CFile::begin);
				}
			}
			fw.Write(cp, (UINT)strlen(cp));
		}

		if (cstrArdbuf.GetSize()) {
			// loop read and write
			for (int i = 0; i < cstrArdbuf.GetSize(); i++) {
				fstr = cstrArdbuf.GetAt(i);
				USES_CONVERSION;
				char *cp = T2A(fstr);
				// read the same size of writing data to a buf,and after finish new writeing ,then write the buf data to file
				
				if (strlen(cp) < 512) {
					memset(rdbuf, 0, sizeof(rdbuf));
					rdlen = 0;
					rdlen = fw.Read(rdbuf, (UINT)strlen(cp));
					if (rdlen > 0) {//Return Value ,The number of bytes transferred to the buffer.
						cstrArdbuf.Add(A2T(rdbuf));
						fw.Seek(fw.GetPosition() - rdlen, CFile::begin);
					}
				}
				fw.Write(cp, (UINT)strlen(cp));
				memcpy(trdbuf, rdbuf, sizeof(rdbuf));
				//fflush(fw.h);
			}
		}

		fw.Close();
	}
	fs.Close();
	setlocale(LC_CTYPE, old_locale); //还原语言区域的设置 
	free(old_locale);//还原区域设定	

	/*
	if (fdelete) {
		BOOL FLAG = PathFileExists(cstrChFileList);
		if (FLAG)
			CFile::Remove(cstrChFileList);
	}
	*/

	return 0;
}

int CMFCApplication1Dlg::DoChangeFileName(CString cstrChFileList, int fdelete)
{
	CStdioFile fs;// chang list file write
	CFile fw;// chang list file write
	CStringArray cstrArdbuf;
	CFileException e;
	int rdlen = 0;





	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	if (!fs.Open(cstrChFileList, CFile::modeRead, &e)) {// open changelist file
		TRACE(_T("open change list file  failed\r\n"));
		return 0;
	}
	else {
		CString rstr;


		if (fs.GetLength() < 11) {
			TRACE(_T("file not long enough to do file change\r\n"));
			fs.Close();
			return 0;
		}

		if (!fw.Open(cstrChangeFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &e)) {// creat a empty file to write,"IListfile.txt"
			TRACE(_T("open changlist file failed\r\n"));
			return 0;
		}
		LONGLONG ansifilelen = fw.GetLength();//获得ansi编码文件的文件长度
		char *p = new char[ansifilelen + 1];//new一块新内存
		fw.Read(p, (int)ansifilelen);
		p[ansifilelen] = '\0';
		fw.Close();

		if (!fw.Open(cstrChangeFile, CFile::modeCreate | CFile::modeReadWrite, &e)) {// creat a empty file to write,"IListfile.txt"
			TRACE(_T("open changlist file failed\r\n"));
			return 0;
		}

		

		CString rcstr;

		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		rcstr.Format(_T("%s m:%04d-%02d-%02d %02d:%02d:%02d c:%04d-%02d-%02d %02d:%02d:%02d\r\n")
			, cstrChangestart.GetBuffer(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
			, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);


		CString fstr, rfstr;
		USES_CONVERSION;
		int wlen = 0;

		while (1) {
			if (fs.ReadString(rstr)) {
				fstr = "";
				rfstr = "";

				CStringArray strResult;

				CString strGap = _T("\x09");
				int nPos = rstr.Find(strGap);

				CString strLeft = _T("");
				int cnt = 0;
				while (0 <= nPos)
				{
					strLeft = rstr.Left(nPos);
					if (!strLeft.IsEmpty())
						strResult.Add(strLeft);
					rstr = rstr.Right(rstr.GetLength() - nPos - 1);
					nPos = rstr.Find(strGap);
					if (nPos == -1) {
						strResult.Add(rstr);
						break;
					}
				}

				cnt = (int)strResult.GetSize();
				if (cnt == 2) {// got gap "\x09"
					fstr = strResult.GetAt(0);
					rfstr = strResult.GetAt(1);
					if (rfstr != fstr) {

						BOOL FLAG = PathFileExists(fstr);
						if (!FLAG)
						{
							//AfxMessageBox(L"不存在该文件:"+fstr);							
						}
						else {
							CFile::Rename(fstr, rfstr);// there is no return value to know wether fail or success
							//USES_CONVERSION;
							char *cp = T2A(rcstr + fstr + _T("\x09") + rfstr + _T("\r\n"));
							CString ts = (rcstr + fstr + _T("\x09") + rfstr + _T("\r\n"));
							wlen = ts.GetLength();
							wlen =  CStringA(rcstr + fstr + (CString)_T("\x09") + rfstr + (CString)_T("\r\n")).GetLength();
							// read the same size of writing data to a buf,and after finish new writeing ,then write the buf data to file
							/*
							if (strlen(cp) < 512) {
								memset(rdbuf, 0, sizeof(rdbuf));
								rdlen = (UINT)strlen(cp);
								//rdlen = 0;
								rdlen = fw.Read(rdbuf, rdlen);
								rdlen = (UINT)strlen(rdbuf);
								if (rdlen > 0) {
									cstrArdbuf.Add(A2T(rdbuf));
									fw.Seek(fw.GetPosition() - rdlen, CFile::begin);
								}
							}*/
							//fw.Write(cp, (UINT)strlen(cp));
							fw.Write(cp, wlen);
							rcstr = "";
						}
					}
				}
				else { // not detected
				}
			}
			else {
				break;
			}
		}


		if (rcstr.GetAllocLength() == 0) {
			//USES_CONVERSION;
			char *cp = T2A(cstrChangeend + _T("\r\n"));
			wlen = CStringA(cstrChangeend + _T("\r\n")).GetLength();
			/*
			if (strlen(cp) < 512) {
				memset(rdbuf, 0, sizeof(rdbuf));
				rdlen = 0;
				rdlen = fw.Read(rdbuf, (UINT)strlen(cp));
				if (rdlen > 0) {
					cstrArdbuf.Add(A2T(rdbuf));
					fw.Seek(fw.GetPosition() - rdlen, CFile::begin);
				}
			}*/
			//fw.Write(cp, (UINT)strlen(cp));
			fw.Write(cp, wlen);
		}
		/*
		if (cstrArdbuf.GetSize()) {
			// loop read and write
			for (int i = 0; i < cstrArdbuf.GetSize(); i++) {
				fstr = cstrArdbuf.GetAt(i);
				USES_CONVERSION;
				char *cp = T2A(fstr);
				// read the same size of writing data to a buf,and after finish new writeing ,then write the buf data to file

				if (strlen(cp) < 512) {
					memset(rdbuf, 0, sizeof(rdbuf));
					rdlen = 0;
					rdlen = fw.Read(rdbuf, (UINT)strlen(cp));
					if (rdlen > 0) {//Return Value ,The number of bytes transferred to the buffer.
						cstrArdbuf.Add(A2T(rdbuf));
						fw.Seek(fw.GetPosition() - rdlen, CFile::begin);
					}
				}
				fw.Write(cp, (UINT)strlen(cp));
				memcpy(trdbuf, rdbuf, sizeof(rdbuf));
				//fflush(fw.h);
			}
		}*/
		fw.Write(p, (UINT)strlen(p));
		fw.Close();
	}
	fs.Close();
	setlocale(LC_CTYPE, old_locale); //还原语言区域的设置 
	free(old_locale);//还原区域设定	

					 /*
					 if (fdelete) {
					 BOOL FLAG = PathFileExists(cstrChFileList);
					 if (FLAG)
					 CFile::Remove(cstrChFileList);
					 }
					 */

	return 0;
}


int CMFCApplication1Dlg::unDoChangeFileName()
{
	CStdioFile fs;// chang list file write
	CFileException e;
	int rflg = 0;
	LONGLONG wpos = 0;
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	if (!fs.Open(cstrChangeFile, CFile::modeReadWrite, &e)) {// open changelist file
		TRACE(_T("open change list file  failed\r\n"));
		return 0;
	}
	else {
		CString rstr;
		CString rcstr;

		CString fstr, rfstr;

		while (1) {
			if (!rflg) {
				wpos = fs.GetPosition();
			}
			if (fs.ReadString(rstr)) {
				fstr = "";
				rfstr = "";

				CStringArray strResult;

				CString strGap = _T("\x09");
				int nPos = rstr.Find(strGap);

				if (nPos == -1) {
					// is start or end?
					if (rstr.Find(cstrChangestart) == 0) {
						//at start
						rflg = 1;
					}
					if (rstr.Find(cstrChangeend) == 0) {
						//at end
						if(rflg)
							break;
					}
				}
				if (!rflg) {
					continue;
				}
				CString strLeft = _T("");
				int cnt = 0;
				while (0 <= nPos)
				{
					strLeft = rstr.Left(nPos);
					if (!strLeft.IsEmpty())
						strResult.Add(strLeft);
					rstr = rstr.Right(rstr.GetLength() - nPos - 1);
					nPos = rstr.Find(strGap);
					if (nPos == -1) {
						strResult.Add(rstr);
						break;
					}
				}

				cnt = (int)strResult.GetSize();
				if (cnt == 2) {// got gap "\x09"
					rfstr = strResult.GetAt(0);
					fstr = strResult.GetAt(1);
					if (rfstr != fstr) {

						BOOL FLAG = PathFileExists(fstr);
						if (FLAG){
							CFile::Rename(fstr, rfstr);// there is no return value to know wether fail or success
						}
					}
				}
				else { // not detected
				}
			}
			else {
				break;
			}
		}
	}
	fs.Seek(wpos,CFile::begin);
	if (rflg) {
		SYSTEMTIME st;
		CString strDate;
		GetLocalTime(&st);
		strDate.Format(_T("%s m:%04d-%02d-%02d %02d:%02d:%02d")
			, cstrChangeUndo.GetBuffer(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		USES_CONVERSION;
		char *cp = T2A(strDate);
		// read the same size of writing data to a buf,and after finish new writeing ,then write the buf data to file
		fs.Write(cp, (UINT)strlen(cp));
	}
	fs.Close();
	setlocale(LC_CTYPE, old_locale); //还原语言区域的设置 
	free(old_locale);//还原区域设定	

	return 0;
}


void CMFCApplication1Dlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	unDoChangeFileName();
}


int CMFCApplication1Dlg::creatIdentifyProcess()
{
	CString str;
	


	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->GetWindowText(str);

	if (!my_CreateProcess(cstrSearchexeName)) {

		CloseHandle(m_hPipeWriteParentDetect);	//创建进程失败，关闭读写句柄
		CloseHandle(m_hPipeReadParentDetect);
		CloseHandle(m_hPipeWriteChildDetect);	//创建进程失败，关闭读写句柄
		CloseHandle(m_hPipeReadChildDetect);
		
		str = _T("CREATE PROCESS FAILED, restar search again\r\n");
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return 0;
	}

	str += _T("\r\n start prcess search.exe \r\n"); 
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SetWindowText(str);
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
	return 0;
}


int CMFCApplication1Dlg::createDetectProcess()
{
	CString str;

	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->GetWindowText(str);

	//ColseChildProcess();
	if (!my_CreateProcess(cstrDarkexeName)) {

		CloseHandle(m_hPipeWriteParentDetect);	//创建进程失败，关闭读写句柄
		CloseHandle(m_hPipeReadParentDetect);
		CloseHandle(m_hPipeWriteChildDetect);	//创建进程失败，关闭读写句柄
		CloseHandle(m_hPipeReadChildDetect);

		str = _T("CREATE PROCESS FAILED, restar darknet_no_gpu again\r\n");
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return 0;
	}
	str += _T("\r\n start process darknet_no_gpu.exe \r\n");
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SetWindowText(str);
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
	
	return 0;
}


void CMFCApplication1Dlg::getGalleryFileList(CString cstrGFile,CString cstrGDir)
{
	CFile fileList;
	if (!fileList.Open(cstrGFile, CFile::modeCreate | CFile::modeWrite))// create a empty file Filelist.txt
	{
		TRACE(_T("File could not be opened\n"));
	}

	if (cstrGDir == _T("")) {
		cstrGDir = _T("gallery");
	}
	TravelFolder(cstrGDir, &fileList);

	fileList.Close();
}


void CMFCApplication1Dlg::doListToChange(CString cstrListin, CString cstrListout)
{
	// TODO: 在此添加控件通知处理程序代码
	// create cstrIListFile,modify tempChangFile according to the cstrDetctedFile
	// rename
	CString rstr;
	CStdioFile fs;
	CFileException e;
	//int pos = 0;

	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

//	if (!fs.Open(cstrDetctedFile, CFile::modeRead, &e)) {
	if (!fs.Open(cstrListin, CFile::modeRead, &e)) {
		return ;
	}
	else {
		int maxcnt = M_MAXCNT;
		CString fstr, rfstr;
		CFile fw;
		CFile cfw;
		CString tempChangFile;
		clock_t rand;
		rand = clock();
		tempChangFile.Format(_T("CF%ld.txt"), rand);
		
		if (cstrListout != "") {
			if (!fw.Open(cstrListout, CFile::modeCreate | CFile::modeWrite, &e)) {// creat a empty file to write,"IListfile.txt"
				TRACE(_T("open fw failed\r\n"));
				return;
			}
		}
		if (!cfw.Open(tempChangFile, CFile::modeCreate | CFile::modeWrite, &e)) {// creat a empty file to write,"IListfile.txt"
			TRACE(_T("open cfw failed\r\n"));
			return;
		}

		while (1) {
			if (fs.ReadString(rstr)) {
				fstr = "";
				rfstr = "";

				CStringArray strResult;

				CString strGap = _T("\x09");
				int nPos = rstr.Find(strGap);

				CString strLeft = _T("");
				int cnt = 0;
				while (0 <= nPos)
				{
					strLeft = rstr.Left(nPos);
					if (!strLeft.IsEmpty())
						strResult.Add(strLeft);

					rstr = rstr.Right(rstr.GetLength() - nPos - 1);
					nPos = rstr.Find(strGap);
					if (nPos == -1) {
						strResult.Add(rstr);
						break;
					}
					//cnt++;
				}

				cnt = (int)strResult.GetSize();
				if (cnt) {// got gap "\x09"
					CString suffix = _T("");
					int pcnt = 0;

					fstr = strResult.GetAt(0);
					rfstr = strResult.GetAt(0);
					//strGap = _T("11");
					int pos = rfstr.ReverseFind(_T('.'));// TCHAR	
					if (pos > 0) { // pcnt must > 0 not = 0 or -1
						suffix = rfstr.Right(rfstr.GetLength() - pos);// ".xxx"
						rfstr = rfstr.Left(pos);
					}
					//pcnt = 0;

					for (int i = 1; i < cnt; i++) {
						// delete number and keep only one identical item
						CString itemp = _T("");
						itemp = strResult.GetAt(i);
						nPos = itemp.Find(_T("-"));

						if (nPos > 0) {
							itemp = itemp.Left(nPos);
							nPos = rfstr.Find(_T("_")+itemp);
							if (nPos == -1) {// there is no itmp in rfstr
								rfstr += _T("_") + itemp;
							}
							else {
								pcnt++;
							}
						}
					}
					rfstr += suffix;
					if (rfstr != fstr) {
						USES_CONVERSION;
						char* cp = T2A(rfstr + _T("\r\n"));

						if (rfstr.Find(_T("_person")) > 0) {
							if (cstrListout != "") {
								fw.Write(cp, (UINT)strlen(cp));
							}
						}
						cp = T2A(fstr + _T("\x09") + rfstr + _T("\r\n"));
						cfw.Write(cp, (UINT)strlen(cp));
						//CFile::Rename(strResult.GetAt(0), rfstr);
					}
					else {
					}
				}
				else { // not detected
				}
			}
			else {
				if (cstrListout != "") {
					fw.Close();
				}
				cfw.Close();
				DoChangeFileName(tempChangFile, 1);
				CFile::Remove(tempChangFile);
				break;
			}
		}
	}
	fs.Close();
	setlocale(LC_CTYPE, old_locale); //还原语言区域的设置 
	free(old_locale);//还原区域设定

}


void CMFCApplication1Dlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	doListToChange(cstrIdentifiedFile, NULL);
}


void CMFCApplication1Dlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	autoFlg = 1;
	OnBnClickedButton1();
	OnBnClickedButtonDetect();
	//OnBnClickedDetectedFile();
	//OnBnClickedButtonIdentify();
	//OnBnClickedButton9();
}


int CMFCApplication1Dlg::cuntFiles(CString cstrFile)
{
	CString rstr;
	CStdioFile fs;
	CFileException e;
	int cnt;

	if (!fs.Open(cstrFile, CFile::modeRead, &e)) {
		return 0;
	}
	else {
		cnt = 0;
		while (1) {
			if (fs.ReadString(rstr)) {
				if (rstr != _T(""))
					cnt++;
			}
			else break;
		}
	}
	fs.Close();
	return cnt;
}


char * CMFCApplication1Dlg::myT2A(CString cstrT2A)
{
	return nullptr;
}


void CMFCApplication1Dlg::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	mEdit1 = _T("");
	UpdateData(false);
}

int CMFCApplication1Dlg::getpid(CString pname)
{
	PROCESSENTRY32 pe;
	int pid = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	TCHAR *ch;// = new TCHAR[MAX_PATH];
	//memset(ch, 0, sizeof(ch));
	ch = pname.GetBuffer(pname.GetLength());
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	if (!::Process32First(hSnapshot, &pe))
		return 0;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if ((_tcscmp(ch, pe.szExeFile)) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
		if (::Process32Next(hSnapshot, &pe) == FALSE)
			break;
	}
	//CloseHandle(hSnapshot);
	//delete []ch;//delete(ch);
	CloseHandle(hSnapshot);
	return pid;
}

//close V3Monitor.exe  
void CMFCApplication1Dlg::closepid(int pid)
{
	HANDLE hProcess;
	DWORD dwPriorityClass;
	//打开进程句柄  
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) return;
	dwPriorityClass = GetPriorityClass(hProcess);
	if (dwPriorityClass == 0) return;
	if (!TerminateProcess(hProcess, 1)) return;
	CloseHandle(hProcess);
	return;
}

void CMFCApplication1Dlg::dopreexit()
{
	m_threadrun = 0;
	ColseChildProcess();
	TerminateThread(m_hThread, 0);

}

UINT kmsServerThread(LPVOID pParam)
{
	CMFCApplication1Dlg *pthis = (CMFCApplication1Dlg *)pParam;
	// 创建socket server ,port 50053
	CSocket aSocket, serverSocket;
	//最好不要使用aSocket.Create创建，因为容易会出现10048错误
	if (!aSocket.Socket())
	{
		char szError[256] = { 0 };

		//sprintf(szError, _T("Create Faild:?%d"), GetLastError());

		//AfxMessageBox(szError);

		return 1;
	}

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);

	//设置Socket的选项, 解决10048错误必须的步骤
	aSocket.SetSockOpt(SO_REUSEADDR, (void *)&bOptVal, bOptLen, SOL_SOCKET);
	//绑定端口
	if (!aSocket.Bind(0))
	{
		char szError[256] = { 0 };
		//sprintf(szError, "Bind Faild: %d", GetLastError());
		return 1;
	}
	//监听
	if (!aSocket.Listen(10))
	{
		char szError[256] = { 0 };

		//sprintf(szError, "Listen?Faild: %d", GetLastError());

		//AfxMessageBox(szError);

		return 1;
	}

	CString strText;
	strText += "Server Start!";
	while (1)
	{
		//接收外部连接
		if (!aSocket.Accept(serverSocket))
		{
			continue;
		}
		else
		{
			char szRecvMsg[256] = { 0 };
			char szOutMsg[256] = { 0 };

			//接收客户端内容:阻塞
			serverSocket.Receive(szRecvMsg, 256);
			//sprintf(szOutMsg, "Receive Msg: %s", szRecvMsg);
			strText += szOutMsg;
			//发送内容给客户端
			serverSocket.Send("Have Receive The Msg", 50);
			//关闭
			serverSocket.Close();
		}
	}

	//关闭
	aSocket.Close();
	serverSocket.Close();



	CString cstrt;
	for (int i = 0; i < 3; i++) {
		cstrt.Format(_T("%d"), i);
		AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_KMS)->SetWindowText(cstrt);

		Sleep(1000);
	}
	cstrt = _T("test end");
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_KMS)->SetWindowText(cstrt);
	AfxEndThread(0, TRUE);
	return 0;   // thread completed successfully
}



void CMFCApplication1Dlg::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	// 创建socket server 线程
	
	//AfxBeginThread(kmsServerThread,this);

	CString cstrt,cstrshow;
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();// PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);// 得到结构体大小,用于GetAdaptersInfo参数
	int netCardNum = 0;// 记录网卡数量 
	int IPnumPerNetCard = 0; // 记录每张网卡上的IP地址数量

							 //调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize]; // 重新申请内存空间用来存储所有网卡信息
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量  
	}
	cstrshow.Append(_T("GetAdaptersInfo:\r\n"));
	if (ERROR_SUCCESS == nRel)
	{
		while (pIpAdapterInfo)
		{
			cstrt.Format(_T("\r\n┌────────────Num.%d────────────┐\r\n"), ++netCardNum);
			cstrshow.Append(cstrt);cstrt.Format(_T("│Name：%s\r\n"), pIpAdapterInfo->AdapterName);
			cstrshow.Append(cstrt);cstrt.Format(_T("│Desc：%s\r\n"), pIpAdapterInfo->Description);
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：OTHER\r\n"));
				break;
			case MIB_IF_TYPE_ETHERNET:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：ETHERNET\r\n"));
				break;
			case MIB_IF_TYPE_TOKENRING:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：TOKENRING\r\n"));
				break;
			case MIB_IF_TYPE_FDDI:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：FDDI\r\n"));
				break;
			case MIB_IF_TYPE_PPP:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：PPP\r\n"));
				break;
			case MIB_IF_TYPE_LOOPBACK:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：LOOPBACK\r\n"));
				break;
			case MIB_IF_TYPE_SLIP:
				cstrshow.Append(cstrt);cstrt.Format(_T("│Type：SLIP\r\n"));
				break;
			default:
				break;
			}

			cstrshow.Append(cstrt);cstrt.Format(_T("│MAC:"));
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
			{
				if (i < pIpAdapterInfo->AddressLength - 1)
				{
					cstrshow.Append(cstrt);cstrt.Format(_T("%02X:"), pIpAdapterInfo->Address[i]);
				}
				else
				{
					cstrshow.Append(cstrt);cstrt.Format(_T("%02X\r\n"), pIpAdapterInfo->Address[i]);
				}
			}
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				cstrshow.Append(cstrt);cstrt.Format(_T("│IP：%s\r\n"), pIpAddrString->IpAddress.String);
				cstrshow.Append(cstrt);cstrt.Format(_T("│MASK：%s\r\n"), pIpAddrString->IpMask.String);
				cstrshow.Append(cstrt);cstrt.Format(_T("│Gate：%s\r\n"), pIpAdapterInfo->GatewayList.IpAddress.String);
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
			cstrshow.Append(cstrt);cstrt.Format(_T("└──────────────────────────┘\r\n"));
			cstrshow.Append(cstrt);
		}

	}
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->SetWindowText(cstrshow);
	
}
