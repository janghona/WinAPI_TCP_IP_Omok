
#define BadukPan_ROWS 15	//오목판 열갯수
#define BadukPan_COLS 15	//오목판 행갯수
#define INTERVAL 30	//오목판 간격
#define BadukPan_TOP  15	//오목판 상단 시작 위치
#define BadukPan_LEFT 15	//오목판 좌측 시작 위치
#define WHITE_SPOT 3	//백돌1
#define BLACK_SPOT 4	//흑돌2

#define WIDTH 30       //윈도우크기를 계산하기 위해 사용되는 비트맵의 크기 지정
#define HIGHT 30       //블록 1개가 30*30픽셀

// 버튼 인스턴스 정의
#define St_But  1
#define Ser_But 2
#define Con_But 3
#define Ex_But  4

// 다이얼로그박스
#define Chat_Dig 201
#define Chat_DigSub 202

//각 버튼 및 컨트롤 핸들
HWND  StButton, SerButton, ConButton, Exbutton, ChatBox, ChatBoxSub;		

//에디트 인스턴스 정의
enum{ID_EDIT=101,ID_EDIT2,ID_EDIT3,ID_EDIT4};
HWND  hEdit1, hEdit2, hEdit3, hEdit4;

static HBITMAP	Badukpan;		// 바둑판모양 
static HBITMAP	Back_Image;		// 배경화면
static HBITMAP	start_Image;		// 초기화면
static HBITMAP	Black_Dol;      // 검은돌 이미지 지정
static HBITMAP	White_Dol;		// 하얀돌 이미지 지정
static HBITMAP	Black_Dol_Point;   // 놓아지는 검은돌 이미지 지정
static HBITMAP	White_Dol_Point;	  // 놓아지는 하얀돌 이미지 지정
static HBITMAP	Black_Char;     // 검은돌 플레이어 케릭터
static HBITMAP	White_Char;     // 하얀돌 플레이어 캐릭터


//승패를 파악하기 위해
static int TotalGameCount = 0;   //게임수증가
static int W_WinGameCount = 0;   // 백승수증가
static int W_LoseGameCount = 0;  //백패수증가
static int B_WinGameCount = 0;   //흑승수증가
static int B_LoseGameCount = 0;  //흑패수증가
static int Cur_W_WinGameCount = 0;   // 현재의 백승수
static int Cur_W_LoseGameCount = 0;  // 현재의 백패수
static int Cur_B_WinGameCount = 0;   // 현재의 흑승수
static int Cur_B_LoseGameCount = 0;  // 현재의 흑패수

static BOOL Game_Select; //게임선택

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int PlayerTurn; //현재 플레이어 지정
int board[BadukPan_ROWS][BadukPan_COLS];	//오목판 배열
HWND hWnd, hWndMain;
int Y_row, X_col;
static int Turn_Flag = 0;  //돌을 놓을 순서가 왔을때 이미지 반전을 위해
static BOOL stFlag;			//start를 했을때 발생 플레그
static BOOL EndFlag;		//게임종료시 발생 플레그
static BOOL Semi_stFlag;  //게임화면에 들어갔을시 게임시작을 위한 마우스 클릭과 돌을 놓기위한 마우스 클릭을 구분하기 위해

static BOOL flag_Ready=TRUE;	//상대방의 재게임 가능여부 상태
int Yr, Xc;

RECT rect;	//화면 갱신 사이즈 지정
static RECT prerect;  //화면 갱신 사이즈 지정- 포인터지정돌이 일반돌로 전환되는

SOCKET m_sockClient; //클라이언트용 소켓 

//게임 시작전 배열을 모두 0으로 초기화
void ClearBoard()	
{
	for(Y_row=0; Y_row < BadukPan_ROWS; Y_row++) // y축
		for(X_col=0; X_col < BadukPan_COLS; X_col++) // x축
			board[Y_row][X_col]=0; // 초기값은 0이다 즉 아무돌도 안놓아진 생태

	//시작돌 지정 - 검은돌이 먼저 시작
	PlayerTurn = BLACK_SPOT;

	//게임 시작시 현재의 전적을 저장해 둔다. 승패이미지 반전을 위해
	Cur_W_WinGameCount = W_WinGameCount;   // 현재의 백승수
	Cur_W_LoseGameCount = W_LoseGameCount;  // 현재의 백패수
	Cur_B_WinGameCount = B_WinGameCount;   // 현재의 흑승수
	Cur_B_LoseGameCount = B_LoseGameCount;  // 현재의 흑패수

}


