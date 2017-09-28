/*
*  version.hpp
*  FSpring
*
*  Created by kimbom on 2017. 9. 28...
*  Copyright 2017 kimbom. All rights reserved.
*
*/
#if !defined(FSPRING_7E1_9_1C_VERSION_HPP_INCLUDED)
#define FSPRING_7E1_9_1C_VERSION_HPP_INCLUDED
#if 0
#define FSPRING_VERSION <fspring-version>1.2</fspring-version>
#endif
#include<iostream>
#include<vector>
#include<string>
#include<Windows.h>
#include<WinInet.h>
#pragma comment(lib, "wininet.lib")
inline std::string GetHtml(std::string url) {
	std::string html;
	HINTERNET hInternet = InternetOpen(TEXT("HTTP"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);	//인터넷 관련 DLL을 초기화한다.
	if (hInternet) {
		HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);	//url에 걸린 파일을 연다.
		if (hUrl) {
			DWORD realSize = 0;
			DWORD possibleSize = 0;
			DWORD recv = 0;
			char* buffer = new char[2000000];
			char* tempBuffer = new char[2000000];
			memset(buffer, 0, 2000000 * sizeof(char));
			memset(tempBuffer, 0, 2000000 * sizeof(char));
			do {
				InternetQueryDataAvailable(hUrl, &possibleSize, 0, 0);
				InternetReadFile(hUrl, buffer, possibleSize, &realSize);
				if (possibleSize>0) {
					memcpy(tempBuffer + recv, buffer, possibleSize * sizeof(char));
				}
				recv += possibleSize;
			} while (realSize != 0);
			html.resize(realSize);
			html = tempBuffer;
			delete[] buffer;
			delete[] tempBuffer;
		}
	}
	return html;
}
std::string GetNewVersion() {
	std::string url_git = "https://github.com/springkim/ISpring/blob/master/prj_ISpring/ispring/version_info.txt";
	std::string html = GetHtml(url_git);
	std::string tag_begin = "&lt;ispring-version&gt;";
	std::string tag_end = "&lt;/ispring-version&gt;";
	size_t pos_beg = html.find(tag_begin) + tag_begin.length();
	size_t pos_end = html.find(tag_end);
	std::string version = html.substr(pos_beg, pos_end - pos_beg);
	if (version.length() == 0) {
		version = "Can't load new version information";
	}
	return version;
}
#endif  //FSPRING_7E1_9_1C_VERSION_HPP_INCLUDED