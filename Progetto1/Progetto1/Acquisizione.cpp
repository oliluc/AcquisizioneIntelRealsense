#include "Acquisizione.h"

#include <iostream>
#include <fstream>
#include <string>
Acquisizione::Acquisizione()
{

}

void saveCloudPoints(const char* name, int width, int height, PXCPoint3DF32 *arrayVertices, pxcBYTE *cpixels){

	std::ofstream myfile;
	myfile.open(name);
	float x = 0, y = 0, z = 0;
	unsigned int bvalueUchar, gvalueUchar, rvalueUchar;

	for (int i = 0; i < width * height; i++)
	{
		if (cpixels != NULL){
			//BGRA
			//Read the first 3 bytes of each 4 bytes, and ignore the last one
			pxcBYTE bvalue = cpixels[(i)* 3 + 0];
			pxcBYTE gvalue = cpixels[(i)* 3 + 1];
			pxcBYTE rvalue = cpixels[(i)* 3 + 2];
			bvalueUchar = static_cast<unsigned int>(bvalue);
			gvalueUchar = static_cast<unsigned int>(gvalue);
			rvalueUchar = static_cast<unsigned int>(rvalue);
		}
		


		x = arrayVertices[i].x;
		y = arrayVertices[i].y;
		z = arrayVertices[i].z;
		myfile << ("%f", x);
		myfile << " ";
		myfile << ("%f", y);
		myfile << " ";
		myfile << ("%f", -z);
		
		if (cpixels != NULL){
			myfile << " ";
			myfile << ("%f", rvalueUchar);
			myfile << " ";
			myfile << ("%f", gvalueUchar);
			myfile << " ";
			myfile << ("%f", bvalueUchar);
		}
		
		myfile << "\n";


	}
	myfile.close();
}
void saveCloudPointsPCD(const char* name, int width, int height, PXCPoint3DF32 *arrayVertices, pxcBYTE *cpixels){

	std::ofstream myfile;
	myfile.open(name);
	float x = 0, y = 0, z = 0;
	unsigned int bvalueUchar, gvalueUchar, rvalueUchar;


	myfile << "# .PCD v.7 - Point Cloud Data file format\n";
	myfile << "VERSION .7\n";
	if (cpixels != NULL){
		myfile << "FIELDS x y z rgb\n";
		myfile << "SIZE 4 4 4 4\n";
		myfile << "TYPE F F F F\n";
		myfile << "COUNT 1 1 1 1\n";
	}else{
		myfile << "FIELDS x y z\n";
		myfile << "SIZE 4 4 4\n";
		myfile << "TYPE F F F\n";
		myfile << "COUNT 1 1 1\n";
	}

	myfile << "WIDTH ";
	myfile << ("%d", width);
	myfile << "\n";
	myfile << "HEIGHT ";
	myfile << ("%d", height);
	myfile << "\n";
	myfile << "VIEWPOINT 0 0 0 1 0 0 0\n";
	myfile << "POINTS ";
	myfile << ("%d", height*width);
	myfile << "\n";
	myfile << "DATA ascii\n";

	for (int i = 0; i < width * height; i++)
	{
		if (cpixels != NULL){
			//BGR
			pxcBYTE bvalue = cpixels[(i)* 3 + 0];// ho RGB24 quindi vado *3 se avessi RGB32 andrei avanti *4
			pxcBYTE gvalue = cpixels[(i)* 3 + 1];
			pxcBYTE rvalue = cpixels[(i)* 3 + 2];
			bvalueUchar = static_cast<unsigned int>(bvalue);
			gvalueUchar = static_cast<unsigned int>(gvalue);
			rvalueUchar = static_cast<unsigned int>(rvalue);

		}



		x = arrayVertices[i].x;
		y = arrayVertices[i].y;
		z = arrayVertices[i].z;
		myfile << ("%f", x);
		myfile << " ";
		myfile << ("%f", y);
		myfile << " ";
		myfile << ("%f", -z);

		if (cpixels != NULL){
			myfile << " "; 
			int rgb =0;			
			rgb = rvalueUchar << 16 | gvalueUchar << 8 | bvalueUchar;

			float frgb = *(reinterpret_cast<float *>(&rgb));
			myfile << ("%f",frgb);

		}

		myfile << "\n";


	}
	myfile.close();
}

