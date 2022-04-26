#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<thread>
#include<queue>
#include<chrono>
#include<windows.h>
#include<cmath>
using namespace cv;


std::queue<std::string> store;
std::string ascii_list = " `*-~^=?/([CXE0$#W@";
std::string addr = "C:/Users/ghost/Desktop/DECO001.flv";		// 文件的地址
int fps = 0;			// 视频的fps
Size zoom;				// 视频的大小
Size zoom_change;		// 处理后的视频的大小
bool flag_is_change = false;
bool is_still_runing = true;
int x_len = 100;
int y_len;
double scal = 3;


void convert_frame(Mat& frame) {
	std::string per_frame((x_len + 1) * y_len, '0');
	int index = 0;
	for (int i = 0; i < y_len; i++) {
		for (int j = 0; j < x_len; j++) {
			int val_index = ceil(((uint)frame.at<uchar>(i, j) * ascii_list.size() / 255)) - 1;
			val_index = val_index >= 0 ? val_index : 0;
			per_frame[index++] = ascii_list[val_index];
		}
		per_frame[index++] = '\n';
	}
	store.push(per_frame);
}

void Debug() {
	std::cout << "当前视频的地址为: " << addr <<
		"\n大小是: " << zoom.width << " * " << zoom.height <<
		"\n帧率为: " << fps <<
		"\n请调小窗口以取得较好的观感" <<
		"\n正在缓冲, 请稍候..." << std::endl;
}

void Cap_convert() {
	is_still_runing = true;
	VideoCapture video;
	video.open(addr);
	if (video.isOpened() == false) {
		printf("open video failed");
		is_still_runing = false;
		return;
	}
	fps = video.get(CAP_PROP_FPS);
	zoom = Size((int)video.get(CAP_PROP_FRAME_WIDTH), (int)video.get(CAP_PROP_FRAME_HEIGHT));

	y_len = x_len / scal;
	zoom_change = Size((int)x_len, (int)y_len);

	Debug();

	Mat frame, out_frame;
	while (video.read(frame)) {

		cvtColor(frame, out_frame, COLOR_BGR2GRAY);	// 处理为黑白
		resize(out_frame, out_frame, zoom_change);
		if (flag_is_change)
			equalizeHist(out_frame, out_frame);
		convert_frame(out_frame);
	}
	is_still_runing = false;
	video.release();
}


void gotoxy(int x, int y) //建立函数 移动光标
{
	COORD position;
	position.X = x;
	position.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void init() {
	std::cout << "请输入视频的地址: ";
	std::cin >> addr;
	std::cout << "请输入视频的横向长度: ";
	std::cin >> x_len;
	std::cout << "长宽比(建议在2.8-4.0之间): ";
	std::cin >> scal;
	int choice;
	std::cout << "是否优化彩色视频?(1: 是 | 0: 否): ";
	std::cin >> choice;
	if (choice == 1)
		flag_is_change = true;
	std::cout << "是否反色?(1: 是 | 0: 否): ";
	std::cin >> choice;
	if (choice == 1)
		reverse(ascii_list.begin(), ascii_list.end());
}

int main() {
	init();
	std::thread convert_video(Cap_convert);
	convert_video.join();
	Sleep(2000);
	system("cls");

	int index = 0;
	std::chrono::steady_clock::time_point start, after;
	while (is_still_runing || store.size()) {
		if (store.size()) {
			start = std::chrono::steady_clock::now();
			gotoxy(1, 1);
			std::cout << "frame: " << index++ <<
				"\n" << store.front();
			store.pop();
			after = std::chrono::steady_clock::now();
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - start);
			int sleep_time = (1000 / fps) - (int)duration.count();
			sleep_time = sleep_time > 0 ? sleep_time : 0;
			Sleep(sleep_time);
		}
		else {
			std::cout << "\n缓冲速度不够, 播放暂停\n";
			system("pause");
		}
	}
	system("pause");
	return 0;
}