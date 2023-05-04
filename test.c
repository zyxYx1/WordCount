#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<malloc.h>
#include<io.h>

void Run(char Type, char Type2, char *Path);

int CodeCount(char *Path) {	//计算字符个数

	FILE *file = fopen(Path, "r");
	assert(file != NULL);	//若文件不存在则报错

	char code;
	int count = 0;

	while ((code = fgetc(file)) != EOF) 	//读取字符直到结束
		count+= ((code != ' ') && (code != '\n') && (code != '\t'));	//判断是否是字符

	fclose(file);

	return count;
}

int WordCount(char *Path) {	//计算单词个数

	FILE *file = fopen(Path, "r");
	assert(file != NULL);

	char word;
	int is_word = 0;	//用于记录字符是否处于单词中
	int count = 0;

	while ((word = fgetc(file)) != EOF) {
		if ((word >= 'a' && word <= 'z') || (word >= 'A' && word <= 'Z')) {	//判断是否是字母
			count += (is_word == 0);
			is_word = 1;	//记录单词状态
		} else
			is_word = 0;	//记录不处于单词状态
	}
	fclose(file);

	return count;
}

int LineCount(char *Path) {	//计算行数

	FILE *file = fopen(Path, "r");
	assert(file != NULL);

	char *s = (char*)malloc(200 * sizeof(char));
	int count = 0;

	for (; fgets(s, 200, file) != NULL; count++);	//逐次读行

	free(s);
	fclose(file);

	return count;
}

void AllDetail(char *Path) {	//显示空行， 代码行，注释行

	FILE *file = fopen(Path, "r");
	assert(file != NULL);

	char *s = (char*)malloc(200 * sizeof(char));//申请空间
	int i;
	int is_codeline = 0;	//状态记录变量
	int is_annoline = 0;
	int AnnoLock = 0;
	int AnnoFileLock = 0;

	int codecount = 0;
	int annocount = 0;
	int blankcount = 0;

	while (fgets(s, 200, file) != NULL) {	//逐次取文件中的行
		for (i = 0; *(s+i) != '\0'; i++) {

			if ( ( ( *(s+i) >= 'a' && *(s+i) <= 'z') || ( *(s+i) >= 'A' && *(s+i) <= 'Z') ) && AnnoFileLock == 0) {//判断是否是代码行
				codecount += (is_codeline == 0 && AnnoLock == 0);	//进入代码行的时候代码行加一
				is_codeline = 1;
			}

			if ( *(s+i) == '/' && *(s+i+1) == '/' && is_codeline == 0 && AnnoFileLock == 0) {	//判断是否为注释行
				annocount++;
				AnnoLock = 1;
			}

			if (*(s + i) == '/' && *(s + i + 1) == '*') {	//判断文档注释开始
				AnnoFileLock = 1;
				annocount -= is_codeline;	//注释在代码后不算注释行，因此减一
			}

			if (*(s + i) == '*' && *(s + i + 1) == '/') {	//判断文档注释结束
				AnnoFileLock = 0;
				annocount += (*(s + i + 2) == '\n');	//注释后换行情况
			}
		}
		annocount += AnnoFileLock;	//注释行结束时算作注释行加一

		blankcount++;	//每一行结束计数加一，并清空状态
		is_codeline = 0;
		is_annoline = 0;
		AnnoLock = 0;
	}
	free(s);
	fclose(file);

	blankcount = blankcount - codecount - annocount;
	printf("codeline:%d, annoline:%d, blankline:%d\n", codecount, annocount, blankcount);
}

void Scan(char *Path, char Type) {
	char *FileName = NULL;
	char *FileType = NULL;
	char Temp[30];	//用于暂存改变得字符串
	long Head;
	struct _finddata_t FileData;
	int i = 0;

	FileName = Path;
	while (*(Path + i) != '\0') {	//找出文件名和文件类型的位置
		if (*(Path + i) == '\\')
			FileName = Path + i + 1;
		if (*(Path + i) == '.')
			FileType = Path + i + 1;
		i++;
	}

	strcpy(Temp, FileType);//调整字符串
	*FileType = '*';
	*(FileType + 1) = '\0';

	Head = _findfirst(Path, &FileData);

	strcpy(FileType, Temp);//恢复字符串

	do {
		if ( !strcmp(FileData.name, "..") || !strcmp(FileData.name, "."))	//去除前驱文件路径
			continue;

		if (_A_SUBDIR == FileData.attrib) {	//是文件夹
			strcpy(Temp, FileName);	//调整字符串
			for (i = 0; *(FileData.name + i) != '\0'; i++) {
				*(FileName + i) = *(FileData.name + i);
			}
			*(FileName + i) = '\\';
			*(FileName + i + 1) = '\0';
			strcat(Path, Temp);

			Scan(Path, Type);

			strcpy(FileName, Temp);	//恢复字符串
		} else { //是文件
			for (i = 0; *(FileData.name + i) != '.'; i++);
			if (!strcmp(FileData.name + i + 1, FileType)) {	//是指定类型的文件

				strcpy(Temp, FileName);
				strcpy(FileName, FileData.name); //调整字符串

				printf("%s:  ", FileData.name);
				Run(Type, NULL, Path);	//将地址及功能传到启动函数
				printf("\n");

				strcpy(FileName, Temp);//恢复字符串
			}
		}
	} while (_findnext(Head, &FileData) == 0);

	_findclose(Head);
}

void Run(char Type, char Type2, char *Path) {

	switch (Type) {
		case 'c':
			printf("code count: %d\n", CodeCount(Path));
			break;
		case 'w':
			printf("word count: %d\n", WordCount(Path));
			break;
		case 'l':
			printf("line count: %d\n", LineCount(Path));
			break;
		case 'a':
			AllDetail(Path);
			break;
		case 's':
			Scan(Path, Type2);
			break;
		default:
			printf("type input error");
			break;
	}
}

int main(int argc, char *argv[]) {

	char Path[100] = "*.c";	//默认参数
	char Type = 's';
	char Type2 = 'c';

	if (argv[1]) {	//有输入参数则以输入为准
		Type = *(argv[1] + 1);
		if (Type == 's') {
			Type2 = *(argv[2] + 1);
			strcpy(Path, argv[3]);
		} else
			strcpy(Path, argv[2]);
	}

	Run(Type, Type2, Path);	//调用启动函数

	printf("\nPress any key to continue");
	getchar();

	return 0;
}
