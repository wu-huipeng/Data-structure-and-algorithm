#include "widget.h"
#include "ui_widget.h"
#include<QPainter>
#include<QPen>
#include<QBrush>
#include<QThread>
#include<QDebug>
#include<QMessageBox>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Q-Learning算法模拟");
    timer = new QTimer(this);
    setFixedSize(500,500);   //固定窗口大小
    des.setX(6);       //初始化宝藏的位置
    des.setY(7);
    S.setX(1);
    S.setY(1);

    EPSILON = 0.9;
    LEARNING = 0.01;
    LAMBDA = 0.8;
    FRESH_TIME = 200;

    //初始化陷阱的位置
    barrier[0].setX(3);
    barrier[0].setY(4);
    barrier[1].setX(6);
    barrier[1].setY(3);
    barrier[2].setX(5);
    barrier[2].setY(8);
    barrier[3].setX(2);
    barrier[3].setY(6);
    barrier[4].setX(7);
    barrier[4].setY(3);
    build_table(S);
    connect(timer,&QTimer::timeout,
            [=]()
    {
        rl();
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    int w = width() / 10; //将窗口的宽平均分成10等分，w为每份的长度
    int h = height() / 10; //将窗口的高平均分成10等分，h为每份的长度

    QPainter p(this); //定义画家
    QPen pen; //定义画笔
    pen.setWidth(3);
    p.setPen(pen);

    for(int i=1;i<10;i++)
    {
        p.drawLine(w,h*i,width()-w,h*i);
        p.drawLine(w*i,h,w*i,height()-h);
    }

    QBrush brush; //设置画刷
    brush.setColor(Qt::red);
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    p.setBrush(brush);
    p.drawEllipse(des.x()*w,des.y()*h,w,h);   //绘制宝藏

    brush.setColor(Qt::green);
    p.setBrush(brush);

    for(int i = 0; i<5;i++)
    {
        p.drawEllipse(barrier[i].x()*w,barrier[i].y()*h,w,h); //绘制陷阱
    }
    brush.setColor(QColor(134,13,156));
    p.setBrush(brush);
    p.drawEllipse(S.x()*w,S.y()*h,w,h);
}



void Widget::build_table(QPoint s)
{
    QString str;
    str.setNum(s.x());
    str.append(char(s.y()));

    table[str][1] = 0.0;
    table[str][2] = 0.0;
    table[str][3] = 0.0;
    table[str][4] = 0.0;

}



void Widget::choice_action()
{
    QString str;
    str.setNum(S.x());
    str.append(char(S.y()));

    if(table.size() == 1)
        A = rand()%4;
    else if (rand()%(1000)/(float)(1000) > 0.9)
    {
        A = rand()%4;
    }
    else
    {
        A = 1;
        for(int i=2;i<=4;i++)
        {
            if(table[str][A]<table[str][i])
                A = i;
        }
    }

}

void Widget::get_env_feedback()
{

    if(A == 1)//up
    {
        S_next.setX(S.x()-1);
        S_next.setY(S.y());
    }
    else if (A == 2) //down
    {
        S_next.setX(S.x()+1);
        S_next.setY(S.y());
    }
    else if (A == 3) //left
    {
        S_next.setX(S.x());
        S_next.setY(S.y()-1);
    }
    else if (A == 4)  //right
    {
        S_next.setX(S.x());
        S_next.setY(S.y()+1);
    }

    if(S_next.x()<1 || S_next.y()<1 || S_next.x()>=9 || S_next.y()>=9)
    {
        S_next = S;
    }

    if(S_next == des) //如果为宝藏 激励为10
    {
        R = 40;
    }
    //如果为陷阱 激励为-10
    else if (S_next == barrier[0] || S_next == barrier[1]|| S_next == barrier[2]|| S_next == barrier[3]|| S_next == barrier[4])
    {
        R = -30;
    }
    //如果为墙 激励为-5
    else if(S_next == S)
    {
        R = -10;
    }
    QString str;
    str.setNum(S_next.x());
    str.append(char(S_next.y()));

    if(table.count(str) == 0)
        build_table(S_next);

}


void Widget::rl()
{
    if(is_termainal == true)
    {
        S.setX(1);
        S.setY(1);
        is_termainal = false;
   }
    update();
    choice_action(); //选择行动
    get_env_feedback(); // 得到环境的反馈
    QString str;
    str.setNum(S.x());
    str.append(char(S.y()));
    double q_pred = table[str][A];
    double q_target;
    if(R == 40 or R == -30)
    {
        if(R == 40)
            QMessageBox::information(this,"恭喜","找到宝藏");
        q_target = R;
        is_termainal =true;
    }
    else
    {
        QString str2;
        str2.setNum(S_next.x());
        str2.append(char(S_next.y()));
        int max = 1;
        for(int j=2;j<=4;j++)
        {
            if(table[str2][max]<table[str2][j])
                max = j;
        }
        q_target = R + LAMBDA * table[str2][max];
    }

    table[str][A] += LEARNING*(q_target - q_pred);
    S = S_next;

}
void Widget::on_pushButton_clicked()
{
    if(timer->isActive() == true)
        return;

    timer->start(100);
}
