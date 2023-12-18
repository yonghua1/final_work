// final_work.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <tchar.h>
#include <list>
#include <vector>
#include "EasyXPng.h"
#include "Timer.h"

using namespace std;
int gw = 800, gh = 600; //gw为窗口宽度，gh为窗口高度
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
        ih = now->getheight();
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

class Player :public Plane   //自机类
{
private:
    int attacki = 5;    //攻击间隔，5帧，0.1秒
    int movei;  //移动图片确定
public:
    int hp = 0; //生命数   
    IMAGE* pbulletImage; //自机子弹贴图
    IMAGE* detectImage;  //判定点贴图
    IMAGE stand;        //正常的图片
    IMAGE left[25];     //向左的图片
    IMAGE right[25];    //向右的图片
    enum
    {
        LEFT,RIGHT,STAND
    }bstatus,nstatus;    //状态枚举向量

    Player()    //无参构造
    {
        ;
    }

    Player(IMAGE* now, int xx, int yy, int ww, int hh, IMAGE* p, IMAGE* d) :Plane(now, xx, yy, ww, hh)      //有参构造，p为自机子弹贴图，d为判定点贴图
    {
        pbulletImage = p;
        detectImage = d;
    }

    void init()
    {
        loadimage(&stand, "images/me0.png");
        for (int i = 0; i < 25; i++)
        {
            char t[50];
            sprintf_s(t, "images/Image%d.png", i + 1);
            loadimage(left + i, t);
            sprintf_s(t, "images/Image%d.png", i + 31);
            loadimage(right + i, t);
        }
        bstatus = STAND;
        nstatus = STAND;
    }

    void move() //移动
    {
        //键盘输入
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
                nstatus = LEFT;
            }
            else if (GetAsyncKeyState(VK_RIGHT))
            {
                x += v;
                if (x + iw / 2 >= gw) x = gw - iw / 2;
                nstatus = RIGHT;
            }
            else
            {
                nstatus = STAND;
            }
        }
        else
        {
            nstatus = STAND;
        }

        //图片更改
        if (nstatus == STAND)
        {
            nowImage = &stand;
        }
        else if (nstatus == LEFT)
        {
            if (bstatus == LEFT)
            {
                movei++;
                if (movei >= 25) movei = 24;
            }
            else
            {
                movei = 0;
            }
            nowImage = left + movei;
        }
        else
        {
            if (bstatus == RIGHT)
            {
                movei++;
                if (movei >= 25) movei = 24;
            }
            else
            {
                movei = 0;
            }
            nowImage = right + movei;
        }
        bstatus = nstatus;
    }

    void detect(list<Bullet>& bullet)   //子弹与自机判定
    {
        for (auto t = bullet.begin(); t != bullet.end();)   //遍历
        {
            auto tt = t;
            t++;
            if (x + w >= tt->x - tt->w && x - w <= tt->x + tt->w && y + h >= tt->y - tt->h && y - h <= tt->y + tt->h)   //碰撞判定，碰到掉血，消除子弹，结束遍历
            {
                hp++;
                bullet.erase(tt);
                break;
            }
        }
    }

    void attack(list<Bullet>& bullet)   //发射子弹，参数是子弹集合与子弹图片
    {
        if (_kbhit() && GetAsyncKeyState(0x5a))    //按z发射子弹
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

    virtual void attack(list<Bullet>& bullet, Player* player)   //发射子弹，参数是子弹集合，还有自机，可能发射与自机相关子弹
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
        timek++;
        x += vx;
        y += vy;
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek % 50 == 0) //每秒1发
        {
            bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
        }
    }
};

class EPlane1_2 :public EPlane  //第一关第二种敌机类，直线移动，发射偶数弹
{
private:

public:
    int vx, vy; //自机xy速度
    double bv;  //子弹总速度
    int attacki;    //攻击间隔
    int attack_time;    //攻击时间
    int number; //左右侧各要增加的数目
    double add_angel;   //每个偏移角度

