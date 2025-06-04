#include <iostream>
#include <easyx.h>
//使用abs绝对值函数
#include<cmath>
//Sleep
#include <Windows.h>
//随机数
#include<cmath>
#include <stdlib.h>
//音乐
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std;

ExMessage msg = { 0 };

//增加双重退出功能，设置showexit可以使得提示图标消失
static bool showexit = true;
static char gamewinner[50];
const int N = 20;
const int black_chese = 1;
const int white_chese = 2;
int board[N][N];
int undoboard[N][N];

void PlayMusic(const char* music);
bool inButtonArea(int mx, int my, int x, int y, int w, int h);
bool button(int x, int y, int w, int h, const char* text);
bool wincheck(int row, int col, int player);
void initboard();
void playervsplayer();
void playervsenvironment();
int evaluate(int board[N][N], int x, int y, int computerPlayer, int humanPlayer);


int main(void)
{
	PlayMusic("assets/music.mp3");
	initgraph(640, 640, EX_NOMINIMIZE);
	setbkcolor(WHITE);
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 3);
	//图片定义、加载、放置
	IMAGE img_background;
	IMAGE img_interface;
	loadimage(&img_interface, "assets/interface.jpg", 640, 640);
	loadimage(&img_background, "assets/background.jpg", 640, 640);
	putimage(0, 0, &img_background);


	while (true)
	{
		if (peekmessage(&msg, EX_MOUSE))
		{
			putimage(0, 0, &img_background);
			if (button(80, 560, 120, 50, "P.V.P."))
			{
				BeginBatchDraw();
				putimage(0, 0, &img_interface);
				settextstyle(50, 0, "微软雅黑");
				outtextxy(15, 15, "P.V.P. mode");
				initboard();
				EndBatchDraw();
				playervsplayer();
				continue;
			}


			if (button(255, 560, 120, 50, "P.V.E."))
			{
				BeginBatchDraw();
				putimage(0, 0, &img_interface);
				settextstyle(50, 0, "微软雅黑");
				outtextxy(15, 15, "P.V.E. mode");
				initboard();
				EndBatchDraw();
				playervsenvironment();
				continue;
			}


			if (button(435, 560, 120, 50, "QUIT"))
			{
				while (showexit)
				{
					if (peekmessage(&msg, EX_MOUSE))
					{
						BeginBatchDraw();
						cleardevice();
						putimage(0, 0, &img_background);
						setfillcolor(RGB(240, 155, 89));
						fillroundrect(130, 200, 500, 400, 20, 20);
						settextstyle(15, 0, "微软雅黑");
						//双重退出
						outtextxy(150, 220, "Are you sure you want to exit the game?");
						if (button(200, 325, 80, 50, "Yes."))
						{
							EndBatchDraw();
							closegraph();
							return 0;
						}
						if (button(330, 325, 80, 50, "No."))
						{
							showexit = false;
						}
						EndBatchDraw();
					}
					Sleep(10);
				}
				cleardevice();
				putimage(0, 0, &img_background);
				EndBatchDraw();
				showexit = true;
			}
			EndBatchDraw();
		}
		Sleep(10);
	}


	getchar();
	return 0;
}

void PlayMusic(const char* music)
{
	char cmd[100] = { 0 };
	// open加载音乐，alias取别名
	sprintf(cmd, "open %s alias bgm", music);
	MCIERROR check = mciSendString(cmd, NULL, 0, NULL);
	if (check != 0)
	{
		char error[50] = { 0 };
		mciGetErrorString(check, error, sizeof(error));
		puts(error);
	}
	//play播放，repeat可以实现循环播放功能
	sprintf(cmd, "play bgm repeat");
	check = mciSendString(cmd, NULL, 0, NULL);
	if (check != 0)
	{
		char error[50] = { 0 };
		mciGetErrorString(check, error, sizeof(error));
		puts(error);
	}
	//这里可以调节声音，范围1-1000
	check = mciSendString("setaudio bgm volume to 500", NULL, 0, NULL);
	if (check != 0)
	{
		char error[50] = { 0 };
		mciGetErrorString(check, error, sizeof(error));
		puts(error);
	}
}

