#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>


#include "darknet.h"
#include "parser.h"
#include "utils.h"
#include "cuda.h"
#include "blas.h"
#include "connected_layer.h"

#ifdef OPENCV
#include "opencv2/highgui/highgui_c.h"
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;


typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;


 

extern void predict_classifier(char *datacfg, char *cfgfile, char *weightfile, char *filename, int top);
extern void run_voxel(int argc, char **argv);
extern void run_yolo(int argc, char **argv);
extern void run_detector(int argc, char **argv);
extern void run_coco(int argc, char **argv);
extern void run_writing(int argc, char **argv);
extern void run_captcha(int argc, char **argv);
extern void run_nightmare(int argc, char **argv);
extern void run_dice(int argc, char **argv);
extern void run_compare(int argc, char **argv);
extern void run_classifier(int argc, char **argv);
extern void run_char_rnn(int argc, char **argv);
extern void run_vid_rnn(int argc, char **argv);
extern void run_tag(int argc, char **argv);
extern void run_cifar(int argc, char **argv);
extern void run_go(int argc, char **argv);
extern void run_art(int argc, char **argv);
extern void run_super(int argc, char **argv);


HANDLE hStdin, hStdout;
char gallerylist[256] = { 0 };
char identifylist[256] = { 0 };
char wlist[256] = { "" }, dlist[256] = { "" };// wlist wirte the detect results,dlist has the file names that will be detected

CHAR rdflg = 0;
CHAR detectflg = 0;

DWORD dr;

unsigned char rdbuf[256] = { 0 };
unsigned char trbuf[256] = { 0 };
char chBuf[256] = { 0 };

int data_read = 5;
int len;

void average(int argc, char *argv[])
{
    char *cfgfile = argv[2];
    char *outfile = argv[3];
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    network sum = parse_network_cfg(cfgfile);

    char *weightfile = argv[4];
    load_weights(&sum, weightfile);

    int i, j;
    int n = argc - 5;
    for(i = 0; i < n; ++i){
        weightfile = argv[i+5];
        load_weights(&net, weightfile);
        for(j = 0; j < net.n; ++j){
            layer l = net.layers[j];
            layer out = sum.layers[j];
            if(l.type == CONVOLUTIONAL){
                int num = l.n*l.c*l.size*l.size;
                axpy_cpu(l.n, 1, l.biases, 1, out.biases, 1);
                axpy_cpu(num, 1, l.weights, 1, out.weights, 1);
                if(l.batch_normalize){
                    axpy_cpu(l.n, 1, l.scales, 1, out.scales, 1);
                    axpy_cpu(l.n, 1, l.rolling_mean, 1, out.rolling_mean, 1);
                    axpy_cpu(l.n, 1, l.rolling_variance, 1, out.rolling_variance, 1);
                }
            }
            if(l.type == CONNECTED){
                axpy_cpu(l.outputs, 1, l.biases, 1, out.biases, 1);
                axpy_cpu(l.outputs*l.inputs, 1, l.weights, 1, out.weights, 1);
            }
        }
    }
    n = n+1;
    for(j = 0; j < net.n; ++j){
        layer l = sum.layers[j];
        if(l.type == CONVOLUTIONAL){
            int num = l.n*l.c*l.size*l.size;
            scal_cpu(l.n, 1./n, l.biases, 1);
            scal_cpu(num, 1./n, l.weights, 1);
                if(l.batch_normalize){
                    scal_cpu(l.n, 1./n, l.scales, 1);
                    scal_cpu(l.n, 1./n, l.rolling_mean, 1);
                    scal_cpu(l.n, 1./n, l.rolling_variance, 1);
                }
        }
        if(l.type == CONNECTED){
            scal_cpu(l.outputs, 1./n, l.biases, 1);
            scal_cpu(l.outputs*l.inputs, 1./n, l.weights, 1);
        }
    }
    save_weights(sum, outfile);
}

