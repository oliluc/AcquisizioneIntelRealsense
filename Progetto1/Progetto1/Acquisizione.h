#include "pxcsensemanager.h"
#include "opencv2/highgui/highgui.hpp"
#include "pxcprojection.h"
#pragma once

ref class Acquisizione
{
public:
	Acquisizione();


	static int acquisisci(bool Color, bool Depth, bool IR, bool CPtxt, bool CPpcd, bool CPRGBtxt, bool CPRGBpcd, bool outputrssdk);
};