//分开两个函数，方便实现鼠标移动到按钮上变色的效果
//x轴向右，y轴向左
bool inButtonArea(int mx, int my, int x, int y, int w, int h)
{
	if (mx > x && mx < x + w && my > y && my < y + h)
	{
		return true;
	}
	return false;
}
bool button(int x, int y, int w, int h, const char* text)
{
	//实现按钮的变色效果
	if (inButtonArea(msg.x, msg.y, x, y, w, h))
	{
		setfillcolor(RGB(255, 185, 119));
	}
	else
	{
		setfillcolor(RGB(240, 155, 89));
	}
	// 绘制按钮
	fillroundrect(x, y, x + w, y + h, 5, 5);
	// 绘制文本
	setbkmode(TRANSPARENT);
	settextstyle(20, 0, "微软雅黑");
	settextcolor(BLACK);
	int xspace = (w - textwidth(text)) / 2;
	int yspace = (h - textheight(text)) / 2;
	outtextxy(x + xspace, y + yspace, text);
	// 判断是否被点击
	if (msg.message == WM_LBUTTONDOWN && inButtonArea(msg.x, msg.y, x, y, w, h))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void initboard()
{
	//提前设置线的颜色粗细
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 3);
	for (int i = 0; i < N; i++)
	{
		//25*19=475,间隔25
		line(20, 70 + 25 * i, 20 + 25 * 19, 70 + 25 * i);
		for (int j = 0; j < N; j++)
		{
			line(20 + 25 * j, 70, 20 + 25 * j, 70 + 25 * 19);
			board[i][j] = { 0 };
		}
	}
	line(20 + 5 * 25, 70, 20 + 5 * 25, 70);
	//边框加粗
	setlinestyle(PS_SOLID, 6);
	line(20, 70, 20 + 25 * 19, 70);
	line(20, 70, 20, 70 + 25 * 19);
	line(20 + 25 * 19, 70, 20 + 25 * 19, 70 + 25 * 19);
	line(20, 70 + 25 * 19, 20 + 25 * 19, 70 + 25 * 19);
	setlinestyle(PS_SOLID, 3);
	settextcolor(BLACK);
	settextstyle(15, 0, "微软雅黑");
}

