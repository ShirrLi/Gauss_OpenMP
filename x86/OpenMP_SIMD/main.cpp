#include<iostream>
#include <windows.h>
#include<omp.h>
#include <stdlib.h>
#include<semaphore.h>
#include<xmmintrin.h>
#include<emmintrin.h>
#include<immintrin.h>
#include<ammintrin.h>
using namespace std;
float A[2000][2000],B[2000][2000],C[2000][2000],D[2000][2000];
int n;
const int NUM_THREADS=7;
void Initialize(int n)//��ʼ��
{
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++){
			//������Ԫ�س�ʼ��Ϊ��
			B[i][j]=0;
			D[i][j]=0;
		}
		//�Խ���Ԫ�س�ʼ��Ϊ1
		B[i][i]=1.0;
		D[i][i]=1.0;
		for(j=i+1;j<n;j++){
			//������Ԫ�س�ʼ��Ϊ�����
			B[i][j]=rand();
			D[i][j]=B[i][j];
		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				//����ÿһ�е�ֵ����һ�е�ֵ����һ�е�ֵ֮��
				B[i][j]+=B[k][j];
				D[i][j]+=D[k][j];
			}
}
void OpenMP_SSE(){
    int i,j,k;
    float tmp[4];
    __m128 t1,t2,t3,t4; //����4�������Ĵ���
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,t1,t2,t3,t4)
    for(k=0;k<n;k++)
    {
         #pragma omp single
        {
        tmp[0]=B[k][k];tmp[1]=B[k][k];tmp[2]=B[k][k];tmp[3]=B[k][k];
        t1=_mm_loadu_ps(tmp); //���ص�t1�����Ĵ���
        for(j=k+1;j+4<=n;j+=4)
        {
            t2=_mm_loadu_ps(B[k]+j); //���ڴ��д�B[k][j]��ʼ���ĸ������ȸ��������ص�t2�Ĵ���
            t3=_mm_div_ps(t2,t1); //�������ŵ�t3�Ĵ���
            _mm_storeu_ps(B[k]+j,t3); //��t3�Ĵ�����ֵ�Ż��ڴ�
        }
        for(j;j<n;j++) //����ʣ�µĲ��ܱ�4������
            B[k][j]/=B[k][k];
        B[k][k]=1.0;
        //��������˶Ե�һ�����ֵ�������
        }
        #pragma omp for schedule(static,1)
        for(i=k+1;i<n;i++)
        {
            tmp[0]=B[i][k];tmp[1]=B[i][k];tmp[2]=B[i][k];tmp[3]=B[i][k];
            t1=_mm_loadu_ps(tmp);
            for(j=k+1;j+4<=n;j+=4)
            {
                t2=_mm_loadu_ps(B[k]+j);
                t3=_mm_loadu_ps(B[i]+j);
                t4=_mm_mul_ps(t1,t2);
                t3=_mm_sub_ps(t3,t4);
                _mm_storeu_ps(B[i]+j,t3);
            }
            for(j=j;j<n;j++)
                B[i][j]-=B[i][k]*B[k][j];
            B[i][k]=0;
        }
    }
}
void OpenMP_AVX(){
    int i,j,k;
    float tmp[8];
    __m256_u t1,t2,t3,t4; //����4�������Ĵ���
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,t1,t2,t3,t4)
    for(k=0;k<n;k++){
        #pragma omp single
        {
            tmp[0]=D[k][k];tmp[1]=D[k][k];tmp[2]=D[k][k];tmp[3]=D[k][k];tmp[4]=D[k][k];tmp[5]=D[k][k];tmp[6]=D[k][k];tmp[7]=D[k][k];
            t1=_mm256_loadu_ps(tmp); //���ص�t1�����Ĵ���
            for(j=k+1;j+8<=n;j+=8)
            {
                t2=_mm256_loadu_ps(D[k]+j); //���ڴ��д�B[k][j]��ʼ���ĸ������ȸ��������ص�t2�Ĵ���
                t3=_mm256_div_ps(t2,t1); //�������ŵ�t3�Ĵ���
                _mm256_storeu_ps(D[k]+j,t3); //��t3�Ĵ�����ֵ�Ż��ڴ�
            }
            for(j;j<n;j++) //����ʣ�µĲ��ܱ�4������
                D[k][j]/=D[k][k];
            D[k][k]=1.0;
            //��������˶Ե�һ�����ֵ�������
        }
        #pragma omp for schedule(static,1)
        for(i=k+1;i<n;i++){
            tmp[0]=D[i][k];tmp[1]=D[i][k];tmp[2]=D[i][k];tmp[3]=D[i][k];tmp[4]=D[i][k];tmp[5]=D[i][k];tmp[6]=D[i][k];tmp[7]=D[i][k];
            t1=_mm256_loadu_ps(tmp);
            for(j=k+1;j+8<=n;j+=8)
            {
                t2=_mm256_loadu_ps(D[k]+j);
                t3=_mm256_loadu_ps(D[i]+j);
                t4=_mm256_mul_ps(t1,t2);
                t3=_mm256_sub_ps(t3,t4);
                _mm256_storeu_ps(D[i]+j,t3);
            }
            for(j=j;j<n;j++)
                D[i][j]-=D[i][k]*D[k][j];
            D[i][k]=0;
        }
    }
}
void Print(int n,float m[][2000]){//��ӡ���
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			cout<<m[i][j]<<" ";
		cout<<endl;
	}
}
int main(){
	int N=2000,count,cycle=1,step=10;
	LARGE_INTEGER t1,t2,tc1,t3,t4,tc2;
	Initialize(2000);
	for(n=10;n<=N;n+=step){
		count=1;
		if(n<=50)
            cycle=1000;
        else
            if(n<=70)
                cycle=100;
            else
                if(n<=400)
                    cycle=10;
                    else
                        cycle=1;

		QueryPerformanceFrequency(&tc1);
		QueryPerformanceCounter(&t1);
		while(count<=cycle){
			OpenMP_SSE();
            count++;}
        QueryPerformanceCounter(&t2);
        cout<<n<<" "<<count-1<<" "<<((t2.QuadPart - t1.QuadPart)*1000.0 / tc1.QuadPart);


        count=1;
		QueryPerformanceFrequency(&tc2);
		QueryPerformanceCounter(&t3);
		while(count<=cycle){
			OpenMP_AVX();
            count++;}
        QueryPerformanceCounter(&t4);
        cout<<" "<<((t4.QuadPart - t3.QuadPart)*1000.0 / tc2.QuadPart)<<endl;
		if(n==100)
            step=100;
        if(n==1000)
            step=500;

	}
}

