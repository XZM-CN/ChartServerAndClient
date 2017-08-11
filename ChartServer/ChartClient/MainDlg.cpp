#include "stdafx.h"
#include "MainDlg.h"

#include "ChatClient.h"


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	m_pClientObj = new CChatClient();
	Innermain();
	

	return TRUE;
}

int CMainDlg::Innermain(int argc /*= 0*/, char* argv /*= NULL*/)
{
	char buf[4096];
	cout<<"This is a client TCP/IP application\nConnecting to port 8084\n";
	cout<<"\nPress ONLY ENTER to quit";
	cout<<"\nWritten by Boby Thomas";
	cout<<"\n===============================================\n";

	char strTempPath[MAX_PATH];
	memset(strTempPath,0,MAX_PATH);
	GetModuleFileNameA(NULL,strTempPath,MAX_PATH);
	strrchr(strTempPath,'\\')[0]=0;   // 将\字符置0
	strrchr(strTempPath,'\\')[1]=0;     // 将\后第一个字符置0,看使用情况酌情处理
	strcat(strTempPath,"server.ini");

	FILE *fp = fopen(strTempPath,"r");
	if(fp == NULL)
	{
		cout<<"\nUnable to open server.ini. Please specify server IPsddress in server.ini";
		return 1; // main failed
	}
	string sServerAddress;
	while((fgets(buf,4096,fp)) != NULL)
	{
		if(buf[0] == '#')
			continue;
		sServerAddress = buf;

	}
	fclose(fp);

	if(sServerAddress.size() == 0)
	{
		cout<<"\nUnable to find server IP address in server.ini";
		cout<<"\nPlease set server IPaddress";
		cout<<"\nThis is Boby Signing off. BYE:";
		getch();
		return 0;
	}

	m_pClientObj->Init(sServerAddress.c_str(),8084);
	if(!m_pClientObj->IsConnected())
	{
		cout<<"\nUnable to connect to the IP address specified in server.ini";
		cout<<"\nPlease check server IPaddress";
		cout<<"\nThis is Boby Signing off. BYE:";
		getch();
		return 0;	
	}

	// AfxBeginThread(MessageRecThread,0);
	HANDLE hThread;
	DWORD dwThreadId;
	hThread=::CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)CChatClient::MessageRecThread, m_pClientObj, 0, &dwThreadId);

	while(gets(buf))
	{
		if(strlen(buf) == 0){
			cout<<"\nthe buffer is NULL,the while is breaking";
			break;
		}
		if(m_pClientObj->SendMessagePort(buf))
		{
			cout<<"Problem in connecting to server. Check whether server is running\n";
			break;
		}
	}

	cout<<"\nThis is Boby Signing off. BYE:";
	getch();
	return 0;
}
