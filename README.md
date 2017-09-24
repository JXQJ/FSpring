# FSpring 1.1
### video Frame extractor.
* * *
#### Demo

[FSpring(win64).zip](https://www.dropbox.com/s/sy0y0n6bpqdrrni/FSpring%28win64%29.zip?dl=1)
[FSpring(linux64).tar.gz](https://www.dropbox.com/s/b166olaybi8r53j/FSpring%28linux64%29.tar.gz?dl=1)

or

This repository..
`FSpring/FSpring(windows)/FSpring(win64)/`
`FSpring/FSpring(ubuntu)/FSpring(linux64)/`

#### Requirement
* ==Windows==
	* [Visual studio 2015](https://imagine.microsoft.com/ko-KR/Catalog/Product/101)
	* [FFmpeg 3.3.3](https://ffmpeg.zeranoe.com/builds/)
	* [Opencv 3.3.0](https://github.com/opencv/opencv/releases/tag/3.3.0)
	* [MSpring](https://github.com/springkim/MSpring)
* ==Ubuntu==
	* [CLion](https://www.jetbrains.com/clion/)
	* [Opencv 3.3.0](https://github.com/opencv/opencv/releases/tag/3.3.0)
	* [nana](http://nanapro.org/en-us/)
	```bash
    apt-get install gcc g++ python3 vim build-essential cmake pkg-config libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev libavcodec-dev libavformat-dev libswscale-dev libxvidcore-dev libx264-dev libxine2-dev libv4l-dev v4l-utils libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libqt4-dev mesa-utils libgl1-mesa-dri libqt4-opengl-dev libatlas-base-dev gfortran libeigen3-dev python2.7-dev python3-dev python-numpy python3-numpy -y
	```

#### How to use


![](https://i.imgur.com/hVAazP0.png)

##### 1. Run **FSpring.exe**
![](https://i.imgur.com/S2Yd6t1.png)
##### 2. `File -> Add video` click. You can select multiple video files.
![](https://i.imgur.com/LyccHxb.png)
##### 3. Select image extension in Extract menu.
![](https://i.imgur.com/qHL40wu.png)

![](https://i.imgur.com/xJC38Vl.png)


#### Version Info
* 0.1.0
	* test.
	![](http://i.imgur.com/ATijqm6.png)
* 1.0.0
	* Support multi-core.
	* Fix minor bug.
* 1.1
	* Fix openmp error