#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<malloc.h>
#include<io.h>

void Run(char Type, char Type2, char *Path);

int CodeCount(char *Path) {	//�����ַ�����

	FILE *file = fopen(Path, "r");
	assert(file != NULL);	//���ļ��������򱨴�

	char code;
	int count = 0;

	while ((code = fgetc(file)) != EOF) 	//��ȡ�ַ�ֱ������
		count+= ((code != ' ') && (code != '\n') && (code != '\t'));	//�ж��Ƿ����ַ�

	fclose(file);

	return count;
}

int WordCount(char *Path) {	//���㵥�ʸ���

	FILE *file = fopen(Path, "r");
	assert(file != NULL);

	char word;
	int is_word = 0;	//���ڼ�¼�ַ��Ƿ��ڵ�����
	int count = 0;

	while ((word = fgetc(file)) != EOF) {
		if ((word >= 'a' && word <= 'z') || (word >= 'A' && word <= 'Z')) {	//�ж��Ƿ�����ĸ
			count += (is_word == 0);
			is_word = 1;	//��¼����״̬
		} else
			is_word = 0;	//��¼�����ڵ���״̬
	}
	fclose(file);

	return count;
}

int LineCount(char *Path) {	//��������

	FILE *file = fopen(Path, "r");
	assert(file != NULL);

	char *s = (char*)malloc(200 * sizeof(char));
	int count = 0;

	for (; fgets(s, 200, file) != NULL; count++);	//��ζ���

	free(s);
	fclose(file);

	return count;
}

void AllDetail(char *Path) {	//��ʾ���У� �����У�ע����

	FILE *file = fopen(Path, "r");
	assert(file != NULL);

	char *s = (char*)malloc(200 * sizeof(char));//����ռ�
	int i;
	int is_codeline = 0;	//״̬��¼����
	int is_annoline = 0;
	int AnnoLock = 0;
	int AnnoFileLock = 0;

	int codecount = 0;
	int annocount = 0;
	int blankcount = 0;

	while (fgets(s, 200, file) != NULL) {	//���ȡ�ļ��е���
		for (i = 0; *(s+i) != '\0'; i++) {

			if ( ( ( *(s+i) >= 'a' && *(s+i) <= 'z') || ( *(s+i) >= 'A' && *(s+i) <= 'Z') ) && AnnoFileLock == 0) {//�ж��Ƿ��Ǵ�����
				codecount += (is_codeline == 0 && AnnoLock == 0);	//��������е�ʱ������м�һ
				is_codeline = 1;
			}

			if ( *(s+i) == '/' && *(s+i+1) == '/' && is_codeline == 0 && AnnoFileLock == 0) {	//�ж��Ƿ�Ϊע����
				annocount++;
				AnnoLock = 1;
			}

			if (*(s + i) == '/' && *(s + i + 1) == '*') {	//�ж��ĵ�ע�Ϳ�ʼ
				AnnoFileLock = 1;
				annocount -= is_codeline;	//ע���ڴ������ע���У���˼�һ
			}

			if (*(s + i) == '*' && *(s + i + 1) == '/') {	//�ж��ĵ�ע�ͽ���
				AnnoFileLock = 0;
				annocount += (*(s + i + 2) == '\n');	//ע�ͺ������
			}
		}
		annocount += AnnoFileLock;	//ע���н���ʱ����ע���м�һ

		blankcount++;	//ÿһ�н���������һ�������״̬
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
	char Temp[30];	//�����ݴ�ı���ַ���
	long Head;
	struct _finddata_t FileData;
	int i = 0;

	FileName = Path;
	while (*(Path + i) != '\0') {	//�ҳ��ļ������ļ����͵�λ��
		if (*(Path + i) == '\\')
			FileName = Path + i + 1;
		if (*(Path + i) == '.')
			FileType = Path + i + 1;
		i++;
	}

	strcpy(Temp, FileType);//�����ַ���
	*FileType = '*';
	*(FileType + 1) = '\0';

	Head = _findfirst(Path, &FileData);

	strcpy(FileType, Temp);//�ָ��ַ���

	do {
		if ( !strcmp(FileData.name, "..") || !strcmp(FileData.name, "."))	//ȥ��ǰ���ļ�·��
			continue;

		if (_A_SUBDIR == FileData.attrib) {	//���ļ���
			strcpy(Temp, FileName);	//�����ַ���
			for (i = 0; *(FileData.name + i) != '\0'; i++) {
				*(FileName + i) = *(FileData.name + i);
			}
			*(FileName + i) = '\\';
			*(FileName + i + 1) = '\0';
			strcat(Path, Temp);

			Scan(Path, Type);

			strcpy(FileName, Temp);	//�ָ��ַ���
		} else { //���ļ�
			for (i = 0; *(FileData.name + i) != '.'; i++);
			if (!strcmp(FileData.name + i + 1, FileType)) {	//��ָ�����͵��ļ�

				strcpy(Temp, FileName);
				strcpy(FileName, FileData.name); //�����ַ���

				printf("%s:  ", FileData.name);
				Run(Type, NULL, Path);	//����ַ�����ܴ�����������
				printf("\n");

				strcpy(FileName, Temp);//�ָ��ַ���
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

	char Path[100] = "*.c";	//Ĭ�ϲ���
	char Type = 's';
	char Type2 = 'c';

	if (argv[1]) {	//�����������������Ϊ׼
		Type = *(argv[1] + 1);
		if (Type == 's') {
			Type2 = *(argv[2] + 1);
			strcpy(Path, argv[3]);
		} else
			strcpy(Path, argv[2]);
	}

	Run(Type, Type2, Path);	//������������

	printf("\nPress any key to continue");
	getchar();

	return 0;
}
