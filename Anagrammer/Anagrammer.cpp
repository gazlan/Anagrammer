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

#include "..\shared\mmf.h"
#include "..\shared\file.h"
#include "..\shared\map_bppt_jannink.h"
#include "..\shared\vector.h"
#include "..\shared\vector_sorted.h"

#include "Anagrammer.h"
#include "AnagrammerDlg.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif 

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

CAnagrammerApp   theApp; // The one and only CAnagrammerApp object

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ MESSAGE_MAP()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CAnagrammerApp, CWinApp)
   //{{AFX_MSG_MAP(CAnagrammerApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG
   ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CAnagrammerApp::CAnagrammerApp()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CAnagrammerApp::CAnagrammerApp()
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}

/* ******************************************************************** **
** @@ CAnagrammerApp::InitInstance()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BOOL CAnagrammerApp::InitInstance()
{
   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.

   // Call this function to initialize the rich edit control for the application. 
   // It will also initialize the common controls library, if the library hasn’t already 
   // been initialized for the process. If you use the rich edit control directly from your 
   // MFC application, you should call this function to assure that MFC has properly 
   // initialized the rich edit control runtime.
   AfxInitRichEdit();

   // Only One Instance of this App is available !
   _hMutex = CreateMutex(NULL,TRUE,"AnagrammerMutex");

   if (!_hMutex)
   {
      return FALSE;
   }

   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      return FALSE;
   }

#ifdef _AFXDLL
   Enable3dControls();        // Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();  // Call this when linking to MFC statically
#endif

   CAnagrammerDlg dlg;

   m_pMainWnd = &dlg;

   int nResponse = dlg.DoModal();

   if (nResponse == IDOK)
   {
      // TODO: Place code here to handle when the dialog is
      //  dismissed with OK
   }
   else if (nResponse == IDCANCEL)
   {
      // TODO: Place code here to handle when the dialog is
      //  dismissed with Cancel
   }

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.
   return FALSE;
}

/* ******************************************************************** **
** @@ CAnagrammerApp::ExitInstance()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

int CAnagrammerApp::ExitInstance()
{
   CloseHandle(_hMutex);
   _hMutex = INVALID_HANDLE_VALUE;

   return CWinApp::ExitInstance();
}

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
