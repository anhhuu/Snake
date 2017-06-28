// Snack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);

//Trạng thái con rắn (di chuyển theo hướng nào)
enum EStatus
{
	RIGHT, 
	LEFT, 
	UP, 
	DOWN
};

//Toạ độ
struct SCoordinates
{
	//Toạ độ hiện tại
	int x; 
	int y;

	//Toạ độ cũ
	int ox;
	int oy;
};

//Hình dạng con rắn
struct SFigure
{
	//Mảng chứa các toạ độ của các đốt
	SCoordinates sections[SECSIONS]; //SECTIONS = 41
};

//Thức ăn
struct SFood
{
	SCoordinates coordinates; //Lưu giữ toạ độ của food
};

struct SSnake
{
	EStatus status;
	SFigure figure;
	SFood food;

	int numOfSection;  //Số đốt của rắn
	int score; //Điểm
	int level;
	bool isDie; //Biến bool xác định rắn đã die chưa
};

/*
	CÁC HÀM:
*/

//Tô màu chữ
void TextColor(int x);
//Đưa con trỏ đến toạ độ X Y của màn hình console
void GotoXY(short x, short y);
//Đổi size màn hình console
void ResizeConsole(int width, int height);
//Xoá thanh Scroll Bar
void DeleteScrollBar();
//Ẩn hiện con trỏ của màn hình console
void ShowCur(bool CursorVisibility);
//Xoá màn hình
void Clrscr(HANDLE hConsole);
//Vô hiệu hoá việc resize màn hình console trong lúc chơi
void DisabledResizeWinCon();
//Vẽ tường
void DrawFrame();
//Khởi tạo
void Init(SSnake &snake);
//Chọn level game
void ChooseLevel(SSnake &snake);
//Tạo hiệu ứng loading
void Loading();
//Xoá con rắn
void DeleteOldSnake(SSnake snake);
//In thông tin trong lúc play
void PrintInfo(SSnake &snake);
//In giao diện
void Graphics(SSnake &snake);
//Xử lý
void Handle(SSnake &snake);
//Điều khiển rắn
void Direction(SSnake &snake);
//Di chuyển rắn
void Action(SSnake &snake);
//In danh sách chọn khi khởi động game
void ChooseList();
//In hướng dẫn 
void PrintHelp();
//In dòng giới thiệu đầu game
void Welcome();
//Gọi khi vào game
void StartGame(SSnake &snake);
//Xử lý thắng thua
void HandleWinFaile(SSnake &snake);
//Xử lý sau khi thắng hoặc thua
void ContinueIfWinOrFaile(SSnake &snake);
//Khỏi nói -_-(TỰ CHÚ HIỂU)
void mainLoop(SSnake &snake);


void TextColor(int x)
{
	HANDLE _color;
	_color = GetStdHandle

	(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(_color, x);
}

void GotoXY(short x, short y)
{
	HANDLE hConsoleOutput;
	COORD Cursor_an_Pos = { x - 1,y - 1 };
	hConsoleOutput = GetStdHandle

	(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsoleOutput, Cursor_an_Pos);
}

void ResizeConsole(int width, int height)
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void DeleteScrollBar()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	COORD new_size =
	{
		info.srWindow.Right - info.srWindow.Left + 1,
		info.srWindow.Bottom - info.srWindow.Top + 1
	};
	SetConsoleScreenBufferSize(handle, new_size);
}

void ShowCur(bool CursorVisibility)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor = { 1, CursorVisibility };
	SetConsoleCursorInfo(handle, &cursor);
}

void Clrscr(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer. 

	if(!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.

	if(!FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
								   dwConSize, coordScreen, &cCharsWritten))
		return;

	// Get the current text attribute.

	if(!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;

	// Set the buffer's attributes accordingly.

	if(!FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
								   dwConSize, coordScreen, &cCharsWritten))
		return;

	// Put the cursor at its home coordinates.

	SetConsoleCursorPosition(hConsole, coordScreen);
}

void DisabledResizeWinCon()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void DrawFrame()
{
	//Hàm này tự nghiên cứu nhé :D
	GotoXY(4, 4); printf_s("%c", 201);
	GotoXY(4, 25); printf_s("%c", 200);
	GotoXY(70, 4); printf_s("%c", 187);
	GotoXY(70, 25); printf_s("%c", 188);
	for(int i = 5; i < 70; i++)
	{
		GotoXY(i, 4);
		printf_s("%c", 205);
		GotoXY(i, 25);
		printf_s("%c", 205);
	}
	for(int i = 5; i < 25; i++)
	{
		GotoXY(4, i);
		printf_s("%c", 186);
		GotoXY(70, i);
		printf_s("%c", 186);
	}
}

