#include <sstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <conio.h>
#include <Windows.h>
#include <stdio.h>

using namespace cv;

//Verwendung von morphologischen Operation ein- oder ausschalten (true/false)
int MorphOpsVerwenden = 1;

//Verwendung von Hough Circles ein- oder ausschalten (true/false)
int houghCirclesVerwenden = 0;

//Objektmarkierung ein- oder ausschalten (true/false)
int trackObjects = 1;

//Rechteck-Eingrenzung ein- oder ausschalten (true/false)
int rechteckVerwenden = 0;


//Namen der Fenster
const string windowName = "Webcam-Stream";
const string windowName1 = "HSV-Farbraum";
const string windowName2 = "Grenzwert";
const string trackbarWindowName = "Schieberegler";
const string windowName3 = "Hough Circles -	Blur";

//Anfangswerte (min/max) der HSV-Filterung
//Diese werden mit Schiebereglern geändert, um dann eine bestimme Farbe im Grenzwert-Fenster zu filtern
//H = Hue (Farbton)
//S = Saturation (Sättigung)
//V = Value (Farbwert)
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//Standard-Fenstergrößen
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

//Maximale Anzahl an Objekten die erkannt werden können
const int MAX_NUM_OBJECTS=50;

//Minimale und maximale Fläche der erkannten Objekte
const int MIN_OBJECT_AREA = 10*10;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;

//Funktion, die aufgerufen wird, wenn die Position der Schieberegler geändert wird
void on_trackbar(int,void*){}

//Erstellt Fenster mit Schiebereglern
void createTrackbars(){
	
	namedWindow(trackbarWindowName,0);

	//Speicher für Schieberegler-Fenster
	char TrackbarName[70];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);

	sprintf( TrackbarName, "box1", MorphOpsVerwenden);
	sprintf( TrackbarName, "box2", trackObjects);
	sprintf( TrackbarName, "box4", rechteckVerwenden);
	sprintf( TrackbarName, "box3", houghCirclesVerwenden);
	

	//Die drei Parameter sind:
	// #1. die Adresse der Variablen, die sich ändert, wenn der Schieberegler bewegt wird
	// #2. der maximale Wert der jeweiligen Schieberegler
	// #3. die Funktion, die aufgerufen wird, wenn der Regler bewegt wird
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
    createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
    createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
    createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
    createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
    createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);

	//Ein- und Ausschalten der Funktionen über Regler
	createTrackbar("MorphOps", trackbarWindowName, &MorphOpsVerwenden, MorphOpsVerwenden, on_trackbar);
	createTrackbar("Tracking", trackbarWindowName, &trackObjects, trackObjects, on_trackbar);
	createTrackbar("Rechteck", trackbarWindowName, &rechteckVerwenden, rechteckVerwenden, on_trackbar);
	createTrackbar("Kreis", trackbarWindowName, &houghCirclesVerwenden, houghCirclesVerwenden, on_trackbar);
	

	}

string intZuStringUmwandlung(int zahl){

	//Wandelt int-Werte in string-Werte um
	//(wird benötigt, um die X- und Y-Koordinaten als Text neben dem Fadenkreuz und den Kreisdurchmesser auszugeben)
	std::stringstream ss;
	ss << zahl;
	return ss.str();
}

