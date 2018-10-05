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
	//选择权值最小的两个节点
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
	//遍历输出哈夫曼树
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
	//从终端读入字符集大小n,以及n个字符和n个权值
	int n;
	cout << "请输入字符集theCharSet的大小n: ";
	cin >> n;
	if (n <= 1) {
		cout << "初始化失败，因为n<=1.";
		return NULL;
	}
	int m = 2 * n - 1;
	t = new HFTree[m + 1];
	for (int i = n+1; i <= m; i++) {
		t[i].data = '#';
	}
	HFNode p1 = t + 1;
	cout << "输入" << n << "个字符及其对应的权值" << endl;
	for (int i = 1; i <= n; i++, p1++) {
		cin.get(p1->data);
		cin>>p1->weight;
		cin.get();
		Code_Size += p1->weight;
	}

	//建立哈夫曼树
	for (i = n + 1; i <= m; i++) {
		selectMinW(t, i - 1, c1, c2);
		t[c1].parent = &t[i]; t[c2].parent = &t[i];
		t[i].lchild = &t[c1]; t[i].rchild = &t[c2];
		t[i].weight = t[c1].weight + t[c2].weight;
	}
	//求所构造的哈夫曼树中n个字符（叶子结点）的哈夫曼编码
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
	//哈夫曼树输出
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
	//如哈夫曼树不在内存，则从文件hfmTree中读入
	if (t == NULL) {
		ifstream fin("hfmTree.txt");
		int i = 1, j = 0;
		if (!fin) {
			cerr << "无法打开文件！" << endl;
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
	//读入文件"ToBeTran"并对其编码
	ifstream fin("ToBeTran.txt");//ToBeTran.txt保存要编码的文本:THIS PROGRAM IS MY FAVORITE
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

	//将结果存入文件CodeFile中
	ofstream fout("CodeFile.txt");
	for (i = 0; i < deCode.length(); i++) {
		fout << deCode[i];
	}
	fin.close();
	fout.close();
}

void Decoding(HFNode t) {
	//利用已建好的哈夫曼树将文件CodeFile中的代码进行译码
	ifstream fin("CodeFile.txt");
	ofstream fout("TextFile.txt");
	string code;
	HFNode head = t;
	char s;
	while (!fin.eof()) {
		fin.get(s);//获取字符
		if (s == '1') {//如果为1则向右孩子移动
			if (t->rchild)t = t->rchild;//如果有右孩子则移动
			else {//如果没有右孩子则输出，并且指针移向根节点的右孩子
				fout << t->data;
				t = head->rchild;
			}
		}
		else{
			if (t->lchild)t = t->lchild;//如果有左孩子则移动
			else {//如果没有左孩子则输出，并且指针移向根节点的左孩子
				fout << t->data;
				t = head->lchild;
			}
		}
		
	//	code += s;//读入代码
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
	//将文件CodeFile以紧凑格式显示在终端上，每行50个代码。同时将此字符形式的编码文件写入文件CodePrin中。
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
	//将已在内存中的哈夫曼树以直观的方式显示在终端上，同时将此字符形式的哈夫曼树写入文件TreePrint中。
	if (t != NULL) {
		fout << t->data << t->weight;
		cout << t->data << t->weight;
		TreePrint(t->lchild,fout);
		TreePrint(t->rchild,fout);
	}
}

int main() {
	//要编码的文本:THIS PROGRAM IS MY FAVORITE
	char** hfC = NULL;
	HFNode t = NULL;
	char flag;
	HFNode m = NULL;
	////从终端读入字符集大小n,以及n个字符和n个权值
	//int n;
	//cout << "请输入字符集theCharSet的大小n: ";
	//cin >> n;
	//if (n <= 1) {
	//	cout << "初始化失败，因为n<=1.";
	//	return NULL;
	//}
	//int m1 = 2 * n - 1;
	//t = new HFTree[m1 + 1];
	//for (int i = 1; i <= m1; i++) {
	//	t[i].data = '#';
	//}
	//HFNode p1 = t + 1;
	//cout << "输入" << n << "个字符及其对应的权值" << endl;
	//for (int i=1; i <= n; i++, p1++) {
	//	cin >> p1->data >> p1->weight;
	//	Code_Size += p1->weight;
	//}

	cout << "请选择功能:'I'代表Initialization,'E'代表Encoding，'D'代表Decoding," << endl
		<< "            'P'代表Print，'T'代表Tree printing，'Q'代表退出运行." << endl;
	cin >> flag;
	while (flag != 'Q') {
		switch (flag) {

		case 'I':m = Initialization(t, hfC);cout << "初始化完毕，请输入下一个功能代号。" << endl; break;

		case 'E':Encoding(t, hfC); cout << "编译完毕，请输入下一个功能代号。" << endl; break;

		case 'D':Decoding(m); cout << "解译完毕，请输入下一个功能代号。" << endl; break;

		case 'P': Print(); cout << "打印完毕，请输入下一个功能代号。" << endl; break;

		case 'T':	
			 ofstream fout("TreePrint.txt");
			 TreePrint(m, fout);
			 cout << endl << "打印完毕，请输入下一个功能代号。" << endl; 
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