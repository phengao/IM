echo ���¹���Ŀ¼�ļ� git --} src
@set srcmfc=D:\imageManage\MFCApplication1\MFCApplication1\
@set srcstf=D:\imageManage\seetaface\src\SeetaFace2-master\SeetaFace2-master\example\search\
@set srcdar=D:\imageManage\darknet\src\darknet-darknet_yolo_v3\src\

@set mfcgitdir=D:\imageManage\github\IM\MFCApplication1\MFCApplication1\
@set stfgitdir=D:\imageManage\github\IM\seetaface\
@set dargitdir=D:\imageManage\github\IM\darknet\

@echo ����git�ļ���mfcĿ¼
@set cmd=copy %mfcgitdir%*.cpp %srcmfc%
%cmd%                                  
@set cmd=copy %mfcgitdir%*.h %srcmfc%
%cmd%                                  
@set cmd=copy %mfcgitdir%*.rc %srcmfc%
%cmd%                                  
@set cmd=copy %mfcgitdir%*.vc* %srcmfc%
%cmd%
@set cmd=copy %mfcgitdir%*.aps %srcmfc%
%cmd%

@echo ����git�ļ���seetafaceĿ¼
@set cmd=copy %stfgitdir%example.cpp %srcstf%
%cmd%

@echo ����git�ļ���darknetĿ¼
@set cmd=copy %dargitdir%darknet.c %srcdar%
%cmd%
@set cmd=copy %dargitdir%detector.c %srcdar%
%cmd%

