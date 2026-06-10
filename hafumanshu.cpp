#include<iostream>
#include<string.h>
#include <bitset>
#include<fstream>
using namespace std;

#define N 255


//哈夫曼树的顺序储存表示
typedef struct
{
    int weight;  //结点的权值
    int parent,lchild,rchild;  //结点的双亲，左孩子，右孩子的下标
}HTNode,*HuffmanTree;  //动态分配数组来储存哈夫曼树

//哈夫曼编码表的储存表示
typedef char **HuffmanCode;  //动态分配数组来储存哈夫曼编码表

//在哈夫曼树HT中选择两个双亲域为0且权值最小的两个结点，并返回它们在HT中的序号s1和s2
void Select(HuffmanTree HT,int len,int &s1,int &s2) //len代表HT数组的长度
{  
    int i,min1 = 0x3f3f3f3f,min2 = 0x3f3f3f3f;  //先赋予最大值，0x3f3f3f3f相当于10的9次方，可以做无穷大使用
    for(i = 1;i <= len;i++)
    {
        if(HT[i].weight < min1 && HT[i].parent == 0)
        {
            min1 = HT[i].weight;
            s1 = i;
        }
    }
    int temp = HT[s1].weight;  //将原值存放起来，然后先赋予最大值，防止s1被重复选择
    HT[s1].weight = 0x3f3f3f3f;
    for(i = 1;i <= len;i++)
    {
        if(HT[i].weight < min2 && HT[i].parent == 0)
        {
            min2 = HT[i].weight;
            s2 = i;
        }
    }
    HT[s1].weight = temp;  //恢复原来的值
}

//构造哈夫曼树HT（书本p147）
void HuffmanTreeCreate(HuffmanTree &HT,int n,int m) //当n大于1时，n个叶子结点需要m = 2*n-1个结点
{    
    int s1,s2;
    if(n <= 1) return;  //包括根节点的所有节点数
    HT = new HTNode[m + 1];  //0号单元未用，所以需要动态分配m+1个单元，HT[m]表示根结点
    for(int i = 1;i <= m;++i)   //将1~m号单元中的双亲、左孩子，右孩子的下标都初始化为0
    {  
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
    }
    //输入前n个单元中叶子结点的权值
    for(int i = 1;i <= n;++i)
    {
        cout << "请输入第" << i << "个叶子结点的权值：";
        cin >> HT[i].weight;
    }
    //通过n-1次的选择、删除、合并来创建哈夫曼树
    for(int i = n + 1;i <= m;++i)
    {
        //在HT[k](1 ≤ k ≤ i-1)中选择两个其双亲域为0且weight最小的结点，并返回它们在HT中的序号s1和s2
        Select(HT,i-1,s1,s2);
        //得到新结点i，从森林中删除s1，s2，将s1和s2的双亲域由0改为i
        HT[s1].parent = i;
        HT[s2].parent = i;
        //s1,s2分别作为i的左右孩子
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        //i的权值为左右孩子权值之和
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }
}

//进行哈夫曼编码
void HuffmanCoding(HuffmanTree HT,HuffmanCode &HC,int n)    //从叶子到根逆向求每个字符的哈夫曼编码，存储在编码表HC中
{   
    HC = new char*[n + 1];  //分配n个字符编码的编码表空间（头指针矢量）
    char *cd = new char[n];  //分配临时存放编码的动态数组空间
    cd[n - 1] = '\0';  //编码结束符
    //从叶子开始逐个字符求哈夫曼编码
    for(int i = 1;i <= n;++i)
    {
        int start = n - 1;  //start开始时指向最后，即编码结束符位置
        int c = i;
        int f = HT[i].parent;  //f是指向结点c的双亲结点
        //从叶子结点开始向上回溯，直到根结点
        for(int c = i,f = HT[i].parent;f != 0;c = f,f = HT[f].parent)
            if(HT[f].lchild == c) cd[--start] = '0';
            else cd[--start] = '1';
        HC[i] = new char[n - start];  //为第i个字符编码分配空间
        strcpy(HC[i],&cd[start]);  //将求得的编码从临时空间cd复制到HC的当前行中
    }
    free(cd);  //释放临时空间
}

