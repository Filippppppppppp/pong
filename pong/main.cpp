//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <cmath>
// ������ ������ ����  

void ShowBitmap(int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false);

struct {
    HWND hWnd;//����� ����
    HDC device_context, context;// ��� ��������� ���������� (��� �����������)
    int width, height;//���� �������� ������� ���� ������� ������� ���������

} window;

struct sprite {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//����� � ������� ������ 
    HBITMAP hBitmap2;//����� � ������� ������
    bool status;
    bool color = false;

    void Load(const char* name)
    {
        hBitmap = (HBITMAP)LoadImageA(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
    void Show() {
        ShowBitmap(x, y, width, height, color ? hBitmap2 : hBitmap);
    }
};

sprite racket;//������� ������
//sprite enemy;//������� ����������
sprite ball;//�����

const int brikscountx = 20;
const int brikscounty = 4;

sprite briks[brikscountx][brikscounty];

struct {
    int score, balls;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
} game;


HBITMAP hBack;// ����� ��� �������� �����������

//c����� ����

void InitGame()
{
    //� ���� ������ ��������� ������� � ������� ������� gdi
    //���� ������������� - ����� ������ ������ ����� � .exe 
    //��������� ������ LoadImageA ��������� � ������� ��������, ��������� �������� ����� ������������� � ������� ���� �������
    ball.Load("ball.bmp");
    //ball.Load("racket_enemy.bmp");
    
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

    racket.width = 300;
    racket.height = 50;
    racket.speed = 30;//�������� ����������� �������
    racket.x = window.width / 2.;//������� ���������� ����
    racket.y = window.height - racket.height;//���� ���� ���� ������ - �� ������ �������

    for (int i = 0; i < brikscountx; i++)
    {
        for (int j= 0; j< brikscounty;j++)
        {
            briks[i][j].width = window.width / brikscountx;
            briks[i][j].height = window.height / (brikscounty * 3);
            briks[i][j].y = briks[i][j].height*j + window.height/3;
            briks[i][j].x = briks[i][j].width*i;
            briks[i][j].hBitmap = racket.hBitmap;
            briks[i][j].status = true;
            briks[i][j].color = false;
            briks[i][j].hBitmap2 = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        }
    }

    //enemy.x = racket.x;//� ���������� �������� ������ � �� �� ����� ��� � ������

    ball.dy = -(rand() % 65 + 35) / 100.;//��������� ������ ������ ������
    ball.dx = -(1 - ball.dy);//��������� ������ ������ ������
    ball.speed = 100;
    ball.rad = 20;
    ball.x = racket.x;//x ���������� ������ - �� ������� �������
    ball.y = racket.y - ball.rad;//����� ����� ������ �������

    game.score = 0;
    game.balls = 9;

   
}

void ProcessSound(const char* name)//������������ ���������� � ������� .wav, ���� ������ ������ � ��� �� ����� ��� � ���������
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//���������� name �������� ��� �����. ���� ASYNC ��������� ����������� ���� ���������� � ����������� ���������
}

void ShowScore()
{
    //�������� �������� � �������
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//����� ��� ������
    _itoa_s(game.score, txt, 10);//�������������� �������� ���������� � �����. ����� �������� � ���������� txt
    TextOutA(window.context, 10, 10, "Score", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "Balls", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) racket.x -= racket.speed;
    if (GetAsyncKeyState(VK_RIGHT)) racket.x += racket.speed;

    if (!game.action && GetAsyncKeyState(VK_SPACE))
    {
        game.action = true;
        ProcessSound("bounce.wav");
    }
}

void ShowBitmap(int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(window.context); // ������� �������� ������, ����������� � ���������� �����������
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// �������� ����������� bitmap � �������� ������

    if (hOldbm) // ���� �� ���� ������, ���������� ������
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // ���������� ������� �����������

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//��� ������� ������� ����� ����� ��������������� ��� ����������
        }
        else
        {
            StretchBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // ������ ����������� bitmap
        }

