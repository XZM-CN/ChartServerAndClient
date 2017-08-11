#include "StdAfx.h"
#include "ChatServer.h"



CChatServer::CChatServer()
{
	cout << "Starting up TCP Chat server\n";
	m_bIsConnected = false;

	WSADATA wsaData;

	sockaddr_in local;

	int wsaret=WSAStartup(0x101,&wsaData);

	if(wsaret!=0)
	{
		return;
	}

	local.sin_family=AF_INET; 
	local.sin_addr.s_addr=inet_addr("192.168.0.67")/*INADDR_ANY*/; 
	local.sin_port=htons((u_short)8084); 

	m_SListenClient=socket(AF_INET,SOCK_STREAM,0);


	if(m_SListenClient==INVALID_SOCKET)
	{
		return;
	}


	if(bind(m_SListenClient,(sockaddr*)&local,sizeof(local))!=0)
	{
		return;
	}


	if(listen(m_SListenClient,10)!=0)
	{
		return;
	}

	m_bIsConnected = true;
	return;
}

CChatServer::~CChatServer()
{
	closesocket(m_SListenClient);

	WSACleanup();
}

void CChatServer::StartListenClient()
{

	sockaddr_in from;
	int fromlen=sizeof(from);

	m_SClient=accept(m_SListenClient,
		(struct sockaddr*)&from,&fromlen);

	if(m_SClient != INVALID_SOCKET)
		m_vClientList.push_back(m_SClient);

	// AfxBeginThread(ServerRecThread,(void *)m_SClient);
	HANDLE hThread;
	DWORD dwThreadId;
	PThreadParam Param = new ThreadParam();
	Param->TP_pChatServer = this;
	Param->TP_socket      = m_SClient;
	hThread=::CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)CChatServer::ServerRecThread, Param, 0, &dwThreadId);

}



int CChatServer::SendMessagePort(string sMessage)
{
	int iStat = 0;
	list<SOCKET>::iterator itl;

	if(m_vClientList.size() == 0)
		return 0;

	for(itl = m_vClientList.begin();itl != m_vClientList.end();itl++)
	{
		iStat = send(*itl,sMessage.c_str(),sMessage.size()+1,0);
		if(iStat == -1)
			m_vClientList.remove(*itl);
	}

	if(iStat == -1)
		return 1;

	return 0;

}

int CChatServer::RecClient(SOCKET sRecSocket)
{
	char temp[4096];
	int iStat;

	iStat = recv(sRecSocket,temp,4096,0);
	if(iStat == -1)
	{
		m_vClientList.remove(sRecSocket);
		return 1;
	}
	else
	{
		SOCKADDR_IN from;
		int nSize = sizeof(from);
		getpeername(sRecSocket,(SOCKADDR *)&from,&nSize);
		cout <<"接收了客户端"<< inet_ntoa(from.sin_addr) <<"的发送请求"<<":"<<temp<<"\r\n"<< endl;  

		SendMessagePort(temp);
		return 0;
	}
	return 0;

}

UINT CChatServer::ServerRecThread(LPVOID pParam)
{
	PThreadParam Param = (PThreadParam)pParam;

	CChatServer* pChatServer = Param->TP_pChatServer;
	SOCKET sRecSocket        = Param->TP_socket;
	while(1)
	{
		if(pChatServer->RecClient(sRecSocket))
			break;
	}
	return 0;
}

UINT CChatServer::ServerListenThread(LPVOID pParam)
{
	CChatServer* pChatServer = (CChatServer*)pParam;

	while(1)
		pChatServer->StartListenClient();
	return 0;
}
