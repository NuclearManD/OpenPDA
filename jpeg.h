#include <HTTPClient.h>
#include <JPEGDecoder.h>

extern Adafruit_HX8357 tft;
extern long idleTime;

void screenIdleHandler();
void println(String s);
void print(String s);
TSPoint getPoint();

#define SCALE 2

void drawBMP(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h){
  w=w/SCALE;
  h=h/SCALE;
  for(int i=0;i<w;i++){
    for(int j=0;j<h;j++){
      tft.drawPixel(x+i,y+j,bitmap[(i*SCALE) + w*j*SCALE]);
    }
  }
}

boolean displayImage(uint8_t* arr, uint32_t len, int xpos, int ypos){
  if(!JpegDec.decodeArray(arr, len))return false;
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = min(mcu_w, max_x % mcu_w);
  uint32_t min_h = min(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      for (int h = 1; h < win_h-1; h++)
      {
        memcpy(pImg + h * win_w, pImg + (h + 1) * mcu_w, win_w << 1);
      }
    }


    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
    {
      tft.drawRGBBitmap(mcu_x, mcu_y, pImg, win_w, win_h);
      //drawBMP(mcu_x/SCALE, mcu_y/SCALE, pImg, win_w, win_h);
    }

    // Stop drawing blocks if the bottom of the screen has been reached,
    // the abort function will close the file
    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();

  }
  return true;
}

#define MAX_IMG_SIZE 38*1024
uint8_t img_data[MAX_IMG_SIZE];

void take_pic_subprogram(){
  HTTPClient http;
  http.begin("http://192.168.4.1/camera");
  if (http.GET()!=200) {
    println("Error: cannot take picture.");
    return;
  }
  uint32_t len = http.getSize();
  WiFiClient * stream = http.getStreamPtr();
  if(len!=-1){
    for(int i=0;i<len;i++){
      while(!stream->available()){
        if(!http.connected())return;
      }
      img_data[i] = stream->read();
    }
  }else{
    len = 0;
    while(http.connected()){
      while(!stream->available()){
        if(!http.connected())break;
      }
      if(!http.connected())break;
      img_data[len] = stream->read();
      len++;
      if(len>=MAX_IMG_SIZE){
        break;
      }
    }
  }
  println(String("Length = ")+len);
  if(!displayImage(img_data,len,0,0))println("Error displaying image.");
}