    EPlane1_2() //无参构造
    {
        ;
    }

    EPlane1_2(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, double bbv, int ai, int at, int num, double add) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        bv = bbv;
        attacki = ai;
        attack_time = at;
        number = num;
        add_angel = add;
    }

    void move() //移动
    {
        timek++;
        x += vx;
        y += vy;
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek <= attack_time && timek % attacki == 0)
        {
            double angel = atan2(player->y - y, player->x - x);
            double bvx, bvy;
            for (int i = 1; i <= number; i++)
            {
                bvx = bv * cos(angel + (2 * i - 1) * add_angel);
                bvy = bv * sin(angel + (2 * i - 1) * add_angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                bvx = bv * cos(angel - (2 * i - 1) * add_angel);
                bvy = bv * sin(angel - (2 * i - 1) * add_angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
            }
        }
    }
};

class EPlane2_1 :public EPlane  //第二关第一种敌机类，从屏幕外飞入到定点，随后不动并发射奇数弹，自机狙为特殊情况
{
private:

public:
    int vx, vy; //自机xy速度
    int move_time;  //移动时间，控制定点
    double bv;  //子弹总速度
    int attacki;    //攻击间隔
    int attack_time;    //攻击时间
    int number; //左右侧各要增加的数目
    double add_angel;   //每个偏移角度

    EPlane2_1() //无参构造
    {
        ;
    }

    EPlane2_1(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, int mt, double bbv, int ai, int at, int num, double add) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        move_time = mt;
        bv = bbv;
        attacki = ai;
        attack_time = at;
        number = num;
        add_angel = add;
    }

    void move() //移动
    {
        timek++;
        if (timek <= move_time)
        {
            x += vx;
            y += vy;
        }
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek >= move_time && timek <= attack_time && timek % attacki == 0)
        {
            double angel = atan2(player->y - y, player->x - x);
            double bvx = bv * cos(angel), bvy = bv * sin(angel);
            bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
            for (int i = 1; i <= number; i++)
            {
                bvx = bv * cos(angel + i * add_angel);
                bvy = bv * sin(angel + i * add_angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                bvx = bv * cos(angel - i * add_angel);
                bvy = bv * sin(angel - i * add_angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
            }
        }
    }
};

class EPlane2_2 :public EPlane  //第二关第二种敌机类，从屏幕外飞入到定点，随后不动并发射偶数弹
{
private:

public:
    int vx, vy; //自机xy速度
    int move_time;  //移动时间，控制定点
    double bv;  //子弹总速度
    int attacki;    //攻击间隔
    int attack_time;    //攻击时间
    int number; //左右侧各要增加的数目
    double add_angel;   //每个偏移角度

    EPlane2_2() //无参构造
    {
        ;
    }

    EPlane2_2(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, int mt, double bbv, int ai, int at, int num, double add) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        move_time = mt;
        bv = bbv;
        attacki = ai;
        attack_time = at;
        number = num;
        add_angel = add;
    }

    void move() //移动
    {
        timek++;
        if (timek <= move_time)
        {
            x += vx;
            y += vy;
        }
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek >= move_time && timek <= attack_time && timek % attacki == 0)
        {
            double angel = atan2(player->y - y, player->x - x);
            double bvx, bvy;
            for (int i = 1; i <= number; i++)
            {
                bvx = bv * cos(angel + (2 * i - 1) * add_angel);
                bvy = bv * sin(angel + (2 * i - 1) * add_angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                bvx = bv * cos(angel - (2 * i - 1) * add_angel);
                bvy = bv * sin(angel - (2 * i - 1) * add_angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
            }
        }
    }
};

class EPlane3_1 :public EPlane  //第一种boss，从屏幕外飞入到定点，随后攻击直到消失
{
private:
    int bhp = 0;
    IMAGE* ebulletImage1;

    int attacki1 = 200;
    bool lr = 0;

    int attacki2 = 75;
    bool ud = 0;

