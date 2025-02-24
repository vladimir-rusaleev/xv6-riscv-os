#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE 25

int
main()
{
	char buf[BUF_SIZE];
	int i = 0;
	int res, c;

	for(i=0; i+1 < BUF_SIZE; ){
		res = read(0, &c, 1);
		if(res < 1)
			break;
		buf[i++] = c;
		if(c == '\n' || c == '\r')
			break;
	}
	if (res < 0) {
		fprintf(2,"%s\n", "Ошибка чтения");
		exit(1);
	}
	if (buf[i-1] != '\n'){
		fprintf(2, "%s\n", "Слишком большая суммарная длина чисел");
		exit(1);
	}

	buf[i-1] = '\0';

	printf("|%s|\n", buf);
	
	char* ptr = buf;
	char first[BUF_SIZE], second[BUF_SIZE];
	char* fst = first; 
	char* snd = second;
	while(*ptr != ' ' && *ptr != '\0')
	{
		if(*ptr < '0' || *ptr > '9'){
			fprintf(2, "%s\n", "Это не число");
			exit(1);
		}
		*fst++ = *ptr++;
	}
	if (strlen(first) == 0){
		fprintf(2, "%s\n", "Первого числа нет");
		exit(1);
	}
	
	ptr++;
	while(*ptr != ' ' && *ptr != '\0'){
		if(*ptr < '0' || *ptr > '9'){
                        fprintf(2, "%s\n", "Это не число");
                        exit(1);
                }
		*snd++ = *ptr++;
	}
	if( strlen(second) == 0){
		fprintf(2, "%s\n", "Второго числа нет");
		exit(1);
	}

	int x, y;
	int sign1 = 1, sign2 = 1;
	
	fst = first;
	if(first[0] == '-')
	{
		sign1 = -1;
		fst++;
	}
	x = atoi(fst) * sign1;
	
	snd = second;
	if(second[0] == '-') {
		sign2 = -1;
		snd++;
	}
	y = atoi(snd) * sign2;

	printf("%d + %d = %d\n", x, y, x + y);

	exit(0);
}
