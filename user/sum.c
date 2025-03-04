#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE 25

int
main()
{
	char buf[BUF_SIZE];
	int i = 0;
	int res, c;

	for(i=0; i < BUF_SIZE; i++){
		res = read(0, &c, 1);
		if(res < 1)
			break;
		buf[i] = c;
		if(c == '\n')
			break;
	}
	if (res < 0) {
		fprintf(2,"%s\n", "Ошибка чтения");
		exit(1);
	}
	if (i == BUF_SIZE && buf[BUF_SIZE - 1] != '\n'){
		fprintf(2, "%s\n", "Слишком большая суммарная длина чисел");
		exit(1);
	}

	buf[i] = '\0';

	printf("|%s|\n", buf);
	
	char* ptr = buf;
	char* fst = buf;

	while(*ptr != ' ' && *ptr != '\0')
	{
		if(*ptr < '0' || *ptr > '9'){
			fprintf(2, "%s\n", "Это не число");
			exit(1);
		}
		ptr++;
	}
	if (fst == ptr){
		fprintf(2, "%s\n", "Первого числа нет либо строка начинается с пробела");
		exit(1);
	}
	if( *ptr == ' '){
		*ptr++ = '\0';
	}

	int x = atoi(fst);
	
	char* snd = ptr;
	while(*ptr != ' ' && *ptr != '\0'){
		if(*ptr < '0' || *ptr > '9'){
                        fprintf(2, "%s\n", "Это не число");
                        exit(1);
                }
		ptr++;
	}
	if( snd == ptr){
		fprintf(2, "%s\n", "Второго числа нет либо между числами больше одного пробела");
		exit(1);
	}
	int y = atoi(snd);

	int ans = add(x, y);

	printf("%d + %d = %d\n", x, y, ans);

	exit(0);
}