//哈夫曼编码(原文转二进制)
void HuffmanPutF(HuffmanCode Code,char a[],char b[],int n)
{
    char data[N];
    FILE *pu = fopen("EnglishToCode.txt","w+");
	FILE *p = fopen("Text.txt","r");
	if((p = fopen("Text.txt","r"))==NULL){
		cout << "ERROR";
		exit(0);
	}
    int x = 0;
	char ch='1';
	while(ch != '*'){
		ch = fgetc(p);
		for(int j = 1;j <= n;j++){
			if(ch == a[j]){
				x = j;
				break;
			}
		}
		if(ch != '*')
        {
            fprintf(pu,"%s",Code[x]);
            cout << Code[x];
        }
	}
    cout << "——";
    for(int i = 0;i < strlen(b)-1;i++)
    {
        if(b[i] == '#')
        {
            cout << ' ';
            continue;
        }
        cout << b[i];
    }
	fclose(pu);
	fclose(p);
}

//哈夫曼译码(二进制转原文)
void HuffmanEncoding(HuffmanTree &HT,char a[],int n)
{
	FILE *pe = fopen("Binary.txt","r");
    ofstream outfile("BinaryToEn.txt",ios::out);
	int p = 2 * n - 1;
	char ch;
	while((ch = fgetc(pe)) != '*'){
		if(ch == '0') p = HT[p].lchild;
		else p = HT[p].rchild;
		if(HT[p].lchild == 0 && HT[p].rchild == 0)
        {
            if(a[p] == '#')
            {
                cout << ' ';
                outfile << ' ';
            }
            else
            {
                cout << a[p];
                outfile << a[p];
            }

			p = 2 * n - 1;
		}
	}
    outfile.close();
	fclose(pe);
}

//将哈夫曼树结构体数组存入文件
void SaveDate(HuffmanTree Tree,char array[],int m)
{
    int i;
    ofstream outfile("HuffmanTree.txt",ios::out);   //打开文件存入
    if(!outfile)
    {
        cerr << "open error!" << endl;
        exit (1);
    }
    outfile << "－－－－－－－－－－－－－－－－－－-" << endl;
	outfile << "|下标|字符|权重|父结点|左结点|右结点|" << endl;
	for(i = 1;i <= m;i++)
    {
	 outfile << "－－－－－－－－－－－－－－－－－－-" << endl;
	 outfile << "| " << i << " | " << array[i] << " |  " << Tree[i].weight << "  |  " << Tree[i].parent << "  |  " << Tree[i].lchild << "  |  " << Tree[i].rchild<< "  |\n" ;
	}
    outfile.close();    //使用后要记得关闭
}

//将哈夫曼编码结构体数组存入文件
void SaveCode(HuffmanCode Code,char array[],int n) 
{
    int i;
    ofstream outfile("HuffmanCode.txt",ios::out);   //打开文件存入
    if(!outfile)
    {
        cerr << "open error!" << endl;
        exit (1);
    }
	outfile << "字符编码结果如下：" << endl;
	outfile << "－－－－－－－－" << endl;
	outfile << "| 字符 | 编码 |" << endl;
	outfile << "－－－－－－－－" << endl;
	for(i = 1;i <= n;i++)
        outfile << "|  " << array[i] << "  |  " << Code[i] << " |\n" << endl;
    outfile.close();    //使用后要记得关闭
}

//将显示屏的输入保存到文件中
void SaveText(char a[])
{
    int i;
    ofstream outfile("Text.txt",ios::out);   //打开文件存入
    if(!outfile)
    {
        cerr << "open error!" << endl;
        exit (1);
    }
    outfile << a << endl;
    outfile.close();    //使用后要记得关闭
}

//将显示屏输入的二进制保存到临时文件中
void SaveBinary(char a[])
{
    int i;
    ofstream outfile("Binary.txt",ios::out);   //打开文件存入
    if(!outfile)
    {
        cerr << "open error!" << endl;
        exit (1);
    }
    outfile << a << endl;
    outfile.close();    //使用后要记得关闭
}