void speed(char *cfgfile, int tics)
{
    if (tics == 0) tics = 1000;
    network net = parse_network_cfg(cfgfile);
    set_batch_network(&net, 1);
    int i;
    time_t start = time(0);
    image im = make_image(net.w, net.h, net.c);
    for(i = 0; i < tics; ++i){
        network_predict(net, im.data);
    }
    double t = difftime(time(0), start);
    printf("\n%d evals, %f Seconds\n", tics, t);
    printf("Speed: %f sec/eval\n", t/tics);
    printf("Speed: %f Hz\n", tics/t);
}

void operations(char *cfgfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    int i;
    long ops = 0;
    for(i = 0; i < net.n; ++i){
        layer l = net.layers[i];
        if(l.type == CONVOLUTIONAL){
            ops += 2l * l.n * l.size*l.size*l.c * l.out_h*l.out_w;
        } else if(l.type == CONNECTED){
            ops += 2l * l.inputs * l.outputs;
		} else if (l.type == RNN){
            ops += 2l * l.input_layer->inputs * l.input_layer->outputs;
            ops += 2l * l.self_layer->inputs * l.self_layer->outputs;
            ops += 2l * l.output_layer->inputs * l.output_layer->outputs;
        } else if (l.type == GRU){
            ops += 2l * l.uz->inputs * l.uz->outputs;
            ops += 2l * l.uh->inputs * l.uh->outputs;
            ops += 2l * l.ur->inputs * l.ur->outputs;
            ops += 2l * l.wz->inputs * l.wz->outputs;
            ops += 2l * l.wh->inputs * l.wh->outputs;
            ops += 2l * l.wr->inputs * l.wr->outputs;
        } else if (l.type == LSTM){
            ops += 2l * l.uf->inputs * l.uf->outputs;
            ops += 2l * l.ui->inputs * l.ui->outputs;
            ops += 2l * l.ug->inputs * l.ug->outputs;
            ops += 2l * l.uo->inputs * l.uo->outputs;
            ops += 2l * l.wf->inputs * l.wf->outputs;
            ops += 2l * l.wi->inputs * l.wi->outputs;
            ops += 2l * l.wg->inputs * l.wg->outputs;
            ops += 2l * l.wo->inputs * l.wo->outputs;
        }
    }
    printf("Floating Point Operations: %ld\n", ops);
    printf("Floating Point Operations: %.2f Bn\n", (float)ops/1000000000.);
}

void oneoff(char *cfgfile, char *weightfile, char *outfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    int oldn = net.layers[net.n - 2].n;
    int c = net.layers[net.n - 2].c;
    net.layers[net.n - 2].n = 9372;
    net.layers[net.n - 2].biases += 5;
    net.layers[net.n - 2].weights += 5*c;
    if(weightfile){
        load_weights(&net, weightfile);
    }
    net.layers[net.n - 2].biases -= 5;
    net.layers[net.n - 2].weights -= 5*c;
    net.layers[net.n - 2].n = oldn;
    printf("%d\n", oldn);
    layer l = net.layers[net.n - 2];
    copy_cpu(l.n/3, l.biases, 1, l.biases +   l.n/3, 1);
    copy_cpu(l.n/3, l.biases, 1, l.biases + 2*l.n/3, 1);
    copy_cpu(l.n/3*l.c, l.weights, 1, l.weights +   l.n/3*l.c, 1);
    copy_cpu(l.n/3*l.c, l.weights, 1, l.weights + 2*l.n/3*l.c, 1);
    *net.seen = 0;
    save_weights(net, outfile);
}

void partial(char *cfgfile, char *weightfile, char *outfile, int max)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights_upto(&net, weightfile, max);
    }
    *net.seen = 0;
    save_weights_upto(net, outfile, max);
}

#include "convolutional_layer.h"
void rescale_net(char *cfgfile, char *weightfile, char *outfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    int i;
    for(i = 0; i < net.n; ++i){
        layer l = net.layers[i];
        if(l.type == CONVOLUTIONAL){
            rescale_weights(l, 2, -.5);
            break;
        }
    }
    save_weights(net, outfile);
}

void rgbgr_net(char *cfgfile, char *weightfile, char *outfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    int i;
    for(i = 0; i < net.n; ++i){
        layer l = net.layers[i];
        if(l.type == CONVOLUTIONAL){
            rgbgr_weights(l);
            break;
        }
    }
    save_weights(net, outfile);
}

