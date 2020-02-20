#include "lcd.h"

Lcd *Lcd::instance = NULL;

Lcd::Lcd(std::string device)
{
    this->LcdOpen(device);
}

Lcd::~Lcd()
{
   this->Lcdclose();
    delete this->bmpbuf;//释放内存
    delete this->bmpcopy;
}

Lcd* Lcd::getInstance()
{
    if(instance == NULL)
    {
        instance = new Lcd;
    }
    return instance;
}

unsigned int* Getfd()
{
    Lcd *lcd = Lcd::getInstance();
    return lcd->mmp;
}

void Lcd::LcdOpen(std::string path)
{
    this->fd = ::open(path.c_str(),O_RDWR);
    if(this->fd>0)
    {
        //打开成功
        this->isOpen = true;
    }else{
        perror("open fail");
    }

    //映射
    struct fb_var_screeninfo info;//屏幕信息结构体

    ioctl(this->fd,FBIOGET_VSCREENINFO,&info);

    std::cout<<info.xres_virtual<<info.yres_virtual<<std::endl;

    this->width = info.xres;
    this->high = info.yres;
    this->pixel = info.bits_per_pixel/8;

    this->mmemsize = this->width*this->high*this->pixel;

    this->mmp = (unsigned int *)mmap(NULL,this->mmemsize,PROT_READ|PROT_WRITE,MAP_SHARED,this->fd,0);
    if(mmp == (void*)-1)
    {
        perror("mmap fail");
    }

}

void Lcd::Lcdclose()
{
    if(this->isOpen)
    {
        munmap(this->mmp,this->mmemsize);
    }
    if(::close(this->fd)!=-1)
    {
        this->isOpen = false;
    }
    ::close(this->pic_fd);
}

void Lcd::PaintPoint(int x, int y)
{
    unsigned int *p = this->mmp+y*this->width+x;//起点
    *p = 0xff0000;
}

void Lcd::PaintLine(double x, double y, double x2,double y2)
{
    double tmp = (y2-y)/(x2-x);
    std::cout<<fabs(tmp)<<std::endl;

    unsigned int *p = this->mmp+this->width*(int)y+(int)x;

    for(int i=0;i<fabs(y2-y);i++)
    {

        p[(int)(i/tmp)+i*this->width]=0xff0000;
    }
}

void Lcd::PaintRect(int x, int y, int w, int h)
{
    unsigned int *p = this->mmp+y*this->width+x;//起点
    for(int i=0;i<h;i++)
    {
        for(int j=0;j<w;j++)
        {
            p[j+i*this->width] = 0xff0000;
        }

    }
    
}

//获取bmp
void Lcd::LcdPicture(std::string pic)
{
    this->pic_fd = ::open(pic.data(),O_RDWR);
    if(this->pic_fd<0)
    {
        perror("pic open fail");

    }
    int x,y,offset,bit;
    char buf[3];
    //获取图片开头BM
    lseek(this->pic_fd,2,0);
    read(this->pic_fd,buf,3);

    //获取偏移量
    lseek(this->pic_fd,10,0);
    read(this->pic_fd,&offset,4);
    //获取图片宽x
    lseek(this->pic_fd,18,0);
    read(this->pic_fd,&x,4);
    //获取图片高y
    lseek(this->pic_fd,22,0);
    read(this->pic_fd,&y,4);
    //获取图片位数
    lseek(this->pic_fd,28,0);
    read(this->pic_fd,&bit,4);
    //定义bmpbuf存放bmp的像素,bmpsize存放大小
    this->pic_x = x;
    this->pic_y = y;
    this->pic_Bit = bit;

    this->bmpsize = x*y*bit/8;
    std::cout<<this->pic_x<<this->pic_y<<std::endl;
    this->bmpbuf = new unsigned char[this->bmpsize];
    //读取图片bmp数据，偏移offset位后在读取，存放在bmpbuf
    lseek(this->pic_fd,offset,0);
    read(this->pic_fd,this->bmpbuf,this->bmpsize);
}

//画图
void Lcd::showPic(int x, int y)
{
    //获取映射内存指针mmp
    unsigned int *p = this->mmp+y*this->width+x;//起始位置
    for(int i=0;i<this->pic_y;i++)
    {
        for(int j=0;j<this->pic_x;j++)
        {
            *(p+i*this->width+j) = *((this->bmpbuf)+((this->high-1-i)*this->width+j)*3+0)|*((this->bmpbuf)+((this->high-1-i)*this->width+j)*3+1)<<8|*((this->bmpbuf)+((this->high-1-i)*this->width+j)*3+2)<<16;
        }
    }
}
//缩放图片
void Lcd::showAsizePic(int x,int y,int w,int h)
{
    unsigned int *p = this->mmp+y*this->width+x;
    int i0,j0;
    for(int i=0;i<h;i++)
    {
        for(int j=0;j<w;j++)
        {
            j0=(j*this->pic_x)/w;//比例后的宽
            i0=(i*this->pic_y)/h;//比例后的高
            *(p+i*this->pic_x+j) = *((this->bmpbuf)+((this->pic_y-1-i0)*this->pic_x+j0)*3+0)|*((this->bmpbuf)+((this->pic_y-1-i0)*this->pic_x+j0)*3+1)<<8|*((this->bmpbuf)+((this->high-1-i0)*this->pic_x+j0)*3+2)<<16;

        }
    }
}
void Lcd::copybmp()
{
    this->bmpcopy = new unsigned char[this->bmpsize];
    memcpy(this->bmpcopy,(unsigned char*)this->mmp,this->bmpsize);
}

void Lcd::copyshow()
{
    memcpy((unsigned char*)this->mmp,this->bmpcopy,this->bmpsize);
}
