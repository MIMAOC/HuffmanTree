// 哈夫曼编码 —— 跨平台版本（Windows / macOS / Linux 通用）
// 文件以 UTF-8 编码保存。
// 编译： g++ -std=c++11 -o huffman huffman_crossplatform.cpp
// 运行： ./huffman   （Windows: huffman.exe）

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <limits>
using namespace std;

#define N 255

// 哈夫曼树结点（顺序存储）
typedef struct
{
    int weight;                  // 结点权值
    int parent, lchild, rchild;  // 双亲、左孩子、右孩子的下标
} HTNode, *HuffmanTree;

// 哈夫曼编码表（动态二维数组）
typedef char **HuffmanCode;

// 跨平台的“暂停”，等待用户回车
void pause()
{
    cout << "\n按回车键继续...";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// 在 HT 中选出双亲为 0、权值最小的两个结点，下标存入 s1、s2
void Select(HuffmanTree HT, int len, int &s1, int &s2)
{
    int i, min1 = 0x3f3f3f3f, min2 = 0x3f3f3f3f;
    s1 = s2 = 0;
    for (i = 1; i <= len; i++)
        if (HT[i].parent == 0 && HT[i].weight < min1)
        {
            min1 = HT[i].weight;
            s1 = i;
        }
    int temp = HT[s1].weight;          // 暂存原值，临时屏蔽 s1，避免重复选中
    HT[s1].weight = 0x3f3f3f3f;
    for (i = 1; i <= len; i++)
        if (HT[i].parent == 0 && HT[i].weight < min2)
        {
            min2 = HT[i].weight;
            s2 = i;
        }
    HT[s1].weight = temp;              // 恢复原值
}

// 构建哈夫曼树。n 个叶子结点需要 m = 2*n-1 个结点
void HuffmanTreeCreate(HuffmanTree &HT, int n, int m)
{
    int s1, s2;
    if (n <= 1) return;
    HT = new HTNode[m + 1];            // 0 号单元不用
    for (int i = 1; i <= m; ++i)
    {
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
    }
    for (int i = 1; i <= n; ++i)       // 输入前 n 个叶子结点的权值
    {
        cout << "请输入第" << i << "个叶子结点的权值：";
        cin >> HT[i].weight;
    }
    for (int i = n + 1; i <= m; ++i)   // n-1 次合并，生成新结点
    {
        Select(HT, i - 1, s1, s2);
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }
}

// 由哈夫曼树生成每个字符的编码，存入 HC
void HuffmanCoding(HuffmanTree HT, HuffmanCode &HC, int n)
{
    HC = new char *[n + 1];
    char *cd = new char[n];
    cd[n - 1] = '\0';
    for (int i = 1; i <= n; ++i)
    {
        int start = n - 1;
        for (int c = i, f = HT[i].parent; f != 0; c = f, f = HT[f].parent)
            if (HT[f].lchild == c) cd[--start] = '0';
            else                   cd[--start] = '1';
        HC[i] = new char[n - start];
        strcpy(HC[i], &cd[start]);
    }
    delete[] cd;                       // 与 new[] 配对，避免 new/free 混用
}

// 编码：原文 -> 二进制编码，结果写入 EnglishToCode.txt
void HuffmanPutF(HuffmanCode Code, char a[], char b[], int n)
{
    ofstream pu("EnglishToCode.txt");
    ifstream p("Text.txt");
    if (!p)
    {
        cout << "ERROR: 无法打开 Text.txt" << endl;
        return;
    }
    char ch = '1';
    while (p.get(ch) && ch != '*')
    {
        int x = 0;
        for (int j = 1; j <= n; j++)
            if (ch == a[j]) { x = j; break; }
        if (x != 0)
        {
            pu << Code[x];
            cout << Code[x];
        }
    }
    cout << "\n对应原文：";
    for (size_t i = 0; i + 1 < strlen(b); i++)
        cout << (b[i] == '#' ? ' ' : b[i]);
    cout << endl;
}

// 译码：二进制编码 -> 原文，结果写入 BinaryToEn.txt
void HuffmanEncoding(HuffmanTree &HT, char a[], int n)
{
    ifstream pe("Binary.txt");
    ofstream outfile("BinaryToEn.txt");
    if (!pe)
    {
        cout << "ERROR: 无法打开 Binary.txt" << endl;
        return;
    }
    int root = 2 * n - 1;
    int p = root;
    char ch;
    while (pe.get(ch) && ch != '*')
    {
        if (ch == '0') p = HT[p].lchild;
        else if (ch == '1') p = HT[p].rchild;
        else continue;
        if (HT[p].lchild == 0 && HT[p].rchild == 0)
        {
            char out = (a[p] == '#') ? ' ' : a[p];
            cout << out;
            outfile << out;
            p = root;
        }
    }
    cout << endl;
}

// 保存哈夫曼树结构到文件
void SaveDate(HuffmanTree Tree, char array[], int m)
{
    ofstream outfile("HuffmanTree.txt");
    if (!outfile) { cerr << "open error!" << endl; return; }
    outfile << "+----+----+----+------+------+------+" << endl;
    outfile << "|下标|字符|权重|双亲 |左孩子|右孩子|" << endl;
    for (int i = 1; i <= m; i++)
    {
        outfile << "+----+----+----+------+------+------+" << endl;
        outfile << "| " << i << " | " << array[i] << " |  " << Tree[i].weight
                << "  |  " << Tree[i].parent << "  |  " << Tree[i].lchild
                << "  |  " << Tree[i].rchild << "  |\n";
    }
}

// 保存哈夫曼编码表到文件
void SaveCode(HuffmanCode Code, char array[], int n)
{
    ofstream outfile("HuffmanCode.txt");
    if (!outfile) { cerr << "open error!" << endl; return; }
    outfile << "字符编码如下：" << endl;
    outfile << "+------+------+" << endl;
    outfile << "| 字符 | 编码 |" << endl;
    outfile << "+------+------+" << endl;
    for (int i = 1; i <= n; i++)
        outfile << "|  " << array[i] << "  |  " << Code[i] << " |\n";
}

// 保存示例文本
void SaveText(char a[])
{
    ofstream outfile("Text.txt");
    if (!outfile) { cerr << "open error!" << endl; return; }
    outfile << a << endl;
}

// 保存示例二进制串
void SaveBinary(char a[])
{
    ofstream outfile("Binary.txt");
    if (!outfile) { cerr << "open error!" << endl; return; }
    outfile << a << endl;
}

int main()
{
    int n = 0, i, m = 0;               // n = 字符个数, m = 总结点数
    HuffmanTree Tree = NULL;
    HuffmanCode Code = NULL;
    char array1[N];

    while (1)
    {
        cout << " =============================================================" << endl;
        cout << "||                  哈夫曼编码 / 译码 系统                   ||" << endl;
        cout << "||===========================================================||" << endl;
        cout << "||        【1】--- 建立哈夫曼树                              ||" << endl;
        cout << "||        【2】--- 进行哈夫曼编码                            ||" << endl;
        cout << "||        【3】--- 编码（原文 -> 二进制）                    ||" << endl;
        cout << "||        【4】--- 译码（二进制 -> 原文）                    ||" << endl;
        cout << "||        【5】--- 退出程序                                  ||" << endl;
        cout << " =============================================================" << endl;
        cout << " 温馨提示：输入时空格用 # 代替，字符串以 * 结尾" << endl;
        cout << "请输入要选择的功能：";

        int num;
        if (!(cin >> num)) break;      // 输入流结束/出错则退出
        switch (num)
        {
            case 1:
                cout << "\n请输入要建立的字符个数：";
                cin >> n;
                m = 2 * n - 1;
                for (i = 1; i <= n; i++)
                {
                    cout << "请输入第" << i << "个字符：";
                    cin >> array1[i];
                }
                HuffmanTreeCreate(Tree, n, m);
                cout << "哈夫曼树建立完成！如下：" << endl;
                cout << "+----+----+----+------+------+------+" << endl;
                cout << "|下标|字符|权重|双亲 |左孩子|右孩子|" << endl;
                for (i = 1; i <= m; i++)
                {
                    cout << "+----+----+----+------+------+------+" << endl;
                    printf("|%3d |%3c |%3d |%4d  |%4d  |%4d  |\n",
                           i, array1[i], Tree[i].weight,
                           Tree[i].parent, Tree[i].lchild, Tree[i].rchild);
                }
                SaveDate(Tree, array1, m);
                cout << "\n已保存到本地的 HuffmanTree.txt 文件中。" << endl;
                pause();
                break;
            case 2:
                if (Tree == NULL) { cout << "请先执行【1】建立哈夫曼树！" << endl; pause(); break; }
                HuffmanCoding(Tree, Code, n);
                cout << "字符编码成功！如下：" << endl;
                cout << "\t+------+------+" << endl;
                cout << "\t| 字符 | 编码 |" << endl;
                cout << "\t+------+------+" << endl;
                for (i = 1; i <= n; i++)
                    cout << "\t|  " << array1[i] << "  |  " << Code[i] << "  |" << endl;
                SaveCode(Code, array1, n);
                cout << "\n已保存到本地的 HuffmanCode.txt 文件中。" << endl;
                pause();
                break;
            case 3:
            {
                if (Code == NULL) { cout << "请先执行【2】生成哈夫曼编码！" << endl; pause(); break; }
                cout << "请输入您要编码的一段字符串（以 '*' 结尾）：";
                char array2[N];
                cin >> array2;
                SaveText(array2);
                HuffmanPutF(Code, array1, array2, n);
                cout << "\n已保存到本地的 EnglishToCode.txt 文件中。" << endl;
                pause();
                break;
            }
            case 4:
            {
                if (Tree == NULL) { cout << "请先执行【1】建立哈夫曼树！" << endl; pause(); break; }
                cout << "请输入您要译码的一段二进制（以 '*' 结尾）：";
                char array2[N];
                cin >> array2;
                SaveBinary(array2);
                cout << "译码结果：";
                HuffmanEncoding(Tree, array1, n);
                cout << "\n已保存到本地的 BinaryToEn.txt 文件中。" << endl;
                pause();
                break;
            }
            case 5:
                cout << "\n\n*************************************************" << endl;
                cout << "******     感谢使用哈夫曼编码译码系统      ******" << endl;
                cout << "*************************************************" << endl;
                return 0;
            default:
                cout << "无效的选项，请重新输入。" << endl;
                break;
        }
    }
    return 0;
}
