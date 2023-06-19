# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "pch.h"
# define lmax 256 //压缩时每一段包含的最大像素数
# define header 11 //每一段需要11位来保存这一段的信息（8位用来表示段长度
// 使代码在新版本的VS中正常运行不报错
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

//3位用来表示每一像素占的位数
int bmpWidth; //图像的宽
int bmpHeight; //图像的高
RGBQUAD* pColorTable; //颜色表指针
int biBitCount; //图像类型， 每像素位数
unsigned char* pBmpBuf; //灰度值数组
int lineByte; //每一行的字节数
BITMAPFILEHEADER h; //头部
BITMAPINFOHEADER head; //头部信息
int i, j; // 循环要用到的遍历变量
int* s, * l, * b; //这三个数组的意义和书上是一样的
int m; //分割的段数
int le(int i) //log(i+1)向上取整
{
	int k = 1;
	i = i / 2;
	while (i > 0) 
	{
		k++;
		i = i / 2;
	}
	return k;
}
void traceback(int n, int s[], int l[]) //和书上一样不解释了
{
	__try
	{
		if (n <= 0) return;
		traceback(n - l[n], s, l);
		s[m++] = n - l[n];
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//忽略栈溢出异常
	}
}
void compress(unsigned char p[], int s[], int l[], int b[])//这个也和书上一样
{
	int n = bmpWidth * bmpHeight;
	int bmax;
	s[0] = 0;
	for (i = 1; i <= n; i++) 
	{
		b[i] = le(p[i]);
		bmax = b[i];
		s[i] = s[i - 1] + bmax;
		l[i] = 1;
		for (j = 2; j <= i && j <= lmax; j++) 
		{
			if (bmax < b[i - j + 1])
				bmax = b[i - j + 1];
			if (s[i] > s[i - j] + j * bmax) 
			{
				s[i] = s[i - j] + j * bmax;
				l[i] = j;
			}
		}
		s[i] += header;
	}
}
void collect(unsigned char p[], int s[], int l[], int b[])
{
	int max;
	int n = bmpWidth * bmpHeight;
	traceback(n, s, l);
	s[m] = n;
	for (j = 1; j <= m; j++) 
	{
		l[j] = l[s[j]];
		max = -1;
		for (i = s[j - 1] + 1; i <= s[j]; i++) 
		{ 
			//特别注意这个地方，书上错了
			if (le(p[i]) > max) //b[i]应该取第 i 段中占用位数最大的那个
				max = le(p[i]);
		}
		b[j] = max;
	}
}
void store(unsigned char* a, int* j, int* left, int bit, int temp)//将 temp 存到数组里
{
	if (*left <= bit) {
		a[(*j)++] += temp >> (bit - (*left));
		a[(*j)] += (temp << (8 + (*left) - bit)) & 0xff;
		*left = 8 - (bit - (*left));
	}
	else {
		a[(*j)] += (temp << (*left - bit)) & 0xff;
		*left -= bit;
	}
}
void compressToFile(const char* file, unsigned char p[], int l[], int b[], int m) //压缩存到文件里
{
	FILE* out = fopen(file, "wb");
	int n = bmpWidth * bmpHeight, left, k = 1, t;
	unsigned char* a = (unsigned char*)malloc(n);
	memset(a, 0, n);
	left = 8;
	j = 0;
	for (i = 1; i <= m; i++) {
		store(a, &j, &left, 8, l[i] - 1);
		store(a, &j, &left, 3, b[i] - 1);
		t = k + l[i];
		for (; k < t; k++) {
			store(a, &j, &left, b[i], p[k]);
		}
	}
	head.biSizeImage = j + 1; //修改文件大小，这个解压图像中要用到
	fwrite(&h, sizeof(BITMAPFILEHEADER), 1, out);
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, out);
	fwrite(pColorTable, sizeof(RGBQUAD), 256, out);
	fwrite(a, 1, static_cast<size_t>(j) + 1, out);
	fclose(out);
}
unsigned char* readBmp(const char* file) //读取文件
{
	unsigned char* pBmpBuf;
	FILE* fp = fopen(file, "rb");
	fread(&h, sizeof(BITMAPFILEHEADER), 1, fp); //获取图片头信息
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //定义位图信息头结构变量，读取位图信息头进内存，存放在变量 head 中
	bmpWidth = head.biWidth; //获取图像宽、高、每像素所占位数等信息
	bmpHeight = head.biHeight;
	biBitCount = head.biBitCount;
	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	if (biBitCount == 8) {
		pColorTable = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);//申请颜色表所需要的空间， 读颜色表进内存
		fread(pColorTable, sizeof(RGBQUAD), 256, fp); //灰度图像有颜色表，且颜色表表项为256
	}
	pBmpBuf = (unsigned char*)malloc(sizeof(unsigned char) * (lineByte * bmpHeight + 1));//申请位图数据所需要的空间，读位图数据进内存
	fread(pBmpBuf + 1, 1, static_cast<size_t>(lineByte) * bmpHeight, fp); //为了数组下标从1开始
	fclose(fp); //关闭文件
	return pBmpBuf;
}
void bmpPack(const char* file)
{
	pBmpBuf = readBmp(file);
	s = (int*)malloc(sizeof(int) * (static_cast<unsigned long long>(bmpWidth) * bmpHeight + 1));
	l = (int*)malloc(sizeof(int) * (static_cast<unsigned long long>(bmpWidth) * bmpHeight + 1));
	b = (int*)malloc(sizeof(int) * (static_cast<unsigned long long>(bmpWidth) * bmpHeight + 1));
	compress(pBmpBuf, s, l, b);
	collect(pBmpBuf, s, l, b);
	compressToFile("packed.bmp", pBmpBuf, l, b, m);
}