// final_work.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <list>
#include <vector>
#include "EasyXPng.h"
#include "Timer.h"

using namespace std;
int gw = 600, gh = 400; //gw为窗口宽度，gh为窗口高度
const int timeSleep = 20;   //1帧20ms，1秒50帧

class Plane //飞机基类
{
public:
    IMAGE* nowImage;    //显示的图片指针
    int x, y;   //中心点坐标
    int iw, ih; //图片宽高
    int w, h;   //判定宽高

    Plane() //无参构造
    {
        ;
    }

    Plane(IMAGE* now, int xx, int yy, int ww, int hh)   //有参构造，图片宽高由传递图片确定,xy为中心点坐标，wh为判定宽高
    {
        nowImage = now;
        x = xx;
        y = yy;
        w = ww;
        h = hh;
        iw = now->getwidth();
        ih = now->getwidth();
    }

    void draw() //显示
    {
        putimagePng(x - iw / 2, y - ih / 2, nowImage);
    }
};

class Bullet //子弹类
{
public:
    IMAGE* nowImage;    //显示的图片指针
    double x, y;    //中心点坐标
    int iw, ih;     //图片宽高
    double w, h;    //判定宽高
    double vx, vy;  //xy轴速度

    Bullet()    //无参构造
    {
        ;
    }

    Bullet(IMAGE* now, double xx, double yy, double ww, double hh, double vvx, double vvy)      //有参构造，图片宽高由传递图片确定,xy为中心点坐标，wh为判定宽高，vxy为xy轴速度
    {
        nowImage = now;
        x = xx;
        y = yy;
        w = ww;
        h = hh;
        vx = vvx;
        vy = vvy;
        iw = now->getwidth();
        ih = now->getwidth();
    }

    void move() //直线运动
    {
        x += vx;
        y += vy;
    }

    void draw() //显示
    {
        putimagePng((int)x - iw / 2, (int)y - ih / 2, nowImage);
    }

    bool isend()    //判断是否应当被清除
    {
        if (x + iw / 2 < 0 || x - iw / 2 > gw || y + ih / 2 < 0 || y - ih / 2 > gh) //出界
        {
            return 1;
        }
        return 0;
    }
};

class EPlane :public Plane  //敌机类
{
private:
    bool befor_isend = 0;   //记录先前状态，是否进过场
public:
    int hp; //生命数
    int timek = 0;  //时间帧，1秒50帧
    int sustain_time;   //持续最长时间
    IMAGE* ebulletImage; //敌机子弹贴图

    EPlane()    //无参构造
    {
        ;
    }

    EPlane(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e) :Plane(now, xx, yy, ww, hh)  //有参构造
    {
        hp = hp0;
        sustain_time = st;
        ebulletImage = e;
    }

    virtual void init() //初始化
    {
        ;
    }

    virtual void move() //移动
    {
        ;
    }

    virtual void attack(list<Bullet>& bullet)   //发射子弹，参数是子弹集合
    {
        ;
    }

    void detect(list<Bullet>& bullet)   //子弹与敌机判定
    {
        for (auto t = bullet.begin(); t != bullet.end();)   //遍历
        {
            auto tt = t;
            t++;
            if (x + w >= tt->x - tt->w && x - w <= tt->x + tt->w && y + h >= tt->y - tt->h && y - h <= tt->y + tt->h)   //碰撞判定，碰到掉血，消除子弹，结束遍历
            {
                hp--;
                bullet.erase(tt);
                break;
            }
        }
    }

    bool isend()    //判断是否应当被清除
    {
        if (hp <= 0 || (x + iw / 2 < 0 || x - iw / 2 > gw || y + ih / 2 < 0 || y - ih / 2 > gh) || timek > sustain_time)   //血量为0或出界或超过持续时间
        {
            if (befor_isend == 1 || hp <= 0) return 1;
        }
        else befor_isend = 1;
        return 0;
    }
};

class EPlane1_1 :public EPlane   //第一关第一种敌机类，直线行动，直线发射子弹
{
private:

public:
    int vx, vy; //飞机xy速度
    int bvx, bvy;   //子弹xy速度

