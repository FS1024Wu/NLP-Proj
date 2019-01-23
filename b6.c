#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<omp.h>
#include<math.h>
#include<time.h>
#include<unistd.h>
#include<ctype.h>
#define maxWordP 2008
#define maxWordN 4782
int* parallelPos(char (*Traincmt)[999],char (*posW)[20],int *posWCount);
int* parallelNeg(char (*Traincmt)[999],char (*negW)[20],int *negWCount);
void linearRegTrain(int *posWCount,int *negWCount);
int* parallelPosTest(char (*testCmt)[1027],char (*posW)[20],int *posCountTest);
int* parallelNegTest(char (*testCmt)[1027],char (*posW)[20],int *negCountTest);
void linearRegTest(int *posCountTest,int *negCountTest);
char* wordBuff(char *lineWord, char (*wordsSplit)[20]);
//void wordBuff(char *lineWord);
int main(int argc, char**  argv){
	FILE *flPos;
	flPos = popen("cat posWord.txt","r");
	char posWord[maxWordP][20]; char ps[20];
	int i=0;
	while(fgets(ps,sizeof(ps),flPos)!=NULL){
	strcat(posWord[i],ps);
	i++;
	}fclose(flPos);
  
	int j = 0;
	FILE *flNeg;
	flNeg = popen("cat negWord.txt","r");
	char negWord[maxWordN][20]; char ng[20];
	while(fgets(ng,sizeof(ng),flNeg)!=NULL){
	strcat(negWord[j],ng);
	j++;
	}fclose(flNeg);
    
	int k = 0;
	FILE *testSet;
	testSet = popen("cat sTest.txt","r");
	char test[100][1027]; char cmt[1027];	
	while(fgets(cmt,sizeof(cmt),testSet)!=NULL){
	strcat(test[k],cmt);
	k++;
	}fclose(testSet);

	int x = 0;
	FILE *trainSet;
	trainSet = popen("cat sTrain.txt","r");
	char train[20][999]; char cmt1[999];	
	while(fgets(cmt1,sizeof(cmt1),trainSet)!=NULL){
	strcat(train[x],cmt1);
	x++;
	}fclose(trainSet);	
	
	int posCount[20]={0};
	parallelPos(train,posWord,posCount);
		for(int il=0;il<20;il++)
			printf("%-3d,",posCount[il]);
printf("\n");
	int negCount[20]={0};
	parallelNeg(train,negWord,negCount);
		for(int il=0;il<20;il++)
			printf("%-3d,",negCount[il]);
		linearRegTrain(posCount,negCount);
	
	int posCountTest[100]={0};
	parallelPosTest(test,posWord,posCountTest);
		for(int il=0;il<100;il++)
			printf("%-3d,",posCountTest[il]);
printf("\n");	
	int negCountTest[100]={0};
	parallelNegTest(test,negWord,negCountTest);
		for(int il=0;il<100;il++)
			printf("%-3d,",negCountTest[il]);
	linearRegTest(posCountTest,negCountTest);
	

	return EXIT_SUCCESS;
}
int* parallelPos(char (*Traincmt)[999],char (*posW)[20],int *posWCount){
	char tc[999]; 	char pw[20]; char wordSplit[188][20];
	int sum =0;int i,ii,iii;
	double time_S = 0.0;
	clock_t begin = clock();
//#pragma omp parallel for schedule(dynamic,1)
#pragma omp parallel for reduction(+:sum) private(i,ii,iii,tc,pw,wordSplit)
	for(i=0;i<20;i++){	
		stpcpy(tc,Traincmt[i]);
		wordBuff(tc,wordSplit);
		for(iii=0;iii<188;iii++){
			for(ii=0;ii<maxWordP;ii++){
				stpcpy(pw,posW[ii]);
				strtok(pw,"\n"); //strtok(pw,"\t"); strtok(pw," "); //strcat(pw,"!");
				int ret=0; ret=strcmp(wordSplit[iii],pw);
					if(ret==0){
					sum++;break;}
			}
		}
		posWCount[i]=sum;
		sum=0;
	}
	 clock_t end = clock();
	 time_S += (double)(end-begin)/CLOCKS_PER_SEC; printf("time took %f sec : ",time_S);
	return posWCount;
}

int* parallelNeg(char (*Traincmt)[999],char (*negW)[20],int *negWCount){
	char tc[999]; 	char nw[20];char wordSplit[188][20];
	int sum =0;int i,ii,iii;
	double time_S = 0.0;
	clock_t begin = clock();
//#pragma omp parallel for schedule(dynamic,1)
#pragma omp parallel for reduction(+:sum) private(i,ii,iii,tc,nw,wordSplit)
	for(i=0;i<20;i++){	
		stpcpy(tc,Traincmt[i]);
		wordBuff(tc,wordSplit);
		for(iii=0;iii<188;iii++){
			for(ii=0;ii<maxWordN;ii++){
				stpcpy(nw,negW[ii]);
				strtok(nw,"\n");// strtok(nw,"\t"); strtok(nw," "); //strcat(pw,"!");
				int ret=0; ret=strcmp(wordSplit[iii],nw);
					if(ret==0){
					sum++;break;}
			}
		}
		negWCount[i]=sum;
		sum=0;
	}
	clock_t end = clock();
	 time_S += (double)(end-begin)/CLOCKS_PER_SEC; printf("time took %f sec : ",time_S);
	return negWCount;
}