    double angel = 0;   //角度
    double angelv = 0;  //角速度
    double angela = PI / 360;    //角加速度
public:
    int vx, vy; //自机xy速度
    int move_time;  //移动时间，控制定点
    double bv;  //子弹总速度

    EPlane3_1() //无参构造
    {
        ;
    }

    EPlane3_1(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, int mt, double bbv, IMAGE* e1) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        move_time = mt;
        bv = bbv;
        ebulletImage1 = e1;
    }

    void move() //移动
    {
        timek++;
        if (timek <= move_time)
        {
            x += vx;
            y += vy;
        }
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek >=move_time)
        {
            if (hp > 400)
            {
                if (hp <= 500 || timek >= 1000) attacki1 = 100;
                if (hp <= 450 || timek >= 1500) attacki1 = 50;
                if (timek >= 2000) hp = 400;
                if (timek % attacki1 == 0)
                {
                    if (lr)
                    {
                        for (int i = 10; i < gh; i += 20)
                        {
                            double v = 2 + rand() % 50 / 10.0;
                            bullet.push_back(Bullet(ebulletImage, gw - 10, i, 10, 10, -v, 0));
                        }
                    }
                    else
                    {
                        for (int i = 10; i < gh; i += 20)
                        {
                            double v = 2 + rand() % 50 / 10.0;
                            bullet.push_back(Bullet(ebulletImage, 10, i, 10, 10, v, 0));
                        }
                    }
                    lr = !lr;
                }
            }
            else if (hp > 200)
            {
                if (hp <= 300 || timek >= 3000) attacki2 = 25;
                if (timek >= 4000) hp = 200;
                if (timek % attacki2 == 0)
                {
                    if (ud)
                    {
                        int t = 10 + rand() % 31;
                        for (int i = t; i < gw; i += 50)
                        {
                            bullet.push_back(Bullet(ebulletImage1, i, gh - 10, 4, 4, 0, -bv));
                        }
                    }
                    else
                    {
                        int t = 10 + rand() % 31;
                        for (int i = t; i < gw; i += 50)
                        {
                            bullet.push_back(Bullet(ebulletImage1, i, 10, 4, 4, 0, bv));
                        }
                    }
                    ud = !ud;
                }
            }
            else
            {
                angelv += angela;
                angel += angelv;
                double bvx = bv * cos(angel);
                double bvy = bv * sin(angel);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                bvx = bv * cos(angel + PI * 2 / 3);
                bvy = bv * sin(angel + PI * 2 / 3);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                bvx = bv * cos(angel - PI * 2 / 3);
                bvy = bv * sin(angel - PI * 2 / 3);
                bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
            }
        }
        if (hp % 200 == 0 && bhp != hp)
        {
            bullet.clear();
        }
        bhp = hp;
    }
};

class EPlane3_2 :public EPlane  //第二种boss，从屏幕外飞入到定点，随后攻击直到消失
{
private:
    int attacki = 200;  //攻击间隔
public:
    int vx, vy; //自机xy速度
    int move_time;  //移动时间，控制定点
    double bv;  //子弹总速度

    EPlane3_2() //无参构造
    {
        ;
    }

    EPlane3_2(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, int mt, double bbv) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        move_time = mt;
        bv = bbv;
    }

    void move() //移动
    {
        timek++;
        if (timek <= move_time)
        {
            x += vx;
            y += vy;
        }
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek >= move_time)
        {
            if (timek % attacki == 0)
            {
                double angel = PI / 2 + PI / 108 * (rand() % 37 - 18);
                for (int i = 0; i < 106; i++)
                {
                    double t = angel + PI / 108 * (2 + i);
                    double bvx = bv * cos(t);
                    double bvy = bv * sin(t);
                    bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                    t = angel - PI / 108 * (2 + i);
                    bvx = bv * cos(t);
                    bvy = bv * sin(t);
                    bullet.push_back(Bullet(ebulletImage, x, y, 10, 10, bvx, bvy));
                }
            }
        }
        if (timek == 500) attacki = 150;
        if (timek == 1000) attacki = 100;
    }
};

