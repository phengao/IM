#pragma warning(disable: 4819)

#include <seeta/FaceEngine.h>

#include <seeta/Struct_cv.h>
#include <seeta/Struct.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <array>
#include <map>
#include <iostream>

#include <seeta/QualityAssessor.h>

#pragma warning(disable: 4819)

#include <seeta/FaceEngine.h>
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>

#include <seeta/Struct_cv.h>
#include <seeta/Struct.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <array>
#include <map>
#include <string.h>
//#include <cstring.h>

#include <time.h>
#include <windows.h>
#include <vector>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <wchar.h>

#include  <fstream>

//#include "identify.h"

using std::string;
using std::vector;
using namespace std;


HANDLE hStdin, hStdout;
char gallerylist[256] = { 0 };
char identifylist[256] = { 0 };
string glist = { "" };
string ilist = { "" };
string wlist = { "" };

char *pglist = "Gallerylist.txt";
char *pilist = "Identifylist.txt";
char *pwlist = "Identifiedlist.txt";



int test_image(seeta::FaceDetector &FD, seeta::FaceLandmarker &FL)
{
//	std::string image_path = "D:/imageManage/seetaface/src/images/1.jpg";
	std::string image_path = "1.jpg";
	std::cout << "Loading image: " << image_path << std::endl;
	auto frame = cv::imread(image_path);
	seeta::cv::ImageData simage = frame;

	if (simage.empty()) {
		std::cerr << "Can not open image: " << image_path << std::endl;
		return EXIT_FAILURE;
	}


	auto faces = FD.detect(simage);

	for (int i = 0; i < faces.size; ++i)
	{
		auto &face = faces.data[i];
		auto points = FL.mark(simage, face.pos);

		cv::rectangle(frame, cv::Rect(face.pos.x, face.pos.y, face.pos.width, face.pos.height), CV_RGB(128, 128, 255), 3);
		for (auto &point : points)
		{
			cv::circle(frame, cv::Point(point.x, point.y), 3, CV_RGB(128, 255, 128), -1);
		}
	}

	auto output_path = image_path + ".pts81.png";
	cv::imwrite(output_path, frame);
	std::cerr << "Saving result into: " << output_path << std::endl;

	cv::namedWindow("Image_Result", 0);
	//cv::resizeWindow("Image_Result",500,500);
	cv::imshow("Image_Result", frame);
	cv::waitKey(0);

	return 0;
	//return EXIT_SUCCESS;
}

