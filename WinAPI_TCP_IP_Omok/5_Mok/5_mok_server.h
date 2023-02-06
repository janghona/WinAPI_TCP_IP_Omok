
#include "5_mok_client.h"   
#include <winsock.h>
#include <string>
#include <stdlib.h>
#pragma once
TCHAR ChatMSG[300];	//ä�ÿ� �޽���

#define WM_SOCKET WM_USER + 1	//���� �̺�Ʈ(����ڰ� ���Ƿ� ������ �޽���)
int PORT = 9999;	 //��� ��Ʈ

/* ä�� �Է� ����Ʈ �ڽ� ����Ŭ���� */
WNDPROC OldEditProc;

int m_MySPOT;		//�ڽ��� ����
BOOL m_bGameStart;  //���� ���� ����

TCHAR szIP_ADDRESS[20];	//�ּҰ��� ������ �迭
BOOL IP_OK = FALSE;	//ip �Է� ���� Ȯ��

TCHAR Login_ID[20];	//���̵��� ������ �迭
TCHAR Login_PW[20];	//�н����尪�� ������ �迭

void RecvMessage();  //���� �޽��� ó�� �Լ�

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

TCHAR buf[300];

void SetStart(int MySpot)
{
	ClearBoard();

	m_bGameStart = TRUE;	//���� ���� �÷���
	m_MySPOT = MySpot;		//Ŭ���̾�Ʈ �� ������ ������ ����

	InvalidateRect(hWndMain,NULL,TRUE);
}


BOOL CheckMyTurn()	//�ڽ��� ���� ������ ���� TRUE
{
	if(m_bGameStart == TRUE && m_MySPOT == PlayerTurn)
		return TRUE;
	else
		return FALSE;
}


void SetDollDown(int Y_row, int X_col)
{
	//�˻��� �Ͽ� �ٵ��ǿ� 3�� ������ 1��, 4�� ������ 2�� ��ȯ�� �ش�
	//3�� �������� 1�� �׳� ��, 4�� �����͹�, 2�� �׳ɹ�
	for(int Y_r=0; Y_r < BadukPan_ROWS; Y_r++) // y��
		for(int X_c=0; X_c < BadukPan_COLS; X_c++) // x��
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
		Turn_Flag = 2;  //�̹��� ������ ����
	}
	else
	{
		PlayerTurn = BLACK_SPOT;
		Turn_Flag = 1;
	}

	SetRect(&rect, X_col*30, Y_row*30, X_col*30+30, Y_row*30+30);	//�ٵϾ� ���� ���� ��ǥ ���
	InvalidateRect(hWndMain,&rect,FALSE);

	//������ �ٵ���� �׳� �ٵϾ˷� ���� �׷��ش�
	InvalidateRect(hWndMain,&prerect,FALSE);
	SetRect(&prerect, rect.left, rect.top, rect.right, rect.bottom);

	SetRect(&rect, 465, 10, 565, 115);		//��ĳ��â �κ�
	InvalidateRect(hWndMain,&rect,FALSE);

	SetRect(&rect, 570, 10, 670, 115);		//�� ĳ��â �κ�
	InvalidateRect(hWndMain,&rect,FALSE);

	CheckGameOver(X_col, Y_row);
}

void Net_OnDollDown(int x, int y)
{
	if(CheckMyTurn()==FALSE)
		return;

	//Ŭ���� ����Ʈ�� ������ ��ǥ ���
	//���콺 Ŭ���� x,y��ǥ�� �ȼ��� ���̷� ������ ��Ʈ�� ����� ������ �������� ����ȴ�.
	//��Ȯ�� ��ǥ�� �ƴ϶� ������ Ŭ���ϸ� ������ ���� �������� ��ġ�� �̹��� ���
	 X_col = x / INTERVAL;  
	Y_row = y / INTERVAL;

	//Ŭ���� ��ġ�� �������� ������� Ȯ��
	//�������� ����ٸ� �ƹ��� ���۵� �Ͼ�� �ʴ´�.
	//cols�� rows�� 15�� ���ǵǾ� ���� ���� 0���� �����ϱ� ������ -1�� ����
	if((X_col < 0 || X_col > BadukPan_COLS-1) || (Y_row < 0 || Y_row > BadukPan_ROWS-1))
		return;

	//���� ������ǥ�� ���� 0�̶��, ����� x,y�� ��ǥ���� �����ϰ�, �ڱ�ȭ�鿡�� �׷��ش�
	if(board[Y_row][X_col]==0)	
	{
		_itoa(X_col*1000 + Y_row, buf, 10);	//x, y�� ��ȯ�Ͽ� �ѹ��� ����
		send(m_sockClient, buf, 10, 0);		//���ۿ� 10ũ�⸸ŭ ����
		SetDollDown(Y_row, X_col);				//���� �� �ڽŵ� setdollDown ȣ��
	}

}

