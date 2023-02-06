
#include "5_mok_client.h"   
#include <winsock.h>
#include <string>
#include <stdlib.h>
#pragma once
TCHAR ChatMSG[300];	//채팅용 메시지

#define WM_SOCKET WM_USER + 1	//소켓 이벤트(사용자가 임의로 정의한 메시지)
int PORT = 9999;	 //통신 포트

/* 채팅 입력 에디트 박스 서브클래싱 */
WNDPROC OldEditProc;

int m_MySPOT;		//자신의 돌색
BOOL m_bGameStart;  //게임 시작 여부

TCHAR szIP_ADDRESS[20];	//주소값을 저장할 배열
BOOL IP_OK = FALSE;	//ip 입력 여부 확인

TCHAR Login_ID[20];	//아이디값을 저장할 배열
TCHAR Login_PW[20];	//패스워드값을 저장할 배열

void RecvMessage();  //받은 메시지 처리 함수

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

TCHAR buf[300];

void SetStart(int MySpot)
{
	ClearBoard();

	m_bGameStart = TRUE;	//게임 시작 플레그
	m_MySPOT = MySpot;		//클라이언트 및 서버의 돌색깔 설정

	InvalidateRect(hWndMain,NULL,TRUE);
}


BOOL CheckMyTurn()	//자신의 턴이 맞으면 리턴 TRUE
{
	if(m_bGameStart == TRUE && m_MySPOT == PlayerTurn)
		return TRUE;
	else
		return FALSE;
}


void SetDollDown(int Y_row, int X_col)
{
	//검색을 하여 바둑판에 3이 있으면 1로, 4가 있으면 2로 변환해 준다
	//3은 포인터흑 1은 그냥 흑, 4는 포인터백, 2는 그냥백
	for(int Y_r=0; Y_r < BadukPan_ROWS; Y_r++) // y축
		for(int X_c=0; X_c < BadukPan_COLS; X_c++) // x축
		{		   
			if(board[Y_r][X_c]==3)
				board[Y_r][X_c]=1;
			else if(board[Y_r][X_c]==4)
				board[Y_r][X_c]=2;
		}

	board[Y_row][X_col]=PlayerTurn;

	if(PlayerTurn == BLACK_SPOT)
	{
		PlayerTurn = WHITE_SPOT;
		Turn_Flag = 2;  //이미지 반전을 위해
	}
	else
	{
		PlayerTurn = BLACK_SPOT;
		Turn_Flag = 1;
	}

	SetRect(&rect, X_col*30, Y_row*30, X_col*30+30, Y_row*30+30);	//바둑알 놓은 곳의 좌표 계산
	InvalidateRect(hWndMain,&rect,FALSE);

	//포인터 바득알을 그냥 바둑알로 새로 그려준다
	InvalidateRect(hWndMain,&prerect,FALSE);
	SetRect(&prerect, rect.left, rect.top, rect.right, rect.bottom);

	SetRect(&rect, 465, 10, 565, 115);		//흑캐릭창 부분
	InvalidateRect(hWndMain,&rect,FALSE);

	SetRect(&rect, 570, 10, 670, 115);		//백 캐릭창 부분
	InvalidateRect(hWndMain,&rect,FALSE);

	CheckGameOver(X_col, Y_row);
}