        SelectObject(hMemDC, hOldbm);// ��������������� �������� ������
    }

    DeleteDC(hMemDC); // ������� �������� ������
}

void ShowRacketAndBall()
{
    ShowBitmap(0, 0, window.width, window.height, hBack);//������ ���
    ShowBitmap(racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ������� ������
    for (int i = 0; i < brikscountx; i++)
    {
        for (int j = 0; j < brikscounty;j++)
        {
            if (briks[i][j].status)
            {
                briks[i][j].Show();
            }

        }
    }

    //if (ball.dy < 0 && (enemy.x - racket.width / 4 > ball.x || ball.x > enemy.x + racket.width / 4))
    //{
    //    //��������� ���������� ���������. �� ����� ����, ��������� ������� �� �����������, � �� �� ������� �������� �� ��� ������� �� ������
    //    //������ �����, �� ������ ������ ������ �� �������, � ������� ���������� ������� - ����������� ��� �����
    //    //�������� ����� ������ ���� ����� ����� �����, � ������ ���� ����� �� ��� X ������� �� ������� �������� ����� �������
    //    //� ���� ������, �� ��������� ���������� ������� � ������ � ��������� 9 � 1
    //    enemy.x = ball.x * .1 + enemy.x * .9;
    //}

    //ShowBitmap(window.context, enemy.x - racket.width / 2, 0, racket.width, racket.height, enemy.hBitmap);//������� ���������
    ShowBitmap( ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// �����


    
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//���� ��������� ������ ���� ������� ������ ����, �������� �� ����
    racket.x = min(racket.x, window.width - racket.width / 2.);//���������� ��� ������� ����
}

void CheckWalls()
{
    if (ball.x < ball.rad || ball.x > window.width - ball.rad)
    {
        ball.dx *= -1;
        ProcessSound("bounce.wav");
    }
}

void CheckRoof()
{
    if (ball.y < ball.rad)
    {
        ball.dy *= -1;
        ProcessSound("bounce.wav");
    }
}

bool tail = false;

void CheckFloor()
{
    if (ball.y > window.height - ball.rad - racket.height)//����� ������� ����� ������� - ����������� �������
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//����� �����, � �� �� � ������ ��������� ������
        {
            game.score++;//�� ������ ������� ���� ���� ����
            ball.speed += 5. / game.score;//�� ����������� ��������� - ���������� �������� ������
            ball.dy *= -1;//������
            racket.width -= 10. / game.score;//������������� ��������� ������ ������� - ��� ���������
            //ProcessSound("bounce.wav");//������ ���� �������
        }
        else
        {//����� �� �����

            tail = true;//����� ������ ������ ���� �������

            if (ball.y - ball.rad > window.height)//���� ����� ���� �� ������� ����
            {
                game.balls--;//��������� ���������� "������"

                ProcessSound("fail.wav");//������ ����

                if (game.balls < 0) { //�������� ������� ��������� "������"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//������� ��������� � ���������
                    InitGame();//������������������ ����
                }

                ball.dy = (rand() % 65 + 35) / 100.;//������ ����� ��������� ������ ��� ������
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//�������������� ���������� ������ - ������ ��� �� �������
                ball.y = racket.y - ball.rad;
                game.action = false;//���������������� ����, ���� ����� �� ������ ������
                tail = false;
            }
        }
    }
}
float sign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    else return 0;
 }
void swap(int& a, int& b)
{
    int c;
    c = a;
    a = b;
    b = c;
}

