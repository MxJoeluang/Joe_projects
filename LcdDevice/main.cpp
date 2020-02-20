#include <iostream>
#include "lcd.h"
using namespace std;



int main()
{
    Lcd *lcd = Lcd::getInstance();
    lcd->LcdPicture("/root/pic/6.bmp");
    lcd->showPic(0,0);//画图
    lcd->copybmp();
    lcd->showAsizePic(100,100,200,100);
    lcd->PaintPoint(300,300);//画点
    lcd->PaintLine(200,200,20,345);//画线
    lcd->PaintRect(100,100,100,100);//画矩形
    sleep(3);
    lcd->copyshow();
    return 0;
}
