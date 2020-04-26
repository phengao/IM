echo 提交工作目录文件 src --} git
@set srcmfc=D:\imageManage\MFCApplication1\MFCApplication1\
@set srcstf=D:\imageManage\seetaface\src\SeetaFace2-master\SeetaFace2-master\example\search\
@set srcdar=D:\imageManage\darknet\src\darknet-darknet_yolo_v3\src\

@set mfcgitdir=D:\imageManage\github\IM\MFCApplication1\MFCApplication1\
@set stfgitdir=D:\imageManage\github\IM\seetaface\
@set dargitdir=D:\imageManage\github\IM\darknet\

@echo 拷贝mfc文件到git目录
@set cmd=copy %srcmfc%*.cpp %mfcgitdir%
%cmd%
@set cmd=copy %srcmfc%*.h %mfcgitdir%
%cmd%
@set cmd=copy %srcmfc%*.rc %mfcgitdir%
%cmd%
@set cmd=copy %srcmfc%*.vc* %mfcgitdir%
%cmd%
@set cmd=copy %srcmfc%*.aps %mfcgitdir%
%cmd%

@echo 拷贝seetaface文件到git目录
@set cmd=copy %srcstf%example.cpp %stfgitdir%
%cmd%

@echo 拷贝darknet文件到git目录
@set cmd=copy %srcdar%darknet.c %dargitdir%
%cmd%
@set cmd=copy %srcdar%detector.c %dargitdir%
%cmd%
