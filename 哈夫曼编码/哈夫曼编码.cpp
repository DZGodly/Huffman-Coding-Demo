#include<iostream>
#include<fstream>
#include<cstring>
#include <map>
using namespace std;

int Code_Size;
map<char, string> hfmCode;
map<char, string>::iterator iter;
typedef struct HFTree {
	char data;
	int weight;
	HFTree *lchild, *rchild, *parent;

	HFTree() {
		data = '#';
		weight = 0;
		lchild = NULL;
		rchild = NULL;
		parent = NULL;
	}
}*HFNode;

void selectMinW(HFNode t, int n, int&c1, int&c2) {
	//ѡ��Ȩֵ��С�������ڵ�
	int minW = 32767;
	c1 = c2 = -1;
	for (int i = 1; i <= n; i++) {
		if (t[i].parent != NULL) continue;
		if (t[i].weight <= minW) {
			c2 = c1;
			minW = t[i].weight;
			c1 = i;
		}
	}
	if (c2 < 0) {
		minW = 32767;
		for (int i = c1 + 1; i <= n; i++) {
			if (t[i].parent != NULL) continue;
			if (t[i].weight <= minW) {
				minW = t[i].weight;
				c2 = i;
			}
		}
	}
}

void preOrder(HFNode t,ofstream& fout) {
	//���������������
	if (t != NULL) {
		fout << t->data << t->weight;
		preOrder(t->lchild,fout);
		preOrder(t->rchild, fout);
	}
}

HFTree* Initialization(HFNode t, char** hfC) {
	int i = 1, j;
	int c1, c2;
	int start;
	//���ն˶����ַ�����Сn,�Լ�n���ַ���n��Ȩֵ
	int n;
	cout << "�������ַ���theCharSet�Ĵ�Сn: ";
	cin >> n;
	if (n <= 1) {
		cout << "��ʼ��ʧ�ܣ���Ϊn<=1.";
		return NULL;
	}
	int m = 2 * n - 1;
	t = new HFTree[m + 1];
	for (int i = n+1; i <= m; i++) {
		t[i].data = '#';
	}
	HFNode p1 = t + 1;
	cout << "����" << n << "���ַ������Ӧ��Ȩֵ" << endl;
	for (int i = 1; i <= n; i++, p1++) {
		cin.get(p1->data);
		cin>>p1->weight;
		cin.get();
		Code_Size += p1->weight;
	}

	//������������
	for (i = n + 1; i <= m; i++) {
		selectMinW(t, i - 1, c1, c2);
		t[c1].parent = &t[i]; t[c2].parent = &t[i];
		t[i].lchild = &t[c1]; t[i].rchild = &t[c2];
		t[i].weight = t[c1].weight + t[c2].weight;
	}
	//��������Ĺ���������n���ַ���Ҷ�ӽ�㣩�Ĺ���������
	hfC = new char*[n];
	char* cd = new char[n];
	cd[n - 1] = '\0';
	for (i = 1; i <= n; i++) {
		start = n - 1;
		for (HFNode c = &t[i], f = t[i].parent; f != NULL; c = f, f = c->parent)
			if (f->lchild == c) cd[--start] = '0';
			else cd[--start] = '1';
			hfC[i] = new char[n - 1];
			strcpy(hfC[i], &cd[start]);
	}
	//�����������
	ofstream fout("hfmTree.txt");
	if (!fout) {
		cerr << "open error!" << endl;
		exit(1);
	}
	HFNode t1 = &t[m];
	preOrder(t1,fout);
	for (i = 1; i <= n; i++) {
		j = 0;
		string code;
		while (hfC[i][j] != '\0') {
			code += hfC[i][j];
			j++;
		}
		//fout << '\n';
		hfmCode.insert(pair<char, string>(t[i].data, code));
	}
	fout.close();
	delete[] cd;
	return &t[m];
}

void Encoding(HFNode t, char** hfC) {
	//��������������ڴ棬����ļ�hfmTree�ж���
	if (t == NULL) {
		ifstream fin("hfmTree.txt");
		int i = 1, j = 0;
		if (!fin) {
			cerr << "�޷����ļ���" << endl;
			exit(0);
		}
		char a;
		while (fin.get(a)) {
			if (a != '\n') hfC[i][j++];
			else {
				i++; j = 0;
			}
		}
		fin.close();
	}
	//�����ļ�"ToBeTran"���������
	ifstream fin("ToBeTran.txt");//ToBeTran.txt����Ҫ������ı�:THIS PROGRAM IS MY FAVORITE
	string text;
	int i;	char s;

	for (i = 1; i <= Code_Size; i++) {
		fin.get(s);
		text += s;
	}
	string deCode;
	for (i = 0; i < Code_Size; i++) {
		for (iter = hfmCode.begin(); iter != hfmCode.end(); iter++) {
			if (text[i] == iter->first) deCode += iter->second;
		}
	}

	//����������ļ�CodeFile��
	ofstream fout("CodeFile.txt");
	for (i = 0; i < deCode.length(); i++) {
		fout << deCode[i];
	}
	fin.close();
	fout.close();
}