void ProcessRoom()
{
    for (int i = 0;i < brikscountx;i++)
    {
        for (int j = 0;j < brikscounty;j++)
        {
            briks[i][j].color = false;
        }
    }

    //������������ �����, ������� � ���. ������� - ���� ������� ����� ���� ���������, � ������, ��� ������� �� ����� ������ ������������� ����� ������� �������� ������
    int s = sqrt(pow(ball.dx * ball.speed, 2) + pow(ball.dy * ball.speed, 2));
    for (float k = 0; k < s; k++) 
    {
        float l = ceil(sqrt(pow(ball.dx * ball.speed, 2) + pow(ball.dy * ball.speed, 2)));
        int start_i = ball.x/ briks[0][0].width;
        int finish_i = (sign(ball.dx) * l + ball.x) / briks[0][0].width;
       
        if (ball.dx < 0)
        {
            if (finish_i > start_i) {
                swap(start_i, finish_i);
            }

            finish_i = min(start_i+1, brikscountx);

        }
        start_i = max(start_i, 0);

         for (int i = start_i; i < finish_i; i++)
        {
            int start_j = 0;
            //int finish_j = brikscounty;
            int finish_j = (sign(ball.dy) * l + (ball.y - window.height / 3)) / briks[0][0].height;
           // finish_j = min(finish_j, brikscounty);

            start_j = (ball.y - window.height / 3 )/ briks[0][0].height;
            start_j = min(start_j, brikscounty);
            //ball.dy *= -1;
            if (ball.dy < 0)
            {
                finish_j += 1;
                if (finish_j < start_j) {
                    swap(start_j, finish_j);
                }
            }
            else
            {
                finish_j -= 1;
                if (finish_j > start_j) {
                    swap(start_j, finish_j);
                }
                
            }
            finish_j = min(start_j, brikscounty);
            start_j = max(start_j, 0);
            
            for (int j = start_j; j < finish_j+1;j++)
            {  
                float ballx = ball.x + k * ball.dx * ball.speed / (float)s;
                float bally = ball.y + k * ball.dy * ball.speed / (float)s;
                SetPixel(window.context, ballx, bally, RGB(0xff, 0xff, 0xff));
                   
                briks[i][j].color = true;

                         
                if ((briks[i][j].y < bally) and (bally < briks[i][j].y + briks[i][j].height) and
                    (briks[i][j].x < ballx) and (ballx < briks[i][j].x + briks[i][j].width))
                { 

                    if (briks[i][j].status)
                    {
                        int dx1 = ballx - briks[i][j].x;
                        int dx2 = briks[i][j].x - ballx + briks[i][j].width;
                        int dy1 = bally - briks[i][j].y;
                        int dy2 = briks[i][j].y - bally + briks[i][j].height;

                        if ((min(dx1, dx2) > min(dy1, dy2)))
                        {
                          //  ball.dy *= -1;
                          //  briks[i][j].status = false;
                        }
                    
                        else {
                         //   ball.dx *= -1;
                           // briks[i][j].status = false;
                        }
                        return;
                    }
                }
            }
    }


            /*if ((briks[i][j].y < ball.y) and (ball.y < briks[i][j].y + briks[i][j].height) and
                (briks[i][j].x-1 < ball.x+ ball.rad) and (ball.x- ball.rad < briks[i][j].x+1 + briks[i][j].width))
            {
                if (briks[i][j].status)
                {
                    ball.dx *= -1;
                    briks[i][j].status = false;
                }
            }*/
            
        }
    
    
    CheckWalls();
    CheckRoof();
    CheckFloor();

}

void ProcessBall()
{
    if (game.action)
    {
        //���� ���� � �������� ������ - ���������� �����
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //����� - ����� "��������" � �������
        ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//�� ������ ���� ������� ����� ��������� ���������� ��� ���������
    window.width = r.right - r.left;//���������� ������� � ���������
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//������ �����
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//����������� ���� � ���������
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����
    InitGame();//����� �������������� ���������� ����

    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowRacketAndBall();//������ ���, ������� � �����
        ShowScore();//������ ���� � �����
        ProcessInput();//����� ����������
        LimitRacket();//���������, ����� ������� �� ������� �� �����
        ProcessRoom();//������������ ������� �� ���� � �������, ��������� ������ � ��������
        //ProcessBall();//���������� �����

        POINT p;
        GetCursorPos(&p);
        ScreenToClient(window.hWnd, &p);
        ball.x = p.x;
        ball.y = p.y;

        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)
    }

}
