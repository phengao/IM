echo 更新工作目录文件 git --} src
@set srcmfc=D:\imageManage\MFCApplication1\MFCApplication1\
@set srcstf=D:\imageManage\seetaface\src\SeetaFace2-master\SeetaFace2-master\example\search\
@set srcdar=D:\imageManage\darknet\src\darknet-darknet_yolo_v3\src\

@set mfcgitdir=D:\imageManage\github\IM\MFCApplication1\MFCApplication1\
@set stfgitdir=D:\imageManage\github\IM\seetaface\
@set dargitdir=D:\imageManage\github\IM\darknet\

@echo 拷贝git文件到mfc目录
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

@echo 拷贝git文件到seetaface目录
@set cmd=copy %stfgitdir%example.cpp %srcstf%
%cmd%

@echo 拷贝git文件到darknet目录
@set cmd=copy %dargitdir%darknet.c %srcdar%
%cmd%
@set cmd=copy %dargitdir%detector.c %srcdar%
%cmd%

