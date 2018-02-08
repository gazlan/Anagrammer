/* ******************************************************************** **
** @@ AnagrammerDlg
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Dscr   :
** ******************************************************************** */

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#if !defined(AFX_LISTERDLG_H__99CF9EC5_1FD8_43B2_BE07_6F48FFD582E5__INCLUDED_)
#define AFX_LISTERDLG_H__99CF9EC5_1FD8_43B2_BE07_6F48FFD582E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   Classes
** ******************************************************************** */

class CAnagrammerDlg : public CDialog
{
   private:

      MMF                  _Src;
      MMF                  _Src2;
      BPPTreeIndex         _Index;
      SortedVector         _List;

   private:

      void     ListCleanup();
      void     Cleanup();
      int      GetWord();
      void     LoadMap();
      void     Calc(HANDLE hDB,HANDLE hDB2);
      void     Report(const char* const pszIn,const char* const pszIn2,FILE* pOut);

// Construction
public:
   CAnagrammerDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
   //{{AFX_DATA(CAnagrammerDlg)
   enum { IDD = IDD_LISTER };
   CProgressCtrl     m_Progress;
   CSpinButtonCtrl   m_SpMax;
   CSpinButtonCtrl   m_SpMin;
   UINT     m_Min;
   UINT     m_Max;
   CString  m_Src;
   CString  m_Out;
   CString  m_Map;
   //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CAnagrammerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   //{{AFX_MSG(CAnagrammerDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   virtual void OnCancel();
   afx_msg void OnBtnSrc();
   afx_msg void OnBtnOut();
   afx_msg void OnBtnMap();
   afx_msg void OnBtnGo();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTERDLG_H__99CF9EC5_1FD8_43B2_BE07_6F48FFD582E5__INCLUDED_)

/* ******************************************************************** **
**                End of File
** ******************************************************************** */
