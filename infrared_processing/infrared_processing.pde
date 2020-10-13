// by Andrés Botero
// modified from Roni Kaufman

import processing.serial.*;

float  kMax;
float step;
int n = 80; // number of blobs
float radius = 0; // diameter of the circle
float inter = 0.1; // difference between the sizes of two blobs
float maxNoise = 300;

Serial myPort;
boolean redActive = true;
String redKey = "001A";
boolean greenActive = true;
String greenKey = "009A";
boolean blueActive = true;
String blueKey = "00A2";

float noiseProg(float x) {return x;}

void setup() {
	size(600, 600);
	//colorMode(HSB, 1);
	//angleMode(DEGREES);
	noFill();
	//noLoop();
	kMax = random(0.6, 1.0);
	step = 0.01;
	noStroke();
	frameRate(30);
	myPort = new Serial(this, Serial.list()[1], 9600);
}


void draw() {
	String data = null;
	while (myPort.available() > 0) {
		data = myPort.readStringUntil('\n');
	}
	if (data != null) {
    String d = trim(data);//data.substring(4);
    println(d);
    
		if (d.equals(redKey))   redActive = !redActive;
		if (d.equals(greenKey)) greenActive = !greenActive;
		if (d.equals(blueKey))  blueActive = !blueActive;
     
	}
  
	blendMode(BLEND);
	background(0);
	blendMode(ADD);
	float t = (float)frameCount/100;
	for (int i = n; i > 0; i--) {
		float alpha = pow(1 - noiseProg(i / n), 3);
		float size = radius + i * inter;
		float k = kMax * sqrt((float)i/n);
		float noisiness = maxNoise * noiseProg((float)i / n);
    
		if (redActive) {
			fill(255, 0, 0, alpha*255);
			blob(size, width/2, height/2, k, t - i * step, noisiness);
		}

		if (greenActive) {
			fill(0, 255, 0, alpha*255);
			blob(size, width/2, height/2, k, t - i * step + 1, noisiness);
		}
		if (blueActive) {
			fill(0, 0, 255, alpha*255);
			blob(size, width/2, height/2, k, t - i * step + 2, noisiness);
		}
	}
}

void blob(float size, float xCenter, float yCenter, float k, float t, float noisiness) {
	beginShape();
	float angleStep = 360 / 12;
	for (float theta = 0; theta <= 360 + 2 * angleStep; theta += angleStep) {
		float r1, r2;
		/*
		  if (theta < PI / 2) {
		  r1 = cos(theta);
		  r2 = 1;
		  } else if (theta < PI) {
		  r1 = 0;
		  r2 = sin(theta);
		  } else if (theta < 3 * PI / 2) {
		  r1 = sin(theta);
		  r2 = 0;
		  } else {
		  r1 = 1;
		  r2 = cos(theta);
		  }
		*/
		r1 = cos(radians(theta))+1;
		r2 = sin(radians(theta))+1;
		float r = size + noise(k * r1,  k * r2, t) * noisiness;
		float x = xCenter + r * cos(radians(theta));
		float y = yCenter + r * sin(radians(theta));
		curveVertex(x, y);
	}
	endShape();
}
