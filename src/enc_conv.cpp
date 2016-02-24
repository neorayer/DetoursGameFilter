
int WideCharToMultiByte(
	UINT CodePage, // code page
	DWORD dwFlags, // performance and mapping flags
	LPCWSTR lpWideCharStr, // wide-character string
	int cchWideChar, // number of chars in string
	LPSTR lpMultiByteStr, // buffer for new string
	int cbMultiByte, // size of buffer
	LPCSTR lpDefaultChar, // default for unmappable chars
	LPBOOL lpUsedDefaultChar // set when default char used
); //将宽字符转换成多个窄字符

int MultiByteToWideChar(
	UINT CodePage, // code page
	DWORD dwFlags, // character-type options
	LPCSTR lpMultiByteStr, // string to map
	int cbMultiByte, // number of bytes in string
	LPWSTR lpWideCharStr, // wide-character buffer
	int cchWideChar // size of buffer
);//将多个窄字符转换成宽字符      

CString CXmlProcess::HexToBin(CString string)//将16进制数转换成2进制
{
	if( string == "0") return "0000";
	if( string == "1") return "0001";
	if( string == "2") return "0010";
	if( string == "3") return "0011";
	if( string == "4") return "0100";
	if( string == "5") return "0101";
	if( string == "6") return "0110";
	if( string == "7") return "0111";
	if( string == "8") return "1000";
	if( string == "9") return "1001";
	if( string == "a") return "1010";
	if( string == "b") return "1011";
	if( string == "c") return "1100";
	if( string == "d") return "1101";
	if( string == "e") return "1110";
	if( string == "f") return "1111";
	return "";
}


CString CXmlProcess::BinToHex(CString BinString)//将2进制数转换成16进制
{
	if( BinString == "0000") return "0";
	if( BinString == "0001") return "1";
	if( BinString == "0010") return "2";
	if( BinString == "0011") return "3";
	if( BinString == "0100") return "4";
	if( BinString == "0101") return "5";
	if( BinString == "0110") return "6";
	if( BinString == "0111") return "7";
	if( BinString == "1000") return "8";
	if( BinString == "1001") return "9";
	if( BinString == "1010") return "a";
	if( BinString == "1011") return "b";
	if( BinString == "1100") return "c";
	if( BinString == "1101") return "d";
	if( BinString == "1110") return "e";
	if( BinString == "1111") return "f";
	return "";
}

int CXmlProcess::BinToInt(CString string)//2进制字符数据转换成10进制整型
{
	int len =0;
	int tempInt = 0;
	int strInt = 0;
	for(int i =0 ;i < string.GetLength() ;i ++)
	{
	        tempInt = 1;
	        strInt = (int)string.GetAt(i)-48;
	        for(int k =0 ;k < 7-i ; k++)
	        {
			tempInt = 2*tempInt;
	        }
	        len += tempInt*strInt;
	}
	return len;
}      

　　UTF-8转换成GB2312先把UTF-8转换成Unicode.然后再把Unicode通过函数WideCharToMultiByte转换成GB2312

WCHAR* CXmlProcess::UTF_8ToUnicode(char *ustart)  //把UTF-8转换成Unicode
{
	char char_one;
	char char_two;
	char char_three;
	int Hchar;
	int Lchar;
	char uchar[2];
	WCHAR *unicode;
	CString string_one;
	CString string_two;
	CString string_three;
	CString combiString;
	char_one = *ustart;
	char_two = *(ustart+1);
	char_three = *(ustart+2);
	string_one.Format("%x",char_one);
	string_two.Format("%x",char_two);
	string_three.Format("%x",char_three);
	string_three = string_three.Right(2);
	string_two = string_two.Right(2);
	string_one = string_one.Right(2);
	string_three = HexToBin(string_three.Left(1))+HexToBin(string_three.Right(1));
	string_two = HexToBin(string_two.Left(1))+HexToBin(string_two.Right(1));
	string_one = HexToBin(string_one.Left(1))+HexToBin(string_one.Right(1));
	combiString = string_one +string_two +string_three;
	combiString = combiString.Right(20);
	combiString.Delete(4,2);
	combiString.Delete(10,2);
	Hchar = BinToInt(combiString.Left(8));
	Lchar = BinToInt(combiString.Right(8));
	uchar[1] = (char)Hchar;
	uchar[0] = (char)Lchar;
	unicode = (WCHAR *)uchar;
	return unicode;
}

