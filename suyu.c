#include <string.h>
#include <ctype.h>
#include <stdio.h>

int main(void)
{
     const char data[12][6]={"zero","one","two","three","four","five","six","seven",   \
			 "eight","nine","+","=" }; /*对应的英文数字*/
     unsigned i,j,k;
     unsigned a,b;
     char str[120] = {0};
     char temp[6] = {0};
     char result[30] = {0};

     do
     {
	   a=b=i=j=k=0;

	   gets(str);
	   for(i=0,k=0;i<strlen(str);i++)
	   {
		for(j=0;!isspace(str[i]) && j<strlen(str);) /* temp接受数字数据*/
		{
			temp[j++]=str[i++];
		}
		temp[j]='\0';

		for(j=0;j<12;j++)
		{
			if(strcmp(temp,data[j])==0)  /*转换成对应的数字保存到result中*/
			{
				 if(j<=9) result[k++]=j+'0';
				 if(j==10) result[k++]='+';
				 if(j==11) result[k++]='=';
				 break;
			}
		}
	   }
	   result[k]='\0';
	   
	   printf("%s", result);
	   
	   sscanf(result,"%d+%d=\n",&a,&b); /*将数组写入a和b*/
	
	   printf("%d %d\n", a, b);
	   if(a==0 && b==0) break;       /*输入one + one =结束*/
	   else printf("%d\n",a + b );
     }
     while(1);

     return 0;
}