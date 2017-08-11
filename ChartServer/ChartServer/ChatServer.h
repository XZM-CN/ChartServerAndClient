//************************************************************************
// Boby Thomas Pazheparampil
// May 2006
// Implementation of CIPMessage class and main.
//************************************************************************
#pragma once


#include <stdio.h>
#include <winsock2.h>
#include <conio.h> 
#include<list>
#include <iostream>
using namespace std;

#pragma comment (lib,"ws2_32.lib")

class CChatServer
{
public:
	CChatServer();
	~CChatServer();
	bool IsConnected(){return m_bIsConnected;} // returns connection status
	void StartListenClient(); // Listen to client
	int SendMessagePort(string sMessage); // Send message to sll clients.
	int RecClient(SOCKET sRecSocket); // receive message for a particulat socket

public:
	static UINT  ServerRecThread(LPVOID pParam);
	static UINT  ServerListenThread(LPVOID pParam);
private:
	bool m_bIsConnected; // true - connected false - not connected
	int m_iServerPort;
	list<SOCKET> m_vClientList; // All socket connected to client
	SOCKET m_SClient;
	SOCKET m_SListenClient; // socket listening for client calls
};

typedef struct tagThreadParam{
	CChatServer* TP_pChatServer;
	SOCKET       TP_socket;
	tagThreadParam::tagThreadParam(){
		TP_pChatServer = NULL;
		TP_socket      = NULL;
	}
}ThreadParam,*PThreadParam;