//ä��â�� ���ڸ� �ѷ��ִ� �Լ�================================================
void DisplayText()
{
	int nLength = GetWindowTextLength(ChatBox);
	SendMessage(ChatBox, EM_SETSEL, nLength, nLength);		//ĳ���� ������ �̵�
	SendMessage(ChatBox, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)ChatMSG);	//���ÿ����� ChatMSG�� ��ü(���⼭�� ĳ���κп� ����)		
}

//���� ������ ä�������� �����ϴ� �Լ�===========================================
void RecvMessage()
{
	TCHAR recvbuff[300];		//���ſ� ����
	int retval, pos, x_x, y_y;		
	
	//���� ó��
	retval = recv(m_sockClient, recvbuff, 300, 0);	//recvbuff�� 300ũ�� ��ŭ ����

	if(recvbuff[0] == ':')//���� �޽����� ä�ø޽������
	{			
		char temp[300];
		//�ٽ� ����ϱ� ���� ������ ':'�ĺ��ڸ� ���� ���� 
		for(int i=0, j=1; i<300; i++, j++)
		{
			temp[i] = recvbuff[j];
		}
		int len = strlen(temp);
		temp[len] = '\0';
		wsprintf(ChatMSG,"[Enemy]%s\r\n",temp);
		DisplayText();		// ä��â�� ���÷���		
		return;
	}

	// ������� ����� ���� �������� �˻�(���ӽ��и޼����� Ȯ���ߴ���)
	if(strncmp(recvbuff, "ready",5) == 0)
	{
		flag_Ready = TRUE;
		return;
	}

	pos = atoi(recvbuff);	//�Է� �� int ��ȯ
	y_y = pos % 1000;		//x, y�� �и� ����
	x_x = (pos - y_y) / 1000;
	
	SetDollDown(y_y, x_x);

	return;
}

// ä�� ��ư �޽��� ó��
void Chatting()
{
	// Ŭ���̾�Ʈ ����
	char text[128];
	char temp[128];

	SendMessage(ChatBoxSub, WM_GETTEXT, 128, (LPARAM)text);	// ä�� �Է�â ���ڸ� �����´�.
	if(!(strlen(text)))
	{
		// �޽����� �Է����� ���� ���
		return;
	}

	else
	{			
		wsprintf(ChatMSG,"[%s]%s\r\n",Login_ID,text);
		DisplayText();
		// ������ ������ 1ĭ�ڷ� ���� temp�� �����ϰ� ��ĭ�� ä�� �޽����� �˸��� ':'���� ����
		for(int i=0, j=1; i<128; i++, j++)
			temp[j] = text[i];

		temp[0] = ':';  //ä�ó���� �ٵϵ��� ���� ������ �и��ϱ� ���� ���Ƿ� 1��°�ε����� �ĺ��ڸ� �ִ´�

		send(m_sockClient, temp, 128, 0);			// ä�� �޽��� ����

		// �޼����� ������ ���� �ʱ�ȭ
		for(int i=0; i<128; i++)
		{
			temp[i]=0;
			text[i]=0;
		}
	}
	SetWindowText(ChatBoxSub, "");				// ä��â �ʱ�ȭ
	SetFocus(ChatBoxSub);						// ��Ŀ���� ä�� �Է�â����

	return;
}