int identify_pic() {

	seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
	int id = 0;
	//seeta::ModelSetting FD_model("D:/imageManage/seetaface/src/model/fd_2_00.dat", device, id);
	//seeta::ModelSetting PD_model("D:/imageManage/seetaface/src/model/pd_2_00_pts5.dat", device, id);
	//seeta::ModelSetting FR_model("D:/imageManage/seetaface/src/model/fr_2_10.dat", device, id);
	//seeta::ModelSetting FL_model("D:/imageManage/seetaface/src/model/pd_2_00_pts81.dat", device, id);
	
	seeta::ModelSetting FD_model("model/fd_2_00.dat", device, id);
	seeta::ModelSetting PD_model("model/pd_2_00_pts5.dat", device, id);
	seeta::ModelSetting FR_model("model/fr_2_10.dat", device, id);
	seeta::ModelSetting FL_model("model/pd_2_00_pts81.dat", device, id);
	seeta::FaceEngine engine(FD_model, PD_model, FR_model, 2, 16);

	seeta::FaceDetector FD(FD_model);
	seeta::FaceLandmarker FL(FL_model);

	// recognization threshold
	float threshold = 0.2f;

	// create identify database first

	//set face detector's min face size
	engine.FD.set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 50);

	//this is the picture identity database directory
	//std::ifstream fin("D:/imageManage/seetaface/src/list.txt");
	std::ifstream fin(glist);

	if (!fin.is_open())
	{
		cout << "can not open listfile.>" << glist << endl;
		return 0;
	}

	std::vector<std::string> GalleryImageFilename;
	string str;
	const char *cp;
	while (std::getline(fin, str))
	{
		GalleryImageFilename.push_back(str);
		printf("gallery file:<%s>\n", str.c_str());
	}
	fin.close();


	std::vector<int64_t> GalleryIndex(GalleryImageFilename.size());
	for (size_t i = 0; i < GalleryImageFilename.size(); ++i)
	{
		//register face into facedatabase
		std::string &filename = GalleryImageFilename[i];

		int64_t &index = GalleryIndex[i];
		std::cout << "Registering... " << filename << std::endl;
		seeta::cv::ImageData image = cv::imread(filename);
		auto id = engine.Register(image);

		index = id;
		std::cout << "Registered id = " << id << std::endl;
	}
	
	std::map<int64_t, std::string> GalleryIndexMap;
	for (size_t i = 0; i < GalleryIndex.size(); ++i)
	{
		// save index and name pair
		if (GalleryIndex[i] < 0) continue;
		GalleryIndexMap.insert(std::make_pair(GalleryIndex[i], GalleryImageFilename[i]));
	}

	// start identify
	//ofstream ofs("D:/imageManage/seetaface/src/iResultsList.txt", fstream::out | ios_base::trunc); //open a file to keep the result
	ofstream ofs(wlist, fstream::out | ios_base::trunc); //open a file to keep the result


	//multipic identify
	//this is the picture identity database directory
	//std::ifstream fin1("D:/imageManage/seetaface/src/Identifylist.txt");
	std::ifstream fin1(ilist);
	// must ensure the image path is right!

	if (!fin1.is_open())
	{
		cout << "can not open identify listfile. >" << ilist << endl;
	}
	else {
		std::vector<std::string> IdentifyImageFilename;
		string str;
		const char *cp;
		while (std::getline(fin1, str))
		{
			IdentifyImageFilename.push_back(str);
			//cp = str.c_str();
			//WriteFile(hStdout, cp, strlen(cp), NULL, NULL);

		}
		fin1.close();

		for (size_t i = 0; i < IdentifyImageFilename.size(); ++i)
		{
			//register face into facedatabase
			std::string image_path = IdentifyImageFilename[i];

			//std::string image_path = "D:/imageManage/seetaface/src/3.jpg";
			if (image_path == "")
				continue;
			std::cout << "LoaDing image: " << image_path << std::endl;
			auto frame = cv::imread(image_path);

			int width1 = frame.cols;
			int height1 = frame.rows;
			cv::resize(frame, frame, cv::Size(width1 / 2, height1 / 2));

			seeta::cv::ImageData image = frame;

			// Detect all faces
			std::vector<SeetaFaceInfo> faces = engine.DetectFaces(image);
			string strrus = image_path;

			for (SeetaFaceInfo &face : faces)
			{
				// Query top 1
				int64_t index = -1;
				float similarity = 0;

				auto points = engine.DetectPoints(image, face);

				auto faces = FD.detect(image);

				for (int i = 0; i < faces.size; ++i)
				{
					auto &face = faces.data[i];
					auto points = FL.mark(image, face.pos);

					cv::rectangle(frame, cv::Rect(face.pos.x, face.pos.y, face.pos.width, face.pos.height), CV_RGB(128, 128, 255), 3);
					for (auto &point : points)
					{
						cv::circle(frame, cv::Point(point.x, point.y), 3, CV_RGB(128, 255, 128), -1);
					}
				}

				auto queried = engine.QueryTop(image, points.data(), 1, &index, &similarity);
				
				// no face queried from database
				if (queried < 1) continue;

				///std::cerr << similarity << std::endl;
				///std::cout << "top_person:" << GalleryIndexMap[index] << std::endl;
				
				// similarity greater than threshold, means recognized
				if (similarity > 0.4)
				{
					///std::cout << "right_person:" << GalleryIndexMap[index] << std::endl;
					cv::putText(frame, GalleryIndexMap[index], cv::Point(face.pos.x, face.pos.y - 5), 3, 1, CV_RGB(255, 128, 128));
					
					string::size_type iPos = GalleryIndexMap[index].find_last_of('\\') + 1;
					string tmp = GalleryIndexMap[index].substr(iPos, GalleryIndexMap[index].length() - iPos);

					tmp = tmp.substr(0, tmp.rfind("."));
					if (strrus.find("\x09"+tmp)==tmp.npos) {
						if(similarity>0.5)
							strrus.append("\x09" + tmp + "-" + std::to_string(similarity));
						else
							strrus.append("\x09似" + tmp + "-" + std::to_string(similarity));

					}
				}
				else {
					;// std::cerr << "Nothing similar." << std::endl;
				}
			}
			strrus.append("\n");
			ofs.write(strrus.c_str(), strlen(strrus.c_str()));


			char showflg = 0;

			if (showflg) {
				cv::namedWindow("Compare_Result", 0);
				cv::imshow("Compare_Result", frame);
				cv::waitKey(0);
			}
		}		
	}
	ofs.close();

	WriteFile(hStdout, "Identify finished\r\n[s] search.exe exit process\r\n", strlen("Identify finished\r\n[s] search.exe exit process\r\n"), NULL, NULL);
	Sleep(100);


	ExitProcess(1);

	return 0;
}