void reset_normalize_net(char *cfgfile, char *weightfile, char *outfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if (weightfile) {
        load_weights(&net, weightfile);
    }
    int i;
    for (i = 0; i < net.n; ++i) {
        layer l = net.layers[i];
        if (l.type == CONVOLUTIONAL && l.batch_normalize) {
            denormalize_convolutional_layer(l);
        }
        if (l.type == CONNECTED && l.batch_normalize) {
            denormalize_connected_layer(l);
        }
        if (l.type == GRU && l.batch_normalize) {
            denormalize_connected_layer(*l.input_z_layer);
            denormalize_connected_layer(*l.input_r_layer);
            denormalize_connected_layer(*l.input_h_layer);
            denormalize_connected_layer(*l.state_z_layer);
            denormalize_connected_layer(*l.state_r_layer);
            denormalize_connected_layer(*l.state_h_layer);
        }
        if (l.type == LSTM && l.batch_normalize) {
            denormalize_connected_layer(*l.wf);
            denormalize_connected_layer(*l.wi);
            denormalize_connected_layer(*l.wg);
            denormalize_connected_layer(*l.wo);
            denormalize_connected_layer(*l.uf);
            denormalize_connected_layer(*l.ui);
            denormalize_connected_layer(*l.ug);
            denormalize_connected_layer(*l.uo);
		}
    }
    save_weights(net, outfile);
}

layer normalize_layer(layer l, int n)
{
    int j;
    l.batch_normalize=1;
    l.scales = calloc(n, sizeof(float));
    for(j = 0; j < n; ++j){
        l.scales[j] = 1;
    }
    l.rolling_mean = calloc(n, sizeof(float));
    l.rolling_variance = calloc(n, sizeof(float));
    return l;
}

void normalize_net(char *cfgfile, char *weightfile, char *outfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    int i;
    for(i = 0; i < net.n; ++i){
        layer l = net.layers[i];
        if(l.type == CONVOLUTIONAL && !l.batch_normalize){
            net.layers[i] = normalize_layer(l, l.n);
        }
        if (l.type == CONNECTED && !l.batch_normalize) {
            net.layers[i] = normalize_layer(l, l.outputs);
        }
        if (l.type == GRU && l.batch_normalize) {
            *l.input_z_layer = normalize_layer(*l.input_z_layer, l.input_z_layer->outputs);
            *l.input_r_layer = normalize_layer(*l.input_r_layer, l.input_r_layer->outputs);
            *l.input_h_layer = normalize_layer(*l.input_h_layer, l.input_h_layer->outputs);
            *l.state_z_layer = normalize_layer(*l.state_z_layer, l.state_z_layer->outputs);
            *l.state_r_layer = normalize_layer(*l.state_r_layer, l.state_r_layer->outputs);
            *l.state_h_layer = normalize_layer(*l.state_h_layer, l.state_h_layer->outputs);
            net.layers[i].batch_normalize=1;
        }
        if (l.type == LSTM && l.batch_normalize) {
            *l.wf = normalize_layer(*l.wf, l.wf->outputs);
            *l.wi = normalize_layer(*l.wi, l.wi->outputs);
            *l.wg = normalize_layer(*l.wg, l.wg->outputs);
            *l.wo = normalize_layer(*l.wo, l.wo->outputs);
            *l.uf = normalize_layer(*l.uf, l.uf->outputs);
            *l.ui = normalize_layer(*l.ui, l.ui->outputs);
            *l.ug = normalize_layer(*l.ug, l.ug->outputs);
            *l.uo = normalize_layer(*l.uo, l.uo->outputs);
            net.layers[i].batch_normalize=1;
        }
    }
    save_weights(net, outfile);
}