    EPlane1_1() //无参构造
    {
        ;
    }

    EPlane1_1(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, int vbx, int vby) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        bvx = vbx;
        bvy = vby;
    }

    void move() //移动
    {
        x += vx;
        y += vy;
    }

    void attack(list<Bullet>& bullet)   //发射子弹
    {
        timek++;
        if (timek % 50 == 0) //每秒1发
        {
            bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
        }
    }
};

class Player :public Plane   //自机类
{
private:
    int attacki = 5;    //攻击间隔，5帧，0.1秒
public:
    int hp = 5; //生命数
    IMAGE* pbulletImage; //自机子弹贴图
    IMAGE* detectImage; //判定点贴图

    Player()    //无参构造
    {
        ;
    }

    Player(IMAGE* now, int xx, int yy, int ww, int hh, IMAGE* p, IMAGE* d) :Plane(now, xx, yy, ww, hh)      //有参构造，p为自机子弹贴图，d为判定点贴图
    {
        pbulletImage = p;
        detectImage = d;
    }

    void move() //移动
    {
        if (_kbhit())
        {
            int v = 5;
            if (GetAsyncKeyState(VK_SHIFT)) //按住SHIFT低速，平常高速
            {
                v = 2;
            }
            if (GetAsyncKeyState(VK_UP))
            {
                y -= v;
                if (y - ih / 2 <= 0) y = ih / 2;
            }
            if (GetAsyncKeyState(VK_DOWN))
            {
                y += v;
                if (y + ih / 2 >= gh) y = gh - ih / 2;
            }
            if (GetAsyncKeyState(VK_LEFT))
            {
                x -= v;
                if (x - iw / 2 <= 0) x = iw / 2;
            }
            if (GetAsyncKeyState(VK_RIGHT))
            {
                x += v;
                if (x + iw / 2 >= gw) x = gw - iw / 2;
            }
        }
    }

    void detect(list<Bullet>& bullet)   //子弹与自机判定
    {
        for (auto t = bullet.begin(); t != bullet.end();)   //遍历
        {
            auto tt = t;
            t++;
            if (x + w >= tt->x - tt->w && x - w <= tt->x + tt->w && y + h >= tt->y - tt->h && y - h <= tt->y + tt->h)   //碰撞判定，碰到掉血，消除子弹，结束遍历
            {
                hp--;
                bullet.erase(tt);
                break;
            }
        }
    }

    void attack(list<Bullet>& bullet)   //发射子弹，参数是子弹集合与子弹图片
    {
        if (_kbhit()&&GetAsyncKeyState(0x5a))    //按z发射子弹
        {
            attacki--;
            if (attacki <= 0)
            {
                attacki = 5;
                bullet.push_back(Bullet(pbulletImage, x, y, 5, 5, 0, -10));
            }
        }
    }
};

class Scene     //场景类，或者说关卡类，用于派生具体的的关卡类
{
private:
    int bky = 0;    //背景移动y
public:
    list<Bullet> pbullet;  //自机子弹集合
    list<Bullet> ebullet;   //敌机子弹集合
    vector<IMAGE> ebulletImage; //敌机子弹图片集合
    list<EPlane*> eplane;    //敌机集合
    vector<IMAGE> eplaneImage;  //敌机图片集合
    Player* player;         //自机指针
    IMAGE bk;               //背景图
    int timek = 0;  //时间帧，每秒50帧

    Scene() //无参构造
    {
        ;
    }

    Scene(IMAGE bbk)    //有参构造，bbk为背景图
    {
        bk = bbk;
    }

    virtual ~Scene()    //析构函数，声明为虚函数
    {
        ;
    }

    virtual void init(Player* player) //初始化
    {
        ;
    }

    virtual bool run()  //运行
    {
        return 1;
    }