// pipe 子进程代码
#include <windows.h>
#include <stdio.h>

#define BUFSIZE 1024 


CHAR rdflg = 0;
CHAR identifyflg = 0;

DWORD dr;

unsigned char rdbuf[256] = { 0 };
unsigned char trbuf[256] = { 0 };
char chBuf[256] = { 0 };

int data_read = 5;
int len;

DWORD WINAPI Pipe_Listen(LPVOID lpParameter)
{
	HANDLE* PipeRead = (HANDLE*)lpParameter;
	//CHAR data[128] = { 0 };
	//printf("thread rd : %d\n",*PipeRead);

	//_sprintf(data, "\n ** This is a message from the child process ");
	while (true)
	{
		if (rdflg == 1) {
			Sleep(200);
			continue;

		}

		memset(rdbuf, 0, sizeof(rdbuf));
		if (!ReadFile((HANDLE)(*PipeRead), rdbuf, 128, &dr, NULL))
			continue;
		//time_t finish;
		data_read = dr;
		len = (int)dr;
		//sprintf(&rdbuf[strlen(rdbuf)], " rd:%d\n", len);
		int j = 0;
		memset(trbuf, 0, sizeof(trbuf));
		for (int i = 0; i < len; i++) {
			//printf("rd%d:%x\n", i, rdbuf[i]);
			if (rdbuf[i] != 0) {
				trbuf[j] = rdbuf[i];
				//printf("tr%d:%x\n", j, trbuf[j]);
				j++;
			}
		}


		//printf("\nlen %d %d\n",len,j);

		data_read = dr;
		len = j;


		//time(&finish);

		
		rdflg = 1;
		//Sleep(1200);
	}

	return 0;
}

int seetofaceInit(void)
{
	//AfxMessageBox(DirectoryBuffer);
	

	identifyflg = 1; // allow to indentify


	return 1;
}

DWORD WINAPI DoIdentify(LPVOID lpParameter)
{
	identifyflg = 0;
	identify_pic();
	identifyflg = 1;

	return 0;
}