void ChooseLevel(SSnake &snake)
{
	do
	{
		Clrscr(hCon);
		GotoXY(20, 14);
		TextColor(13);
		printf_s("Please! Choose level (1-5) : ");
		fflush(stdin);
		scanf_s("%d", &snake.level);
	} while(snake.level<1 || snake.level>5);

	TextColor(7);
	Clrscr(hCon);
}

void Loading()
{
	TextColor(14);
	Clrscr(hCon);
	GotoXY(24, 16);
	for(int i = 0; i<28; i++)
	{
		printf_s("%c", 177);
		Sleep(60);
	}
	Sleep(200);
	TextColor(7);
	Clrscr(hCon);
}

void DeleteOldSnake(SSnake snake)
{
	for(int i = 0; i < snake.numOfSection; i++)
	{
		GotoXY(snake.figure.sections[i].x, snake.figure.sections[i].x);
		putchar(32);
	}
}

void PrintInfo(SSnake &snake)
{
	GotoXY(32, 2);
	TextColor(10);
	printf_s("SNAKE GAME");

	GotoXY(61, 26);
	TextColor(15);
	printf_s("SCORE: %d", snake.score);

	TextColor(13);
	GotoXY(48, 26);
	printf_s("LEVEL: %d", snake.level);

	GotoXY(20, 3);
	TextColor(5);
	printf_s("Design and code by Pham Minh Anh Huu ");

	GotoXY(4, 26);
	TextColor(6);
	printf_s("Version: 1.0 beta - Console");
	TextColor(7);
}

void Init(SSnake &snake)
{
	srand(static_cast<unsigned int>(time(NULL)));
	snake.status = RIGHT; //Hướng di chuyển được khởi tạo là sang phải
	snake.figure.sections[0].x = 16; //Toạ độ x khởi tạo của đầu rắn là 16
	snake.figure.sections[0].y = 15; //Toạ độ y khởi tạo của đầu rắn là 16
	snake.numOfSection = 2; //Số đốt ban đầu được khởi tạo là 2 bao gồm đầu
	snake.score = 0; //Điểm khởi tạo bằng 0
	snake.isDie = false; //Rắn không chết
	snake.level = 1;

	//Cho toạ độ của thức ăn là ngẫu nhiên
	snake.food.coordinates.x = (rand() % 62) + 6;
	//Vòng lặp buộc toạ độ x của thức ăn phải là chẵn vì khi di chuyển toạ độ x được tăng thêm 2 để khi chạy ngang hoặc dọc rắn chạy đều, muốn thì sửa lại 1 ở Action xem nó ntn -_-
	while(snake.food.coordinates.x % 2 == 1)
		snake.food.coordinates.x = (rand() % 62) + 6;
	snake.food.coordinates.y = (rand() % 19) + 5;
	Clrscr(hCon);
}

void Graphics(SSnake &snake)
{
	TextColor(12);
	DrawFrame(); //Vẽ khung
	TextColor(13);
	
	//Vẽ đầu rắn kí tự chữ "o"
	GotoXY(snake.figure.sections[0].x, snake.figure.sections[0].y);
	putchar('o');
	
	//Vẽ thân rắn
	TextColor(14);
	for(int i = 1; i < snake.numOfSection; i++)
	{
		GotoXY(snake.figure.sections[i].x, snake.figure.sections[i].y);
		putchar(254);
	}
	
	//Xoá phần đuôi cuối con rắn khi đầu di chuyển lên 1 ô, để tránh dùng xoá màn hình
	//Đoạn này quan trong nhất trong Game, thử comment 2 dòng phía dưới rồi xem _-_
	GotoXY(snake.figure.sections[snake.numOfSection - 1].ox, snake.figure.sections[snake.numOfSection - 1].oy);
	putchar(32);
	
	//Đơn giản là in thức ăn :v
	GotoXY(snake.food.coordinates.x, snake.food.coordinates.y);
	TextColor(rand() % 15);
	putchar(254);
	TextColor(7);

	PrintInfo(snake);
}

void Handle(SSnake &snake)
{
	//Xử lý khi ăn thức ăn
	if((snake.figure.sections[0].x == snake.food.coordinates.x) && (snake.figure.sections[0].y == snake.food.coordinates.y))
	{
		//Tuỳ theo level để tăng điểm
		switch(snake.level)
		{
			case 1:
				snake.score += 1;
				break;
			case 2:
				snake.score += 2;
				break;
			case 3:
				snake.score += 3;
				break;
			case 4:
				snake.score += 4;
				break;
			case 5:
				snake.score += 5;
				break;
		}
		snake.numOfSection++; //Tăng đốt lên 1
		//Lưu toạ độ cũ của thức ăn
		snake.food.coordinates.ox = snake.food.coordinates.x; 
		snake.food.coordinates.oy = snake.food.coordinates.y;
		//Random lại thức ăn
		snake.food.coordinates.x = (rand() % 62) + 6;
		while(snake.food.coordinates.x % 2 == 1)
		{
			snake.food.coordinates.x = (rand() % 62) + 6;
		}
		snake.food.coordinates.y = (rand() % 19) + 5;
		//Xoá thức ăn cũ tại toạ độ đã lưu
		GotoXY(snake.food.coordinates.ox = snake.food.coordinates.ox, snake.food.coordinates.ox = snake.food.coordinates.oy);
		putchar(32);
	}
}

