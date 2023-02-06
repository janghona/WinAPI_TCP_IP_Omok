
#define BadukPan_ROWS 15	//������ ������
#define BadukPan_COLS 15	//������ �హ��
#define INTERVAL 30	//������ ����
#define BadukPan_TOP  15	//������ ��� ���� ��ġ
#define BadukPan_LEFT 15	//������ ���� ���� ��ġ
#define WHITE_SPOT 3	//�鵹1
#define BLACK_SPOT 4	//�浹2

#define WIDTH 30       //������ũ�⸦ ����ϱ� ���� ���Ǵ� ��Ʈ���� ũ�� ����
#define HIGHT 30       //��� 1���� 30*30�ȼ�

// ��ư �ν��Ͻ� ����
#define St_But  1
#define Ser_But 2
#define Con_But 3
#define Ex_But  4

// ���̾�α׹ڽ�
#define Chat_Dig 201
#define Chat_DigSub 202

//�� ��ư �� ��Ʈ�� �ڵ�
HWND  StButton, SerButton, ConButton, Exbutton, ChatBox, ChatBoxSub;		

//����Ʈ �ν��Ͻ� ����
enum{ID_EDIT=101,ID_EDIT2,ID_EDIT3,ID_EDIT4};
HWND  hEdit1, hEdit2, hEdit3, hEdit4;

static HBITMAP	Badukpan;		// �ٵ��Ǹ�� 
static HBITMAP	Back_Image;		// ���ȭ��
static HBITMAP	start_Image;		// �ʱ�ȭ��
static HBITMAP	Black_Dol;      // ������ �̹��� ����
static HBITMAP	White_Dol;		// �Ͼᵹ �̹��� ����
static HBITMAP	Black_Dol_Point;   // �������� ������ �̹��� ����
static HBITMAP	White_Dol_Point;	  // �������� �Ͼᵹ �̹��� ����
static HBITMAP	Black_Char;     // ������ �÷��̾� �ɸ���
static HBITMAP	White_Char;     // �Ͼᵹ �÷��̾� ĳ����


//���и� �ľ��ϱ� ����
static int TotalGameCount = 0;   //���Ӽ�����
static int W_WinGameCount = 0;   // ��¼�����
static int W_LoseGameCount = 0;  //���м�����
static int B_WinGameCount = 0;   //��¼�����
static int B_LoseGameCount = 0;  //���м�����
static int Cur_W_WinGameCount = 0;   // ������ ��¼�
static int Cur_W_LoseGameCount = 0;  // ������ ���м�
static int Cur_B_WinGameCount = 0;   // ������ ��¼�
static int Cur_B_LoseGameCount = 0;  // ������ ���м�

static BOOL Game_Select; //���Ӽ���

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int PlayerTurn; //���� �÷��̾� ����
int board[BadukPan_ROWS][BadukPan_COLS];	//������ �迭
HWND hWnd, hWndMain;
int Y_row, X_col;
static int Turn_Flag = 0;  //���� ���� ������ ������ �̹��� ������ ����
static BOOL stFlag;			//start�� ������ �߻� �÷���
static BOOL EndFlag;		//��������� �߻� �÷���
static BOOL Semi_stFlag;  //����ȭ�鿡 ������ ���ӽ����� ���� ���콺 Ŭ���� ���� �������� ���콺 Ŭ���� �����ϱ� ����

static BOOL flag_Ready=TRUE;	//������ ����� ���ɿ��� ����
int Yr, Xc;

RECT rect;	//ȭ�� ���� ������ ����
static RECT prerect;  //ȭ�� ���� ������ ����- �������������� �Ϲݵ��� ��ȯ�Ǵ�

SOCKET m_sockClient; //Ŭ���̾�Ʈ�� ���� 

//���� ������ �迭�� ��� 0���� �ʱ�ȭ
void ClearBoard()	
{
	for(Y_row=0; Y_row < BadukPan_ROWS; Y_row++) // y��
		for(X_col=0; X_col < BadukPan_COLS; X_col++) // x��
			board[Y_row][X_col]=0; // �ʱⰪ�� 0�̴� �� �ƹ����� �ȳ����� ����

	//���۵� ���� - �������� ���� ����
	PlayerTurn = BLACK_SPOT;

	//���� ���۽� ������ ������ ������ �д�. �����̹��� ������ ����
	Cur_W_WinGameCount = W_WinGameCount;   // ������ ��¼�
	Cur_W_LoseGameCount = W_LoseGameCount;  // ������ ���м�
	Cur_B_WinGameCount = B_WinGameCount;   // ������ ��¼�
	Cur_B_LoseGameCount = B_LoseGameCount;  // ������ ���м�

}


