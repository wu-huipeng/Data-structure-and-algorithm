#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<map>
#include<QTimer>
using namespace std;
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void build_table(QPoint);   //构建Q表
    void choice_action();  //采取行动
    void get_env_feedback(); //得到反馈
    void rl();

protected:
    void paintEvent(QPaintEvent *);
private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;

    QPoint des;   // 宝藏的位置
    QPoint barrier[5];  //陷阱的坐标
    QPoint S;    //当前状态
    QPoint S_next; //下一个状态
    double EPSILON;     //greedy police
    double LEARNING;    //学习率
    double LAMBDA;      //衰减率
    int FRESH_TIME;  //每次采取动作的时间
    map<QString,map<int,double>> table;
    int A;  //选择的行动
    int R; //回馈

     QTimer *timer;
     bool is_termainal;
};

#endif // WIDGET_H