void fadenkreuz(int x, int y,Mat &webcam){

	//Der Mittelpunkt des getrackten Objekts wird mit X- und Y- Koordinaten übergeben
	//Von diesem Punkt aus werden Linien in vier Richtungen gezeichnet (oben, unten, links, rechts)
	
	//Da es zu Fehlern kam, wenn der Mittelpunkt so nah am Rand war, sodass die Linie nicht mehr ganz gezeichnet werden konnte,
	//wurden if/else Anweisung für die Randbereiche verwendet.

    if(y-25>0)
    line(webcam,Point(x,y),Point(x,y-25),Scalar(0,0,255),2);
    else line(webcam,Point(x,y),Point(x,0),Scalar(0,0,255),2);

    if(y+25<FRAME_HEIGHT)
    line(webcam,Point(x,y),Point(x,y+25),Scalar(0,0,255),2);
    else line(webcam,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,0,255),2);

    if(x-25>0)
    line(webcam,Point(x,y),Point(x-25,y),Scalar(0,0,255),2);
    else line(webcam,Point(x,y),Point(0,y),Scalar(0,0,255),2);

    if(x+25<FRAME_WIDTH)
    line(webcam,Point(x,y),Point(x+25,y),Scalar(0,0,255),2);
    else line(webcam,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,0,255),2);

	putText(webcam,intZuStringUmwandlung(x)+","+intZuStringUmwandlung(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

	
}

void arduinoSerial(int x, int y)
{
	char x_wert[4];
	sprintf(x_wert,"%d",x);
	x_wert[3] = 42;

	char y_wert[4];
	sprintf(y_wert,"%d",y);
	y_wert[3] = 43;
 
    // Deklariert Variablen und Strukturen
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
         
    // Einstellen des COM-Ports
    hSerial = CreateFileA("\\\\.\\COM4", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    // 38400 baud, 1 Start bit, 1 Stop bit, No Parity
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_38400;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);
 
    // Setzt COM-Port timeout-Einstellungen
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);
 
    // Sendet die X- und Y-Koordinate an das Arduino Board
	WriteFile(hSerial, x_wert, 4, NULL, NULL);
	WriteFile(hSerial, y_wert, 4, NULL, NULL);

    CloseHandle(hSerial);
}


void trackObject(int &x, int &y, Mat grenzwert, Mat &webcam){

	//Funktion zur Erkennung und Markierung des gefilterten Objekts

	Mat temp;
	grenzwert.copyTo(temp);

	//Vektoren, die für die findContours-Funktion benötigt werden
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Findet den Umriss des gefilterten Bildes mit der OpenCV-Funktion "findContours"
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );

	//Initialisierung der lokalen Variablen
	double refArea = 0;
	bool objectFound = false;

	//If-Anweisung wird nur ausgeführt wenn mindestens ein Umriss gefunden wurde
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();

        //Wenn die Anzahl an Objekten den vordefinierten Grenzwert (MAX_NUM_OBJECTS) überschreitet, wird die Tracking-Routine übersprungen
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				//OpenCV-Funktion "moments" wird genutzt, um den Schwerpunkt (X-Y-Koordinate) der gefilterten Form zu finden
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//Wenn die Fläche kleiner als 20x20 ist, wird angenommen, dass es sich nur um Bildrauschen handelt
				//Bei einer zu großen Fläche wurde wahrscheinlich noch keine Filterung angepasst
				//Das Objekt mit der größten Fläche wird bei jedem Durchlauf der Schleife als Referenz gespeichert und mit der nächsten Schleife verglichen
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					
					x = moment.m10/area;
					y = moment.m01/area;

					objectFound = true;

					refArea = area;

					}

				else objectFound = false;

			}

			//Zeigt dem Nutzer an, ob ein Objekt erkannt wurde
			if(objectFound == true){
				putText(webcam,"Objekt erkannt",Point(5,25),2,1,Scalar(0,255,0),2);
				
				//Funktion zum Malen des Fadenkreuzes wird aufgerufen
				fadenkreuz(x,y,webcam);
				
				arduinoSerial(x,y);
			}

		}
		
		//Weist den Nutzer darauf hin, dass die Filterung angepasst werden muss
		else putText(webcam,"Zu viel Bildrauschen! Filter anpassen.",Point(2,50),1,2,Scalar(0,0,255),2);
	}
}


void morphOps(Mat &thresh){

	//Erstellt Rechtecke (Größe: 3x3 Pixel) mit denen das Bild erodiert wird
	//Erosion bedeutet, dass zu kleine Bildpunkte im gefilterten Bild nicht mehr dargestellt werden
	Mat erodeElement = getStructuringElement(MORPH_RECT,Size(3,3));

    //Erstellt Rechtecke (Größe: 8x8 Pixel) mit denen das Bild dilatiert wird
	//Dilatation bedeutet, dass die Bildpunkte, die die Erosion "Überstanden" haben, nun vergrößert werden
	Mat dilateElement = getStructuringElement(MORPH_RECT,Size(8,8));


	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
	
	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
	
}