void split(char *src, const char *separator, char **dest, int *num) {
	/*
	src 源字符串的首地址(buf的地址)
	separator 指定的分割字符
	dest 接收子字符串的数组
	num 分割后子字符串的个数
	*/
	char *pNext;
	int count = 0;
	if (src == NULL || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止 
		return;
	if (separator == NULL || strlen(separator) == 0) //如未指定分割的字符串，直接终止 
		return;
	pNext = (char *)strtok(src, separator); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
	while (pNext != NULL) {
		*dest++ = pNext;
		++count;
		pNext = (char *)strtok(NULL, separator);  //必须使用(char *)进行强制类型转换
	}
	*num = count;
}


int main()
{

	// pipe 子进程代码	
	DWORD dwRead, dwWritten;
	BOOL bSuccess;
	time_t  start;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (
		(hStdout == INVALID_HANDLE_VALUE) ||
		(hStdin == INVALID_HANDLE_VALUE)
		)
		ExitProcess(1);

	seetofaceInit();

	// create listen thread
	//HANDLE hListenThread = CreateThread(NULL, 0, Pipe_Listen, &hStdin, 0, NULL);	//create pipe listen thread

	time(&start);

	// message process loop
	for (;;)
	{
		// read pipe get order
		//printf("start reading\n");
		memset(rdbuf, 0, sizeof(rdbuf));
		if (!ReadFile(hStdin, rdbuf, 256, &dr, NULL)) {
			printf("read fail\n");
			continue;
		}
		//time_t finish;
		data_read = dr;
		len = (int)dr;

		int j = 0;
		memset(trbuf, 0, sizeof(trbuf));
		for (int i = 0; i < len; i++) {
			//printf("rd%d:%x\n", i, rdbuf[i]);
			if (rdbuf[i] != 0) {
				trbuf[j] = rdbuf[i];
				//printf("tr%d:%x\n", j, trbuf[j]);
				j++;
			}
		}

		data_read = dr;
		len = j;

		rdflg = 1;

		time(&start);
		// Write to standard output and stop on error.
		//printf("start handle messages\n");
		if (rdflg == 1) {

			rdflg = 0;

			if (strstr((char *)trbuf, "QUIT") != NULL) {//find "stop" in trbuf
				// do something for close process
				Sleep(100);
				sprintf(chBuf, "[s] search.exe exit process");
				bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
				Sleep(100);


				ExitProcess(1);
			}
			else if (strstr((char *)trbuf, "IdenTifY") != NULL){
				if (identifyflg == 1) {// identifyflg 0 means that the identify is doing now,set and clear by identify_pic()
					const char* dst = (char *)trbuf;
					string tmp;
					char *revbuf[8] = { 0 };
					int num;
					
					tmp = (char *)trbuf;
					memset(revbuf, 0, sizeof(revbuf));
					split((char *)trbuf, " ", revbuf, &num);
					if (num > 3) {//must three items
						glist.clear();
						ilist.clear();
						wlist.clear();
						
						if (strlen(revbuf[1]) == 0) {
							glist = "Gallerylist.txt";
						}
						else {
							glist = revbuf[1];
						}
						if (strlen(revbuf[2]) == 0) {
							ilist = "IFilelist.txt";
						}
						else {
							ilist = revbuf[2];
						}
						if (strlen(revbuf[3]) == 0) {
							//use default path						
							wlist = "Identifiedlist.txt";
						}
						else {
							wlist = revbuf[3];
						}
						
						HANDLE hListenThread1 = CreateThread(NULL, 0, DoIdentify, NULL, 0, NULL);	//create identify thread &trbuf[iPos]
						///identify_pic();
						memset((char *)chBuf, 0, sizeof(chBuf));
						sprintf(chBuf, "[s] listfile: g<%s> i<%s> w<%s>", glist.c_str(), ilist.c_str(), wlist.c_str());
						bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
					}
					else {
						glist.clear();
						ilist.clear();
						wlist.clear();
						glist = pglist;// "Gallerylist.txt";
						ilist = pilist;// "Identifylist.txt";
						wlist = pwlist;// "Identifiedlist.txt";

						memset((char *)chBuf, 0, sizeof(chBuf));
						///HANDLE hListenThread1 = CreateThread(NULL, 0, DoIdentify, NULL, 0, NULL);	//create identify thread &trbuf[iPos]
						identify_pic();
						sprintf(chBuf, "[s] no listfile: g<%s> i<%s> w<%s>", glist.c_str(),ilist.c_str(),wlist.c_str());
						bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
					}
					continue;
				}
				else {
					sprintf(chBuf, "Do identifying...");
					bSuccess = WriteFile(hStdout, chBuf, strlen(chBuf), &dwWritten, NULL);
					continue;
				}
			}
			sprintf(chBuf, "\n ** clock:%ld rd data:%d - %d <%s> outhandle:0x%x  inhandle:0x%x    ** \n", start, rdflg, len,trbuf, hStdout, hStdin);
			bSuccess = WriteFile(hStdout, chBuf, strlen(chBuf), &dwWritten, NULL);
			if (!bSuccess);
				//break;
		}
	}
	//printf("\n out for loop no write");
	//printf(chBuf);

	//Sleep(5000);

	//CloseHandle(hStdout);
	//CloseHandle(hStdin);

	//ExitProcess(1);
	return 0;
}