void statistics_net(char *cfgfile, char *weightfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if (weightfile) {
        load_weights(&net, weightfile);
    }
    int i;
    for (i = 0; i < net.n; ++i) {
        layer l = net.layers[i];
        if (l.type == CONNECTED && l.batch_normalize) {
            printf("Connected Layer %d\n", i);
            statistics_connected_layer(l);
        }
        if (l.type == GRU && l.batch_normalize) {
            printf("GRU Layer %d\n", i);
            printf("Input Z\n");
            statistics_connected_layer(*l.input_z_layer);
            printf("Input R\n");
            statistics_connected_layer(*l.input_r_layer);
            printf("Input H\n");
            statistics_connected_layer(*l.input_h_layer);
            printf("State Z\n");
            statistics_connected_layer(*l.state_z_layer);
            printf("State R\n");
            statistics_connected_layer(*l.state_r_layer);
            printf("State H\n");
            statistics_connected_layer(*l.state_h_layer);
        }
        if (l.type == LSTM && l.batch_normalize) {
            printf("LSTM Layer %d\n", i);
            printf("wf\n");
            statistics_connected_layer(*l.wf);
            printf("wi\n");
            statistics_connected_layer(*l.wi);
            printf("wg\n");
            statistics_connected_layer(*l.wg);
            printf("wo\n");
            statistics_connected_layer(*l.wo);
            printf("uf\n");
            statistics_connected_layer(*l.uf);
            printf("ui\n");
            statistics_connected_layer(*l.ui);
            printf("ug\n");
            statistics_connected_layer(*l.ug);
            printf("uo\n");
            statistics_connected_layer(*l.uo);
        }
        printf("\n");
    }
}

void denormalize_net(char *cfgfile, char *weightfile, char *outfile)
{
    gpu_index = -1;
    network net = parse_network_cfg(cfgfile);
    if (weightfile) {
        load_weights(&net, weightfile);
    }
    int i;
    for (i = 0; i < net.n; ++i) {
        layer l = net.layers[i];
        if (l.type == CONVOLUTIONAL && l.batch_normalize) {
            denormalize_convolutional_layer(l);
            net.layers[i].batch_normalize=0;
        }
        if (l.type == CONNECTED && l.batch_normalize) {
            denormalize_connected_layer(l);
            net.layers[i].batch_normalize=0;
        }
        if (l.type == GRU && l.batch_normalize) {
            denormalize_connected_layer(*l.input_z_layer);
            denormalize_connected_layer(*l.input_r_layer);
            denormalize_connected_layer(*l.input_h_layer);
            denormalize_connected_layer(*l.state_z_layer);
            denormalize_connected_layer(*l.state_r_layer);
            denormalize_connected_layer(*l.state_h_layer);
            l.input_z_layer->batch_normalize = 0;
            l.input_r_layer->batch_normalize = 0;
            l.input_h_layer->batch_normalize = 0;
            l.state_z_layer->batch_normalize = 0;
            l.state_r_layer->batch_normalize = 0;
            l.state_h_layer->batch_normalize = 0;
            net.layers[i].batch_normalize=0;
        }
        if (l.type == GRU && l.batch_normalize) {
            denormalize_connected_layer(*l.wf);
            denormalize_connected_layer(*l.wi);
            denormalize_connected_layer(*l.wg);
            denormalize_connected_layer(*l.wo);
            denormalize_connected_layer(*l.uf);
            denormalize_connected_layer(*l.ui);
            denormalize_connected_layer(*l.ug);
            denormalize_connected_layer(*l.uo);
            l.wf->batch_normalize = 0;
            l.wi->batch_normalize = 0;
            l.wg->batch_normalize = 0;
            l.wo->batch_normalize = 0;
            l.uf->batch_normalize = 0;
            l.ui->batch_normalize = 0;
            l.ug->batch_normalize = 0;
            l.uo->batch_normalize = 0;
            net.layers[i].batch_normalize=0;
		}
    }
    save_weights(net, outfile);
}

void visualize(char *cfgfile, char *weightfile)
{
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    visualize_network(net);
#ifdef OPENCV
    cvWaitKey(0);
#endif
}