void DrawBoard(HDC hdc, HDC MemDC)
{
	//오목돌을 그린다.
	for(Yr = 0; Yr < BadukPan_ROWS; Yr++)		//r:row
	{
		for(Xc = 0; Xc < BadukPan_COLS; Xc++)	//c:col
		{
			switch(board[Yr][Xc])	//놓인 오목돌
			{
				//백돌(2)을 그려준다 4-2
				case WHITE_SPOT-2:	
					SelectObject(MemDC, White_Dol);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//백돌 화면표시
					if(Turn_Flag == 1)
					{
						SelectObject(MemDC, White_Char);  //백캐릭터 화면표시
						BitBlt(hdc, 615, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;

				//흑돌(1)을 그려준다 3-2
				case BLACK_SPOT-2:	
					SelectObject(MemDC, Black_Dol);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//흑돌 화면표시
					if(Turn_Flag == 2)
					{
						SelectObject(MemDC, Black_Char);  //흑캐릭터 화면표시
						BitBlt(hdc, 495, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;

				//백돌포인트(4)을 그려준다
				case WHITE_SPOT:	
					SelectObject(MemDC, White_Dol_Point);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//백돌 화면표시
					if(Turn_Flag == 1)
					{
						SelectObject(MemDC, White_Char);  //백캐릭터 화면표시
						BitBlt(hdc, 615, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;

				//흑돌포인트(3)을 그려준다
				case BLACK_SPOT:	
					SelectObject(MemDC, Black_Dol_Point);
					BitBlt(hdc, Xc*INTERVAL, Yr*INTERVAL, WIDTH, HIGHT, MemDC, 0, 0, SRCCOPY);//흑돌 화면표시
					if(Turn_Flag == 2)
					{
						SelectObject(MemDC, Black_Char);  //흑캐릭터 화면표시
						BitBlt(hdc, 495, 55, 100, 105, MemDC, 0, 0, SRCCOPY);
					}
					break;
		
				default:
					break;
			}
		}
	}
}

//바둑알을 둘 곳 없는지 여부 검사
BOOL NotToSpotCheck()
{
	for(Y_row=0; Y_row < BadukPan_ROWS; Y_row++) // y축
		for(X_col=0; X_col < BadukPan_COLS; X_col++) // x축
		{
			if(board[Y_row][X_col]==0)
				return FALSE;
		}
		return TRUE; // 더 둘곳이 없으면
}


void CheckGameOverSub(int cur_PlayerSpot, int col, int row, int colIncrease, int rowIncrease)
{
	int i;
	int count =1;	//연속된 갯수(일단 현재돌이 있으니까 1부터 시작)

	int curRow, curCol;

	//현재돌이 놓인 위치를 기준으로 -방향의 연속된 돌 카운트
	for(i = -1; i > -6; i--)
	{
		curRow = row + (i * rowIncrease);
		curCol = col + (i * colIncrease);

		//오목판을 벗어났으면 break;
		if(curRow < 0 || curRow > BadukPan_ROWS - 1 
			|| curCol < 0 || curCol > BadukPan_COLS - 1)
			break;

		//내 돌이면 +1(포인터를 가진 3,4일때와 포인터를 안가진 1,2일때), 아니면 break;
		if(board[curRow][curCol]==cur_PlayerSpot || board[curRow][curCol]==cur_PlayerSpot-2)
			count++;
		else
			break;
	}

	//현재돌이 놓인 위치를 기준으로 +방향의 연속된 돌 카운트
	for(i = 1; i < 6; i++)
	{
		curRow = row + (i * rowIncrease);
		curCol = col + (i * colIncrease);

		//오목판을 벗어났으면 break;
		if(curRow < 0 || curRow > BadukPan_ROWS - 1 
			|| curCol < 0 || curCol > BadukPan_COLS - 1)   
			break;

		//내 돌이면 +1(포인터를 가진 3,4일때와 포인터를 안가진 1,2일때), 아니면 break;
		if(board[curRow ][curCol]==cur_PlayerSpot || board[curRow ][curCol]==cur_PlayerSpot-2)
			count++;
		else
			break;
	}
	//바둑알을 다 두어도 계임이 종료되지 않을시(true일때) 메세지전송==============
	if(NotToSpotCheck())	
	{
		MessageBox(hWndMain,TEXT("비겼습니다."),TEXT("무승부"), NULL);
		TotalGameCount++;
		ClearBoard();
		InvalidateRect(hWndMain,NULL,TRUE);
		return;
	}

	if(count==5)	//연속된 돌의 갯수가 5이면 게임 종료
	{
		flag_Ready = FALSE;
		if(cur_PlayerSpot==WHITE_SPOT) //현재 플레이어가 백일때 백승리
		{
			TotalGameCount++;
			W_WinGameCount++;
			B_LoseGameCount++;
		
			InvalidateRect(hWndMain,NULL,TRUE);
			MessageBox(hWndMain,TEXT("백돌이 승리하였습니다."),TEXT("게임 종료"), NULL);
		}
		else    //현재 플레이어가 흑일때 흑승리
		{
			TotalGameCount++;
			B_WinGameCount++;
			W_LoseGameCount++;
		
			InvalidateRect(hWndMain,NULL,TRUE);
			MessageBox(hWndMain,TEXT("흑돌이 승리하였습니다."),TEXT("게임 종료"), NULL);
		}

		if(Game_Select == TRUE)
		{
			flag_Ready = TRUE;
			
			ClearBoard();	
		}

		else if(Game_Select == FALSE)
		{
			ClearBoard();  //바둑판을 초기화시켜준다.
			send(m_sockClient, "ready", 6, 0);	//재게임 준비완료 되었음을 상대에게 전달
		}
		InvalidateRect(hWndMain,NULL,TRUE);
	}
}



void CheckGameOver(int col, int row)
{
	//게임이 끝났는지 여부 검사.
	int cur_PlayerSpot = board[row][col];	//현재 플레이어 돌 색깔

	CheckGameOverSub(cur_PlayerSpot, col, row, 1, 0); //→ : colIncrease 1, rowIncrease 0
	CheckGameOverSub(cur_PlayerSpot, col, row, 0, 1); //↑ : colIncrease 0, rowIncrease 1
	CheckGameOverSub(cur_PlayerSpot, col, row, 1, 1); //↘ : colIncrease 1, rowIncrease 1
	CheckGameOverSub(cur_PlayerSpot, col, row, 1, -1);//↗ : colIncrease 1, rowIncrease -1
}

void OnDollDown(int x, int y)
{
	//검색을 하여 바둑판에 3이 있으면 1로, 4가 있으면 2로 변환해 준다
	//3은 포인터흑 1은 그냥 흑, 4는 포인터백, 2는 그냥백
	for(Y_row=0; Y_row < BadukPan_ROWS; Y_row++) // y축
		for(X_col=0; X_col < BadukPan_COLS; X_col++) // x축
		{
			if(board[Y_row][X_col]==3)
				board[Y_row][X_col]=1;
			else if(board[Y_row][X_col]==4)
				board[Y_row][X_col]=2;
		}

	//클릭된 포인트의 오목판 좌표 계산
	//마우스 클릭된 x,y좌표를 픽셀의 넓이로 나누면 인트로 선언된 변수에 정수값이 저장된다.
	//정확한 좌표가 아니라도 주위를 클릭하면 나누어 나온 정수값의 위치에 이미지 출력
	X_col = x / INTERVAL;  
	Y_row = y / INTERVAL;

	//클릭된 위치가 오목판을 벗어났는지 확인
	//cols와 rows를 15로 정의되어 있음 따라서 0부터 시작하기 때문에 -1을 해줌
	if((X_col < 0 || X_col > BadukPan_COLS-1) || (Y_row < 0 || Y_row > BadukPan_ROWS-1))
		return;

	//돌 놓는 순서를 Turn한다
	//board[row][col]에는 0, 1, 2중에 하나의 값이 저장된다.(돌을 놓는 순간은 3,4)
	if(board[Y_row][X_col]==0)	
	{
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

		//전체 화면 깜박거림 방지를 위해 바둑알 놓은 곳의 좌표 계산 그부분만 새로 그려준다
		SetRect(&rect, X_col*30, Y_row*30, X_col*30+30, Y_row*30+30);	
		InvalidateRect(hWndMain,&rect,FALSE);

		//포인터 바득알을 그냥 바둑알로 새로 그려준다
		InvalidateRect(hWndMain,&prerect,TRUE);
		SetRect(&prerect, rect.left, rect.top, rect.right, rect.bottom);

		SetRect(&rect, 465, 10, 565, 115);		//흑캐릭창 부분
		InvalidateRect(hWndMain,&rect,FALSE);

		SetRect(&rect, 570, 10, 670, 115);		//백 캐릭창 부분
		InvalidateRect(hWndMain,&rect,FALSE);

		CheckGameOver(X_col, Y_row);
	}

}