void DrawBoard(HDC hdc, HDC MemDC)
{
	//������ �׸���.
	for(Yr = 0; Yr < BadukPan_ROWS; Yr++)		//r:row
	{
		for(Xc = 0; Xc < BadukPan_COLS; Xc++)	//c:col
		{
			switch(board[Yr][Xc])	//���� ����
			{
				//�鵹(2)�� �׷��ش� 4-2
				case WHITE_SPOT-2:	
					SelectObject(MemDC, White_Dol);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//�鵹 ȭ��ǥ��
					if(Turn_Flag == 1)
					{
						SelectObject(MemDC, White_Char);  //��ĳ���� ȭ��ǥ��
						BitBlt(hdc, 615, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;

				//�浹(1)�� �׷��ش� 3-2
				case BLACK_SPOT-2:	
					SelectObject(MemDC, Black_Dol);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//�浹 ȭ��ǥ��
					if(Turn_Flag == 2)
					{
						SelectObject(MemDC, Black_Char);  //��ĳ���� ȭ��ǥ��
						BitBlt(hdc, 495, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;

				//�鵹����Ʈ(4)�� �׷��ش�
				case WHITE_SPOT:	
					SelectObject(MemDC, White_Dol_Point);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//�鵹 ȭ��ǥ��
					if(Turn_Flag == 1)
					{
						SelectObject(MemDC, White_Char);  //��ĳ���� ȭ��ǥ��
						BitBlt(hdc, 615, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;

				//�浹����Ʈ(3)�� �׷��ش�
				case BLACK_SPOT:	
					SelectObject(MemDC, Black_Dol_Point);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//�浹 ȭ��ǥ��
					if(Turn_Flag == 2)
					{
						SelectObject(MemDC, Black_Char);  //��ĳ���� ȭ��ǥ��
						BitBlt(hdc, 495, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;
		
				default:
					break;
			}
		}
	}
}

//�ٵϾ��� �� �� ������ ���� �˻�
BOOL NotToSpotCheck()
{
	for(Y_row=0; Y_row < BadukPan_ROWS; Y_row++) // y��
		for(X_col=0; X_col < BadukPan_COLS; X_col++) // x��
		{
			if(board[Y_row][X_col]==0)
				return FALSE;
		}
		return TRUE; // �� �Ѱ��� ������
}


void CheckGameOverSub(int cur_PlayerSpot, int col, int row, int colIncrease, int rowIncrease)
{
	int i;
	int count =1;	//���ӵ� ����(�ϴ� ���絹�� �����ϱ� 1���� ����)

	int curRow, curCol;

	//���絹�� ���� ��ġ�� �������� -������ ���ӵ� �� ī��Ʈ
	for(i = -1; i > -6; i--)
	{
		curRow = row + (i * rowIncrease);
		curCol = col + (i * colIncrease);

		//�������� ������� break;
		if(curRow < 0 || curRow > BadukPan_ROWS - 1 
			|| curCol < 0 || curCol > BadukPan_COLS - 1)
			break;

		//�� ���̸� +1(�����͸� ���� 3,4�϶��� �����͸� �Ȱ��� 1,2�϶�), �ƴϸ� break;
		if(board[curRow][curCol]==cur_PlayerSpot || board[curRow][curCol]==cur_PlayerSpot-2)
			count++;
		else
			break;
	}

	//���絹�� ���� ��ġ�� �������� +������ ���ӵ� �� ī��Ʈ
	for(i = 1; i < 6; i++)
	{
		curRow = row + (i * rowIncrease);
		curCol = col + (i * colIncrease);

		//�������� ������� break;
		if(curRow < 0 || curRow > BadukPan_ROWS - 1 
			|| curCol < 0 || curCol > BadukPan_COLS - 1)   
			break;

		//�� ���̸� +1(�����͸� ���� 3,4�϶��� �����͸� �Ȱ��� 1,2�϶�), �ƴϸ� break;
		if(board[curRow ][curCol]==cur_PlayerSpot || board[curRow ][curCol]==cur_PlayerSpot-2)
			count++;
		else
			break;
	}
	//�ٵϾ��� �� �ξ ������ ������� ������(true�϶�) �޼�������==============
	if(NotToSpotCheck())	
	{
		MessageBox(hWndMain,TEXT("�����ϴ�."),TEXT("���º�"), NULL);
		TotalGameCount++;
		ClearBoard();
		InvalidateRect(hWndMain,NULL,TRUE);
		return;
	}

	if(count==5)	//���ӵ� ���� ������ 5�̸� ���� ����
	{
		flag_Ready = FALSE;
		if(cur_PlayerSpot==WHITE_SPOT) //���� �÷��̾ ���϶� ��¸�
		{
			TotalGameCount++;
			W_WinGameCount++;
			B_LoseGameCount++;
		
			InvalidateRect(hWndMain,NULL,TRUE);
			MessageBox(hWndMain,TEXT("�鵹�� �¸��Ͽ����ϴ�."),TEXT("���� ����"), NULL);
		}
		else    //���� �÷��̾ ���϶� ��¸�
		{
			TotalGameCount++;
			B_WinGameCount++;
			W_LoseGameCount++;
		
			InvalidateRect(hWndMain,NULL,TRUE);
			MessageBox(hWndMain,TEXT("�浹�� �¸��Ͽ����ϴ�."),TEXT("���� ����"), NULL);
		}

		if(Game_Select == TRUE)
		{
			flag_Ready = TRUE;
			
			ClearBoard();	
		}

		else if(Game_Select == FALSE)
		{
			ClearBoard();  //�ٵ����� �ʱ�ȭ�����ش�.
			send(m_sockClient, "ready", 6, 0);	//����� �غ�Ϸ� �Ǿ����� ��뿡�� ����
		}
		InvalidateRect(hWndMain,NULL,TRUE);
	}
}



void CheckGameOver(int col, int row)
{
	//������ �������� ���� �˻�.
	int cur_PlayerSpot = board[row][col];	//���� �÷��̾� �� ����

	CheckGameOverSub(cur_PlayerSpot, col, row, 1, 0); //�� : colIncrease 1, rowIncrease 0
	CheckGameOverSub(cur_PlayerSpot, col, row, 0, 1); //�� : colIncrease 0, rowIncrease 1
	CheckGameOverSub(cur_PlayerSpot, col, row, 1, 1); //�� : colIncrease 1, rowIncrease 1
	CheckGameOverSub(cur_PlayerSpot, col, row, 1, -1);//�� : colIncrease 1, rowIncrease -1
}

void OnDollDown(int x, int y)
{
	//�˻��� �Ͽ� �ٵ��ǿ� 3�� ������ 1��, 4�� ������ 2�� ��ȯ�� �ش�
	//3�� �������� 1�� �׳� ��, 4�� �����͹�, 2�� �׳ɹ�
	for(Y_row=0; Y_row < BadukPan_ROWS; Y_row++) // y��
		for(X_col=0; X_col < BadukPan_COLS; X_col++) // x��
		{
			if(board[Y_row][X_col]==3)
				board[Y_row][X_col]=1;
			else if(board[Y_row][X_col]==4)
				board[Y_row][X_col]=2;
		}

	//Ŭ���� ����Ʈ�� ������ ��ǥ ���
	//���콺 Ŭ���� x,y��ǥ�� �ȼ��� ���̷� ������ ��Ʈ�� ����� ������ �������� ����ȴ�.
	//��Ȯ�� ��ǥ�� �ƴ϶� ������ Ŭ���ϸ� ������ ���� �������� ��ġ�� �̹��� ���
	X_col = x / INTERVAL;  
	Y_row = y / INTERVAL;

	//Ŭ���� ��ġ�� �������� ������� Ȯ��
	//cols�� rows�� 15�� ���ǵǾ� ���� ���� 0���� �����ϱ� ������ -1�� ����
	if((X_col < 0 || X_col > BadukPan_COLS-1) || (Y_row < 0 || Y_row > BadukPan_ROWS-1))
		return;

	//�� ���� ������ Turn�Ѵ�
	//board[row][col]���� 0, 1, 2�߿� �ϳ��� ���� ����ȴ�.(���� ���� ������ 3,4)
	if(board[Y_row][X_col]==0)	
	{
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

		//��ü ȭ�� ���ڰŸ� ������ ���� �ٵϾ� ���� ���� ��ǥ ��� �׺κи� ���� �׷��ش�
		SetRect(&rect, X_col*30, Y_row*30, X_col*30+30, Y_row*30+30);	
		InvalidateRect(hWndMain,&rect,FALSE);

		//������ �ٵ���� �׳� �ٵϾ˷� ���� �׷��ش�
		InvalidateRect(hWndMain,&prerect,TRUE);
		SetRect(&prerect, rect.left, rect.top, rect.right, rect.bottom);

		SetRect(&rect, 465, 10, 565, 115);		//��ĳ��â �κ�
		InvalidateRect(hWndMain,&rect,FALSE);

		SetRect(&rect, 570, 10, 670, 115);		//�� ĳ��â �κ�
		InvalidateRect(hWndMain,&rect,FALSE);

		CheckGameOver(X_col, Y_row);
	}

}