int main()
{
    int n,i,m;//n = 输入的字符个数,m = 所有结点
    HuffmanTree Tree = NULL;//哈夫曼树为空
    HuffmanCode Code = NULL;
    char array1[N];
    char array2[N];

    while(1)
    {
        system("date/t");
        system("time/t");
        cout << " =============================================================================" << endl;
        cout << "||                ★★★★★★★哈夫曼编码与译码★★★★★★★                ||" << endl;
        cout << "||============================================================================||" << endl;
        cout << "||============================================================================||" << endl;
        cout << "||                     【1】--- 创建哈夫曼树                                  ||" << endl;
        cout << "||                     【2】--- 进行哈夫曼编码                                ||" << endl;
        cout << "||                     【3】--- 进行哈夫曼编码（原文转二进制）                 ||" << endl;
        cout << "||                     【4】--- 进行哈夫曼译码（二进制转还原）                 ||" << endl;
        cout << "||                     【5】--- 退出程序                                      ||" << endl;
        cout << " ==============================================================================" << endl;
        cout << " =============       温馨提示：输入时的空格以“#”键表示         ==================" << endl;
        cout << "请输入数字来选择对应的功能：";
        int num;
        cin >> num;
        switch (num)
        {
            case 1:
                cout << endl;
                cout << "请输入，需要编译的字符个数：";
                cin >> n;
                m = 2 * n - 1;
                for(i = 1;i <= n ;i++)
                {
                    cout << "请输入第" << i << "个字符：";
                    cin >> array1[i];
                }
                HuffmanTreeCreate(Tree,n,m);//创建哈夫曼树
                cout << "哈夫曼树创建完成！！！" << endl;
                cout << "－－－－－－－－－－－－－－－－－－-" << endl;
			    cout << "|下标|字符|权重|父结点|左结点|右结点|" << endl;
			    for(i = 1;i <= m;i++)
                {
                    cout << "－－－－－－－－－－－－－－－－－－-" << endl;
                    printf("|%3d |%3c |%3d |%4d  |%4d  |%4d  |\n",i,array1[i],Tree[i].weight,Tree[i].parent,Tree[i].lchild,Tree[i].rchild);
			    }
                SaveDate(Tree,array1,m);//保存入文件
			    cout << "\n打印结束,数据已默认存入本地磁盘 HuffmanTree.txt 文件！" << endl;
			    system("pause");
			    break;
            case 2:
                cout << endl;
                HuffmanCoding(Tree,Code,n);//对各个字符进行编码
				cout << "字符编码成功!结果如下：" << endl;
				cout << "\t－－－－－－－－" << endl;
				cout << "\t| 字符 | 编码 |" << endl;
				cout << "\t－－－－－－－－" << endl;
				for(i = 1;i <= n;i++)
                    cout << "\t|  " << array1[i] << "  |  " << Code[i] << "  |\n" << endl;
                SaveCode(Code,array1,n);//保存入文件
			    cout << "\n打印结束,数据已默认存入本地磁盘 HuffmanCode.txt 文件！" << endl;
				system("pause");
				break;
            case 3:
                cout << "请输入想要翻译的一串字符串(以'*'结尾)：";
                cin >> array2;
                SaveText(array2);
                HuffmanPutF(Code,array1,array2,n);
                cout << "\n打印结束,数据已默认存入本地磁盘 EnglishToCode.txt 文件！" << endl;
                system("pause");
                break;
            case 4:
                cout << "请输入想要翻译的一串二进制(以'*'结尾)：";
                cin >> array2;
                SaveBinary(array2);
                cout << "译码结果：";
                HuffmanEncoding(Tree,array1,n);
                cout << "\n打印结束,数据已默认存入本地磁盘 BinaryToEn.txt 文件！" << endl;
                system("pause");
                break;
            case 5:
                cout << "\n\n";
                cout << "*************************************************" << endl;
                cout << "******                                     ******" << endl;
                cout << "******     谢谢使用哈夫曼编码与译码程序    ******" << endl;
                cout << "******                                     ******" << endl;
                cout << "*************************************************" << endl;
                exit(0);
                default:
                break;
        }
    }
    return 0;
}