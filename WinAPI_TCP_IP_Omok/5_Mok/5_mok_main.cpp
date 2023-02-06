
#include <windows.h>
#include <mmsystem.h>
#include "resource.h"
#include "5_mok_server.h"   //���Ƿ� ���۵� ��� ���� ��ũ
#include<time.h>
#include<fstream>
#pragma comment(lib, "ws2_32")
using namespace std;
//�ð��Լ�
time_t currentTime = time(&currentTime);
struct tm* struct_time = localtime(&currentTime);
//���������ν���
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ConnectSer(HWND, UINT, WPARAM, LPARAM); //��Ʈ��IP�Է� �޼��� ó�� �Լ�
LRESULT CALLBACK EditSubProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);// ä��â
LRESULT CALLBACK Login(HWND, UINT, WPARAM, LPARAM); //ȸ������ ó���Լ�

HINSTANCE	g_hInst;
LPCTSTR		lpszClass = TEXT("���Ӽ����ǽ� B777040 ����ȣ �������");
//����������Լ�
wofstream outFile("Log.txt");
wofstream outFile1("Login.txt");
int APIENTRY WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND       hWnd;
	MSG        Message;
	WNDCLASS   WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;
	WndClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance		= hInstance;
	WndClass.lpfnWndProc	= WndProc;
	WndClass.lpszClassName	= lpszClass;
	WndClass.lpszMenuName	= NULL;
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass
		, lpszClass
		, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, 0     
		, 0
		, NULL
		, (HMENU)NULL
		, hInstance
		,NULL);
	hWndMain=hWnd;
	ShowWindow(hWnd,nCmdShow);

	while (GetMessage(&Message,NULL,0,0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

//��Ʈ������ ���ӽ� ���ӽ��� ��ư�� ���⶧���� ���� ����� GameSet�Լ��� ȣ�� �ʱ�ȭ �Ѵ�
void InitGame()
{
	EndFlag = FALSE;
	TotalGameCount=0;
	B_WinGameCount=0;
	B_LoseGameCount=0;
	W_WinGameCount=0;
	W_LoseGameCount=0;
	Game_Select = FALSE; 
	InvalidateRect(hWndMain,NULL,TRUE);
}

LRESULT CALLBACK WndProc( HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	WSADATA wsd;	//net����� ���� ���� ���̺귯�� ���� ����ü
	LONG Style;	//������ ��Ÿ�� �ٲ� LOHG ����
	HDC				hdc;
	HDC				MemDC;		// ȭ��ǥ�� �ӵ��������� �޸� ������ ����
	PAINTSTRUCT		ps;
	RECT            rt;
	int				x, y;

	//�ʱ�ȭ�� ���ڰŸ��� ���� ���� 
	HFONT           font, oldfont;  //�ʱ�ȭ�� ��Ʈ���
	TCHAR           stText[128];
	RECT			strt={200,300,300,40};
	static BOOL		TimeFlag;

	
	TCHAR           B_Game_Str[255];  // ����� ����� ���� ����
	TCHAR           W_Game_Str[255];  // ����� ����� ���� ����

	switch (iMessage)
	{
		case WM_CREATE:

			ClearBoard();  //��ǥ���� ����� �迭�� �ʱ�ȭ �ϴ� �Լ� ȣ��

			//������ �ʱ�ȭ����
			//���� ���̺귯���� ������ �� �ְ� �Ѵ�.
			WSAStartup(MAKEWORD(2,2), &wsd);

			//�̹��� �ҷ�����===================================================
			Badukpan	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
			Black_Char	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
			White_Char	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
			Back_Image	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
			Black_Dol	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
			White_Dol	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
			Black_Dol_Point	   = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP20));
			White_Dol_Point	   = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP19));
			start_Image        = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
			// ������ ũ�⸦ ������ ����=========================================
			SetRect(&rt, 0, 0, WIDTH*23, HIGHT*17.5);
			AdjustWindowRect(&rt, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);
			SetWindowPos(hWnd, NULL, 0, 0, rt.right-rt.left, rt.bottom-rt.top,
				SWP_NOMOVE|SWP_NOZORDER);
			//��ɹ�ư����==========================================================
			StButton = CreateWindow(TEXT("button"),TEXT("ȸ������"),WS_CHILD|
				BS_PUSHBUTTON,10,460,100,35,hWnd,(HMENU)St_But,g_hInst,NULL);
			SerButton = CreateWindow(TEXT("button"),TEXT("��������"),WS_CHILD|
				BS_PUSHBUTTON,120,460,100,35,hWnd,(HMENU)Ser_But,g_hInst,NULL);
			ConButton = CreateWindow(TEXT("button"),TEXT("�����ϱ�"),WS_CHILD|
				BS_PUSHBUTTON,230,460,100,35,hWnd,(HMENU)Con_But,g_hInst,NULL);
			Exbutton =CreateWindow(TEXT("button"),TEXT("���Ӳ���"),WS_CHILD|
				BS_PUSHBUTTON,340,460,100,35,hWnd,(HMENU)Ex_But,g_hInst,NULL);
			//���Ӽ� ������ ǥ���ϴ� ����Ʈ ����=========================================
			hEdit2=CreateWindow(TEXT("edit"), NULL, WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|
				ES_READONLY|ES_CENTER,465,125,75,20,hWnd,(HMENU)ID_EDIT2,g_hInst,NULL);
			hEdit4=CreateWindow(TEXT("edit"), NULL, WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|
				ES_READONLY|ES_CENTER,594,125, 75,20,hWnd,(HMENU)ID_EDIT4,g_hInst,NULL);
			// ä��â ���� ���=========================================================
			ChatBox=CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_BORDER|ES_MULTILINE|WS_VSCROLL|
				ES_AUTOVSCROLL, 465, 220, 205, 200,hWnd,(HMENU)Chat_Dig,g_hInst,NULL);
			ChatBoxSub = CreateWindow("edit", NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE,
				465, 442, 188, 20, hWnd, (HMENU)Chat_DigSub, g_hInst, NULL);
			//�ʱ�ȭ�� ���� ���ڰŸ��� ���� Ÿ�̸�=================================
			SetTimer(hWnd,1,800,NULL);
			SendMessage(hWnd,WM_TIMER,NULL,NULL);

			EnableWindow(Exbutton, FALSE); //�� �������� ��ư ��Ȱ��ȭ
			
			//ä�ð��� ChatBoxSub��GWL_WNDPROC(���������ν���)��EditSubProc���μ���
			OldEditProc = (WNDPROC)SetWindowLong(ChatBoxSub, GWL_WNDPROC, (LONG)EditSubProc);
			//OldEditProc���������������̸��ϵǾ��
			return 0;


		//�ʱ�ȭ�� ���ڸ� ���ڰŸ��� �ϱ����� �÷��׷� ����
		case WM_TIMER:
			if(TimeFlag==FALSE)
				TimeFlag=TRUE;
			else TimeFlag=FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;

		//��ư�� �������� ����=================================
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				//ȸ������ ��ư ��������========================
				case St_But:	
					DialogBox(g_hInst, (LPCTSTR)IDD_DIALOG2, hWnd, (DLGPROC)Login);
				
					break;

				//�游��� ��ư�� ��������================================
				case Ser_But: //���� ��ư�� ���õ� ���(���������·� ��)

					InitGame();
					Semi_stFlag=TRUE;
					
					SOCKET ListenSocket;  //���� ��� ����
					SOCKADDR_IN InternetAddr;  

					ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					if(ListenSocket==INVALID_SOCKET)  //������� ��
					{
						MessageBox(hWnd, "���� ��� ����!", "����!", MB_OK|MB_ICONERROR);
						return 0;
					}

					EnableWindow(Exbutton, TRUE);	//�������� ��ư Ȱ��ȭ

					memset(&InternetAddr, 0 ,sizeof(InternetAddr));   // SOCKADDR�ʱ�ȭ

					InternetAddr.sin_family = AF_INET;		
					InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//�ڽ��� ip�� �ڵ� ����
					InternetAddr.sin_port = htons(PORT);

					//9999�� ��Ʈ�� ���ε�
					bind(ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));

					//WM_SOCKET�� ������ �޽����� �뺸�ϵ��� AsyncSelect ����
					WSAAsyncSelect(ListenSocket, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

					//���� ���(1:1 �����̱� ������ ��� 1)
					listen(ListenSocket,1);				

					//���������� ���ÿ� ����,����,Ŭ���̾�Ʈ��ư ��Ȱ��ȭ
					EnableWindow(SerButton, FALSE);   
					EnableWindow(ConButton, FALSE);
					EnableWindow(StButton, FALSE);

					MessageBox(hWnd, "���� ��� ���°� �Ǿ����ϴ�.", "Server", MB_OK);
					break;

				//�����ϱ� ��ư�� ����������============================================
				case Con_But:	//���� ��ư�� ���õ� ���
					InitGame();
					Semi_stFlag=TRUE;
					
					//ip�Է¿� DialogBox ����,�޽���ó�� �Լ��� ConnectSer
					DialogBox(g_hInst, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)ConnectSer);
					
					// ConnectSer �Լ��� �޼������� IP_OK(Ȯ��)��ư�� Ŭ���Ǿ���
					//IP_OK �� True�� �Ǹ鼭 ������ �����ϰ� �ȴ�
					if(IP_OK)
					{
						SOCKADDR_IN ServerAddr;
						//���� ����
						m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						
						//���� ������н� ����ڵ� 
						if(m_sockClient==INVALID_SOCKET)  // ������� ��
						{
							MessageBox(hWnd, "���� ���� ����!", "����!", MB_OK|MB_ICONERROR);
							return 0;
						}

						EnableWindow(Exbutton, TRUE);	//�������� ��ư Ȱ��ȭ

						memset(&ServerAddr, 0 ,sizeof(ServerAddr));  // SOCKADDR�ʱ�ȭ
						
						//������ �ּ� ����
						ServerAddr.sin_family = AF_INET;
						ServerAddr.sin_port = htons(PORT);
						ServerAddr.sin_addr.s_addr = inet_addr(szIP_ADDRESS);	

						//���� �õ�
						if(connect(m_sockClient, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr))== SOCKET_ERROR)
						{
							MessageBox(hWndMain, "���� ����.", "Ŭ���̾�Ʈ", MB_OK);
							break;
						}

						WSAAsyncSelect(m_sockClient, hWndMain, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

						//���������� ���ÿ� ����,����,Ŭ���̾�Ʈ��ư ��Ȱ��ȭ
						EnableWindow(ConButton, FALSE);
						EnableWindow(SerButton, FALSE);
						EnableWindow(StButton, FALSE);

						EnableWindow(Exbutton, TRUE);	//�������� ���� �� �� �������� ��ư Ȱ��ȭ

						MessageBox(hWndMain, "������ ����Ǿ����ϴ�. \n\n   GAME START!", "Client", MB_OK);					
						outFile << "Log : [" << struct_time->tm_mon + 1 << "��" << struct_time->tm_mday << "�� " <<
						struct_time->tm_hour << "��" << struct_time->tm_min << "��] " << Login_ID << " connected" << endl;
						SetStart(WHITE_SPOT);		//SetStart �Լ��� ȣ���ϸ� Ŭ���̾�Ʈ�� �鵹�� ����
					}
					break;

				//�������� ��ư �������� ==========================================
				case Ex_But:
					EnableWindow(Exbutton, FALSE);
					closesocket((SOCKET)wParam); //��������
					closesocket(m_sockClient); //��������
					closesocket(ListenSocket); //��������
					EndFlag = TRUE;
					TotalGameCount=0;
					B_WinGameCount=0;
					B_LoseGameCount=0;
					W_WinGameCount=0;
					W_LoseGameCount=0;
					IP_OK=FALSE;		//IP�Է�â Ȯ�� �÷��� �ʱ�ȭ
					m_bGameStart=FALSE;	//��ŸƮ �÷��� �ʱ�ȭ
					EnableWindow(ConButton, TRUE);
					EnableWindow(SerButton, TRUE);
					EnableWindow(StButton, TRUE);
					InvalidateRect(hWndMain,NULL,TRUE);
					
						outFile << "Log : [" << struct_time->tm_mon + 1 << "��" << struct_time->tm_mday << "�� " <<
							struct_time->tm_hour << "��" << struct_time->tm_min << "��] " << Login_ID << " disconnected" << endl;
					
					break;
			}
			return 0;


		case WM_SOCKET:
			//���� ���� üũ
			if(WSAGETSELECTERROR(lParam))
			{
				closesocket((SOCKET)wParam);
				break;
			}

			//���� �̺�Ʈ ������ ���� ó��
			switch(WSAGETSELECTEVENT(lParam))
			{
				case FD_ACCEPT:	//���� ����
					m_sockClient = accept(wParam, NULL, NULL);  //������ �����Ѵ�.
					
					//���� ������н� 
					if(m_sockClient==INVALID_SOCKET)
					{
						MessageBox(hWnd, "accept ����!", "����!", MB_OK|MB_ICONERROR);   //�������
						return 0;
					}					

					//����� ������ �̺�Ʈ ó���� ����(4��° �Ķ������ �̺�Ʈ �߻��� WM_SOCKET�߻�)
					WSAAsyncSelect(m_sockClient, hWndMain, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);	
					
					MessageBox(hWndMain, "Ŭ���̾�Ʈ�� �����߽��ϴ�. \n\n   GAME START!", "Server", MB_OK);

					SetStart(BLACK_SPOT);		//�������� �浹�� ����.
					break;
				case FD_READ:	//������ ����
					RecvMessage(); //���ú� �Լ�ȣ��
					break; 
				case FD_WRITE:	//������ �۽� �޽���
					break;
				case FD_CLOSE:  //��Ʈ�� ������ ��������
					//SendMessage(hWnd,WM_COMMAND,Ex_But,NULL);
					MessageBox(hWndMain, "������ ������ �������ϴ�.", "�˸�", MB_ICONSTOP);
					closesocket((SOCKET)wParam);
				break;
			}
			return 0;


		case WM_LBUTTONDOWN:
			
			//�ʱ�ȭ���� stFlag FALSE�϶� ����ȭ���� �������� ���콺 Ŭ���� �����ֱ�����
			stFlag=TRUE;
			KillTimer(hWnd,1); //Ÿ�̸� ���̱�

			//����ȭ�鿡 ������ ���ӽ����� ���� ���콺 Ŭ���� ���� �������� ���콺 Ŭ���� �����ϱ� ����
			//���ӽ��� ��ư�� Ŭ���Ǹ� Semi_stFlag�� TRUE�� �ȴ�
			if(Semi_stFlag == FALSE)
			{
				//������ ������� �ʱ�ȭ�鿡�� ������� ����ٰ�
				//���콺 Ŭ���� �����ְ� �ϱ����� ����
				Style = GetWindowLongPtr(StButton,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(StButton,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(ChatBox,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(ChatBox,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(hEdit2,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(hEdit2,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(hEdit4,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(hEdit4,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(SerButton,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(SerButton,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(ConButton,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(ConButton,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(Exbutton,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(Exbutton,GWL_STYLE,Style);		//��Ÿ�� set
				Style = GetWindowLongPtr(ChatBoxSub,GWL_STYLE);	//��Ÿ�� get
				Style |= WS_VISIBLE;		//���̱� �Ӽ� �߰�
				SetWindowLongPtr(ChatBoxSub,GWL_STYLE,Style);		//��Ÿ�� set
				InvalidateRect(hWndMain,NULL,TRUE);
				Semi_stFlag = TRUE;
			}
			//���ӽ��� ��ư�� Ŭ���Ǹ� Semi_stFlag�� TRUE�� �ȴ�
			if(Semi_stFlag == TRUE)
			{
				//���� ����Ǵ� ��������
				if(stFlag==TRUE && EndFlag==FALSE && flag_Ready==TRUE) 
				{	
					//Ű�ٿ�� �߻��� ��ǥ���� x��y�� ����
					x = LOWORD(lParam);
					y = HIWORD(lParam);
					if(Game_Select == TRUE)
					{
						//���� ���� ���� ��ǥ���� ����ϴ� �Լ� ȣ��
						OnDollDown(x, y);
					}
					if(Game_Select == FALSE)
					{
						//���� ���� ���� ��ǥ���� ����ϴ� �Լ� ȣ��
						Net_OnDollDown(x, y);
					}
				}
			}
			return 0;

		//���� ȭ�鿡 ������ ����κ�=============================================
		case WM_PAINT:
			hdc	= BeginPaint(hWnd, &ps);
			MemDC = CreateCompatibleDC(hdc);
			if(stFlag==TRUE)
			{
				//���ȭ�� �׸��� ===================================================
				SelectObject(MemDC, Back_Image);
				BitBlt(hdc, 0, 0, 800, 600, MemDC, 0, 0,SRCCOPY);

				// �ٵ��� �׸���========================================================
				SelectObject(MemDC, Badukpan); 
				BitBlt(hdc, 0, 0, 450, 450, MemDC, 0, 0,SRCCOPY);

				// ĳ�� ǥ�� ===================================================
					SelectObject(MemDC, Black_Char);  //��ĳ�� ȭ��ǥ��
					BitBlt(hdc, 495, 55, 100, 200, MemDC, 0, 0, SRCCOPY);
					SelectObject(MemDC, White_Char);  //��ĳ�� ȭ��ǥ��
					BitBlt(hdc, 615, 55, 100, 200, MemDC, 0, 0, SRCCOPY);
				
		
				//���� ��ǥ���� ���� �׸��� �Լ� ȣ��
				DrawBoard(hdc, MemDC);

				
				//������ ���и� ����ϴ� �κ�
				SetBkColor(hdc, RGB(255,255,255));
				TextOut(hdc,465, 200, TEXT("ä��â"),6);
				TextOut(hdc,465, 423, TEXT("�Է�â"),6);
				SetBkMode(hdc, TRANSPARENT);
				wsprintf(stText, TEXT("��    ��"));
				TextOut(hdc,542,125,stText, lstrlen(stText));
				wsprintf(B_Game_Str,TEXT("%d��%d��"), B_WinGameCount, B_LoseGameCount);
				SetWindowText(hEdit2, B_Game_Str);
				wsprintf(W_Game_Str,TEXT("%d��%d��"), W_WinGameCount, W_LoseGameCount);
				SetWindowText(hEdit4, W_Game_Str);
			}

			//�ʱ�ȭ�� �׸��� ================================================
			if(stFlag==FALSE)
			{
				//�̹��� �׸���=============================================
				SelectObject(MemDC, start_Image);  
				BitBlt(hdc, 130, 20, 500, 500, MemDC, 0, 0,SRCCOPY);
			
				if(TimeFlag == TRUE)
				{
					//�ؽ�Ʈ ���==========================================
					SetBkMode(hdc, TRANSPARENT);
					font=CreateFont(30,0,0,0,0,1,1,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_ROMAN,"����");
					oldfont=(HFONT)SelectObject(hdc,font);
					wsprintf(stText, TEXT("���콺�� Ŭ���ϼ���!"));
					TextOut(hdc, 200, 300, stText, lstrlen(stText));
					SelectObject(hdc,oldfont);
					DeleteObject(font);
				}
				Semi_stFlag = FALSE;
			}
		
			DeleteDC(MemDC);
			EndPaint(hWnd, &ps);
			return 0;

		// �ҷ��� ��Ʈ�� ����============================================
		case WM_DESTROY:
			DeleteObject(Badukpan); 
			DeleteObject(Black_Char); 
			DeleteObject(White_Char); 
			DeleteObject(Black_Dol); 
			DeleteObject(Black_Dol_Point); 
			DeleteObject(White_Dol_Point);
			DeleteObject(White_Dol);
			DeleteObject(start_Image);
			closesocket(m_sockClient); //��������
			WSACleanup();  //���ҽ� ����
			PostQuitMessage(0);
			return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//��Ʈ�� ��� ȯ���� ���� �޼��� ó�� �Լ�================================================
LRESULT CALLBACK ConnectSer(HWND hwndconn, UINT message, WPARAM wParam, LPARAM lParam)//DialogCallback�Լ�
{
	switch (message)
	{
		case WM_INITDIALOG:
			//Dialog Addressâ�� ����Ʈ�� 127.0.0.1����
			SetDlgItemText(hwndconn, SER_IPADDRESS1, "127.0.0.1");
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case SER_IP_OK:
					//�Է��� ip�� szIP_ADDRESS �迭�� �����´�.
					GetDlgItemText(hwndconn, SER_IPADDRESS1, szIP_ADDRESS, 20);	
	
					//IP�Է��� OK��ư�� ������ ������ �⺻ �Լ����� �� ���� üũ�Ͽ� ������ �õ��ϰų� ����Ѵ�.
					IP_OK = TRUE;	// OK��ư Ŭ���� �߻��ϴ� �÷���
					
					//IP�� �Է¹޴� ���̾�α� �ݱ�
					EndDialog(hwndconn, 0);	  

					return TRUE; //�� ��ȯ���� connect��ư�� Ŭ���Ǿ����� ����

				case SER_IP_CANCEL:
					//IP�� �Է¹޴� ���̾�α� �ݱ�
					EndDialog(hwndconn, 0);
					return TRUE;

				case WM_DESTROY:
					EndDialog(hwndconn, LOWORD(wParam));
					return TRUE;
			}
			break;
	}
	return FALSE;
}
//�α��� ó���Լ�
LRESULT CALLBACK Login(HWND hwndconn, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hwndconn, IDC_EDIT1, Login_ID, 20);
			GetDlgItemText(hwndconn, IDC_EDIT2, Login_PW, 20);
		
			outFile1 <<"[�α��� ����]"<< endl <<"ID : " << Login_ID << endl << "PW : " << Login_PW << endl;

			MessageBox(hwndconn, "ȸ�������� �Ǽ̽��ϴ�!!", "Login", MB_OK);
			//�Է¹޴� ���̾�α� �ݱ�
			EndDialog(hwndconn, 0);

			return TRUE; 

		case IDCANCEL:
			//�Է¹޴� ���̾�α� �ݱ�
			EndDialog(hwndconn, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//ä��â�� ������ �Է��ϰ� ����Ű�� �������� ä�ø޼����� �����ϰ� �ٿ��ִ� �Լ� 
LRESULT CALLBACK EditSubProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch (iMessage) {
	case WM_KEYDOWN:
		if(wParam==VK_RETURN){
			Chatting();
		}
		break;
	}
	return CallWindowProc(OldEditProc,hWnd,iMessage,wParam,lParam);
}