void linearRegTrain(int *posWCount,int *negWCount){
		double sumX,sumY,sumPow2X,sumPow2Y,sumXY;
		double a,b;
		for(int i=0;i<20;i++){
			sumX+=posWCount[i];
			sumY+=negWCount[i];
			sumPow2X+=posWCount[i]*posWCount[i];
			sumPow2Y+=negWCount[i]*negWCount[i];
			sumXY+=posWCount[i]*negWCount[i];
		}
		a=(sumY*sumPow2X-sumX*sumXY)/(20*sumPow2X-(sumX*sumX));
		b=(20*sumXY-sumX*sumY)/(20*sumPow2X-(sumX*sumX));
		printf("\n\t Linear regression form y=%lf*x+%lf\n",b,a);
}

int* parallelPosTest(char (*testCmt)[1027],char (*posW)[20],int *posCountTest){
	char tc[1027]; char pw[20]; char wordSplit[188][20];
	int sum =0;int i,ii,iii;
	double time_S = 0.0;clock_t begin = clock();
#pragma omp parallel for reduction(+:sum) private(i,ii,iii,tc,pw,wordSplit)//#pragma omp parallel for schedule(dynamic,1)
	for(i=0;i<100;i++){	
		stpcpy(tc,testCmt[i]);
		wordBuff(tc,wordSplit);
		for(iii=0;iii<188;iii++){
			for(ii=0;ii<maxWordP;ii++){
			stpcpy(pw,posW[ii]);
			strtok(pw,"\n");// strtok(pw,"\t"); strtok(pw," "); //strcat(pw,"!");
				int ret=0; ret=strcmp(wordSplit[iii],pw);
					if(ret==0){
					sum++;break;}
			}
		}
		posCountTest[i]=sum;sum=0;
	}
	 clock_t end = clock();
	 time_S += (double)(end-begin)/CLOCKS_PER_SEC; printf("time took %f sec  \n",time_S);
	return posCountTest;
}

int* parallelNegTest(char (*testCmt)[1027],char (*negW)[20],int *negCountTest){
	char tc[1027]; 	char nw[20]; char wordSplit[188][20];
	int sum =0;int i,ii,iii;
	double time_S = 0.0;clock_t begin = clock();
#pragma omp parallel for reduction(+:sum) private(i,ii,iii,tc,nw,wordSplit)//#pragma omp parallel for schedule(dynamic,1)
	for(i=0;i<100;i++){	
		stpcpy(tc,testCmt[i]);
		wordBuff(tc,wordSplit);
		for(iii=0;iii<188;iii++){
			for(ii=0;ii<maxWordN;ii++){
			stpcpy(nw,negW[ii]);strtok(nw,"\n"); //strtok(pw,"\t"); strtok(pw," "); //strcat(pw,"!");
				int ret=0; ret=strcmp(wordSplit[iii],nw);
				if(ret==0){
				sum++;break;}
			}
		}
		negCountTest[i]=sum;sum=0;
	}
	 clock_t end = clock();
	 time_S += (double)(end-begin)/CLOCKS_PER_SEC; printf("time took %f sec  \n",time_S);
	return negCountTest;
}

void linearRegTest(int *posCountTest,int *negCountTest){
		double sumX,sumY,sumPow2X,sumPow2Y,sumXY;
		double a,b;
		for(int i=0;i<100;i++){
			sumX+=posCountTest[i];
			sumY+=negCountTest[i];
			sumPow2X+=posCountTest[i]*posCountTest[i];
			sumPow2Y+=negCountTest[i]*negCountTest[i];
			sumXY+=posCountTest[i]*negCountTest[i];
		}
		a=(sumY*sumPow2X-sumX*sumXY)/(20*sumPow2X-(sumX*sumX));
		b=(20*sumXY-sumX*sumY)/(20*sumPow2X-(sumX*sumX));
		printf("\n\t Linear regression form y=%lf*x+%lf\n",b,a);
}
char*  wordBuff(char *lineWord, char (*wordsSplit)[20]){
//void  wordBuff(char *lineWord){
    char *param[188];  char buffer[1027]; 
    int i; int j = 0; int k = 0; int inspace = 0;
    param[j] = buffer;
    for(i = 0; i < 1027; i++) {
        if(lineWord[i] == '\0') {
            param[j++][k] = '\0';
            param[j] = NULL;
            break;
        }
        else if(!isspace(lineWord[i])) {
            inspace = 0;
            param[j][k++] = lineWord[i];
        }
        else if (! inspace) {
            inspace = 1;
            param[j++][k] = '\0';
            param[j] = &(param[j-1][k+1]);
            k = 0;
            if(j == 188) {
                param[j] = NULL;
                break;
            }
        }
    }
    i = 0;
    while(param[i] != NULL)
    {
		char wordTook[20];
		char tok[]="!?:\", ";
		strcpy(wordTook,param[i]);
		strtok(wordTook,tok);
		strcpy(wordsSplit[i],wordTook);
        //printf("%s\n", param[i]);
        i++;
    }

    return *wordsSplit;
}
	
//exit