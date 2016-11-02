//============================================================================
// Name        : fatmat.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : fatmat in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

int Start_Sector = 2048;
int Bytes_Sector = 512;
int Hello_Sector = 5000;	// 打印hello world的elf数据数据函数
int Hello2_Sector = 5500;
int Hello3_Sector = 6000;
int Hello4_Sector = 6500;	// 打印hello world的elf数据数据函数
int Hello5_Sector = 7000;
int Hello6_Sector = 7500;
int Hello7_Sector = 8000;	// 打印hello world的elf数据数据函数
int Hello8_Sector = 8500;
int Hello9_Sector = 9000;

int main(void) {
	const char * imgFileName = "fishos.img";
	const char * fat32Bin = "fat32.bin";
	const char * bootFile = "boot.bin";
	const char * helloBin = "elf_file/hello.bin";	//hello world		--> 5000
	const char * hello2Bin = "elf_file/hello2.bin";	//hello , os kernel	--> 5500 
	const char * hello3Bin = "elf_file/hello3.bin";	//hello ,kernel		--> 6000
	const char * hello4Bin = "elf_file/hello4.bin";	//hello world		--> 6500
	const char * hello5Bin = "elf_file/hello5.bin";	//hello , os kernel	--> 7000 
	const char * hello6Bin = "elf_file/hello6.bin";	//hello ,kernel		--> 7500
	const char * hello7Bin = "elf_file/hello7.bin";	//hello world		--> 8000
	const char * hello8Bin = "elf_file/hello8.bin";	//hello , os kernel	--> 8500 
	const char * hello9Bin = "elf_file/hello9.bin";	//hello ,kernel		--> 9000
	char tmp[5*Bytes_Sector];
	char buffer[Bytes_Sector];
	char hello_tmp[40*Bytes_Sector];
	char hello2_tmp[15 * Bytes_Sector];
	char hello3_tmp[15 * Bytes_Sector];
	char hello4_tmp[15 * Bytes_Sector];
	char hello5_tmp[15 * Bytes_Sector];
	char hello6_tmp[15 * Bytes_Sector];
	char hello7_tmp[15 * Bytes_Sector];
	char hello8_tmp[15 * Bytes_Sector];
	char hello9_tmp[15 * Bytes_Sector];
	//char buffer[Bytes_Sector];
	FILE *imgFile, *binFile, *bootBinFile, *helloFile, *hello2File, *hello3File, *hello4File, *hello5File, *hello6File, *hello7File, *hello8File, *hello9File;
	imgFile = fopen(imgFileName,"rb+");
	binFile = fopen(fat32Bin,"rb");
	bootBinFile = fopen(bootFile,"rb");
	helloFile = fopen(helloBin, "rb");
	hello2File = fopen(hello2Bin, "rb");
	hello3File = fopen(hello3Bin, "rb");
	hello4File = fopen(hello4Bin, "rb");
	hello5File = fopen(hello5Bin, "rb");
	hello6File = fopen(hello6Bin, "rb");
	hello7File = fopen(hello7Bin, "rb");
	hello8File = fopen(hello8Bin, "rb");
	hello9File = fopen(hello9Bin, "rb");
	if(!imgFile)
	{
		cout<<"open image file failed!"<<endl;
	}
	if(!binFile)
	{
		cout<<"open bin file failed!"<<endl;
	}
	if(!bootBinFile)
	{
		cout<<"open boot file failed!"<<endl;
	}
	if(!helloFile)
	{
		cout<<"open hello bin file failed!"<<endl;
	}
	if(!hello2File)
	{
		cout<<"open hello2 bin file failed!"<<endl;
	}
	if(!hello6File)
	{
		cout<<"open hello6 bin file failed!"<<endl;
	}
	cout<<"read "<<fat32Bin<<" file..."<<endl;

	fseek(binFile,0,0);
	fseek(bootBinFile,0,0);
	fseek(helloFile, 0, 0);
	fseek(hello2File, 0, 0);
	fseek(hello3File, 0, 0);
	fseek(hello4File, 0, 0);
	fseek(hello5File, 0, 0);
	fseek(hello6File, 0, 0);
	fseek(hello7File, 0, 0);
	fseek(hello8File, 0, 0);
	fseek(hello9File, 0, 0);
	if(!fread(tmp, 1, 5*Bytes_Sector, binFile))
	{
		if(feof(binFile))
			cout<<"End of file!"<<endl;
		else
			cout<<"Read error!"<<endl;
	}
        if(!fread(buffer, 1, Bytes_Sector, bootBinFile))
        {
                if(feof(bootBinFile))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read error!"<<endl;
        }
        if(!fread(hello_tmp, 1, 40*Bytes_Sector, helloFile))
        {
                if(feof(helloFile))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read error!"<<endl;
        }

        if(!fread(hello2_tmp, 1, 15*Bytes_Sector, hello2File))
        {
                if(feof(hello2File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read hello2.bin error!"<<endl;
        }

        if(!fread(hello3_tmp, 1, 15*Bytes_Sector, hello3File))
        {
                if(feof(hello3File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read hello3.bin error!"<<endl;
        }
        if(!fread(hello4_tmp, 1, 15*Bytes_Sector, hello4File))
        {
                if(feof(hello4File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read error!"<<endl;
        }

        if(!fread(hello5_tmp, 1, 15*Bytes_Sector, hello5File))
        {
                if(feof(hello5File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read hello5.bin error!"<<endl;
        }

        if(!fread(hello6_tmp, 1, 15*Bytes_Sector, hello6File))
        {
                if(feof(hello6File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read hello6.bin error!"<<endl;
        }
        if(!fread(hello7_tmp, 1, 40*Bytes_Sector, hello7File))
        {
                if(feof(hello7File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read error!"<<endl;
        }

        if(!fread(hello8_tmp, 1, 15*Bytes_Sector, hello8File))
        {
                if(feof(hello8File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read hello8.bin error!"<<endl;
        }

        if(!fread(hello9_tmp, 1, 15*Bytes_Sector, hello9File))
        {
                if(feof(hello3File))
                        cout<<"End of file!"<<endl;
                else
                        cout<<"Read hello9.bin error!"<<endl;
        }
	//镜像文件中写入boot.bin
	fseek(imgFile,0,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(buffer, 1, Bytes_Sector, imgFile))
	{
		cout<<"write boot file failed!"<<endl;
	}
	cout<<"write boot file successed!"<<endl;
	//镜像文件中写入fat32.bin -> loader.bin
	fseek(imgFile,Start_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(tmp, 1, 5*Bytes_Sector, imgFile))
	{
		cout<<"write fat32.bin file failed!"<<endl;
	}
	cout<<"write fat32.bin file successed!"<<endl;
	//镜像文件中写入hello.bin
	fseek(imgFile,Hello_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello.bin file failed!"<<endl;
	}
	cout<<"write hello.bin file successed!"<<endl;
	//镜像文件中写入hello2.bin
	fseek(imgFile,Hello2_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello2_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello2.bin file failed!"<<endl;
	}
	cout<<"write hello2.bin file successed!"<<endl;
	//镜像文件中写入hello3.bin
	fseek(imgFile,Hello3_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello3_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello3.bin file failed!"<<endl;
	}
	cout<<"write hello3.bin file successed!"<<endl;
	


	//镜像文件中写入hello4.bin
	fseek(imgFile,Hello4_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello4_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello.bin file failed!"<<endl;
	}
	cout<<"write hello4.bin file successed!"<<endl;
	//镜像文件中写入hello5.bin
	fseek(imgFile,Hello5_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello5_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello5.bin file failed!"<<endl;
	}
	cout<<"write hello5.bin file successed!"<<endl;
	//镜像文件中写入hello3.bin
	fseek(imgFile,Hello6_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello6_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello6.bin file failed!"<<endl;
	}
	cout<<"write hello6.bin file successed!"<<endl;



	//镜像文件中写入hello7.bin
	fseek(imgFile,Hello7_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello7_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello7.bin file failed!"<<endl;
	}
	cout<<"write hello7.bin file successed!"<<endl;
	//镜像文件中写入hello8.bin
	fseek(imgFile,Hello8_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello8_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello8.bin file failed!"<<endl;
	}
	cout<<"write hello8.bin file successed!"<<endl;
	//镜像文件中写入hello9.bin
	fseek(imgFile,Hello9_Sector*Bytes_Sector,0);
	cout<<"current location is: "<<ftell(imgFile)<<endl;
	if(!fwrite(hello9_tmp, 1, 15*Bytes_Sector, imgFile))
	{
		cout<<"write hello9.bin file failed!"<<endl;
	}
	cout<<"write hello9.bin file successed!"<<endl;

	fclose(imgFile);
	fclose(bootBinFile);
	fclose(binFile);
	fclose(helloFile);
	fclose(hello2File);
	fclose(hello3File);
	fclose(hello4File);
	fclose(hello5File);
	fclose(hello6File);
	fclose(hello7File);
	fclose(hello8File);
	fclose(hello9File);
	return EXIT_SUCCESS;
}