//复制前一步
//三个按钮
//如果在键盘范围内左击，下棋子，数组赋值
//最后交换玩家
void playervsplayer()
{
	bool gamestart = true;
	int currentplayer = black_chese;
	while (gamestart)
	{
		const char* currentplayercolor = (currentplayer == 1) ? "BLACK" : "WHITE";
		BeginBatchDraw();
		//伪加粗，符合整个程序像素风格
		settextstyle(15, 0, "微软雅黑");
		outtextxy(515, 80, "Player 1: BLACK");
		outtextxy(516, 81, "Player 1: BLACK");
		outtextxy(515, 120, "Player 2: WHITE");
		outtextxy(516, 121, "Player 2: WHITE");

		//悔棋
		settextstyle(11, 0, "微软雅黑");
		outtextxy(501, 295, "Undomove only available .");
		outtextxy(501, 305, "for one click pre round.");
		if (button(510, 320, 120, 50, "Undomove"))
		{
			cleardevice();
			IMAGE img_interface;
			loadimage(&img_interface, "assets/interface.jpg", 640, 640);
			putimage(0, 0, &img_interface);
			settextstyle(50, 0, "微软雅黑");
			outtextxy(15, 15, "P.V.P. mode");
			initboard();
			currentplayer = (currentplayer == black_chese) ? white_chese : black_chese;
			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
				{
					board[i][j] = undoboard[i][j];
					if (board[i][j] == white_chese)
					{
						setfillcolor(WHITE);
						solidcircle(20 + 25 * i, 70 + 25 * j, 9);
					}
					if (board[i][j] == black_chese)
					{
						setfillcolor(BLACK);
						solidcircle(20 + 25 * i, 70 + 25 * j, 9);
					}
				}
		}
		//投降
		if (button(510, 400, 120, 50, "Surrender"))
		{
			while (gamestart)
			{
				if (peekmessage(&msg, EX_MOUSE))
				{
					BeginBatchDraw();
					setfillcolor(RGB(240, 155, 89));
					fillroundrect(130, 200, 500, 400, 20, 20);
					settextstyle(25, 0, "微软雅黑");
					int winner = (currentplayer == black_chese) ? white_chese : black_chese;
					const char* currentplayercolor = (winner == 1) ? "BLACK" : "WHITE";
					sprintf(gamewinner, "Player %i: %s wins!", winner, currentplayercolor);
					outtextxy(150, 220, gamewinner);
					if (button(275, 325, 80, 50, "Quit"))
					{
						cleardevice();
						IMAGE img_background;
						loadimage(&img_background, "assets/background.jpg", 640, 640);
						putimage(0, 0, &img_background);
						gamestart = false;
						continue;
					}
				}
				EndBatchDraw();
			}
			gamestart = false;
			continue;
		}
		//退出
		if (button(510, 480, 120, 50, "Quit"))
		{
			IMAGE img_background;
			loadimage(&img_background, "assets/background.jpg", 640, 640);
			putimage(0, 0, &img_background);
			gamestart = false;
		}
		EndBatchDraw();
		//鼠标移动
		if (peekmessage(&msg, EX_MOUSE))
		{
			//鼠标左击
			if (msg.message == WM_LBUTTONDOWN)
			{

				for (int i = 0; i < N; i++)
				{
					for (int j = 0; j < N; j++)
					{
						//绘制棋子，12是响应范围，25是间距
						if ((abs(msg.x - 20 - 25 * i) <= 12) && (abs(msg.y - 70 - 25 * j) <= 12))
						{
							if (board[i][j] == 0)
							{
								for(int x=0;x<N;x++)
									for(int y=0;y<N;y++)
										undoboard[x][y] = board[x][y];
								if (currentplayer == black_chese)
								{
									setfillcolor(BLACK);
									board[i][j] = black_chese;
								}
								else if (currentplayer == white_chese)
								{
									setfillcolor(WHITE);
									board[i][j] = white_chese;
								}
								solidcircle(20 + 25 * i, 70 + 25 * j, 9);
							}
							//如果按到已经落过的区域无效，返回下一次循环
							//在此代码前不能改变玩家
							else
							{
								continue;
							}
							if (wincheck(i, j, currentplayer))
							{
								while (gamestart)
								{
									if (peekmessage(&msg, EX_MOUSE))
									{
										BeginBatchDraw();
										setfillcolor(RGB(240, 155, 89));
										fillroundrect(130, 200, 500, 400, 20, 20);
										settextstyle(25, 0, "微软雅黑");
										sprintf(gamewinner, "Player %i: %s wins!", currentplayer, currentplayercolor);
										outtextxy(150, 220, gamewinner);
										//双重退出
										if (button(275, 325, 80, 50, "Quit"))
										{
											cleardevice();
											IMAGE img_background;
											loadimage(&img_background, "assets/background.jpg", 640, 640);
											putimage(0, 0, &img_background);
											gamestart = false;
											continue;
										}
										EndBatchDraw();
									}
								}

							}
							//在一次循环的最后再进行玩家交换
							else
							{
								currentplayer = (currentplayer == black_chese) ? white_chese : black_chese;
							}
						}
					}
				}
			}

		}
		Sleep(10);
	}
}