int DoDetect(char *argv)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	gpu_index = -1;

	clock_t start, finish;
	double  duration;
	printf("detect on w<%s> d<%s>\n",wlist,dlist);
	start = clock();

	float thresh = .24;
	//darknet detect cfg/yolov3.cfg yolov3.weights data/dog.jpg
	//test_detector("cfg/coco.data", argv[2], argv[3], filename, thresh, 0.5, 0, 1, 0, NULL);
	//test_detector("cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights", "D:/imageManage/seetaface/src/Detctedlist.txt", thresh, 0.5, 0, 1, 0, NULL);
	// dlist include all detect file names ,wlist is a file name for recode detected file names
	test_detector("cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights", dlist, thresh, 0.5, 0, 1, 0, wlist);

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("%f seconds\n", duration);
	DWORD dwWritten = 0;
	sprintf(chBuf, "used <%fs> detect finished w<%s> d<%s>", duration,wlist, dlist);
	bool bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
	
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

int main(void)
{
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

	//seetofaceInit();
	//init:
	detectflg = 1;

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

		rdflg = 1;

		time(&start);
		// Write to standard output and stop on error.
		//printf("start handle messages\n");
		if (rdflg == 1) {

			char *revbuf[8] = { 0 };
			int num = 0;

			rdflg = 0;
			if (strstr((char *)trbuf, "QUIT") != NULL) {//find "stop" in trbuf
				sprintf(chBuf, "[s] draknet_no_gpu.exe exit process");
				bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
				Sleep(100);
				ExitProcess(1);
			}
			else if (strstr((char *)trbuf, "DetecT") != NULL) {
				if (detectflg == 1) {// detectflg 0 means that the identify is doing now,set and clear by identify_pic()
					const char* dst = (char *)trbuf;
					char *tmp;

					tmp = (char *)trbuf;
					split(tmp, " ", revbuf, &num);
					if(num > 2){//must three items
						memset(dlist, sizeof(dlist), 0);
						memset(wlist, sizeof(wlist), 0);
						if (strlen(revbuf[1]) == 0) {
							memcpy(dlist, "Filelist.txt", strlen("Filelist.txt"));
						}
						else {
							memcpy(dlist, revbuf[1], strlen(revbuf[1]));
						}

						if (strlen(revbuf[2]) == 0) {
							//use default path						
							memcpy(wlist, "Detctedlist.txt", strlen("Detctedlist.txt"));
						}
						else {
							memcpy(wlist, revbuf[2], strlen(revbuf[2]));
						}
						HANDLE hListenThread1 = CreateThread(NULL, 0, DoDetect, NULL, 0, NULL);	//create identify thread &trbuf[iPos]
						sprintf(chBuf, "listfile: w<%s> d<%s>", wlist, dlist);
						bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
					}
					else {
						memset(dlist, 0,sizeof(dlist));
						memset(wlist, 0,sizeof(wlist));
						memcpy(wlist, "Detctedlist.txt", strlen("Detctedlist.txt"));
						memcpy(dlist, "Filelist.txt", strlen("Filelist.txt"));

						//darknet detect cfg/yolov3.cfg yolov3.weights data/dog.jpg
						///HANDLE hListenThread1 = CreateThread(NULL, 0, DoDetect, NULL, 0, NULL);	//create identify thread &trbuf[iPos]
						//test_detector("cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights", "D:/imageManage/seetaface/src/Detctedlist.txt", .24, 0.5, 0, 1, 0, NULL);

						sprintf(chBuf, "no listfile: g<%s> i<%s>", wlist, dlist);
						bSuccess = WriteFile(hStdout, &chBuf, strlen(chBuf), &dwWritten, NULL);
					}
					continue;
				}
				else {
					sprintf(chBuf, "Do detecting...");
					bSuccess = WriteFile(hStdout, chBuf, strlen(chBuf), &dwWritten, NULL);
					continue;
				}
			}

			sprintf(chBuf, "** clock:%ld rd data:%d - %d <%s> outhandle:0x%x  inhandle:0x%x    **", start, rdflg, len, trbuf, hStdout, hStdin);
			bSuccess = WriteFile(hStdout, chBuf, strlen(chBuf), &dwWritten, NULL);
			if (!bSuccess)
				break;
		}
	}
}