void Net_OnDollDown(int x, int y)
{
	if(CheckMyTurn()==FALSE)
		return;

	//클릭된 포인트의 오목판 좌표 계산
	//마우스 클릭된 x,y좌표를 픽셀의 넓이로 나누면 인트로 선언된 변수에 정수값이 저장된다.
	//정확한 좌표가 아니라도 주위를 클릭하면 나누어 나온 정수값의 위치에 이미지 출력
	 X_col = x / INTERVAL;  
	Y_row = y / INTERVAL;

	//클릭된 위치가 오목판을 벗어났는지 확인
	//오목판을 벗어났다면 아무런 동작도 일어나지 않는다.
	//cols와 rows를 15로 정의되어 있음 따라서 0부터 시작하기 때문에 -1을 해줌
	if((X_col < 0 || X_col > BadukPan_COLS-1) || (Y_row < 0 || Y_row > BadukPan_ROWS-1))
		return;

	//만일 보드좌표의 값이 0이라면, 상대편에 x,y의 좌표값을 전송하고, 자기화면에도 그려준다
	if(board[Y_row][X_col]==0)	
	{
		_itoa(X_col*1000 + Y_row, buf, 10);	//x, y값 변환하여 한번에 전송
		send(m_sockClient, buf, 10, 0);		//버퍼에 10크기만큼 전송
		SetDollDown(Y_row, X_col);				//전송 후 자신도 setdollDown 호출
	}

}

//채팅창에 글자를 뿌려주는 함수================================================
void DisplayText()
{
	int nLength = GetWindowTextLength(ChatBox);
	SendMessage(ChatBox, EM_SETSEL, nLength, nLength);		//캐럴을 끝으로 이동
	SendMessage(ChatBox, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)ChatMSG);	//선택영역을 ChatMSG로 대체(여기서는 캐럴부분에 삽입)		
}

//돌의 정보및 채팅정보를 수신하는 함수===========================================
void RecvMessage()
{
	TCHAR recvbuff[300];		//수신용 버퍼
	int retval, pos, x_x, y_y;		
	
	//수신 처리
	retval = recv(m_sockClient, recvbuff, 300, 0);	//recvbuff로 300크기 만큼 수신

	if(recvbuff[0] == ':')//받은 메시지가 채팅메시지라면
	{			
		char temp[300];
		//다시 출력하기 위해 템프에 ':'식별자를 빼고 저장 
		for(int i=0, j=1; i<300; i++, j++)
		{
			temp[i] = recvbuff[j];
		}
		int len = strlen(temp);
		temp[len] = '\0';
		wsprintf(ChatMSG,"[Enemy]%s\r\n",temp);
		DisplayText();		// 채팅창에 디스플레이		
		return;
	}

	// 상대편이 재게임 가능 상태인지 검사(게임승패메세지를 확인했는지)
	if(strncmp(recvbuff, "ready",5) == 0)
	{
		flag_Ready = TRUE;
		return;
	}

	pos = atoi(recvbuff);	//입력 값 int 변환
	y_y = pos % 1000;		//x, y값 분리 연산
	x_x = (pos - y_y) / 1000;
	
	SetDollDown(y_y, x_x);

	return;
}

// 채팅 버튼 메시지 처리
void Chatting()
{
	// 클라이언트 소켓
	char text[128];
	char temp[128];

	SendMessage(ChatBoxSub, WM_GETTEXT, 128, (LPARAM)text);	// 채팅 입력창 문자를 가져온다.
	if(!(strlen(text)))
	{
		// 메시지를 입력하지 않은 경우
		return;
	}

	else
	{			
		wsprintf(ChatMSG,"[%s]%s\r\n",Login_ID,text);
		DisplayText();
		// 전송할 내용을 1칸뒤로 물려 temp에 복사하고 그칸에 채팅 메시지를 알리는 ':'문자 삽입
		for(int i=0, j=1; i<128; i++, j++)
			temp[j] = text[i];

		temp[0] = ':';  //채팅내용과 바둑돌의 정보 내용을 분리하기 위해 임의로 1번째인덱스에 식별자를 넣는다

		send(m_sockClient, temp, 128, 0);			// 채팅 메시지 보냄

		// 메세지를 저장할 버퍼 초기화
		for(int i=0; i<128; i++)
		{
			temp[i]=0;
			text[i]=0;
		}
	}
	SetWindowText(ChatBoxSub, "");				// 채팅창 초기화
	SetFocus(ChatBoxSub);						// 포커스를 채팅 입력창으로

	return;
}