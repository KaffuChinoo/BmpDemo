# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "pch.h"
// 使代码在新版本的VS中正常运行不报错
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
int bmpW; //图像的宽
int bmpH; //图像的高
RGBQUAD* pColor; //颜色表指针
int biBitCnt; //图像类型， 每像素位数
unsigned char* pBmpPixels; //要恢复灰度值数组
int byteLine; //每一行的字节数
BITMAPFILEHEADER hinfo; //头部
BITMAPINFOHEADER header; //头部信息
unsigned char* a; //压缩的灰度值数组

void load(unsigned char* a, int* i, int* left, int bit, int* temp)//将 temp 存到数组里
{
	if (*left <= bit) 
	{
		*temp += ((a[(*i)++] << (8 - (*left))) & 0xff) >> (8 - bit);
		*temp += a[(*i)] >> (8 - (bit - (*left)));
		*left = 8 - (bit - (*left));
	}
	else 
	{
		*temp += ((a[(*i)] >> ((*left) - bit) << (8 - bit)) & 0xff)
			>> (8 - bit);
		*left -= bit;
	}
}

void craming()
{
	FILE* ss = fopen("unpack.bmp", "wb");
	int left = 8, bit, length, k;
	int i = 0;//解压前数组 index
	int j = 0;//解压后数组 index
	while (i < header.biSizeImage)
	{
		length = 0;
		bit = 0;
		load(a, &i, &left, 8, &length);
		load(a, &i, &left, 3, &bit);
		length++; //因为压缩的时候有减1
		bit++;
		k = j;
		for (; j < k + length; j++)
		{
			load(a, &i, &left, bit, (int*)&pBmpPixels[j]);
		}
	}
	header.biSizeImage = bmpW * bmpH; //恢复像素所占的空间
	fwrite(&hinfo, sizeof(BITMAPFILEHEADER), 1, ss);
	fwrite(&header, sizeof(BITMAPINFOHEADER), 1, ss);
	fwrite(pColor, sizeof(RGBQUAD), 256, ss);
	fwrite(pBmpPixels, 1, header.biSizeImage, ss);
	fclose(ss);
}

unsigned char* readBmpUn(const char* file)
{
	unsigned char* a;
	FILE* fp = fopen(file, "rb");
	if (fp == nullptr)
	{
		perror("Failed to open file");
	}
	fread(&hinfo, sizeof(BITMAPFILEHEADER), 1, fp); //获取图片头信息
	fread(&header, sizeof(BITMAPINFOHEADER), 1, fp); //定义位图信息头结构变量， 读取位图信息头进内存， 存放在变量 header 中
	bmpW = header.biWidth; //获取图像宽、 高、 每像素所占位数等信息
	bmpH = header.biHeight;
	biBitCnt = header.biBitCount;
	byteLine = (bmpW * biBitCnt / 8 + 3) / 4 * 4;//定义变量， 计算图像每行像素所占的字节数（必须是4的倍数）
	if (biBitCnt == 8)
	{
		pColor = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);//申请颜色表所需要的空间， 读颜色表进内存
		fread(pColor, sizeof(RGBQUAD), 256, fp); //灰度图像有颜色表， 且颜色表表项为256
	}
	a = (unsigned char*)malloc(header.biSizeImage);
	fread(a, 1, header.biSizeImage, fp);
	fclose(fp); //关闭文件
	return a;
}

void bmpUnpack(const char* file)
{
	a = readBmpUn(file);
	pBmpPixels = (unsigned char*)malloc(sizeof(unsigned char) * byteLine * bmpH);//申请位图数据所需要的空间， 读位图数据进内存
	memset(pBmpPixels, 0, byteLine * bmpH);
	craming();
}

