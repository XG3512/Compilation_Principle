# 准备工作

## 系统环境

基础环境：macOS Monterey 12.3.1  Intel Core i7 32G DDR4

所需依赖及版本：

clang -v

```shell
Apple clang version 13.1.6 (clang-1316.0.21.2.3)
Target: x86_64-apple-darwin21.4.0
Thread model: posix
```

LLVM -v

```shell
llvm@12
```

cmake -v

```shell
cmake version 3.23.0

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

flex -v

```shell
flex 2.6.4 Apple(flex-34)
```

Homebrew -v

```shell
Homebrew 3.4.11
Homebrew/homebrew-core (git revision ef44e758a2e; last commit 2022-03-30)
Homebrew/homebrew-cask (git revision e291f3896c; last commit 2022-03-30)
```

## 依赖安装

依赖安装均基于Homebrew

安装llvm以及cmake

```shell
brew install llvm@12 cmake
```

本课程设计所有代码运行以及Demo测试均在此环境完成

# 理论支持

## 词法分析器的作用

词法分析是编译的第一阶段。词法分析器的主要任务是读入源程序的输入字符、将它们组成词素，生成并输出一个词法单元序列，每个词法单元对应于一个词素。这个词法单元序列被输出到语法分析器进行语法分析

词法分析器通常还要和符号表进行交互。当词法分析器发现了一个标识符的词素时，它要将这个词素添加到符号表中，在某些情况下，词法分析器会从符号表中读取有关标识符的种类的信息，以确定向语法分析器传送哪个词法单元。

![image-20220602195052123](https://blog-xg.oss-cn-qingdao.aliyuncs.com/blog_imgimage-20220602195052123.png)

词法分析器在编译器中负责读取源程序，因此它还会完成一些识别词素之外的具体其他任务。
（1）过滤掉源程序中的注释和空白（空格、换行符和制表符以及在输入中用于分隔词法单元的其他字符）
（2）将编译器生成的错误消息和源程序的位置联系起来。
**有时，词法分析器可以分成两个级联的处理阶段：**
**1）扫描阶段主要负责完成一些不需要生词词法单元的简单处理，比如删除注释和将多个连续的空白字符压缩成一个字符**
**2）词法分析阶段是较为复杂的部分，它处理扫描阶段的输出并生成词法单元**

## 基于Flex思想的词法分析器设计分析

前面提到 `词法分析器` 要将源程序分解成 `单词`，我们的语法格式很简单，只包括：标识符，数字，数学运算符，括号和大括号等，我们将通过 Flex 来生成 `词法分析器` 的源码，给定Flex特定的规则，即可生成给予Flex词法分析器的源码。根据其生成的源码，结合其思想，我们可以抽相出简单的词法分析器框架，即预处理、界符处理、运算符处理、字母或数字开头的字符串的处理。



## 词法分析器的工作流程

为方便后续设计，该工作流程仅作为功能实现的思路参考，具体实现请参考功能实现部分

### 1. 预处理

如图1所示；词法分析器工作的第一步是输入源程序文本。在许多情况下，为了更好地对单词符号识别，把输入串预处理一下。预处理主要滤掉空格，跳过注释、换行符等。

![image-20220604164826848](https://blog-xg.oss-cn-qingdao.aliyuncs.com/blog_imgimage-20220604164826848.png)

#### **状态转换图**

根据预处理步骤，设计出状态转换图并进行以下定义：

   0 ：初态
   1：读入空格
   2：读入 '/'
   3：读入其他字符
   4：读入 "//" 单行注释 
   5：读入 "/*" 多行开始注释

![image-20220604141928022](https://blog-xg.oss-cn-qingdao.aliyuncs.com/blog_imgimage-20220604141928022.png)

#### **预处理函数**

根据状态转换图 设计出预处理函数

```c++
void Preprocess(){
	char cur; int index;	//cur:读取文件字符；index：当前缓冲区存放位置 
	int length = 0;			// length：读取到字符长度 
	if(BufferFlag) index = capacity/2; else index = 0; // 预处理的内容存放在左半区还是右半区 
	while(!feof(fp)){
		cur = fgetc(fp);
		if(cur == 9 || cur == 10 || cur == 13) continue;	// 跳过制表符、换行、回车 
		// 状态转换 
		switch(PreprocessFlag){
			case 0:{
				if(cur == ' ') break;
				if(cur == '/') PreprocessFlag = 2;
				else PreprocessFlag = 3;
				break;
			}
			case 1:{
				if(cur == ' ') PreprocessFlag = 0;
				else if(cur == '/') PreprocessFlag = 2;
				else PreprocessFlag = 3;
				break;
			}
			case 2:{
				if(cur == ' ') PreprocessFlag = 1;
				else if(cur == '/') PreprocessFlag = 4;
				else if(cur == '*') PreprocessFlag = 5;
				else PreprocessFlag = 3;
				break;
			}
			case 3:{
				if(cur == ' ') PreprocessFlag = 1;
				else if(cur == '/') PreprocessFlag = 2;
				break;
			}	
		}
		// 状态转换后的处理 
		if(PreprocessFlag == 3 || PreprocessFlag == 1 || PreprocessFlag == 2) {ScanBuffer[index++] = cur; length++;}
		else if(PreprocessFlag == 4) {				//单行注释 
			char buff[1024]; fgets(buff, 1024, fp);
			index--; length--; PreprocessFlag = 0;
		}else if(PreprocessFlag == 5) {				//多行注释 
			char pre;
			while(!feof(fp)) {
				pre = fgetc(fp);
				if(pre == '*' && (cur = fgetc(fp)) == '/') {
					index--; length--; PreprocessFlag = 0; break;
				}
			}
		}
		if(length == capacity/2) break;			// 每次处理60字节 
	}
}
```

### 2.界符处理

该部分主要是完成对界符的处理和判断。

![image-20220604164143987](https://blog-xg.oss-cn-qingdao.aliyuncs.com/blog_imgimage-20220604164143987.png)

#### **界符处理函数**

```c++
is_range(int &i)
{
	switch (line[i]){
	case '#':return 1;
	case '<':if (line[i - 7] == '#') return 2;
			 else return -1; break;
	case '>':
		for (int j = i; line[j] != '\n'; ++j)
		if (is_digit(line[j]) || is_alpbat(line[j])) return -1; return 3; break;
	case '(':return 4;
	case ')':return 5;
	case '[':return 6;
	case ']':return 7;
	case '{':return 8;
	case '}':return 9;
	case ',':return 10;
	case ';':return 11;
	case '_':return 12;
	default:;
	}
	return -1; //不属于界符
}
```

### 3.运算符的处理

该部分主要是完成对运算符的处理和判断。除调用函数换为is_operator()之外，处理流程与界符处理函数一致。

#### **运算符处理函数**

```c++
is_oprator(int &i)
{
	switch (line[i]){
	case '+':if (line[i + 1] == '+'){ ++i; return 18; }
			 else { return 13; }
	case '-':if (line[i + 1] == '-'){ ++i; return 19; }
			 else { return 14; }
	case '*':return 15;
	case '/':if (line[i + 1] == '/'){ return -3; }  //预处理注释
			 else return 16;
	case '%':return 17;
	case '&':if (line[i + 1] == '&'){ ++i; return 20; }
			 else { return -2; };
	case '|':if (line[i + 1] == '|'){ ++i; return 21; }
			 else { return -2; };
	case '!':if (line[i + 1] == '='){ ++i; return 29; }
			 else { return 22; }
	case '>':if (line[i + 1] == '>'){ ++i; return 30; }
			 else if (line[i + 1] == '='){ ++i; return 26; }
			 else return 23;
	case '<':if (line[i + 1] == '<'){ ++i; return 31; }
			 else if (line[i + 1] == '='){ ++i; return 27; }
			 else return 24;
	case'=':if (line[i + 1] == '='){ ++i; return 28; }
			else  return 25;
	default:;
	}
	return -2; //不属于操作符
}
```

### 4.字母开头的字符串处理

![image-20220604171725776](https://blog-xg.oss-cn-qingdao.aliyuncs.com/blog_imgimage-20220604171725776.png)

#### 字母开头的字符串处理函数

```c++
//判断是否字母
is_alpbat(char c){ //判断是否字母
	if ((c >= 'a'&&c <= 'z') || (c >= 'A'&&c <= 'Z'))return true;
	else return false;
}
cope_firsrtaph(int &i){   //处理首字符是字母
	int num = 0;
	recog_reserve(i, num);
	if (num){
		resout << '<' << num << ',' << key[num - 32] << ',' << "保留字" << '>' << "  ";
		preout << key[num - 32];
	}
	else{
		int j = 0;
		resout << '<';
		for (; is_digit(line[i]) || is_alpbat(line[i]); i++, j++){
			preout << line[i];
			resout << line[i];
		}
		resout << ',' << "标识符" << '>' << "  ";
	}
}
```

### 5.数字字符串的处理

该部分的处理方式与字母的处理相类似，该字符串全是数字就识别为数字，否则识别为其他。

#### 数字开头的字符串处理函数

```c++
cope_firsrtnum(int &i){   //处理首字符是数字
	int j = 0;
	resout << '<';
	for (; is_digit(line[i]); i++, j++){
		preout << line[i];
		resout << line[i];
	}
	resout << ',' << "数字" << '>' << '  ';
}
```

# 功能实现

由理论分析过可知，实现词法分析，受限需要进行预处理、界符处理、运算符处理、字母以及数字开头的字符串处理，最终输出二元组所构成的序列

源程序将在附录中给出

实际运行效果

```shell
Please input string:
begin a:=9; if b>0 then a:=19*x+3/9; end#
(1,begin)
(10,a)
(18,:=)
(11,9)
(26,;)
(2,if)
(10,b)
(20,>)
(11,0)
(3,then)
(10,a)
(18,:=)
(11,19)
(13,*)
(10,x)
(15,+)
(11,3)
(14,/)
(11,9)
(26,;)
(6,end)
(0,#)
```

由此本次课程设计完成了所选题目的所有工作

# 附录

```c++
#include<string.h>
#include <iostream>

using namespace std;
char Project[80];
char Token[8];
char KeyChar;
int sum=0;
int row;
int n;
int p;
int m = 0;
int syn;
//定义关键字
char *ListOfCodeKeywords[6]={"begin", "if", "then", "while", "do", "end"};

//词法分析程序主要函数，其主要分为三个部分：标识符、数字、符号
void SourceScan()
{
    for(n=0;n<8;n++) Token[n]=NULL;
    KeyChar=Project[p++];
    while(KeyChar == ' ')
    {
        KeyChar=Project[p];
        p++;
    }
    //处理可能是标示符或者变量名
    if((KeyChar >= 'a' && KeyChar <= 'z') || (KeyChar >= 'A' && KeyChar <= 'Z')) 
    {
        m=0;
        while((KeyChar >= '0' && KeyChar <= '9') || (KeyChar >= 'a' && KeyChar <= 'z') || (KeyChar >= 'A' && KeyChar <= 'Z'))
        {
            Token[m++]=KeyChar;
            KeyChar=Project[p++];
        }
        Token[m++]='\0';
        p--;
        syn=10;
        for(n=0;n<6;n++)  //将识别出来的字符和已定义的标示符作比较，
            if(strcmp(Token, ListOfCodeKeywords[n]) == 0)
            {
                syn=n+1;
                break;
            }
    }
    else if((KeyChar >= '0' && KeyChar <= '9'))  //数字
    {
        {
            sum=0;
            while((KeyChar >= '0' && KeyChar <= '9'))
            {
                sum=sum*10 + KeyChar - '0';
                KeyChar=Project[p++];
            }
        }
        p--;
        syn=11;
        if(sum>32767)
            syn=-1;
    }
    else switch(KeyChar)   //其他字符
        {
            case'<':m=0;Token[m++]=KeyChar;
                KeyChar=Project[p++];
                if(KeyChar == '>')
                {
                    syn=21;
                    Token[m++]=KeyChar;
                }
                else if(KeyChar == '=')
                {
                    syn=22;
                    Token[m++]=KeyChar;
                }
                else
                {
                    syn=23;
                    p--;
                }
                break;
            case'>':m=0;Token[m++]=KeyChar;
                KeyChar=Project[p++];
                if(KeyChar == '=')
                {
                    syn=24;
                    Token[m++]=KeyChar;
                }
                else
                {
                    syn=20;
                    p--;
                }
                break;
            case':':m=0;Token[m++]=KeyChar;
                KeyChar=Project[p++];
                if(KeyChar == '=')
                {
                    syn=18;
                    Token[m++]=KeyChar;
                }
                else
                {
                    syn=17;
                    p--;
                }
                break;
            case'*':syn=13;Token[0]=KeyChar;break;
            case'/':syn=14;Token[0]=KeyChar;break;
            case'+':syn=15;Token[0]=KeyChar;break;
            case'-':syn=16;Token[0]=KeyChar;break;
            case'=':syn=25;Token[0]=KeyChar;break;
            case';':syn=26;Token[0]=KeyChar;break;
            case'(':syn=27;Token[0]=KeyChar;break;
            case')':syn=28;Token[0]=KeyChar;break;
            case'#':syn=0;Token[0]=KeyChar;break;
            case'\n':syn=-2;break;
            default: syn=-1;break;
        }
}

int main()
{
    p=0;
    row=1;
    cout<<"Please input string:"<<endl;
    do
    {
        cin.get(KeyChar);
        Project[p++]=KeyChar;
    }
    while(KeyChar != '#');
    p=0;
    do
    {
        SourceScan();
        switch(syn)
        {
            case 11: cout<<"("<<syn<<","<<sum<<")"<<endl; break;
            case -1: cout<<"Error in row "<<row<<"!"<<endl; break;
            case -2: row=row++;break;
            default: cout << "(" << syn << "," << Token << ")" << endl;break;
        }
    }
    while (syn!=0);
}
```

```shell


LLVM 本质上是一个使用编程方式创建机器原生代码的软件库。开发人员调用其 API，生成一种使用“中间表示”（IR，Intermediate Representation）格式的指令。进而，LLVM 将 IR 编译为独立软件库，或者使用另一种语言的上下文（例如，使用该语言的编译器）对代码执行 JIT（即时，just-in-time）编译。

LLVM API 提供了一些原语，用于表示开发编程语言中常见结构和模式。例如，几乎所有的语言都具有函数和全局变量的概念。LLVM 也将函数和全局变量作为 IR 的标准元素。这样，开发人员可以直接使用 LLVM 的实现，并聚焦于自身语言中的独到之处，不再需要花费时间和精力去重造这些特定的轮子。


```