void playervsenvironment()
{
	bool gamestart = true;
	int currentplayer = black_chese;
	while (gamestart)
	{
		BeginBatchDraw();
		//伪加粗，符合整个程序像素风格
		settextstyle(15, 0, "微软雅黑");
		outtextxy(515, 80, "Player: BLACK");
		outtextxy(516, 81, "Player: BLACK");
		outtextxy(515, 120, "Computer: WHITE");
		outtextxy(516, 121, "Computer: WHITE");
		//悔棋
		settextstyle(11, 0, "微软雅黑");
		outtextxy(501, 295, "Undomove only available .");
		outtextxy(501, 305, "for one click pre round.");
		if (button(510, 320, 120, 50, "Undomove"))
		{
			cleardevice();
			IMAGE img_interface;
			loadimage(&img_interface, "assets/interface.jpg", 640, 640);
			putimage(0, 0, &img_interface);
			settextstyle(50, 0, "微软雅黑");
			outtextxy(15, 15, "P.V.E. mode");
			initboard();
			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
				{
					board[i][j] = undoboard[i][j];
					if (board[i][j] == white_chese)
					{
						setfillcolor(WHITE);
						solidcircle(20 + 25 * i, 70 + 25 * j, 9);
					}
					if (board[i][j] == black_chese)
					{
						setfillcolor(BLACK);
						solidcircle(20 + 25 * i, 70 + 25 * j, 9);
					}
				}
		}
		//投降
		if (button(510, 400, 120, 50, "Surrender"))
		{
			while (gamestart)
			{
				if (peekmessage(&msg, EX_MOUSE))
				{
					BeginBatchDraw();
					setfillcolor(RGB(240, 155, 89));
					fillroundrect(130, 200, 500, 400, 20, 20);
					settextstyle(25, 0, "微软雅黑");
					outtextxy(150, 220, "Computer wins.");
					if (button(275, 325, 80, 50, "Quit"))
					{
						cleardevice();
						IMAGE img_background;
						loadimage(&img_background, "assets/background.jpg", 640, 640);
						putimage(0, 0, &img_background);
						gamestart = false;
						continue;
					}
				}
				EndBatchDraw();
			}
			gamestart = false;
			continue;
		}
		//退出
		if (button(510, 480, 120, 50, "Quit"))
		{
			IMAGE img_background;
			loadimage(&img_background, "assets/background.jpg", 640, 640);
			putimage(0, 0, &img_background);
			gamestart = false;
		}
		EndBatchDraw();
		if (currentplayer == white_chese)
		{
			int a, b;
			int max_score = 0;
			int score = 0;
			srand(time(0));
			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
				{
					if (board[i][j] == 0)
					{
						score = evaluate(board, i, j, white_chese, black_chese);
						if (max_score < score)
						{
							//循环结束最大得分的坐标点传入a,b中
							a = i;
							b = j;
							max_score = score;
						}
						//增加前期相同得分的随机性
						else if (max_score == score)
						{
							int randombit = rand() % 2;
							if (randombit)
							{
								a = i;
								b = j;
							}
						}
					}

				}
			setfillcolor(WHITE);
			board[a][b] = white_chese;

			solidcircle(20 + 25 * a, 70 + 25 * b, 9);
			if (wincheck(a, b, currentplayer))
			{
				while (gamestart)
				{
					if (peekmessage(&msg, EX_MOUSE))
					{
						BeginBatchDraw();
						setfillcolor(RGB(240, 155, 89));
						fillroundrect(130, 200, 500, 400, 20, 20);
						settextstyle(25, 0, "微软雅黑");
						outtextxy(150, 220, "Computer win!");
						if (button(275, 325, 80, 50, "Quit"))
						{
							cleardevice();
							IMAGE img_background;
							loadimage(&img_background, "assets/background.jpg", 640, 640);
							putimage(0, 0, &img_background);
							gamestart = false;
						}
						EndBatchDraw();
					}
				}
			}
			currentplayer = black_chese;
		}
		//鼠标移动
		if (peekmessage(&msg, EX_MOUSE))
		{
			//鼠标左击
			if (msg.message == WM_LBUTTONDOWN)
			{
				for (int i = 0; i < N; i++)
				{
					for (int j = 0; j < N; j++)
					{
						//绘制棋子，8是响应范围，25是间距
						if ((abs(msg.x - 20 - 25 * i) <= 12) && (abs(msg.y - 70 - 25 * j) <= 12))
						{
							if (board[i][j] == 0)
							{
								for(int x=0;x<N;x++)
									for (int y = 0; y < N; y++)
									{
										undoboard[x][y] = board[x][y];
									}
								if (currentplayer == black_chese)
								{
									setfillcolor(BLACK);
									board[i][j] = black_chese;
								}
								solidcircle(20 + 25 * i, 70 + 25 * j, 9);
							}
							//如果按到已经落过的区域无效，返回下一次循环
							//在此代码前不能改变玩家
							else
							{
								continue;
							}
							if (wincheck(i, j, currentplayer))
							{
								while (gamestart)
								{
									if (peekmessage(&msg, EX_MOUSE))
									{
										BeginBatchDraw();
										setfillcolor(RGB(240, 155, 89));
										fillroundrect(130, 200, 500, 400, 20, 20);
										settextstyle(25, 0, "微软雅黑");
										outtextxy(150, 220, "Player win!");
										//双重退出
										if (button(275, 325, 80, 50, "Quit"))
										{
											cleardevice();
											IMAGE img_background;
											loadimage(&img_background, "assets/background.jpg", 640, 640);
											putimage(0, 0, &img_background);
											gamestart = false;
										}
										EndBatchDraw();
									}
								}
							}
							//循环最后切换玩家
							else
								currentplayer = (currentplayer == black_chese) ? white_chese : black_chese;
						}
					}
				}
			}

		}
		Sleep(10);
	}
}