void Direction(SSnake &snake)
{
	int i = 0;
	do
	{
		fflush(stdin); //Xoá bộ đệm
		if(_kbhit()) //Nhận bàn phím
		{
			char c = _getch();
			if(c == 72 || c == 'w' || c == 'W') //Ấn W hoặc mũi tên lên thì rắn đi lên
			{
				if(snake.status == DOWN) //Nếu đang đi xuống thì không được đi ngược lại, các dòng dưới tương tự
				{
					snake.status = DOWN;
				} else
				{
					snake.status = UP;
				}
			} else if(c == 75 || c == 'a' || c == 'A')
			{
				if(snake.status == RIGHT)
				{
					snake.status = RIGHT;
				} else
				{
					snake.status = LEFT;
				}
			} else if(c == 77 || c == 'd' || c == 'D')
			{
				if(snake.status == LEFT)
				{
					snake.status = LEFT;
				} else
				{
					snake.status = RIGHT;
				}
			} else if(c == 80 || c == 's' || c == 'S')
			{
				if(snake.status == UP)
				{
					snake.status = UP;
				} else
				{
					snake.status = DOWN;
				}
			} else if(c == 'p' || c == 'P')//Ấn 'P' thì dừng màn hình.
			{
				_getch();
			} else if	(c == 'q' || c == 'Q')//Ấn 'Q' thì thoát.
			{
				snake.isDie = true;
				Clrscr(hCon);
			}
		}
		i++;
	} while(i < 3); //Vòng lặp 3 lần để ấn phím nhạy hơn, bỏ vòng lặp rồi thưởng thức -_-
}

void Action(SSnake &snake)
{
	for(int i = 0; i < snake.numOfSection; i++)
	{
		if(i == 0)
		{
			//Di chuyển đầu rắn
			
			//Lưu toạ độ phần đầu
			snake.figure.sections[i].ox = snake.figure.sections[i].x; 
			snake.figure.sections[i].oy = snake.figure.sections[i].y;
			if(snake.status == LEFT)
			{
				snake.figure.sections[i].x -= 2;  //Nếu trạng thái là rắn đi sang trái thì trừ toạ độ x đầu đi 2
				if(snake.figure.sections[i].x < 6) //Qua tường
				{
					snake.figure.sections[i].x = 68;
				}
			} else if(snake.status == RIGHT)
			{
				snake.figure.sections[i].x += 2; //Nếu trạng thái là rắn đi sang phải thì cộng toạ độ x đầu đi 2
				if(snake.figure.sections[i].x > 68)
				{
					snake.figure.sections[i].x = 6;
				}
			} else if(snake.status == UP) //Nếu trạng thái là rắn đi lên trên thì trừ toạ độ y đầu đi 1
			{
				snake.figure.sections[i].y--;
				if(snake.figure.sections[i].y < 5)
				{
					snake.figure.sections[i].y = 24;
				}
			} else if(snake.status == DOWN) //Nếu trạng thái là rắn đi xuống dưới thì trừ toạ độ y đầu đi 1
			{
				snake.figure.sections[i].y++;
				if(snake.figure.sections[i].y > 24)
				{
					snake.figure.sections[i].y = 5;
				}
			}
		} else
		{
			//Di chuyển phần thân rắn

			//Lưu toạ độc cũ của các đốt
			snake.figure.sections[i].ox = snake.figure.sections[i].x; 
			snake.figure.sections[i].oy = snake.figure.sections[i].y;
			
			//Toạ độ của đốt mới bằng toạ độ đốt cũ, tạo hiệu ứng di chuyển tiến lên phía trước. Không hiểu ib hỏi anh
			snake.figure.sections[i].x = snake.figure.sections[i - 1].ox; 
			snake.figure.sections[i].y = snake.figure.sections[i - 1].oy;
		}
	}
}

void ChooseList()
{
	TextColor(10);
	GotoXY(30, 12);
	printf_s("SNAKE GAME");

	TextColor(9);
	GotoXY(30, 14);
	printf_s("1. Play");

	GotoXY(30, 15);
	printf_s("2. Help");

	GotoXY(30, 16);
	printf_s("3. Exit");
	TextColor(11);
}