void houghCircles(Mat grenzwert,Mat &webcam){
		
	//Hough Circles Transformation

		//Erstellt temporäre Matrix
		Mat temp;
		grenzwert.copyTo(temp);
		
		//Vermindert Rauschen (Gaußscher Weichzeichner) und verrundet die Kanten	
		GaussianBlur(temp, temp, Size(25, 25), 15, 15);
			
		//Vektor für die Kreise
		 vector<Vec3f> circles;

		 //Führt Hough Transformation Circles aus
		 //Der drittletzte Parameter stellt die Sensibilität ein. (Hier: 25. Bei 100 werden nur perfekte Kreise markiert)
			HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 1, temp.rows/8, 100, 25, 10, 200);

		  //Zeichnet die erkannten Kreise in den Webcam-Stream
		  for( size_t i = 0; i < circles.size(); i++ )
		 {
		   Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		   int radius = cvRound(circles[i][2]);
		   // Kreismitte
		   circle(webcam, center, 3, Scalar(0,0,0), -1, 8, 0 );
		   // Kreisumfang
		   circle(webcam, center, radius, Scalar(0,0,255), 3, 8, 0 ); 
		   putText(webcam,"Durchmesser: "+intZuStringUmwandlung(2*radius),Point(10,FRAME_HEIGHT-15),1,1,Scalar(0,255,0),2);
		  }

		 // imshow(windowName3,temp);  //Test des GaussianBlur

		  
}

void rechteckUmriss(Mat grenzwert, Mat &webcam){

	//Erstellt temporäre Matrix
	Mat temp2;
	grenzwert.copyTo(temp2);

	//Vektoren für Umrisse
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

    // Findet Umrisse mit "findContours"
  findContours(temp2,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );

  // Approximiert Umrisse in Polygone
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	 }

    //Zeichnet die Rechteck-Eingrenzung in der Farbe Rot
    for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( 0, 0, 255 );
	  rectangle( webcam, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
      }

}


int main() {

	//Matrix zum Speichern des Webcam-Streams
	Mat webcam;

	//Matrix für das auf den HSV-Farbraum geänderter Webcam-Bild
	Mat HSV;

	//Matrix des Grenzwert-Fensters
	Mat grenzwert;

	//X und Y Werte des getrackten Objekts
	int x=0, y=0;

	//Aufnahme initialisieren / Kamera 0 ist die eingebaute Kamera im Laptop / Kamera 1 wäre eine extern angeschlossene Webcam
	VideoCapture cap;
	cap.open(1);

	//Setze Standardgröße für die Frames
	 cap.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	 cap.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	 //Warten bis Kamera initialisiert ist / (kann sonst zu Fehler bei Ausführung der Schleife führen)
	waitKey(1000);

	//Startet Schleife des Programms mit Tastendruck
	printf("\n ############################################### \n"); 
	printf("\n Beliebige Taste druecken, um Programm zu starten... \n");
	printf("\n ############################################### \n");
	getch();

	//Funktion zur Erstellung der Schieberegler wird aufgerufen
	createTrackbars();

	

	//Schleife in der das Webcam-Bild von RGB zu HSV umgewandelt wird und die Funktion zur Objekterkennun aufgerufen werden
	while(1){
		//Speichert Webcam-Bild in der Matrix
		cap.read(webcam);

		//wandelt Frame von RGB in den HSV-Farbraum
		cvtColor(webcam,HSV,COLOR_BGR2HSV);
		
		//Filterung des HSV-Bildes mit den Werten der Schieberegler
		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),grenzwert);

		//Führt morphologische Operation auf dem Grenzwert-Bild aus
		//Vermindert Rauschen
		if(MorphOpsVerwenden)
			morphOps(grenzwert);

		//Sucht im Bild nach Kreisen und markiert diese
		if(houghCirclesVerwenden)
			houghCircles(grenzwert,webcam);

		//Trackt und markiert das Objekt
		if(trackObjects)
			trackObject(x,y,grenzwert,webcam);

		//Zeichnet ein Rechteck, dass das getrackte Objekt eingrenzt
		if(rechteckVerwenden)
		rechteckUmriss(grenzwert,webcam);


		//Anzeige der Fenster
		imshow(windowName,webcam);
		imshow(windowName1,HSV);
		imshow(windowName2,grenzwert);
		
		
		//20ms Verzögerung um Bild neu zu schreiben
		waitKey(20);
	}

	return 0;
}