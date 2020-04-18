
// MFCApplication1Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#define MDEBUG 0




// CMFCApplication1Dlg 对话框
class CMFCApplication1Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// 标准构造函数
//	virtual ~CMFCApplication1Dlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
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
	afx_msg void OnBnClickedButton1();
	void TravelFolder(CString strDir,CFile *fp);
	CButton m_button1;
	CString strAllFile;
//	int cntFile;
//	long double ldFile;
	long double ldPathCnt;
	long double ldFileCnt;
	CString charToHexCString(BYTE *ca, int len);

	HANDLE m_hPipeWriteParentDetect;		//管道写数据句柄
	HANDLE m_hPipeReadParentDetect;			//管道读数据句柄
	HANDLE m_hPipeWriteChildDetect;		//管道写数据句柄
	HANDLE m_hPipeReadChildDetect;			//管道读数据句柄

	void my_PipeInit();
	bool my_CreateProcess(CString epath);						//创建进程
	bool my_CreatePipe();							//创建管道
	void my_SendDataPipe(CString data);			//通过管道发送数据
	void SetStartInfo(STARTUPINFO& si);				//初始化启动信息
	void SetSecurity_attr(SECURITY_ATTRIBUTES& ps);	//初始化安全属性
	static DWORD WINAPI Pipe_Listen(LPVOID lpParameter);	//接收管道数据线程
	afx_msg void OnBnClickedButtonIdentify();
	afx_msg void OnBnClickedButtonDetect();
	CString mEdit1;
	typedef struct RECVPARAM
	{
		HANDLE sock;
		CMFCApplication1Dlg *ptr;
		HANDLE hw;
	}RECVPAR;

	struct RECVPARAM pRecvParam;

	afx_msg void OnBnClickedButtonStop();


	afx_msg void OnBnClickedButtonIdentifyDefaul();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int ColseChildProcess();
	CString mEditFolderName;
	afx_msg void OnBnClickedChooseFolder();
	int cfgRead(CString fstr);
	int cfgWrite(CString fstr,CString *wstr);
	int cfgDefault(CString fstr);
	CString getWorkDir();
	#define M_MAXCNT 5;
	CString CFGStr[5];
	CString pszFileName = { _T("cfg.txt") };

	//CString cstrListFile = _T("D:/imageManage/seetaface/src/Filelist.txt");// keep all files in the setting floder
	CString cstrListFile = _T("Filelist.txt");// keep all files in the setting floder
	//CString cstrDetctedFile = _T("D:/imageManage/seetaface/src/Detctedlist.txt");// keep all files detected results
	CString cstrDetctedFile = _T("Detctedlist.txt");// keep all files detected results

	//CString cstrIListFile = _T("D:/imageManage/seetaface/src/IFilelist.txt");// files to identify
	CString cstrIListFile = _T("IFilelist.txt");// files to identify
	//CString cstrGalleryFile = _T("D:/imageManage/seetaface/src/Gallerylist.txt");// pictures for identify database
	CString cstrGalleryFile = _T("Gallerylist.txt");// pictures for identify database
	//CString cstrIdentifiedFile = _T("D:/imageManage/seetaface/src/Identifiedlist.txt");// pictures for identify results
	CString cstrIdentifiedFile = _T("Identifiedlist.txt");// pictures for identify results

	//CString cstrChangeFile = _T("D:/imageManage/seetaface/src/Changelist.txt");// file names which has been changed
	CString cstrChangeFile = _T("Changelist.txt");// file names which has been changed
	CString cstrChangestart = _T("<ChanGeLisT StaRt>");
	CString cstrChangeUndo = _T("<ChanGeLisT Undo >");
	CString cstrChangeend = _T("<ChanGeLisT eNd>");

	CString cstrDarkexeName = _T("darknet_no_gpu.exe");
	CString cstrSearchexeName = _T("search.exe");

	char pipeflg = 0;
	CString workdir;

	afx_msg void OnBnClickedDetectedFile();
	int DoChangeFileName(CString cstrChFileList,int fdelete);
	int unDoChangeFileName();
	afx_msg void OnBnClickedButton8();
	int creatIdentifyProcess();
	int createDetectProcess();

	char proFlg = 0; // process running flag
	void getGalleryFileList(CString cstrGFile, CString cstrGDir);
	void doListToChange(CString cstrList, CString cstrListout);
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	char autoFlg = 0;
	CString cstrStatichead = _T("");
	int fileCount = 0;
	int fileGrow = 0;
	int cuntFiles(CString cstrFile);
	clock_t clockStart;
	clock_t clockCurrent;

	char * CMFCApplication1Dlg::myT2A(CString cstrT2A);
	int CMFCApplication1Dlg::DoChangeFileNamebak(CString cstrChFileList, int fdelete);
};