class EPlane3_3 :public EPlane  //第三种boss，从屏幕外飞入到定点，随后攻击直到消失
{
private:
    int attacki = 2;
public:
    int vx, vy; //自机xy速度
    int move_time;  //移动时间，控制定点
    double bv;  //子弹总速度
    vector<IMAGE>* ebulletImage; //子弹贴图数组指针，以直接使用场景中的子弹贴图数组

    EPlane3_3() //无参构造
    {
        ;
    }

    EPlane3_3(IMAGE* now, int xx, int yy, int ww, int hh, int hp0, int st, IMAGE* e, int vvx, int vvy, int mt, double bbv, vector<IMAGE>* ebul) :EPlane(now, xx, yy, ww, hh, hp0, st, e)//有参构造
    {
        vx = vvx;
        vy = vvy;
        move_time = mt;
        bv = bbv;
        ebulletImage = ebul;
    }

    void move() //移动
    {
        timek++;
        if (timek <= move_time)
        {
            x += vx;
            y += vy;
        }
    }

    void attack(list<Bullet>& bullet, Player* player)   //发射子弹
    {
        if (timek >= move_time) //乱射
        {
            double angel = PI / 2 + PI / 90 * (rand() % 181 - 90);
            int i = rand() % 9;
            double bvx = bv * cos(angel);
            double bvy = bv * sin(angel);
            bullet.push_back(Bullet(&ebulletImage->at(i), x, y, i / 3 * 5 + 10, i / 3 * 5 + 10, bvx, bvy));
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
    int sustain_time = 6000;
public:

    Scene1()    //无参构造
    {
        ;
    }

    void init(Player* player) //初始化
    {
        IMAGE tImage[3];    //加载图片用临时变量

        //加载背景图片
        loadimage(&tImage[0], _T("images/bk.png"));
        bk = tImage[0];

        //加载敌机图片
        loadimage(&tImage[1], _T("images/enemy3.png"));
        eplaneImage.push_back(tImage[1]);

        //加载敌机子弹图片
        loadimage(&tImage[2], _T("images/ebullet0.png"));
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
            if (timek % 50 == 0) //每秒从左方生成一个向右的敌机
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], -100, 100, 30, 25, 10, sustain_time - timek, &ebulletImage[0], 2, 0, 0, 4);
                eplane.push_back(t);
            }
        }
        if (timek >= 500 && timek < 800)  //10~16秒
        {
            if (timek % 50 == 0)    //每秒从右方生成一个向左的敌机
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], gw + 100, 250, 30, 25, 5, sustain_time - timek, &ebulletImage[0], -4, 0, 0, 4);
                eplane.push_back(t);
            }
        }
        if (timek >= 700 && timek < 1000) //16~20秒
        {
            if ((timek + 25) % 50 == 0)
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], -100, 150, 30, 25, 5, sustain_time - timek, &ebulletImage[0], 4, 0, 0, 4);
                eplane.push_back(t);
            }
        }
        if (timek == 1000) //20秒
        {
            EPlane* t = new EPlane2_1(&eplaneImage[0], -100, 100, 30, 25, 50, 650, &ebulletImage[0], 5, 0, 40, 5, 25, 450, 0, 0);
            eplane.push_back(t);
        }
        if (timek >= 1100 && timek < 1300) //22~26秒
        {
            if (timek % 40 == 0)
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], 400, -100, 30, 25, 5, sustain_time - timek, &ebulletImage[0], 0, 4, -3, 4);
                eplane.push_back(t);
            }
        }
        if (timek == 1500) //30秒
        {
            EPlane* t = new EPlane2_2(&eplaneImage[0], gw+100, 100, 30, 25, 50, 650, &ebulletImage[0], -5, 0, 40, 5, 25, 450, 2, PI / 36);
            eplane.push_back(t);
        }
        if (timek >= 1500 && timek < 2000) //30~40秒
        {
            if (timek % 40 == 0)
            {
                EPlane* t = new EPlane1_1(&eplaneImage[0], -100, 200, 30, 25, 5, sustain_time - timek, &ebulletImage[0], 4, 0, 4, 4);
                eplane.push_back(t);
            }
        }
        if (timek == 2000) //第40秒
        {
            EPlane* t = new EPlane2_1(&eplaneImage[0], -100, 100, 30, 25, 50, 650, &ebulletImage[0], 5, 0, 40, 5, 25, 450, 0, 0);
            eplane.push_back(t);
        }
        if (timek == 2200) //第42秒
        {
            EPlane* t = new EPlane2_2(&eplaneImage[0], 400, -100, 30, 25, 80, 1300, &ebulletImage[0], 0, 5, 40, 5, 25, 1200, 2, PI / 36);
            eplane.push_back(t);
        }
        if (timek == 2500) //第50秒
        {
            EPlane* t = new EPlane2_1(&eplaneImage[0], 150, -100, 30, 25, 20, 450, &ebulletImage[0], 0, 5, 40, 5, 25, 300, 0, 0);
            eplane.push_back(t);
            t = new EPlane2_1(&eplaneImage[0], gw - 150, -100, 30, 25, 20, 450, &ebulletImage[0], 0, 5, 40, 5, 25, 300, 0, 0);
            eplane.push_back(t);
        }
        if (timek == 3000) //第60秒
        {
            EPlane* t = new EPlane2_1(&eplaneImage[0], 350, -100, 30, 25, 20, 450, &ebulletImage[0], 0, 5, 40, 5, 25, 300, 2, PI / 6);
            eplane.push_back(t);
            t = new EPlane2_1(&eplaneImage[0], 450, -100, 30, 25, 20, 450, &ebulletImage[0], 0, 5, 40, 5, 25, 300, 2, PI / 6);
            eplane.push_back(t);
        }
        if (timek == 3500) //第70秒
        {
            for (int i = 50; i <= 750; i += 100)
            {
                EPlane* t = new EPlane2_2(&eplaneImage[0], i, -100, 30, 25, 20, 450, &ebulletImage[0], 0, 5, 40, 5, 25, 400, 2, PI / 36);
                eplane.push_back(t);
            }
        }
        if (timek == 4100)   //第82秒
        {
            EPlane* t = new EPlane3_2(&eplaneImage[0], 400, -100, 30, 25, 200, sustain_time-timek, &ebulletImage[0], 0, 5, 40, 5);
            eplane.push_back(t);
        }
        if (timek > sustain_time)  //120秒
        {
            for (auto& t : eplane)  //释放所有未释放内存
            {
                delete t;   
            }
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
        for (auto t = eplane.begin(); t != eplane.end();)   //敌机相关
        {
            auto tt = t;
            t++;
            auto t0 = *tt;
            t0->move();
            t0->attack(ebullet, player);
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
        setfillcolor(RED);
        if (timek >= 4100)
        {
            if (!eplane.empty())
            {
                rectangle(0, 0, gw, 5);
                fillrectangle(0, 0, eplane.front()->hp* gw / 200, 5);
            }
            else
            {
                return 1;   //关卡结束
            }
        }
        FlushBatchDraw();
        return 0;
    }
};

