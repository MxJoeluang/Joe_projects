#ifndef LCD_H
#define LCD_H
#include <iostream>
extern "C"
{
#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/ioctl.h"
#include <sys/mman.h>
#include <stdbool.h>
#include <sys/time.h>
#include <linux/fb.h>//fb0
#include <linux/input.h>//input.h
#include "math.h"
#include "string.h"
}


class Lcd
{

public:
    Lcd(std::string device="/dev/fb0");
    ~Lcd();
    void LcdOpen(std::string path);
    void Lcdclose();
    void PaintPoint(int x,int y);
    void PaintLine(double x, double y, double x2, double y2);
    void PaintRect(int x,int y,int w, int h);

    static Lcd *getInstance();
    void LcdPicture(std::string pic);
    void showPic(int x, int y);
    void showAsizePic(int x, int y, int w, int h);
    void copybmp();
    void copyshow();
private:

    int fd;
    bool isOpen;
    int width;//宽度
    int high;//高度
    int pixel;//像素bits_per_pixel
    unsigned int *mmp;
    unsigned long mmemsize;

    //图片
    int pic_fd;
    int pic_x;
    int pic_y;
    int pic_Bit;
    unsigned char *bmpcopy;
    unsigned char *bmpbuf;
    unsigned long bmpsize;
    static Lcd *instance;
    friend unsigned int* Getfd();

};

#endif // LCD_H