void Decoding(HFNode t) {
	//�����ѽ��õĹ����������ļ�CodeFile�еĴ����������
	ifstream fin("CodeFile.txt");
	ofstream fout("TextFile.txt");
	string code;
	HFNode head = t;
	char s;
	while (!fin.eof()) {
		fin.get(s);//��ȡ�ַ�
		if (s == '1') {//���Ϊ1�����Һ����ƶ�
			if (t->rchild)t = t->rchild;//������Һ������ƶ�
			else {//���û���Һ��������������ָ��������ڵ���Һ���
				fout << t->data;
				t = head->rchild;
			}
		}
		else{
			if (t->lchild)t = t->lchild;//������������ƶ�
			else {//���û�����������������ָ��������ڵ������
				fout << t->data;
				t = head->lchild;
			}
		}
		
	//	code += s;//�������
	//	for (iter = hfmCode.begin(); iter != hfmCode.end(); iter++) {
	//		if (code == iter->second) {
	//			fout << iter->first;
	//			code = "";
	//		}
	//	}
	}
	fin.close();
	fout.close();
}

void Print() {
	//���ļ�CodeFile�Խ��ո�ʽ��ʾ���ն��ϣ�ÿ��50�����롣ͬʱ�����ַ���ʽ�ı����ļ�д���ļ�CodePrin�С�
	ifstream fin("CodeFile.txt");
	ofstream fout("CodePrin.txt");
	string code;
	char s;
	for (int i = 1; !fin.eof(); i++) {
		if (i % 51 != 0) {
			fin.get(s);
			cout << s;
			fout << s;
		}
		else {
			cout << endl;
			fout << endl;
		}
	}
	cout << endl;
	fin.close();
	fout.close();
}

void TreePrint(HFNode t,ofstream& fout) {
	//�������ڴ��еĹ���������ֱ�۵ķ�ʽ��ʾ���ն��ϣ�ͬʱ�����ַ���ʽ�Ĺ�������д���ļ�TreePrint�С�
	if (t != NULL) {
		fout << t->data << t->weight;
		cout << t->data << t->weight;
		TreePrint(t->lchild,fout);
		TreePrint(t->rchild,fout);
	}
}

int main() {
	//Ҫ������ı�:THIS PROGRAM IS MY FAVORITE
	char** hfC = NULL;
	HFNode t = NULL;
	char flag;
	HFNode m = NULL;
	////���ն˶����ַ�����Сn,�Լ�n���ַ���n��Ȩֵ
	//int n;
	//cout << "�������ַ���theCharSet�Ĵ�Сn: ";
	//cin >> n;
	//if (n <= 1) {
	//	cout << "��ʼ��ʧ�ܣ���Ϊn<=1.";
	//	return NULL;
	//}
	//int m1 = 2 * n - 1;
	//t = new HFTree[m1 + 1];
	//for (int i = 1; i <= m1; i++) {
	//	t[i].data = '#';
	//}
	//HFNode p1 = t + 1;
	//cout << "����" << n << "���ַ������Ӧ��Ȩֵ" << endl;
	//for (int i=1; i <= n; i++, p1++) {
	//	cin >> p1->data >> p1->weight;
	//	Code_Size += p1->weight;
	//}

	cout << "��ѡ����:'I'����Initialization,'E'����Encoding��'D'����Decoding," << endl
		<< "            'P'����Print��'T'����Tree printing��'Q'�����˳�����." << endl;
	cin >> flag;
	while (flag != 'Q') {
		switch (flag) {

		case 'I':m = Initialization(t, hfC);cout << "��ʼ����ϣ���������һ�����ܴ��š�" << endl; break;

		case 'E':Encoding(t, hfC); cout << "������ϣ���������һ�����ܴ��š�" << endl; break;

		case 'D':Decoding(m); cout << "������ϣ���������һ�����ܴ��š�" << endl; break;

		case 'P': Print(); cout << "��ӡ��ϣ���������һ�����ܴ��š�" << endl; break;

		case 'T':	
			 ofstream fout("TreePrint.txt");
			 TreePrint(m, fout);
			 cout << endl << "��ӡ��ϣ���������һ�����ܴ��š�" << endl; 
			 fout.close();
			 break;

		}
		cin >> flag;
	}

	delete[] hfC;
	delete[] t;
	cout << endl;
	system("pause");
	return 0;
}