class Scene2 :public Scene  //关卡2
{
private:
    int sustain_time = 4000;
public:

    Scene2()    //无参构造
    {
        ;
    }

    void init(Player* player) //初始化
    {
        IMAGE tImage[2];    //加载图片用临时变量

        //加载背景图片
        loadimage(&tImage[0], _T("images/bk1.png"));
        bk = tImage[0];

        //加载敌机图片
        loadimage(&tImage[1], _T("images/enemy4.png"));
        eplaneImage.push_back(tImage[1]);

        //加载敌机子弹图片
        for (int i = 0; i < 9; i++)
        {
            char c[50];
            sprintf_s(c, "images/ebullet%d.png", i);
            IMAGE t;
            loadimage(&t, c);
            ebulletImage.push_back(t);
        }
        swap(ebulletImage[0], ebulletImage[1]);

        //加载自机
        this->player = player;
    }

    bool run()
    {
        timek++;

        //时间相关区域
        if (timek >= 100 && timek < 200)  //2-4秒
        {
            if (timek % 10 == 0) //1秒5个
            {
                EPlane* t = new EPlane2_1(&eplaneImage[0], (timek - 100) * 5 + 150, -100, 30, 25, 3, 180, &ebulletImage[0], 0, 10, 20, 10, 50, 180, 0, 0);
                eplane.push_back(t);
            }
        }
        if (timek >= 400 && timek < 450)  //8秒
        {
            if (timek % 10 == 0)
            {
                EPlane* t = new EPlane2_1(&eplaneImage[0], -100, 300 - (timek - 400) * 5, 30, 25, 5, 980-timek, &ebulletImage[0], 10, 0, 20, 5, 50, 980 - timek, 0, 0);
                eplane.push_back(t);
            }
        }
        if(timek >= 450 && timek < 500)   //9秒
        {
            if (timek % 10 == 0)
            {
                EPlane* t = new EPlane2_2(&eplaneImage[0], (timek - 450) * 10 + 150, -100, 30, 25, 4, 980 - timek, &ebulletImage[0], 0, 10, 20, 5, 50, 980 - timek, 2, PI / 24);
                eplane.push_back(t);
            }
        }
        if (timek >= 500 && timek < 550)   //10秒
        {
            if (timek % 10 == 0)
            {
                EPlane* t = new EPlane2_1(&eplaneImage[0], gw + 100, 300 - (timek - 500) * 5, 30, 25, 3, 980 - timek, &ebulletImage[0], -10, 0, 20, 5, 50, 980 - timek, 0, 0);
                eplane.push_back(t);
            }
        }
        if (timek == 1000) //20秒
        {
            EPlane* t = new EPlane1_2(&eplaneImage[0], 400, -60, 30, 25, 40, 2000 - timek, &ebulletImage[0], 0, 1, 3, 75, 1350 - timek, 36, PI / 72);
            eplane.push_back(t);
        }
        if (timek == 1400) //28秒
        {
            EPlane* t = new EPlane1_2(&eplaneImage[0], 100, -60, 30, 25, 40, 2000 - timek, &ebulletImage[0], 0, 1, 3, 75, 1750 - timek, 36, PI / 72);
            eplane.push_back(t);
        }
        if (timek == 1600) //32秒
        {
            EPlane* t = new EPlane2_1(&eplaneImage[0], 700, -100, 30, 25, 20, 2000 - timek, &ebulletImage[0], 0, 5, 40, 5, 50, 2000 - timek, 0, 0);
            eplane.push_back(t);
        }
        if (timek == 2100)     //第42秒
        {
            EPlane* t = new EPlane3_3(&eplaneImage[0], 400, -100, 30, 25, 200, sustain_time - timek, &ebulletImage[0], 0, 10, 30, 5, &ebulletImage);
            eplane.push_back(t);
        }
        if (timek > sustain_time)  //80秒
        {
            for (auto& t : eplane)  //释放所有未释放内存
            {
                delete t;
            }
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
        for (auto t = eplane.begin(); t != eplane.end();)   //敌机相关
        {
            auto tt = t;
            t++;
            auto t0 = *tt;
            t0->move();
            t0->attack(ebullet, player);
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
        setfillcolor(RED);
        if (timek>=2100)
        {
            if (!eplane.empty())
            {
                rectangle(0, 0, gw, 5);
                fillrectangle(0, 0, eplane.front()->hp* gw / 200, 5);
            }
            else
            {
                return 1;
            }
        }
        FlushBatchDraw();
        return 0;
    }
};

class Scene3 :public Scene
{
private:
    int sustain_time = 6100;
public:

    Scene3()    //无参构造
    {
        ;
    }

    void init(Player* player) //初始化
    {
        IMAGE tImage[4];    //加载图片用临时变量

        //加载背景图片
        loadimage(&tImage[0], _T("images/bk2.png"));
        bk = tImage[0];

        //加载敌机图片
        loadimage(&tImage[1], _T("images/enemy1.png"));
        eplaneImage.push_back(tImage[1]);

        //加载敌机子弹图片
        loadimage(&tImage[2], _T("images/ebullet2.png"));
        ebulletImage.push_back(tImage[2]);
        loadimage(&tImage[3], _T("images/ebullet00.png"));
        ebulletImage.push_back(tImage[3]);

        //加载自机
        this->player = player;
    }

    bool run()
    {
        timek++;

        //时间相关区域
        if (timek == 100)     //第2秒
        {
            EPlane* t = new EPlane3_1(&eplaneImage[0], 400, -100, 30, 25, 600, sustain_time-timek, &ebulletImage[0], 0, 5, 40, 5, &ebulletImage[1]);
            eplane.push_back(t);
        }
        if (timek > sustain_time)  //122秒
        {
            for (auto& t : eplane)  //释放所有未释放内存
            {
                delete t;
            }
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
        for (auto t = eplane.begin(); t != eplane.end();)   //敌机相关
        {
            auto tt = t;
            t++;
            auto t0 = *tt;
            t0->move();
            t0->attack(ebullet, player);
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
        if (!eplane.empty())
        {
            int hpt = eplane.front()->hp;
            rectangle(0, 0, gw, 5);
            if (hpt > 400)
            {
                setfillcolor(GREEN);
                fillrectangle(0, 0, gw, 5);
                setfillcolor(BLUE);
                fillrectangle(0, 0, (hpt - 400) * gw / 200, 5);
            }
            else if (hpt > 200)
            {
                setfillcolor(RED);
                fillrectangle(0, 0, gw, 5);
                setfillcolor(GREEN);
                fillrectangle(0, 0, (hpt - 200) * gw / 200, 5);
            }
            else
            {
                setfillcolor(RED);
                fillrectangle(0, 0, hpt * gw / 200, 5);
            }
        }
        else if (timek > 200)
        {
            return 1;
        }
        FlushBatchDraw();
        return 0;
    }
};

void run()  //运行函数
{
    srand(unsigned(time(0)));
    //加载三张自机相关图片
    IMAGE playerImage, pbulletImage, detectImage;
    loadimage(&playerImage, _T("images/me0.png"));
    loadimage(&pbulletImage, _T("images/pbullet.png"));
    loadimage(&detectImage, _T("images/detect.png"));

    //初始化自机
    Player player(&playerImage, 400, 500, 2, 2, &pbulletImage, &detectImage);
    player.init();

    //场景初始化
    int number = 1;
    Scene* scene = nullptr;

    //窗口初始化
    initgraph(gw, gh);
    setbkmode(TRANSPARENT);
    settextstyle(30, 0, _T("宋体"));
    BeginBatchDraw();
    cleardevice();
    Timer timer;
    while (1)
    {
        if (scene == nullptr)
        {
            if (number == 1)
            {
                scene = new Scene1();
            }
            else if (number == 2)
            {
                scene = new Scene2();
            }
            else if (number == 3)
            {
                scene = new Scene3();
            }
            else if (number >= 4)
            {
                cleardevice();
                TCHAR ct[100];
                _stprintf_s(ct, "恭喜通关，您被击中的次数是%d", player.hp);
                settextcolor(YELLOW);
                outtextxy(200, 280, ct);
                FlushBatchDraw();
                timer.Sleep(2000);
                break;
            }
            scene->init(&player);
        }
        if (scene->run())
        {
            TCHAR ct[100];
            _stprintf_s(ct, "恭喜通过第 %d 关", number);
            settextcolor(YELLOW);
            outtextxy(300, 280, ct);
            FlushBatchDraw();
            delete scene;
            scene = nullptr;
            number++;
            timer.Sleep(2000);
            continue;
        }
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