char * CXmlProcess::UnicodeToGB2312(unsigned short uData)  //把Unicode 转换成 GB2312
{
	char *buffer ;
	buffer = new char[sizeof(WCHAR)];
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,buffer,sizeof(WCHAR),NULL,NULL);
	return buffer;
}      

　　GB2312转换成UTF-8：先把GB2312通过函数MultiByteToWideChar转换成Unicode.然后再把Unicode通过拆开Unicode后拼装成UTF-8。

WCHAR * CXmlProcess::Gb2312ToUnicode(char *gbBuffer)  //GB2312 转换成　Unicode
{ 
	WCHAR *uniChar;
	uniChar = new WCHAR[1];
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,uniChar,1);
	return uniChar;
}
char * CXmlProcess::UnicodeToUTF_8(WCHAR *UniChar) // Unicode 转换成UTF-8
{
	char *buffer;
	CString strOne;
	CString strTwo;
	CString strThree;
	CString strFour;
	CString strAnd;
	buffer = new char[3];
	int hInt,lInt;
	hInt = (int)((*UniChar)/256);
	lInt = (*UniChar)%256;
	CString string ;
	string.Format("%x",hInt);
	strTwo = HexToBin(string.Right(1));
	string = string.Left(string.GetLength() - 1);
	strOne = HexToBin(string.Right(1));
	string.Format("%x",lInt);
	strFour = HexToBin(string.Right(1));
	string = string.Left(string.GetLength() -1);
	strThree = HexToBin(string.Right(1));
	strAnd = strOne +strTwo + strThree + strFour;
	strAnd.Insert(0,"1110");
	strAnd.Insert(8,"10");
	strAnd.Insert(16,"10");
	strOne = strAnd.Left(8);
	strAnd = strAnd.Right(16);
	strTwo = strAnd.Left(8);
	strThree = strAnd.Right(8);
	*buffer = (char)BinToInt(strOne);
	buffer[1] = (char)BinToInt(strTwo);
	buffer[2] = (char)BinToInt(strThree);
	return buffer;
}     

例子：将GB2312转换成UTF-8的调用：

char * CXmlProcess::translateCharToUTF_8(char *xmlStream, int len) 
{
	int newCharLen =0 ;
	int oldCharLen = 0;
	int revCharLen = len;
	char* newCharBuffer;
	char* finalCharBuffer;
	char *buffer ;
	CString string;
	buffer  = new char[sizeof(WCHAR)];
	newCharBuffer = new char[int(1.5*revCharLen)];//设置最大的一个缓冲区
	while(oldCharLen < revCharLen)
	{
		if( *(xmlStream + oldCharLen) >= 0)
		{
			*(newCharBuffer+newCharLen) = *(xmlStream +oldCharLen);
			newCharLen ++;
			oldCharLen ++;
		}//如果是英文直接复制就可以
		else
		{
			WCHAR *pbuffer = this->Gb2312ToUnicode(xmlStream+oldCharLen);
			buffer = this->UnicodeToUTF_8(pbuffer);
			*(newCharBuffer+newCharLen) = *buffer;
			*(newCharBuffer +newCharLen +1) = *(buffer + 1);
			*(newCharBuffer +newCharLen +2) = *(buffer + 2);
			newCharLen += 3;
			oldCharLen += 2;
		}
	}
	newCharBuffer[newCharLen] = ''\0'';
	CString string1 ;
	string1.Format("%s",newCharBuffer);
	finalCharBuffer = new char[newCharLen+1];
	memcpy(finalCharBuffer,newCharBuffer,newCharLen+1);
	return finalCharBuffer;
}
