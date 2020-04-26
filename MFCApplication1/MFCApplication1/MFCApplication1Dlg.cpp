
// MFCApplication1Dlg.cpp : ʵ���ļ�
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


#include "stdafx.h"
#include <Windows.h>
#include "folder_dialog.h"

using namespace std;
//using namespace BaseFunc;

#pragma comment(lib,"Shlwapi.lib") 
#pragma comment(lib, "cryptlib.lib")
using namespace CryptoPP;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMFCApplication1Dlg �Ի���
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
END_MESSAGE_MAP()


// CMFCApplication1Dlg ��Ϣ��������

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	// ��ʼ�������� pipe
	my_PipeInit();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication1Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	// �ļ���ǰĿ¼
	TCHAR Buffer[MAX_PATH];
	DWORD dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
	CString strCurrent(Buffer);
	CString strtime, strdir;

	CString mstr;

	//��ȡϵͳʱ��
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
		MessageBox(_T("�����ļ�����"), NULL, MB_OK);
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
	CFileFind filefind;                                         //����CFileFind���ͱ���
	CFile file;
	Weak::MD5 hash;
	CString strWildpath = strDir + _T("/*.*");     //����jpg�ļ����г���

	if (filefind.FindFile(strWildpath, 0))                    //��ʼ�����ļ�
	{
		BOOL bRet = TRUE;

		while (bRet)
		{
			bRet = filefind.FindNextFile();                 //ö��һ���ļ�            

			if (filefind.IsDots())                                 //�����. �� .. ����һ��
			{
				continue;
			}

			// �ļ��� begin
			CString strFileName = filefind.GetFileName();
			CString strLastName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1);
			// �ļ��� end

			// �ļ��޸�ʱ�� begin
			FILETIME   filetime;
			FILETIME   localtime;
			SYSTEMTIME systemtime;
			
			filefind.GetLastWriteTime(&filetime);

			FileTimeToLocalFileTime(&filetime, &localtime); //���ɱ���ʱ��

			FileTimeToSystemTime(&localtime, &systemtime);  //����ϵͳʱ���ʽ

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

			// �ļ��޸�ʱ�� end            
			if (!filefind.IsDirectory())                          //������Ŀ¼�����ļ�����ӡ����
			{
				//filter: jpg only;
				CString strLastName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1).MakeLower();
				if (strLastName.Find(_T("jpg")) == -1)
					continue;
				/*
				int haslen = hash.DigestSize();
				memset(buf, 0, sizeof(buf));//��ʼ���ڴ棬��ֹ�����ַ�ĩβ��������
				memset(digest, 0, sizeof(digest));//��ʼ���ڴ棬��ֹ�����ַ�ĩβ��������
				try
				{
					if (!file.Open(strFilePath, CFile::modeRead))
					{
						CString str; 
						str.Format(_T("û���ļ�! %s\n"), strFilePath);
						TRACE(str);
						return;
					}
					
					//�ṹ���ʽ��ȡ
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
					str.Format(_T("��ȡ����ʧ�ܵ�ԭ����:%d ,%s"), e->m_cause, strFilePath);
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
			else                                                   //�������Ŀ¼���ݹ���øú���
			{
				CString strNewDir = strDir + CString(_T("/")) + filefind.GetFileName();
				ldPathCnt++;

				TravelFolder(strNewDir,fp);//�ݹ���øú�����ӡ��Ŀ¼����ļ�
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
		CloseHandle(m_hPipeWriteParentDetect);	//��������ʧ�ܣ��رն�д���
		CloseHandle(m_hPipeReadParentDetect);
		CloseHandle(m_hPipeWriteChildDetect);	//��������ʧ�ܣ��رն�д���
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
	m_hThread = CreateThread(NULL, 0, Pipe_Listen, (LPVOID)&pRecvParam, 0, NULL);	//����socket�ķ����߳�
}

void CMFCApplication1Dlg::SetSecurity_attr(SECURITY_ATTRIBUTES& ps)
{
	//������뽫 bInheritHandle ����Ϊ TRUE��
	//�Ӷ�ʹ���ӽ��̿��Լ̳и����̴����������ܵ��ľ��
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
									  //�ӽ��̵ı�׼������Ϊ�����̹ܵ��Ķ����ݾ��
	si.hStdInput = m_hPipeReadChildDetect;
	//�ӽ��̵ı�׼������Ϊ�����̹ܵ���д���ݾ��
	si.hStdOutput = m_hPipeWriteParentDetect;
	//�ӽ��̵ı�׼����������͸����̵ı�׼���������һ��
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

	//д������
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
	//���ڽṹ��pi�е������Ѿ�ʹ�ò����ˣ�������Դ�ͷ�
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
		int unicodefilelen = MultiByteToWideChar(CP_ACP, 0, data, (UINT)strlen(data), NULL, 0);//��ansi�ļ�����תΪUnicode�ļ�����
		MultiByteToWideChar(CP_ACP, 0, data, (UINT)strlen(data), wp, (int)unicodefilelen);//�Ѷ������ļ�ת�ɿ��ַ�
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

			cstrProgress.Format(_T("%s  %d/%d ��%02d:%02d:%02d�� %s")
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CString str;
	cstrStatichead = _T(" �沿ʶ��: ");

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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CString str;
	cstrStatichead = _T("Ŀ����: ");

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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CString str;
	str = "QUIT";
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
}


void CMFCApplication1Dlg::OnBnClickedButtonIdentifyDefaul()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CString str;
	str = "test";
	my_SendDataPipe(str);
	UpdateData(true);
	mEdit1 += _T("P[") + str + _T("]\r\n");
	UpdateData(false);
}

void CMFCApplication1Dlg::OnClose()
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	//ColseChildProcess();	
	dopreexit();
	CDialogEx::OnClose();
}


void CMFCApplication1Dlg::OnBnClickedOk()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	//ColseChildProcess();	
	dopreexit();
	CDialogEx::OnOK();
}


void CMFCApplication1Dlg::OnBnClickedCancel()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	CString strDir = _T("D:\\imageManage");//����ͨ��strFile����Ŀ¼,CFileDialog���Զ���ס
	CString filename = _T("hi.txt");  //ͨ��strFile�����ļ���
	CString filter = strExt + _T("�ļ� (*.") + strExt + _T(")|*.") + strExt + _T("||");
	CString ext = _T(".") + strExt;


	CFileDialog dlg(((bool)bOpen, _T("*..*"), filename, OFN_READONLY | OFN_OVERWRITEPROMPT, filter, NULL));
	
	dlg.GetOFN().lpstrInitialDir = strFile;// Ĭ��Ŀ¼

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
		AfxMessageBox(_T("��Ч��Ŀ¼��������ѡ��"));
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
	setlocale(LC_CTYPE, old_locale); //��ԭ������������� 
	free(old_locale);//��ԭ�����趨

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
							//AfxMessageBox(L"�����ڸ��ļ�:"+fstr);							
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
	setlocale(LC_CTYPE, old_locale); //��ԭ������������� 
	free(old_locale);//��ԭ�����趨	

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
		LONGLONG ansifilelen = fw.GetLength();//���ansi�����ļ����ļ�����
		char *p = new char[ansifilelen + 1];//newһ�����ڴ�
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
							//AfxMessageBox(L"�����ڸ��ļ�:"+fstr);							
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
	setlocale(LC_CTYPE, old_locale); //��ԭ������������� 
	free(old_locale);//��ԭ�����趨	

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
	setlocale(LC_CTYPE, old_locale); //��ԭ������������� 
	free(old_locale);//��ԭ�����趨	

	return 0;
}


void CMFCApplication1Dlg::OnBnClickedButton8()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	unDoChangeFileName();
}


int CMFCApplication1Dlg::creatIdentifyProcess()
{
	CString str;
	


	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1)->GetWindowText(str);

	if (!my_CreateProcess(cstrSearchexeName)) {

		CloseHandle(m_hPipeWriteParentDetect);	//��������ʧ�ܣ��رն�д���
		CloseHandle(m_hPipeReadParentDetect);
		CloseHandle(m_hPipeWriteChildDetect);	//��������ʧ�ܣ��رն�д���
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

		CloseHandle(m_hPipeWriteParentDetect);	//��������ʧ�ܣ��رն�д���
		CloseHandle(m_hPipeReadParentDetect);
		CloseHandle(m_hPipeWriteChildDetect);	//��������ʧ�ܣ��رն�д���
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	setlocale(LC_CTYPE, old_locale); //��ԭ������������� 
	free(old_locale);//��ԭ�����趨

}


void CMFCApplication1Dlg::OnBnClickedButton9()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	doListToChange(cstrIdentifiedFile, NULL);
}


void CMFCApplication1Dlg::OnBnClickedButton10()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	//�򿪽��̾��  
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