bool wincheck(int row, int col, int player)
{
	int count = 1;
	for (int i = 1; row + i < N && board[row + i][col] == player; i++)
		count++;
	for (int i = 1; row - i >= 0 && board[row - i][col] == player; i++)
		count++;
	if (count >= 5)
		return true;

	count = 1;
	for (int i = 1; col + i < N && board[row][col + i] == player; i++)
		count++;
	for (int i = 1; col - i >= 0 && board[row][col - i] == player; i++)
		count++;
	if (count >= 5)
		return true;

	count = 1;
	for (int i = 1; row + i < N && col - i >= 0 && board[row + i][col - i] == player; i++)
		count++;
	for (int i = 1; row - i >= 0 && col + i < N && board[row - i][col + i] == player; i++)
		count++;
	if (count >= 5)
		return true;

	count = 1;
	for (int i = 1; row + i < N && col + i < N && board[row + i][col + i] == player; i++)
		count++;
	for (int i = 1; row - i >= 0 && col - i >= 0 && board[row - i][col - i] == player; i++)
		count++;
	if (count >= 5)
		return true;

	return false;
}

// 评估某个位置的得分，同时考虑阻止对手和自身发展
// 参数：board - 二维数组，表示当前棋盘的状态
//       x - 要评估的位置的行坐标
//       y - 要评估的位置的列坐标
//       computerPlayer - 电脑玩家（黑子或白子）
//       humanPlayer - 人类玩家（黑子或白子）
// 返回值：该位置的得分
int evaluate(int board[N][N], int x, int y, int computerPlayer, int humanPlayer) {
	// 定义四个方向：水平、垂直、正斜、反斜
	int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
	int selfScore = 1;
	int blockScore = 1;

	for (int i = 0; i < 4; ++i) {
		int selfCount = 1;
		int blockCount = 1;
		int dx = directions[i][0];
		int dy = directions[i][1];

		// 正向检查电脑自身连子
		for (int j = 1; j < 5; ++j) {
			int newX = x + j * dx;
			int newY = y + j * dy;
			if (newX >= 0 && newX < N && newY >= 0 && newY < N && board[newX][newY] == computerPlayer) {
				++selfCount;
			}
			else {
				break;
			}
		}

		// 反向检查电脑自身连子
		for (int j = 1; j < 5; ++j) {
			int newX = x - j * dx;
			int newY = y - j * dy;
			if (newX >= 0 && newX < N && newY >= 0 && newY < N && board[newX][newY] == computerPlayer) {
				++selfCount;
			}
			else {
				break;
			}
		}

		// 正向检查阻止人类玩家连子
		for (int j = 1; j < 5; ++j) {
			int newX = x + j * dx;
			int newY = y + j * dy;
			if (newX >= 0 && newX < N && newY >= 0 && newY < N && board[newX][newY] == humanPlayer) {
				++blockCount;
			}
			else {
				break;
			}
		}

		// 反向检查阻止人类玩家连子
		for (int j = 1; j < 5; ++j) {
			int newX = x - j * dx;
			int newY = y - j * dy;
			if (newX >= 0 && newX < N && newY >= 0 && newY < N && board[newX][newY] == humanPlayer) {
				++blockCount;
			}
			else {
				break;
			}
		}

			// 计算自身连子得分
			switch (selfCount) {
			case 2:
				selfScore += 100;
				break;
		case 3:
			selfScore += 1000;
			break;
		case 4:
			selfScore += 10000;
			break;
		case 5:
			selfScore += 999999;
			break;
		}

		// 计算阻止对手连子得分
		switch (blockCount) {
		case 2:
			blockScore += 50;
			break;
		case 3:
			blockScore += 500;
			break;
		case 4:
			blockScore += 5000;
			break;
		case 5:
			blockScore += 100000;
			break;
		}
	}

	return selfScore + blockScore;
}