void PrintHelp()
{
	TextColor(13);
	Clrscr(hCon);
	GotoXY(30, 14);
	printf_s("Press P to Pause");
	GotoXY(30, 16);
	printf_s("Press Q to Quit");
	GotoXY(30, 18);
	printf_s("(Press any key to back)");
	TextColor(7);
	_getch();
	Clrscr(hCon);
}

void Welcome()
{
	//Chỉ là tạo hiệu ứng vào game thôi, màu mé xíu haha
	Clrscr(hCon);
	std::string wCome = "Welcome to Snake Game";
	TextColor(14);
	GotoXY(25, 15);
	for(int i = 0; i < wCome.length(); i++)
	{
		char c = wCome[i];
		putchar(c);
		Sleep(50);
		if(_kbhit())
		{
			c = _getch();
			GotoXY(25, 15);
			printf_s("Welcome to Snake Game");
			break;
		}
	}
	Sleep(400);
}

void HandleWinFaile(SSnake &snake)
{
	for(int i = 1; i < snake.numOfSection; i++)
	{
		//Nếu đầu chạm thân thì die, hay toạ độ đầu bằng toạ độ 1 trong các đốt thì die
		if(snake.figure.sections[0].x == snake.figure.sections[i].x && snake.figure.sections[0].y == snake.figure.sections[i].y)
		{
			Graphics(snake); //Mục đích gọi hàm vẽ ở đây để thể hiện chon rắn đã bị die.
			TextColor(3);
			GotoXY(4, 28); printf_s("You died with score is: %d", snake.score);
			ContinueIfWinOrFaile(snake);
			TextColor(7);
		}
	}
	if(snake.numOfSection == 40) //40 đốt thì win, tránh tràn bộ nhớ, haha
	{
		Graphics(snake);
		TextColor(3);
		if(snake.level < 5) //Nếu Level bé hơn 5 thì tăng thêm 1 level
		{
			GotoXY(4, 28); printf_s("Happy, YOU WIN level %d", snake.level);
			GotoXY(4, 29); printf_s("Press any key to continue new level");
			_getch();
			DeleteOldSnake(snake);
			int levelOld = snake.level;
			int scoreOld = snake.score;
			Init(snake);
			snake.level = ++levelOld;
			snake.score = scoreOld;
			Loading();
		} else if(snake.level == 5)
		{
			ContinueIfWinOrFaile(snake);
		}
		TextColor(7);
	}
}

void ContinueIfWinOrFaile(SSnake &snake)
{
	GotoXY(4, 29); printf_s("You want replay: ");
	char c;
	do
	{
		fflush(stdin);
		scanf_s("%c", &c, 1);
		if(c != 'n' && c != 'y' && c != 'N' && c != 'Y')
		{
			GotoXY(4, 30);
			printf_s("[y/n]: ");
		}
	} while(c != 'n' && c != 'y' && c != 'N' && c != 'Y');
	if(c == 'n' || c == 'N') //Chọn N thì trở về menu.
	{
		DeleteOldSnake(snake);
		Clrscr(hCon);
		snake.isDie = true;
	} else //Chọn Y thì khởi tạo lại và vào game
	{
		DeleteOldSnake(snake); //Xoá con rắn cũ đi.
		Clrscr(hCon);
		Init(snake);
		ChooseLevel(snake);
		Loading();
	}
}

void mainLoop(SSnake &snake)
{
	while(!snake.isDie)
	{
		Graphics(snake);
		Direction(snake);
		Handle(snake);
		Action(snake);
		HandleWinFaile(snake);
		Sleep(200/snake.level);
	}
}

void StartGame(SSnake &snake)
{
	Welcome(); //Gọi hàm chào
	Clrscr(hCon);
	do
	{
		int choose = 0;
		do
		{
			Clrscr(hCon);
			ChooseList();
			GotoXY(30, 18);
			printf_s("Your choose: ");
			fflush(stdin);
			scanf_s("%d", &choose);
		} while(choose < 1 || choose > 3);
		switch(choose)
		{
			case 1:
			{
				Init(snake);

				DeleteOldSnake(snake); //Xoá rắn cũ

				ChooseLevel(snake);

				Loading();

				mainLoop(snake);

				break;
			}
			case 2:
			{
				PrintHelp();
				break;
			}
			case 3:
				Clrscr(hCon);
				GotoXY(25, 15);
				TextColor(13);
				printf_s("Good Bye, exiting");
				for(int i = 0; i < 4; i++)
				{
					printf_s("."); Sleep(300);
				}
				snake.isDie = true;
				TextColor(7);
				exit(0);
		}
	} while(true);
}

int main()
{
	SetConsoleTitle(L"Snake Game");
	ResizeConsole(600, 530);
	DeleteScrollBar();
	ShowCur(false);
	DisabledResizeWinCon();
	
	SSnake snake;
	StartGame(snake);
	
	return 0;
}

