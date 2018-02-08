/* ******************************************************************** **
** @@ Anagrammer
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include <afxrich.h>     
#include <locale.h>
                 
#include "..\shared\mmf.h"
#include "..\shared\file.h"
#include "..\shared\map_bppt_jannink.h"
#include "..\shared\vector.h"
#include "..\shared\vector_sorted.h"

#include "Anagrammer.h"
#include "AnagrammerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#define ASCII_SIZE                                 (256)
#define MAX_WORD_SIZE                              (255)

#define WORDLIST_DEFAULT_SIZE                      (32)
#define WORDLIST_DEFAULT_SIZE_DEFAULT_DELTA        (8)

struct DB_RECORD
{
   BYTE        _bySize;
   char        _pszWord[1];
};

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD   dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

static char    _pszWord[MAX_PATH];
static char    _pXLAT  [MAX_PATH];

static DWORD   _dwPos   = 0;
static DWORD   _dwKeep  = 0;
static BYTE*   _pText   = NULL;
static DWORD   _dwSize  = 0;
static DWORD   _dwDelta = 0;

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ inline int CMP_Counters()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

static int __cdecl CMP_Names(const void** const pKey1,const void** const pKey2)
{
   char*    p1 = *(char**)pKey1;
   char*    p2 = *(char**)pKey2;

   setlocale( LC_ALL,".1251" );
   
   return stricmp(strupr(p1),strupr(p2));
}

/* ******************************************************************** **
** @@ inline int CMP_Counters()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

inline int CMP_Counters(const void* const pKey1,const void* const pKey2)
{
   BYTE*    p1 = (BYTE*)pKey1;
   BYTE*    p2 = (BYTE*)pKey2;

   for (int ii = 0; ii < ASCII_SIZE; ++ii)
   {
      if (p1[ii] > p2[ii])
      {
         return 1;
      }
      else if (p1[ii] < p2[ii])
      {
         return -1;
      }
      else
      {
         ;  // Do Nothing, Continue comparision 
      }
   }

   return 0;
}

/* ******************************************************************** **
** @@ inline void CalcMetrics()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

inline void CalcMetrics(BYTE* pWord,int dwLength,BYTE* pMetrics)
{
   for (int ii = 0; ii < dwLength; ++ii) 
   {
      ++pMetrics[pWord[ii]];
   }
}

/* ******************************************************************** **
** @@ inline void Dix_Sorter()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

static int __cdecl Dix_Sorter(const void* pKey1,const void* pKey2)
{
   DWORD*      p1 = (DWORD*)pKey1;   
   DWORD*      p2 = (DWORD*)pKey2;   

   if (p1[0] > p2[0])
   {
      return 1;
   }
   else if (p1[0] < p2[0])
   {
      return -1;
   }
   else
   {
      return 0;
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::CAnagrammerDlg()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CAnagrammerDlg::CAnagrammerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CAnagrammerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CAnagrammerDlg)
   m_Min = 2;
   m_Max = 255;
   m_Src  = _T("");
   m_Map  = _T("");
   m_Out  = _T("");
   //}}AFX_DATA_INIT

   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::DoDataExchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAnagrammerDlg)
   DDX_Control(pDX, IDC_PROGRESS, m_Progress);
   DDX_Control(pDX, IDC_SPN_MAX, m_SpMax);
   DDX_Control(pDX, IDC_SPN_MIN, m_SpMin);
   DDX_Text(pDX, IDC_EDT_SIZE_MIN, m_Min);
   DDV_MinMaxUInt(pDX, m_Min, 2, 255);
   DDX_Text(pDX, IDC_EDT_SIZE_MAX, m_Max);
   DDV_MinMaxUInt(pDX, m_Max, 2, 255);
   DDX_Text(pDX, IDC_EDT_SRC, m_Src);
   DDV_MaxChars(pDX, m_Src, 260);
   DDX_Text(pDX, IDC_EDT_OUT, m_Out);
   DDV_MaxChars(pDX, m_Out, 260);
   DDX_Text(pDX, IDC_EDT_MAP, m_Map);
   DDV_MaxChars(pDX, m_Map, 255);
   //}}AFX_DATA_MAP
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CAnagrammerDlg, CDialog)
   //{{AFX_MSG_MAP(CAnagrammerDlg)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BTN_SRC, OnBtnSrc)
   ON_BN_CLICKED(IDC_BTN_OUT, OnBtnOut)
   ON_BN_CLICKED(IDC_BTN_MAP, OnBtnMap)
   ON_BN_CLICKED(IDC_BTN_GO, OnBtnGo)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnInitDialog()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BOOL CAnagrammerDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon,TRUE);     // Set big icon
   SetIcon(m_hIcon,FALSE);    // Set small icon

   m_SpMin.SetRange(2,255);
   m_SpMax.SetRange(2,255);

   m_SpMin.SetPos(2);
   m_SpMax.SetPos(255);

   GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
   GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);

   _List.Resize(WORDLIST_DEFAULT_SIZE);
   _List.Delta(WORDLIST_DEFAULT_SIZE_DEFAULT_DELTA);

   _List.SetSorter(CMP_Names);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnPaint()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CAnagrammerDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnQueryDragIcon()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

// The system calls this to obtain the cursor to display while the user drags
// the minimized window.

HCURSOR CAnagrammerDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::GetWord()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int CAnagrammerDlg::GetWord()
{
   memset(_pszWord,0,sizeof(_pszWord));

   if (!_Src.IsOpen())
   {
      return 0;
   }

   if (_dwPos >= (_dwSize - 1))
   {
      return 0;
   }

   // Skip
   while (!_pXLAT[_pText[_dwPos]])
   {
      ++_dwPos;

      if ((_dwPos - _dwKeep) > _dwDelta)
      {
         m_Progress.SetPos(_dwPos);
         _dwKeep = _dwPos;
      }

      if (_dwPos >= (_dwSize - 1))
      {
         return 0;
      }
   }

   int      iSize = 0;

   DWORD    dwStart = _dwPos;

   // Collect
   while (_pXLAT[_pText[_dwPos]])
   {
      ++_dwPos;
      ++iSize;

      if ((_dwPos - _dwKeep) > _dwDelta)
      {
         m_Progress.SetPos(_dwPos);
         _dwKeep = _dwPos;
      }

      if ((_dwPos >= (_dwSize - 1)) || (iSize > MAX_WORD_SIZE))
      {
         break;
      }
   }

   memcpy(_pszWord,&_pText[dwStart],iSize);

   return iSize;
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnCancel()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::OnCancel()
{
   ListCleanup();
   Cleanup();

   CDialog::OnCancel();
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::Cleanup()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::Cleanup()
{
   _Src.Close();
   _Src2.Close();
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::ListCleanup()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::ListCleanup()
{
   DWORD    dwCnt = _List.Count();

   if (dwCnt)
   {
      // Should be int !
      for (int ii = (dwCnt - 1); ii >= 0; --ii)
      {
         char*     pEntry = (char*)_List.At(ii);

         _List.RemoveAt(ii);

         if (pEntry)
         {
            delete[] pEntry;
            pEntry = NULL;
         }
      }
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnBtnSrc()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::OnBtnSrc()
{
   DWORD    dwFlags =   OFN_ENABLESIZING     |
                        OFN_FILEMUSTEXIST    |
                        OFN_PATHMUSTEXIST;
                                    
   char     pszFilter[MAX_PATH] = "TXT  (*.txt)|*.txt|"
                                  "DLL  (*.dll)|*.dll|"
                                  "EML  (*.eml)|*.eml|"
                                  "EXE  (*.exe)|*.exe|"
                                  "FB2  (*.fb2)|*.fb2|"
                                  "HTML  (*.htm*)|*.htm*|"
                                  "LIB  (*.lib)|*.lib|"
                                  "ALL  (*.*)|*.*||";

   CFileDialog    FileSrc(TRUE,NULL,NULL,dwFlags,pszFilter);
   
   if (FileSrc.DoModal() == IDOK)
   {  
      m_Src = FileSrc.GetPathName();

      SetDlgItemText(IDC_EDT_SRC,(LPCTSTR)m_Src);
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnBtnOut()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::OnBtnOut()
{
   DWORD    dwFlags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

   CFileDialog       FileOut(FALSE,"TXT","output.txt",dwFlags);

   if (FileOut.DoModal() == IDOK)
   {
      m_Out = FileOut.GetPathName(); 

      SetDlgItemText(IDC_EDT_OUT,(LPCTSTR)m_Out);
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnBtnMap()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::OnBtnMap()
{
   DWORD    dwFlags =   OFN_ENABLESIZING     |
                        OFN_FILEMUSTEXIST    |
                        OFN_PATHMUSTEXIST;
                                    
   char     pszFilter[MAX_PATH] = "XLT  (*.xlt)|*.xlt|"
                                  "ALL  (*.*)|*.*||";

   CFileDialog    FileXlt(TRUE,NULL,NULL,dwFlags,pszFilter);
   
   if (FileXlt.DoModal() == IDOK)
   {  
      m_Map = FileXlt.GetPathName();

      SetDlgItemText(IDC_EDT_MAP,(LPCTSTR)m_Map);
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::OnBtnGo()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::OnBtnGo()
{
   CWaitCursor    Waiter;

   GetDlgItem(IDC_STT_READY)->ShowWindow(FALSE);
   GetDlgItem(IDC_PROGRESS)-> ShowWindow(TRUE);

   LoadMap();

   UpdateData();

   if (_Src.IsOpen())
   {
      _Src.Close();
   }

   if (!_Src.OpenReadOnly((LPCTSTR)m_Src))
   {
      AfxMessageBox("Err: [OnBtnSrc]: Can't open Src file.",MB_OK | MB_ICONSTOP);
      GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
      GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);
      Waiter.Restore();
      return;
   }

   // Reset ALL
   _pText   = _Src.Buffer();
   _dwSize  = _Src.Size();
   _dwPos   = 0;
   _dwKeep  = 0;
   _dwDelta = (_dwSize >> 7) + 1;

   m_Progress.SetRange32(0,_dwSize);

   if (!m_Out.GetLength())
   {
      AfxMessageBox("Err: [OnBtnGo]: Gimme Out file name.",MB_OK | MB_ICONSTOP);
      GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
      GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);
      Waiter.Restore();
      return;
   }

   FILE*    pOut = fopen((LPCTSTR)m_Out,"wt");

   if (!pOut)
   {
      // Error !
      GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
      GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);
      Waiter.Restore();
      return;
   }

   char     pszTempPath[MAX_PATH + 1];

   memset(pszTempPath,0,sizeof(pszTempPath));

   // Windows NT/2000 or later: 
   // The GetTempPath function does not verify that the directory 
   // specified by the TMP or TEMP environment variables exists. 
   if (GetTempPath(MAX_PATH,pszTempPath))
   {
      EnsureDirectory(pszTempPath);
   }

   char     pszTempIdx[MAX_PATH + 1];
   char     pszTempDB1[MAX_PATH + 1];
   char     pszTempDB2[MAX_PATH + 1];

   if (!GetTempFileName(pszTempPath,"@IX",0,pszTempIdx))
   {
      // Error !
   }

   if (!GetTempFileName(pszTempPath,"@DB1",0,pszTempDB1))
   {
      // Error !
   }

   if (!GetTempFileName(pszTempPath,"@DB2",0,pszTempDB2))
   {
      // Error !
   }

   BPPT_INDEX_INFO      Info;

   memset(&Info,0,sizeof(Info));

   strcpy(Info._pszIndexName,pszTempIdx);

   Info._pCompare = CMP_Counters;

   Info._bDuplicate   = false;
   Info._bInitialized = false;  
                                    
   Info._iKeySize    = sizeof(BYTE) * ASCII_SIZE;
   Info._iSectorSize = (1 << 16);   // 64 Kb !

   if (!_Index.Open(Info))
   {
      // Error !
   }

   HANDLE   hDB1 = CreateFile(pszTempDB1);

   if (hDB1 == INVALID_HANDLE_VALUE)
   {
      // Error !
      DeleteFile(pszTempIdx);
      GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
      GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);
      Waiter.Restore();
      return;
   }
   
   HANDLE   hDB2 = CreateFile(pszTempDB2);

   if (hDB2 == INVALID_HANDLE_VALUE)
   {
      // Error !
      DeleteFile(pszTempIdx);
      GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
      GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);
      CloseHandle(hDB1);
      hDB1 = INVALID_HANDLE_VALUE;
      Waiter.Restore();
      return;
   }
   
   Calc(hDB1,hDB2);

   _Src.Close();

   CloseHandle(hDB1);
   hDB1 = INVALID_HANDLE_VALUE;

   CloseHandle(hDB2);
   hDB2 = INVALID_HANDLE_VALUE;

   _Index.Close();

   Report(pszTempDB1,pszTempDB2,pOut);

   fclose(pOut);
   pOut = NULL;

   DeleteFile(pszTempIdx);
   DeleteFile(pszTempDB1);
   DeleteFile(pszTempDB2);

   GetDlgItem(IDC_STT_READY)->ShowWindow(TRUE);
   GetDlgItem(IDC_PROGRESS)-> ShowWindow(FALSE);
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::Calc()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::Calc(HANDLE hDB1,HANDLE hDB2)
{
   DWORD       dwSerial = 0;  // Anagram serial index
   DWORD       dwLength = 0;

   while (dwLength = GetWord())
   {
      if ((dwLength >= m_Min) && (dwLength <= m_Max))
      {
         BYTE     pSignatute[ASCII_SIZE];

         memset(&pSignatute,0,sizeof(BYTE) * ASCII_SIZE);

         CalcMetrics((BYTE*)_pszWord,dwLength,pSignatute);

         DWORD    dwOfs2 = GetFileSizeLo(hDB2);

         // Insert {Key,Value} pair
         if (_Index.Insert((char*)pSignatute,dwSerial))
         {
            // DB2  //////////////////////////////////////////////////////////////////
            if (!WriteBuffer(hDB2,&dwLength,sizeof(BYTE)))
            {
               // Error !
               return;
            }

            if (!WriteBuffer(hDB2,_pszWord,dwLength + 1))
            {
               // Error !
               return;
            }

            // DB1  //////////////////////////////////////////////////////////////////
            if (!WriteBuffer(hDB1,&dwSerial,sizeof(DWORD)))
            {
               // Error !
               return;
            }

            if (!WriteBuffer(hDB1,&dwOfs2,sizeof(DWORD)))
            {
               // Error !
               return;
            }

            ++dwSerial;
         }
         else
         {
            DWORD    dwSerial = 0;

            char     pszStr[MAX_PATH + 1];

            memset(pszStr,0,sizeof(pszStr));

            if (!_Index.Find((char*)pSignatute,&dwSerial))
            {
               // Error !
               return;
            }

            // DB2  //////////////////////////////////////////////////////////////////
            if (!WriteBuffer(hDB2,&dwLength,sizeof(BYTE)))
            {
               // Error !
               return;
            }

            if (!WriteBuffer(hDB2,_pszWord,dwLength + 1))
            {
               // Error !
               return;
            }

            // DB1  //////////////////////////////////////////////////////////////////
            if (!WriteBuffer(hDB1,&dwSerial,sizeof(DWORD)))
            {
               // Error !
               return;
            }

            if (!WriteBuffer(hDB1,&dwOfs2,sizeof(DWORD)))
            {
               // Error !
               return;
            }
         }
      }
   }
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::Report()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::Report(const char* const pszIn,const char* const pszIn2,FILE* pOut)
{
   if (!_Src.OpenReadWrite(pszIn))
   {
      AfxMessageBox("Err: [Report]: Can't open Src file.",MB_OK | MB_ICONSTOP);
      return;
   }

   BYTE*    pBuf    = _Src.Buffer();
   DWORD    dwSize  = _Src.Size();

   const DWORD    SIZE2 = sizeof(DWORD) * 2;

   DWORD    dwCount = dwSize / SIZE2;

   qsort(pBuf,dwCount,SIZE2,Dix_Sorter);

   if (!_Src2.OpenReadOnly(pszIn2))
   {
      AfxMessageBox("Err: [Report]: Can't open Src2 file.",MB_OK | MB_ICONSTOP);
      return;
   }

   BYTE*    pBuf2   = _Src2.Buffer();
//   DWORD    dwSize2 = _Src2.Size();

   DWORD    dwOldKey = (DWORD)-1;

   for (DWORD ii = 0; ii < dwCount; ++ii)
   {
      DWORD    dwNewKey = *(DWORD*)(pBuf + SIZE2 * ii);
      
      if (dwOldKey != dwNewKey)
      {
         // Dump Current
         DWORD    dwCnt = _List.Count();

         if (dwCnt > 1)
         {
            // Should be int !
            for (int ii = (dwCnt - 1); ii >= 0; --ii)
            {
               char*     pEntry = (char*)_List.At(ii);

               _List.RemoveAt(ii);

               if (pEntry)
               {
                  fprintf(pOut,"%s\n",pEntry);

                  delete[] pEntry;
                  pEntry = NULL;
               }
            }

            fprintf(pOut,"\n");
         }

         // Start New List
         dwOldKey = dwNewKey;

         ListCleanup();

         DWORD    dwOfs = *(DWORD*)(pBuf + sizeof(DWORD) + SIZE2 * ii);

         DB_RECORD*     pRecord = (DB_RECORD*)(pBuf2 + dwOfs);

         char*    pEntry = new char[pRecord->_bySize + 1];

         strcpy(pEntry,pRecord->_pszWord);

         if (_List.Insert(pEntry) == -1)
         {
            delete[] pEntry;
            pEntry = NULL;
         }
      }
      else
      {
         // Continue List
         DWORD    dwOfs = *(DWORD*)(pBuf + sizeof(DWORD) + SIZE2 * ii);

         DB_RECORD*     pRecord = (DB_RECORD*)(pBuf2 + dwOfs);

         char*    pEntry = new char[pRecord->_bySize + 1];

         strcpy(pEntry,pRecord->_pszWord);

         if (_List.Insert(pEntry) == -1)
         {
            delete[] pEntry;
            pEntry = NULL;
         }
      }
   }
      
   // Dump Rest
   DWORD    dwCnt = _List.Count();

   if (dwCnt > 1)
   {
      // Should be int !
      for (int ii = (dwCnt - 1); ii >= 0; --ii)
      {
         char*     pEntry = (char*)_List.At(ii);

         _List.RemoveAt(ii);

         if (pEntry)
         {
            fprintf(pOut,"%s\n",pEntry);

            delete[] pEntry;
            pEntry = NULL;
         }
      }

      fprintf(pOut,"\n");
   }

   _Src2.Close();
   _Src.Close();
}

/* ******************************************************************** **
** @@ CAnagrammerDlg::LoadMap()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CAnagrammerDlg::LoadMap()
{
   if (!_Src.OpenReadOnly((LPCTSTR)m_Map) || (_Src.Size() < ASCII_SIZE))
   {
      return;
   }

   memcpy(_pXLAT,_Src.Buffer(),ASCII_SIZE);

   _Src.Close();
}

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
