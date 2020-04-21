#pragma once

// CFolderDialog dialog
class CFolderDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CFolderDialog)

public:
	CFolderDialog(CString* pPath, CString dPath, CWnd* pParentWnd = NULL);
	static WNDPROC m_wndProc;
	CString* m_pPath;

protected:
	DECLARE_MESSAGE_MAP()

private:
	virtual void OnInitDone();
	virtual void OnFileNameChange();
	virtual void OnFolderChange();
	virtual void OnLBSelChangedNotify();
	void ChangeFolder();
};