    void draw() //显示，先显示背景，再显示敌机，再显示自机子弹，再显示自机，再显示敌机子弹，有的话最上层显示判定点
    {
        //背景显示
        bky++;
        if (bky >= gh) bky = 0;
        putimage(0, bky - gh, &bk);
        putimage(0, bky, &bk);

        //敌机显示
        for (auto& t : eplane)
        {
            t->draw();
        }

        //自机子弹显示
        for (auto& t : pbullet)
        {
            t.draw();
        }

        //自机显示
        player->draw();

        //敌机子弹显示
        for (auto& t : ebullet)
        {
            t.draw();
        }

        //判定点显示
        if (_kbhit() && GetAsyncKeyState(VK_SHIFT))    //按下SHIFT显示
        {
            putimagePng(player->x - player->detectImage->getwidth() / 2, player->y - player->detectImage->getheight() / 2, player->detectImage);
        }
    }
};

class Scene1 :public Scene  //关卡1
{
private:
    int sustain_time = 2000;
public:

    Scene1()    //无参构造
    {
        ;
    }

    void init(Player* player) //初始化
    {
        IMAGE tImage[3];    //加载图片用临时变量

        //加载背景图片
        loadimage(&tImage[0], "images/bk.png");
        bk = tImage[0];
        gw = bk.getwidth();
        gh = bk.getheight();    //根据背景图片改变gw,gh

        //加载敌机图片
        loadimage(&tImage[1], "images/enemy1.png");
        eplaneImage.push_back(tImage[1]);

        //加载敌机子弹图片
        loadimage(&tImage[2], "images/ebullet.png");
        ebulletImage.push_back(tImage[2]);

        //加载自机
        this->player = player;
    }

    bool run()
    {
        timek++;

        //时间相关区域
        if (timek >= 200 && timek < 400)   //4~8秒
        {
            if (timek % 50 == 0)    //每秒从上方生成一个向下的敌机
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], gw / 2, -100, 40, 40, 10, sustain_time - timek, &ebulletImage[0], 0, 2, 0, 4);
                eplane.push_back(t);
            }
        }
        if (timek >= 500 && timek < 700)  //10~14秒
        {
            if (timek % 50 == 0) //每秒从左方生成一个向右的敌机
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], -100, 100, 40, 40, 10, sustain_time - timek, &ebulletImage[0], 2, 0, 0, 4);
                eplane.push_back(t);
            }
        }
        if (timek >= 1000 && timek < 1500) //20~30秒
        {
            if (timek % 100 == 0)    //每两秒从右方生成一个向左的敌机
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], gw+100, 250, 40, 40, 10, sustain_time - timek, &ebulletImage[0], -4, 0, 0, 4);
                eplane.push_back(t);
            }
        }
        if (timek > sustain_time)  //40秒
        {
            return 1;   //关卡结束
        }

        //时间无关区域
        for (auto t = pbullet.begin(); t != pbullet.end();) //自机子弹相关
        {
            auto tt = t;
            t++;
            tt->move();
            if (tt->isend()) pbullet.erase(tt);
        }
        for (auto t = ebullet.begin(); t != ebullet.end();) //敌机子弹相关
        {
            auto tt = t;
            t++;
            tt->move();
            if (tt->isend()) ebullet.erase(tt);
        }
        for (auto t = eplane.begin(); t != eplane.end();)   //子弹相关
        {
            auto tt = t;
            t++;
            auto t0 = *tt;
            t0->move();
            t0->attack(ebullet);
            t0->detect(pbullet);
            if (t0->isend())
            {
                delete t0;
                eplane.erase(tt);
            }
        }
        player->move();
        player->attack(pbullet);
        player->detect(ebullet);    //自机相关

        //显示
        cleardevice();
        draw();
        FlushBatchDraw();
        return 0;
    }
};

void run()  //运行函数
{
    //加载三张自机相关图片
    IMAGE playerImage, pbulletImage, detectImage;
    loadimage(&playerImage, "images/me0.png");
    loadimage(&pbulletImage, "images/pbullet.png");
    loadimage(&detectImage, "images/detect.png");
    Player player(&playerImage, 400, 500, 5, 5, &pbulletImage, &detectImage);   //初始化自机
    Scene* scene = new Scene1();
    scene->init(&player);
    initgraph(gw, gh);
    BeginBatchDraw();
    cleardevice();
    Timer timer;
    while (1)
    {
        if (scene->run()) break;
        timer.Sleep(timeSleep);
    }
}

int main()
{
    run();

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
