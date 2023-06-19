# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "pch.h"
// ʹ�������°汾��VS���������в�����
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
int bmpW; //ͼ��Ŀ�
int bmpH; //ͼ��ĸ�
RGBQUAD* pColor; //��ɫ��ָ��
int biBitCnt; //ͼ�����ͣ� ÿ����λ��
unsigned char* pBmpPixels; //Ҫ�ָ��Ҷ�ֵ����
int byteLine; //ÿһ�е��ֽ���
BITMAPFILEHEADER hinfo; //ͷ��
BITMAPINFOHEADER header; //ͷ����Ϣ
unsigned char* a; //ѹ���ĻҶ�ֵ����

void load(unsigned char* a, int* i, int* left, int bit, int* temp)//�� temp �浽������
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
	int i = 0;//��ѹǰ���� index
	int j = 0;//��ѹ������ index
	while (i < header.biSizeImage)
	{
		length = 0;
		bit = 0;
		load(a, &i, &left, 8, &length);
		load(a, &i, &left, 3, &bit);
		length++; //��Ϊѹ����ʱ���м�1
		bit++;
		k = j;
		for (; j < k + length; j++)
		{
			load(a, &i, &left, bit, (int*)&pBmpPixels[j]);
		}
	}
	header.biSizeImage = bmpW * bmpH; //�ָ�������ռ�Ŀռ�
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
	fread(&hinfo, sizeof(BITMAPFILEHEADER), 1, fp); //��ȡͼƬͷ��Ϣ
	fread(&header, sizeof(BITMAPINFOHEADER), 1, fp); //����λͼ��Ϣͷ�ṹ������ ��ȡλͼ��Ϣͷ���ڴ棬 ����ڱ��� header ��
	bmpW = header.biWidth; //��ȡͼ��� �ߡ� ÿ������ռλ������Ϣ
	bmpH = header.biHeight;
	biBitCnt = header.biBitCount;
	byteLine = (bmpW * biBitCnt / 8 + 3) / 4 * 4;//��������� ����ͼ��ÿ��������ռ���ֽ�����������4�ı�����
	if (biBitCnt == 8)
	{
		pColor = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);//������ɫ������Ҫ�Ŀռ䣬 ����ɫ����ڴ�
		fread(pColor, sizeof(RGBQUAD), 256, fp); //�Ҷ�ͼ������ɫ�� ����ɫ�����Ϊ256
	}
	a = (unsigned char*)malloc(header.biSizeImage);
	fread(a, 1, header.biSizeImage, fp);
	fclose(fp); //�ر��ļ�
	return a;
}

void bmpUnpack(const char* file)
{
	a = readBmpUn(file);
	pBmpPixels = (unsigned char*)malloc(sizeof(unsigned char) * byteLine * bmpH);//����λͼ��������Ҫ�Ŀռ䣬 ��λͼ���ݽ��ڴ�
	memset(pBmpPixels, 0, byteLine * bmpH);
	craming();
}

