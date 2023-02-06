
#include <windows.h>
#include <mmsystem.h>
#include "resource.h"
#include "5_mok_server.h"   //임의로 제작된 헤더 파일 링크
#include<time.h>
#include<fstream>
#pragma comment(lib, "ws2_32")
using namespace std;
//시간함수
time_t currentTime = time(&currentTime);
struct tm* struct_time = localtime(&currentTime);
//윈도우프로시저
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ConnectSer(HWND, UINT, WPARAM, LPARAM); //네트웍IP입력 메세지 처리 함수
LRESULT CALLBACK EditSubProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);// 채팅창
LRESULT CALLBACK Login(HWND, UINT, WPARAM, LPARAM); //회원가입 처리함수

HINSTANCE	g_hInst;
LPCTSTR		lpszClass = TEXT("게임서버실습 B777040 나장호 오목게임");
//파일입출력함수
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

//네트웍게임 접속시 게임시작 버튼이 없기때문에 게임 종료시 GameSet함수를 호출 초기화 한다
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
	WSADATA wsd;	//net통신을 위한 소켓 라이브러리 정보 구조체
	LONG Style;	//윈도우 스타일 바꿀 LOHG 변수
	HDC				hdc;
	HDC				MemDC;		// 화면표시 속도개선위해 메모리 영역에 복사
	PAINTSTRUCT		ps;
	RECT            rt;
	int				x, y;

	//초기화면 깜박거림을 위한 설정 
	HFONT           font, oldfont;  //초기화면 폰트사용
	TCHAR           stText[128];
	RECT			strt={200,300,300,40};
	static BOOL		TimeFlag;

	
	TCHAR           B_Game_Str[255];  // 흑승패 출력을 위해 저장
	TCHAR           W_Game_Str[255];  // 백승패 출력을 위해 저장

	switch (iMessage)
	{
		case WM_CREATE:

			ClearBoard();  //좌표값이 저장될 배열을 초기화 하는 함수 호출

			//원도우 초기화에서
			//윈속 라이브러리를 시작할 수 있게 한다.
			WSAStartup(MAKEWORD(2,2), &wsd);

			//이미지 불러오기===================================================
			Badukpan	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
			Black_Char	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
			White_Char	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
			Back_Image	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
			Black_Dol	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
			White_Dol	       = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
			Black_Dol_Point	   = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP20));
			White_Dol_Point	   = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP19));
			start_Image        = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
			// 윈도우 크기를 지정한 내용=========================================
			SetRect(&rt, 0, 0, WIDTH*23, HIGHT*17.5);
			AdjustWindowRect(&rt, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);
			SetWindowPos(hWnd, NULL, 0, 0, rt.right-rt.left, rt.bottom-rt.top,
				SWP_NOMOVE|SWP_NOZORDER);
			//기능버튼생성==========================================================
			StButton = CreateWindow(TEXT("button"),TEXT("회원가입"),WS_CHILD|
				BS_PUSHBUTTON,10,460,100,35,hWnd,(HMENU)St_But,g_hInst,NULL);
			SerButton = CreateWindow(TEXT("button"),TEXT("서버생성"),WS_CHILD|
				BS_PUSHBUTTON,120,460,100,35,hWnd,(HMENU)Ser_But,g_hInst,NULL);
			ConButton = CreateWindow(TEXT("button"),TEXT("접속하기"),WS_CHILD|
				BS_PUSHBUTTON,230,460,100,35,hWnd,(HMENU)Con_But,g_hInst,NULL);
			Exbutton =CreateWindow(TEXT("button"),TEXT("접속끊기"),WS_CHILD|
				BS_PUSHBUTTON,340,460,100,35,hWnd,(HMENU)Ex_But,g_hInst,NULL);
			//게임수 전적을 표시하는 에디트 생성=========================================
			hEdit2=CreateWindow(TEXT("edit"), NULL, WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|
				ES_READONLY|ES_CENTER,465,125,75,20,hWnd,(HMENU)ID_EDIT2,g_hInst,NULL);
			hEdit4=CreateWindow(TEXT("edit"), NULL, WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|
				ES_READONLY|ES_CENTER,594,125, 75,20,hWnd,(HMENU)ID_EDIT4,g_hInst,NULL);
			// 채팅창 구현 대기=========================================================
			ChatBox=CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_BORDER|ES_MULTILINE|WS_VSCROLL|
				ES_AUTOVSCROLL, 465, 220, 205, 200,hWnd,(HMENU)Chat_Dig,g_hInst,NULL);
			ChatBoxSub = CreateWindow("edit", NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE,
				465, 442, 188, 20, hWnd, (HMENU)Chat_DigSub, g_hInst, NULL);
			//초기화면 글자 깜박거림을 위한 타이머=================================
			SetTimer(hWnd,1,800,NULL);
			SendMessage(hWnd,WM_TIMER,NULL,NULL);

			EnableWindow(Exbutton, FALSE); //넷 접속종료 버튼 비활성화
			
			//채팅관련 ChatBoxSub의GWL_WNDPROC(윈도우프로시져)를EditSubProc으로설정
			OldEditProc = (WNDPROC)SetWindowLong(ChatBoxSub, GWL_WNDPROC, (LONG)EditSubProc);
			//OldEditProc에는이전설정값이리턴되어들어감
			return 0;


		//초기화면 글자를 깜박거리게 하기위해 플레그로 제어
		case WM_TIMER:
			if(TimeFlag==FALSE)
				TimeFlag=TRUE;
			else TimeFlag=FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;

		//버튼을 눌렀을때 설정=================================
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				//회원가입 버튼 눌렀을때========================
				case St_But:	
					DialogBox(g_hInst, (LPCTSTR)IDD_DIALOG2, hWnd, (DLGPROC)Login);
				
					break;

				//방만들기 버튼을 눌렀을때================================
				case Ser_But: //서버 버튼이 선택된 경우(서버대기상태로 들어감)

					InitGame();
					Semi_stFlag=TRUE;
					
					SOCKET ListenSocket;  //연결 대기 소켓
					SOCKADDR_IN InternetAddr;  

					ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					if(ListenSocket==INVALID_SOCKET)  //연결실패 방어문
					{
						MessageBox(hWnd, "서버 대기 실패!", "에러!", MB_OK|MB_ICONERROR);
						return 0;
					}

					EnableWindow(Exbutton, TRUE);	//접속종료 버튼 활성화

					memset(&InternetAddr, 0 ,sizeof(InternetAddr));   // SOCKADDR초기화

					InternetAddr.sin_family = AF_INET;		
					InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//자신의 ip로 자동 설정
					InternetAddr.sin_port = htons(PORT);

					//9999번 포트에 바인드
					bind(ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));

					//WM_SOCKET로 원도우 메시지를 통보하도록 AsyncSelect 설정
					WSAAsyncSelect(ListenSocket, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

					//연결 대기(1:1 게임이기 때문에 대기 1)
					listen(ListenSocket,1);				

					//서버생성과 동시에 시작,서버,클라이언트버튼 비활성화
					EnableWindow(SerButton, FALSE);   
					EnableWindow(ConButton, FALSE);
					EnableWindow(StButton, FALSE);

					MessageBox(hWnd, "서버 대기 상태가 되었습니다.", "Server", MB_OK);
					break;

				//접속하기 버튼이 눌러졌을때============================================
				case Con_But:	//연결 버튼이 선택된 경우
					InitGame();
					Semi_stFlag=TRUE;
					
					//ip입력용 DialogBox 열기,메시지처리 함수는 ConnectSer
					DialogBox(g_hInst, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)ConnectSer);
					
					// ConnectSer 함수의 메세지에서 IP_OK(확인)버튼이 클릭되었때
					//IP_OK 가 True가 되면서 연결을 실행하게 된다
					if(IP_OK)
					{
						SOCKADDR_IN ServerAddr;
						//소켓 생성
						m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						
						//소켓 연경실패시 방어코드 
						if(m_sockClient==INVALID_SOCKET)  // 연결실패 방어문
						{
							MessageBox(hWnd, "서버 연결 실패!", "에러!", MB_OK|MB_ICONERROR);
							return 0;
						}

						EnableWindow(Exbutton, TRUE);	//접속종료 버튼 활성화

						memset(&ServerAddr, 0 ,sizeof(ServerAddr));  // SOCKADDR초기화
						
						//연결할 주소 설정
						ServerAddr.sin_family = AF_INET;
						ServerAddr.sin_port = htons(PORT);
						ServerAddr.sin_addr.s_addr = inet_addr(szIP_ADDRESS);	

						//연결 시도
						if(connect(m_sockClient, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr))== SOCKET_ERROR)
						{
							MessageBox(hWndMain, "연결 오류.", "클라이언트", MB_OK);
							break;
						}

						WSAAsyncSelect(m_sockClient, hWndMain, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

						//서버생성과 동시에 시작,서버,클라이언트버튼 비활성화
						EnableWindow(ConButton, FALSE);
						EnableWindow(SerButton, FALSE);
						EnableWindow(StButton, FALSE);

						EnableWindow(Exbutton, TRUE);	//서버연결 성공 후 넷 접속종료 버튼 활성화

						MessageBox(hWndMain, "서버에 연결되었습니다. \n\n   GAME START!", "Client", MB_OK);					
						outFile << "Log : [" << struct_time->tm_mon + 1 << "월" << struct_time->tm_mday << "일 " <<
						struct_time->tm_hour << "시" << struct_time->tm_min << "분] " << Login_ID << " connected" << endl;
						SetStart(WHITE_SPOT);		//SetStart 함수를 호출하며 클라이언트를 백돌로 지정
					}
					break;

				//접속종료 버튼 눌렀을때 ==========================================
				case Ex_But:
					EnableWindow(Exbutton, FALSE);
					closesocket((SOCKET)wParam); //소켓종료
					closesocket(m_sockClient); //소켓종료
					closesocket(ListenSocket); //소켓종료
					EndFlag = TRUE;
					TotalGameCount=0;
					B_WinGameCount=0;
					B_LoseGameCount=0;
					W_WinGameCount=0;
					W_LoseGameCount=0;
					IP_OK=FALSE;		//IP입력창 확인 플레그 초기화
					m_bGameStart=FALSE;	//스타트 플레그 초기화
					EnableWindow(ConButton, TRUE);
					EnableWindow(SerButton, TRUE);
					EnableWindow(StButton, TRUE);
					InvalidateRect(hWndMain,NULL,TRUE);
					
						outFile << "Log : [" << struct_time->tm_mon + 1 << "월" << struct_time->tm_mday << "일 " <<
							struct_time->tm_hour << "시" << struct_time->tm_min << "분] " << Login_ID << " disconnected" << endl;
					
					break;
			}
			return 0;


		case WM_SOCKET:
			//소켓 에러 체크
			if(WSAGETSELECTERROR(lParam))
			{
				closesocket((SOCKET)wParam);
				break;
			}

			//소켓 이벤트 종류에 따라 처리
			switch(WSAGETSELECTEVENT(lParam))
			{
				case FD_ACCEPT:	//소켓 연결
					m_sockClient = accept(wParam, NULL, NULL);  //소켓을 연결한다.
					
					//소켓 연경실패시 
					if(m_sockClient==INVALID_SOCKET)
					{
						MessageBox(hWnd, "accept 실패!", "에러!", MB_OK|MB_ICONERROR);   //연결실패
						return 0;
					}					

					//연결된 소켓의 이벤트 처리를 설정(4번째 파라미터의 이벤트 발생시 WM_SOCKET발생)
					WSAAsyncSelect(m_sockClient, hWndMain, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);	
					
					MessageBox(hWndMain, "클라이언트가 접속했습니다. \n\n   GAME START!", "Server", MB_OK);

					SetStart(BLACK_SPOT);		//서버측을 흑돌로 지정.
					break;
				case FD_READ:	//데이터 수신
					RecvMessage(); //리시브 함수호출
					break; 
				case FD_WRITE:	//데이터 송신 메시지
					break;
				case FD_CLOSE:  //네트웍 연결이 끊겼을시
					//SendMessage(hWnd,WM_COMMAND,Ex_But,NULL);
					MessageBox(hWndMain, "상대방이 연결을 끊었습니다.", "알림", MB_ICONSTOP);
					closesocket((SOCKET)wParam);
				break;
			}
			return 0;


		case WM_LBUTTONDOWN:
			
			//초기화면이 stFlag FALSE일때 게임화면을 숨김으로 마우스 클릭시 보려주기위해
			stFlag=TRUE;
			KillTimer(hWnd,1); //타이머 죽이기

			//게임화면에 들어갔을시 게임시작을 위한 마우스 클릭과 돌을 놓기위한 마우스 클릭을 구분하기 위해
			//게임시작 버튼이 클릭되면 Semi_stFlag가 TRUE가 된다
			if(Semi_stFlag == FALSE)
			{
				//다음의 내용들은 초기화면에서 모든기능을 숨겼다가
				//마우스 클릭시 보여주게 하기위한 내용
				Style = GetWindowLongPtr(StButton,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(StButton,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(ChatBox,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(ChatBox,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(hEdit2,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(hEdit2,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(hEdit4,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(hEdit4,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(SerButton,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(SerButton,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(ConButton,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(ConButton,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(Exbutton,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(Exbutton,GWL_STYLE,Style);		//스타일 set
				Style = GetWindowLongPtr(ChatBoxSub,GWL_STYLE);	//스타일 get
				Style |= WS_VISIBLE;		//보이기 속성 추가
				SetWindowLongPtr(ChatBoxSub,GWL_STYLE,Style);		//스타일 set
				InvalidateRect(hWndMain,NULL,TRUE);
				Semi_stFlag = TRUE;
			}
			//게임시작 버튼이 클릭되면 Semi_stFlag가 TRUE가 된다
			if(Semi_stFlag == TRUE)
			{
				//게임 진행되는 조건제시
				if(stFlag==TRUE && EndFlag==FALSE && flag_Ready==TRUE) 
				{	
					//키다운시 발생한 좌표값을 x와y에 대입
					x = LOWORD(lParam);
					y = HIWORD(lParam);
					if(Game_Select == TRUE)
					{
						//돌을 놓기 위한 좌표값을 계산하는 함수 호출
						OnDollDown(x, y);
					}
					if(Game_Select == FALSE)
					{
						//돌을 놓기 위한 좌표값을 계산하는 함수 호출
						Net_OnDollDown(x, y);
					}
				}
			}
			return 0;

		//게임 화면에 보여질 내용부분=============================================
		case WM_PAINT:
			hdc	= BeginPaint(hWnd, &ps);
			MemDC = CreateCompatibleDC(hdc);
			if(stFlag==TRUE)
			{
				//배경화면 그리기 ===================================================
				SelectObject(MemDC, Back_Image);
				BitBlt(hdc, 0, 0, 800, 600, MemDC, 0, 0,SRCCOPY);

				// 바둑판 그리기========================================================
				SelectObject(MemDC, Badukpan); 
				BitBlt(hdc, 0, 0, 450, 450, MemDC, 0, 0,SRCCOPY);

				// 캐릭 표시 ===================================================
					SelectObject(MemDC, Black_Char);  //흑캐릭 화면표시
					BitBlt(hdc, 495, 55, 100, 200, MemDC, 0, 0, SRCCOPY);
					SelectObject(MemDC, White_Char);  //백캐릭 화면표시
					BitBlt(hdc, 615, 55, 100, 200, MemDC, 0, 0, SRCCOPY);
				
		
				//계산된 좌표값에 돌을 그리는 함수 호출
				DrawBoard(hdc, MemDC);

				
				//게임의 승패를 출력하는 부분
				SetBkColor(hdc, RGB(255,255,255));
				TextOut(hdc,465, 200, TEXT("채팅창"),6);
				TextOut(hdc,465, 423, TEXT("입력창"),6);
				SetBkMode(hdc, TRANSPARENT);
				wsprintf(stText, TEXT("전    적"));
				TextOut(hdc,542,125,stText, lstrlen(stText));
				wsprintf(B_Game_Str,TEXT("%d승%d패"), B_WinGameCount, B_LoseGameCount);
				SetWindowText(hEdit2, B_Game_Str);
				wsprintf(W_Game_Str,TEXT("%d승%d패"), W_WinGameCount, W_LoseGameCount);
				SetWindowText(hEdit4, W_Game_Str);
			}

			//초기화면 그리기 ================================================
			if(stFlag==FALSE)
			{
				//이미지 그리기=============================================
				SelectObject(MemDC, start_Image);  
				BitBlt(hdc, 130, 20, 500, 500, MemDC, 0, 0,SRCCOPY);
			
				if(TimeFlag == TRUE)
				{
					//텍스트 출력==========================================
					SetBkMode(hdc, TRANSPARENT);
					font=CreateFont(30,0,0,0,0,1,1,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_ROMAN,"굴림");
					oldfont=(HFONT)SelectObject(hdc,font);
					wsprintf(stText, TEXT("마우스를 클릭하세요!"));
					TextOut(hdc, 200, 300, stText, lstrlen(stText));
					SelectObject(hdc,oldfont);
					DeleteObject(font);
				}
				Semi_stFlag = FALSE;
			}
		
			DeleteDC(MemDC);
			EndPaint(hWnd, &ps);
			return 0;

		// 불러온 비트맵 삭제============================================
		case WM_DESTROY:
			DeleteObject(Badukpan); 
			DeleteObject(Black_Char); 
			DeleteObject(White_Char); 
			DeleteObject(Black_Dol); 
			DeleteObject(Black_Dol_Point); 
			DeleteObject(White_Dol_Point);
			DeleteObject(White_Dol);
			DeleteObject(start_Image);
			closesocket(m_sockClient); //소켓종료
			WSACleanup();  //리소스 해제
			PostQuitMessage(0);
			return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//네트웍 통신 환경을 위한 메세지 처리 함수================================================
LRESULT CALLBACK ConnectSer(HWND hwndconn, UINT message, WPARAM wParam, LPARAM lParam)//DialogCallback함수
{
	switch (message)
	{
		case WM_INITDIALOG:
			//Dialog Address창에 디폴트로 127.0.0.1전달
			SetDlgItemText(hwndconn, SER_IPADDRESS1, "127.0.0.1");
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case SER_IP_OK:
					//입력한 ip를 szIP_ADDRESS 배열로 가져온다.
					GetDlgItemText(hwndconn, SER_IPADDRESS1, szIP_ADDRESS, 20);	
	
					//IP입력후 OK버튼을 누르면 윈도우 기본 함수에서 이 값을 체크하여 연결을 시도하거나 취소한다.
					IP_OK = TRUE;	// OK버튼 클릭시 발생하는 플레그
					
					//IP를 입력받는 다이얼로그 닫기
					EndDialog(hwndconn, 0);	  

					return TRUE; //이 반환값을 connect버튼이 클릭되었을때 전달

				case SER_IP_CANCEL:
					//IP를 입력받는 다이얼로그 닫기
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
//로그인 처리함수
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
		
			outFile1 <<"[로그인 정보]"<< endl <<"ID : " << Login_ID << endl << "PW : " << Login_PW << endl;

			MessageBox(hwndconn, "회원가입이 되셨습니다!!", "Login", MB_OK);
			//입력받는 다이얼로그 닫기
			EndDialog(hwndconn, 0);

			return TRUE; 

		case IDCANCEL:
			//입력받는 다이얼로그 닫기
			EndDialog(hwndconn, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//채팅창에 내용을 입력하고 엔터키를 눌렀을때 채팅메세지를 전송하고 뛰워주는 함수 
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
