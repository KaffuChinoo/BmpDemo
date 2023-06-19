# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "pch.h"
# define lmax 256 //ѹ��ʱÿһ�ΰ��������������
# define header 11 //ÿһ����Ҫ11λ��������һ�ε���Ϣ��8λ������ʾ�γ���
// ʹ�������°汾��VS���������в�����
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

//3λ������ʾÿһ����ռ��λ��
int bmpWidth; //ͼ��Ŀ�
int bmpHeight; //ͼ��ĸ�
RGBQUAD* pColorTable; //��ɫ��ָ��
int biBitCount; //ͼ�����ͣ� ÿ����λ��
unsigned char* pBmpBuf; //�Ҷ�ֵ����
int lineByte; //ÿһ�е��ֽ���
BITMAPFILEHEADER h; //ͷ��
BITMAPINFOHEADER head; //ͷ����Ϣ
int i, j; // ѭ��Ҫ�õ��ı�������
int* s, * l, * b; //����������������������һ����
int m; //�ָ�Ķ���
int le(int i) //log(i+1)����ȡ��
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
void traceback(int n, int s[], int l[]) //������һ����������
{
	__try
	{
		if (n <= 0) return;
		traceback(n - l[n], s, l);
		s[m++] = n - l[n];
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//����ջ����쳣
	}
}
void compress(unsigned char p[], int s[], int l[], int b[])//���Ҳ������һ��
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
			//�ر�ע������ط������ϴ���
			if (le(p[i]) > max) //b[i]Ӧ��ȡ�� i ����ռ��λ�������Ǹ�
				max = le(p[i]);
		}
		b[j] = max;
	}
}
void store(unsigned char* a, int* j, int* left, int bit, int temp)//�� temp �浽������
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
void compressToFile(const char* file, unsigned char p[], int l[], int b[], int m) //ѹ���浽�ļ���
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
	head.biSizeImage = j + 1; //�޸��ļ���С�������ѹͼ����Ҫ�õ�
	fwrite(&h, sizeof(BITMAPFILEHEADER), 1, out);
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, out);
	fwrite(pColorTable, sizeof(RGBQUAD), 256, out);
	fwrite(a, 1, static_cast<size_t>(j) + 1, out);
	fclose(out);
}
unsigned char* readBmp(const char* file) //��ȡ�ļ�
{
	unsigned char* pBmpBuf;
	FILE* fp = fopen(file, "rb");
	fread(&h, sizeof(BITMAPFILEHEADER), 1, fp); //��ȡͼƬͷ��Ϣ
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ��� head ��
	bmpWidth = head.biWidth; //��ȡͼ����ߡ�ÿ������ռλ������Ϣ
	bmpHeight = head.biHeight;
	biBitCount = head.biBitCount;
	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;//�������������ͼ��ÿ��������ռ���ֽ�����������4�ı�����
	if (biBitCount == 8) {
		pColorTable = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);//������ɫ������Ҫ�Ŀռ䣬 ����ɫ����ڴ�
		fread(pColorTable, sizeof(RGBQUAD), 256, fp); //�Ҷ�ͼ������ɫ������ɫ�����Ϊ256
	}
	pBmpBuf = (unsigned char*)malloc(sizeof(unsigned char) * (lineByte * bmpHeight + 1));//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�
	fread(pBmpBuf + 1, 1, static_cast<size_t>(lineByte) * bmpHeight, fp); //Ϊ�������±��1��ʼ
	fclose(fp); //�ر��ļ�
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