/* Converte le PXCImage in formato Mat utilizzabile con le librerie Opencv*/
cv::Mat PXCImage2CVMat(PXCImage *pxcImage, PXCImage::PixelFormat format){
	PXCImage::ImageData data;
	pxcImage->AcquireAccess(PXCImage::ACCESS_READ, format, &data);
	int width = pxcImage->QueryInfo().width;
	int height = pxcImage->QueryInfo().height;

	if (!format)
		format = pxcImage->QueryInfo().format;

	int type;
	if (format == PXCImage::PIXEL_FORMAT_Y8)
		type = CV_8UC1;
	else if (format == PXCImage::PIXEL_FORMAT_RGB24)
		type = CV_8UC3;
	else if (format == PXCImage::PIXEL_FORMAT_DEPTH_F32)
		type = CV_32FC1;

	cv::Mat ocvImage = cv::Mat(cv::Size(width, height), type, data.planes[0]);
	pxcImage->ReleaseAccess(&data);

	return ocvImage;
}

int Acquisizione::acquisisci(bool Color, bool Depth, bool IR, bool CPtxt, bool CPpcd, bool CPRGBtxt, bool CPRGBpcd, bool outputrssdk){
	if (!(Color == false && Depth == false && IR == false && CPtxt == false && CPpcd == false && CPRGBtxt == false && CPRGBpcd == false && outputrssdk == false)){

		cv::Size frameSize = cv::Size(640, 480);
		float frameRate = 60;

		cv::Mat frameIR;
		cv::Mat frameColor;
		cv::Mat frameDepth;
		PXCSenseManager *pxcSenseManager = PXCSenseManager::CreateInstance();

		if (IR || outputrssdk){
			cv::namedWindow("IR", cv::WINDOW_NORMAL);
			cv::Mat frameIR = cv::Mat::zeros(frameSize, CV_8UC1);
			pxcSenseManager->EnableStream(PXCCapture::STREAM_TYPE_IR, frameSize.width, frameSize.height, frameRate);
		}
		if (Color || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk){
			cv::namedWindow("Color", cv::WINDOW_NORMAL);
			cv::Mat frameColor = cv::Mat::zeros(frameSize, CV_8UC3);
			pxcSenseManager->EnableStream(PXCCapture::STREAM_TYPE_COLOR, frameSize.width, frameSize.height, frameRate);
		}
		if (Depth || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk){
			cv::namedWindow("Depth", cv::WINDOW_NORMAL);
			cv::Mat frameDepth = cv::Mat::zeros(frameSize, CV_8UC1);
			pxcSenseManager->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, frameSize.width, frameSize.height, frameRate);
		}

		if (outputrssdk){
			// save streams to output video	
			pxcCHAR *fileName = L"output.rssdk";
			pxcSenseManager->QueryCaptureManager()->SetFileName(fileName, true);
		}

	

		pxcSenseManager->Init();

		
		int countSavedIR = 0;
		int countSavedColor = 0;
		int countSavedDepth = 0;
		int countSavedCPtxt = 0;
		int countSavedCPpcd = 0;
		int countSavedCPRGBtxt = 0;
		int countSavedCPRGBpcd = 0;



		PXCImage  *depthColorIm = NULL;
		PXCProjection * projection = pxcSenseManager->QueryCaptureManager()->QueryDevice()->CreateProjection();
		PXCPoint3DF32 *arrayVertices = new PXCPoint3DF32[frameSize.width*frameSize.height];

		bool keepRunning = true;

		while (keepRunning){

			pxcSenseManager->AcquireFrame();
			PXCCapture::Sample *sample = pxcSenseManager->QuerySample();


			if (IR || outputrssdk)
				frameIR = PXCImage2CVMat(sample->ir, PXCImage::PIXEL_FORMAT_Y8);
			if (Color || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk)
				frameColor = PXCImage2CVMat(sample->color, PXCImage::PIXEL_FORMAT_RGB24);
			if (Depth || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk)
				PXCImage2CVMat(sample->depth, PXCImage::PIXEL_FORMAT_DEPTH_F32).convertTo(frameDepth, CV_8UC1);


			if (IR || outputrssdk){
				cv::imshow("IR", frameIR);
			}
			if (Color || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk){
				cv::imshow("Color", frameColor);
			}
			if (Depth || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk){
				cv::imshow("Depth", frameDepth);
			}
			
			int key = cv::waitKey(1);
			if (key == 32){ // spacebar pressed

			if (IR){
				std::stringstream name;
				name << "IR_" << countSavedIR << ".png";
				const std::string tmp = name.str();
				const char* cstr = tmp.c_str();

				cv::imwrite(cstr, frameIR);
				countSavedIR++;
				}
		
				if (Color){
					std::stringstream name;
					name << "Color_" << countSavedColor << ".png";
					const std::string tmp = name.str();
					const char* cstr = tmp.c_str();

					cv::imwrite(cstr, frameColor);
					countSavedColor++;
				}

				if (Depth){
					std::stringstream name;
					name << "Depth_" << countSavedDepth << ".png";
					const std::string tmp = name.str();
					const char* cstr = tmp.c_str();
					cv::imwrite(cstr, frameDepth);
					countSavedDepth++;
				}

				if (CPtxt){
					std::stringstream name;
					name << "CPtxt_" << countSavedCPtxt << ".txt";
					const std::string tmp = name.str();
					const char* cstr = tmp.c_str();
				
					projection->QueryVertices(sample->depth, (PXCPoint3DF32 *)arrayVertices);
					saveCloudPoints(cstr, frameSize.width, frameSize.height, arrayVertices, NULL);

					countSavedCPtxt++;

				}
				if (CPpcd){
					std::stringstream name;
					name << "CPpcd_" << countSavedCPtxt << ".pcd";
					const std::string tmp = name.str();
					const char* cstr = tmp.c_str();

					projection->QueryVertices(sample->depth, (PXCPoint3DF32 *)arrayVertices);
					saveCloudPointsPCD(cstr, frameSize.width, frameSize.height, arrayVertices, NULL);

					countSavedCPpcd++;

				}
				if (CPRGBtxt){
					std::stringstream name;
					name << "CPRGBtxt_" << countSavedCPRGBtxt << ".txt";
					const std::string tmp = name.str();
					const char* cstr = tmp.c_str();


					depthColorIm = projection->CreateColorImageMappedToDepth( sample->depth,sample->color);

					PXCImage::ImageData * data = new PXCImage::ImageData;
					depthColorIm->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, data);

					//Now we just need to paint the canvas with each pixel from the image data
					projection->QueryVertices(sample->depth, (PXCPoint3DF32 *)arrayVertices); //data un immagine depth ne calcola i vertici e li inserisce in un array passato come parametro

					pxcBYTE *cpixels ;

				
					saveCloudPoints(cstr, frameSize.width, frameSize.height, arrayVertices, data->planes[0]);

					depthColorIm->ReleaseAccess(data);
					delete data;

					countSavedCPRGBtxt++;

				}
					
				if (CPRGBpcd){
					std::stringstream name;
					name << "CPRGBpcd_" << countSavedCPRGBtxt << ".pcd";
					const std::string tmp = name.str();
					const char* cstr = tmp.c_str();


					depthColorIm = projection->CreateColorImageMappedToDepth(sample->depth, sample->color);

					PXCImage::ImageData * data = new PXCImage::ImageData;
					depthColorIm->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, data);

					//Now we just need to paint the canvas with each pixel from the image data
					projection->QueryVertices(sample->depth, (PXCPoint3DF32 *)arrayVertices); //data un immagine depth ne calcola i vertici e li inserisce in un array passato come parametro

					pxcBYTE *cpixels;


					saveCloudPointsPCD(cstr, frameSize.width, frameSize.height, arrayVertices, data->planes[0]);

					depthColorIm->ReleaseAccess(data);
					delete data;

					countSavedCPRGBpcd++;
				}
			

			}
			
			pxcSenseManager->ReleaseFrame();

			if (key == 27) { // Esc pressed
				if (IR || outputrssdk){
					cv::destroyWindow("IR");
				}
				if (Color || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk){
					cv::destroyWindow("Color");
				}
				if (Depth || CPtxt || CPpcd || CPRGBtxt || CPRGBpcd || outputrssdk){
					cv::destroyWindow("Depth");
				}


				keepRunning = false;
			
			}
		}

		pxcSenseManager->Release();
	}
	return 0;
}
