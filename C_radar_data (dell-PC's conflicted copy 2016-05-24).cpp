#define PI 3.141592654f
#include "C_radar_data.h"
#include <math.h>
#include <QtDebug>

//static period_t                curPeriod;
//static std::queue<period_t>    period_cache;
//static unsigned short cur_mark_index = 0;
//static unsigned short cur_object_index = 0;

C_radar_data::C_radar_data()
{
    img_ppi = new QImage(RAD_M_PULSE_RES*2+1,RAD_M_PULSE_RES*2+1,QImage::Format_ARGB32);
    img_alpha = new QImage(RAD_M_PULSE_RES,256,QImage::Format_Mono);
    size_thresh = 4;
    isProcessing = true;
    imgMode = DRAW_NO_TERR;
    xl_nguong = false;
    clk_adc = 0;
    isClkAdcChanged = true;
    setTrueN(0);
    setViewScale(1);
    resetData();
}
C_radar_data::~C_radar_data()
{
    delete img_ppi;
    //if(pFile)fclose(pFile);
//    if(pScr_map)
//    {
//        delete[] pScr_map;
//    }
}
void C_radar_data::setProcessing(bool onOff)
{
    if(onOff)
    {

        //initData(true);
        isProcessing = true;
        printf("\nSecondary processing mode - on.");
    }
    else
    {
        isProcessing = false;
        printf("\nSecondary processing mode - off.");
    }
}
void C_radar_data::drawVet(short azi, short r_pos)
{
    /*if(!signal_map.frame[azi].raw_map[r_pos].vet)return;
    signal_map.frame[azi].raw_map[r_pos].vet -= 15;//vet*0.75
    if(signal_map.frame[azi].raw_map[r_pos].vet < 50)
    {
        signal_map.frame[azi].raw_map[r_pos].vet = 0;
    }

    switch(imgMode)
    {
    case DRAW_NO_TERR:
    {
        signal_map.frame[azi].raw_map[r_pos].vet = 0;
        uint color = 0;//0x00004060|((signal_map.frame[azi].raw_map[r_pos].vet)<<24);
        sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x,signal_map.frame[azi].raw_map[r_pos].y,color);
        if(spreading)
        {
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x+1,signal_map.frame[azi].raw_map[r_pos].y,color);
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x-1,signal_map.frame[azi].raw_map[r_pos].y,color);
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x,signal_map.frame[azi].raw_map[r_pos].y+1,color);
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x,signal_map.frame[azi].raw_map[r_pos].y-1,color);
        }
        break;
    }
    case DRAW_NULL:
        break;
    case DRAW_SNG_N_VET:
    {

        uint color = 0x00004060|((signal_map.frame[azi].raw_map[r_pos].vet)<<24);
        sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x,signal_map.frame[azi].raw_map[r_pos].y,color);
        if(spreading)
        {
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x+1,signal_map.frame[azi].raw_map[r_pos].y,color);
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x-1,signal_map.frame[azi].raw_map[r_pos].y,color);
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x,signal_map.frame[azi].raw_map[r_pos].y+1,color);
            sgn_img->setPixel(signal_map.frame[azi].raw_map[r_pos].x,signal_map.frame[azi].raw_map[r_pos].y-1,color);
        }

    }
        break;
    }

*/
}
void C_radar_data::drawSgn(short azi, short r_pos,unsigned char value,unsigned char dopler)
{
    //printf("value:%d",value);
    short px = signal_map.x[azi][r_pos];
    short py = signal_map.y[azi][r_pos];
    if(px<0||py<0)return;
    /*if(alphaMode == true)*/
    //else img_alpha->setPixel(r_pos,signal_map.frame[azi].raw_map[255-r_pos].dopler,1);
    short pSize = (short)viewScale+1;
    if(pSize>4)pSize=4;
    if(pSize<1)pSize=1;
    if((px<pSize)||(py<pSize)||(px>=img_ppi->width()-pSize)||(py>=img_ppi->height()-pSize))return;
    switch(imgMode)
    {
    case DRAW_NO_TERR:
            if(value)
            {
                uint color;

                for(short x = -pSize;x <= pSize;x++)
                {
                    for(short y = -pSize;y <= pSize;y++)
                    {
                        float k ;
                        switch(short(x*x+y*y)*2/(pSize*pSize))
                        {

                        case 0:
                            k=1;
                            break;
                        case 1:
                            k=0.9f;
                            break;
                        case 2:
                            k=0.85f;
                            break;
                        case 3:
                            k=0.7f;
                            break;
                        case 4:
                            k=0.4f;
                            break;
                        case 5:
                            k=0.1f;
                            break;
                        default:
                            k=0;
                            break;
                        }
                        //k=1;

                        if(isDisplaydopler)
                        {
                            color = (256-((dopler*16))<<16)+((dopler*16)<<8);
                        }
                        else color = 0x00ffff00;//
                        //printf("k:%f",k);
                        //color = 0xff00ff00;
                        unsigned char pvalue = unsigned char((float)value*k);
                        color = color|(pvalue<<24);
                        if((img_ppi->pixel(px+x,py+y)>>24)<pvalue)
                        img_ppi->setPixel(px+x,py+y,color);
                    }
                }
            }
            /*if(spreading>3)
            {
                color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px,py)<color)sgn_img->setPixel(px,py,color);

                if(sgn_img->pixel(px+1,py)<color)sgn_img->setPixel(px+1,py,color);
                if(sgn_img->pixel(px-1,py)<color)sgn_img->setPixel(px-1,py,color);
                if(sgn_img->pixel(px,py-1)<color)sgn_img->setPixel(px,py-1,color);
                if(sgn_img->pixel(px,py+1)<color)sgn_img->setPixel(px,py+1,color);

                value/=1.1f;color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px+1,py+1)<color)sgn_img->setPixel(px+1,py+1,color);
                if(sgn_img->pixel(px-1,py-1)<color)sgn_img->setPixel(px-1,py-1,color);
                if(sgn_img->pixel(px-1,py+1)<color)sgn_img->setPixel(px-1,py+1,color);
                if(sgn_img->pixel(px+1,py-1)<color)sgn_img->setPixel(px+1,py-1,color);

                value/=1.3f;color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px+2,py)<color)sgn_img->setPixel(px+2,py,color);
                if(sgn_img->pixel(px-2,py)<color)sgn_img->setPixel(px-2,py,color);
                if(sgn_img->pixel(px,py-2)<color)sgn_img->setPixel(px,py-2,color);
                if(sgn_img->pixel(px,py+2)<color)sgn_img->setPixel(px,py+2,color);

                if(sgn_img->pixel(px-2,py+1)<color)sgn_img->setPixel(px-2,py,color);
                if(sgn_img->pixel(px+2,py+1)<color)sgn_img->setPixel(px+2,py,color);
                if(sgn_img->pixel(px,py-2)<color)sgn_img->setPixel(px,py-2,color);
                if(sgn_img->pixel(px,py+2)<color)sgn_img->setPixel(px,py+2,color);

            }
            else if(spreading>2)
            {
                color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px,py)<color)sgn_img->setPixel(px,py,color);

                if(sgn_img->pixel(px+1,py)<color)sgn_img->setPixel(px+1,py,color);
                if(sgn_img->pixel(px-1,py)<color)sgn_img->setPixel(px-1,py,color);
                if(sgn_img->pixel(px,py-1)<color)sgn_img->setPixel(px,py-1,color);
                if(sgn_img->pixel(px,py+1)<color)sgn_img->setPixel(px,py+1,color);

                value/=1.3f;color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px+1,py+1)<color)sgn_img->setPixel(px+1,py+1,color);
                if(sgn_img->pixel(px-1,py-1)<color)sgn_img->setPixel(px-1,py-1,color);
                if(sgn_img->pixel(px-1,py+1)<color)sgn_img->setPixel(px-1,py+1,color);
                if(sgn_img->pixel(px+1,py-1)<color)sgn_img->setPixel(px+1,py-1,color);

                value/=2;color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px+2,py)<color)sgn_img->setPixel(px+2,py,color);
                if(sgn_img->pixel(px-2,py)<color)sgn_img->setPixel(px-2,py,color);
                if(sgn_img->pixel(px,py-2)<color)sgn_img->setPixel(px,py-2,color);
                if(sgn_img->pixel(px,py+2)<color)sgn_img->setPixel(px,py+2,color);

            }
            else if(spreading >1)
            {
                color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px,py)<color)sgn_img->setPixel(px,py,color);
                value/=1.3;color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px+1,py)<color)sgn_img->setPixel(px+1,py,color);
                if(sgn_img->pixel(px-1,py)<color)sgn_img->setPixel(px-1,py,color);
                if(sgn_img->pixel(px,py-1)<color)sgn_img->setPixel(px,py-1,color);
                if(sgn_img->pixel(px,py+1)<color)sgn_img->setPixel(px,py+1,color);
                value/=1.7;color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px+1,py+1)<color)sgn_img->setPixel(px+1,py+1,color);
                if(sgn_img->pixel(px-1,py-1)<color)sgn_img->setPixel(px-1,py-1,color);
                if(sgn_img->pixel(px-1,py+1)<color)sgn_img->setPixel(px-1,py+1,color);
                if(sgn_img->pixel(px+1,py-1)<color)sgn_img->setPixel(px+1,py-1,color);
            }
            else
            {
                color = ((0xff000000))|(value<<16)|(value<<8);
                if(sgn_img->pixel(px,py)<color)sgn_img->setPixel(px,py,color);
            }*/


        break;
    case DRAW_NULL:
        break;
    case DRAW_SNG_N_VET:
    {/*
        unsigned char value;
        if(signal_map.frame[azi].raw_map[r_pos].displaylevel*brightness>0xff)
        {
            value  = 0xff;
        }
        else
        {
            value  = signal_map.frame[azi].raw_map[r_pos].displaylevel*brightness;
        }
        uint color = 0x0000ff00|((value<<16))|((((0xff-value)>>1)+value)<<24);
        img_ppi->setPixel(px,py,color);
        if(spreading)
        {
            img_ppi->setPixel(px+1,py,color);
            img_ppi->setPixel(px-1,py,color);
            img_ppi->setPixel(px,py+1,color);
            img_ppi->setPixel(px,py-1,color);
        }*/
    }
        break;
    }

}
void C_radar_data::blackLine(short x0, short y0, short x1, short y1)
{

    short dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    short dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    short err = (dx>dy ? dx : -dy)/2, e2;
    unsigned short len=0;
    for(;;){
        len++;
        if((x0<1)||(y0<1)||(x0>=img_ppi->width())||(y0>=img_ppi->height()))break;
      img_ppi->setPixel(x0,y0,0);

      //sgn_img->setPixel(x0-1,y0-1,0xff000000);
      if(len>=30)
      {
          img_ppi->setPixel(x0-1,y0,0);
          img_ppi->setPixel(x0,y0-1,0);
      }
      //sgn_img->setPixel(x0,y0,0xff000000);
      if (x0==x1 && y0==y1) break;
      e2 = err;
      if (e2 >-dx) { err -= dy; x0 += sx; }
      if (e2 < dy) { err += dx; y0 += sy; }
    }
}
void C_radar_data::drawBlackAzi(short azi)
{
    for (short r_pos = 1;r_pos<range_max-1;r_pos++)
    {

        short px = signal_map.x[azi][r_pos];
        short py = signal_map.y[azi][r_pos];
        short pSize = (short)viewScale+1;
        if((px<pSize)||(py<pSize)||(px>=img_ppi->width()-pSize)||(py>=img_ppi->height()-pSize))return;

        for(short x = -pSize;x <= pSize;x++)
        {
            for(short y = -pSize;y <= pSize;y++)
            {

                img_ppi->setPixel(px+x,py+y,0);
            }
        }
    }

}
void C_radar_data::drawAzi(short azi)
{

    //drawingDone = false;
    img_alpha->fill(0);
    short prev_azi =azi + 20;
    if(prev_azi>=MAX_AZIR)prev_azi -= MAX_AZIR;
    drawBlackAzi(prev_azi);
    //blackLine(signal_map.frame[prev_azi].raw_map[1].x,signal_map.frame[prev_azi].raw_map[1].y,signal_map.frame[prev_azi].raw_map[RAD_M_PULSE_RES-1].x,signal_map.frame[prev_azi].raw_map[RAD_M_PULSE_RES-1].y);
    //blackLine(signal_map.x[prev_azi][1],signal_map.y[prev_azi][1],signal_map.x[prev_azi][RAD_M_PULSE_RES-1],signal_map.y[prev_azi][RAD_M_PULSE_RES-1]);

    for (short r_pos = 1;r_pos<range_max-1;r_pos++)
    {

            unsigned short value,dopler;

            if(isPulse&&(r_pos<200))
            {
                value = signal_map.level_s[azi][r_pos];
                dopler = signal_map.dopler_s[azi][r_pos/2];

            }else
            {
//                value = signal_map.frame[azi].raw_map[r_pos].displaylevel*brightness;
//                dopler = signal_map.frame[azi].raw_map[r_pos].dopler;
                value = signal_map.level_m[azi][r_pos];
                dopler = signal_map.dopler_m[azi][r_pos/2];

            }

            if(r_pos&0x01)
            {
                dopler = dopler&0x0f;
            }
            else
            {
                dopler = dopler>>4;
            }
            //xu ly nguong
            if(xl_nguong)
            {
                if(r_pos<4)
                {
                    rainLevel = noiseAverage;
                }
                else

                   rainLevel += krain*(value-rainLevel);

                unsigned short thresh = rainLevel+ noiseVar*kgain;
                if(value<thresh)
                {
                    value = 0;
                }

            }

            //________________________________///////////////////////
            value*=brightness;
            if(value>0xff)
            {
                value = 0xff;
            }
            if(dopler>0x0f)
            {
                dopler = 0x0f;
            }
            //printf("value:%d\n",value);
            //alpha display
            if(isDisplayAlpha)
            {
                if(isDisplaydopler)
                {
                    for(short i=255;i>255-(dopler<<4);i--)
                    {img_alpha->setPixel(r_pos,i,1);}
                }else
                    for(short i=255;i>255-value;i--)
                    {img_alpha->setPixel(r_pos,i,1);}
            }
            /**/
            drawSgn(azi,r_pos,value,dopler);
    }
    //drawingDone = true;

}
void C_radar_data::drawDopler(short azi)
{
    /*short prev_azi =azi +50;
    if(prev_azi>=MAX_AZIR)prev_azi -= MAX_AZIR;
    blackLine(signal_map.frame[prev_azi].raw_map[1].x,signal_map.frame[prev_azi].raw_map[1].y,signal_map.frame[prev_azi].raw_map[RAD_M_PULSE_RES-1].x,signal_map.frame[prev_azi].raw_map[RAD_M_PULSE_RES-1].y);
    for (short r_pos = 1;r_pos<RAD_M_PULSE_RES-1;r_pos++)
    {

            unsigned short value;
            if(isPulse&&(r_pos<RAD_S_PULSE_RES))
            {
                value = signal_map.frame[azi].raw_map[r_pos+RAD_M_PULSE_RES].displaylevel*brightness;
            }else
            {
                value = signal_map.frame[azi].raw_map[r_pos].displaylevel*brightness;
            }

            if(value>0xff)
            {
                value = 0xff;
            }

            drawSgn(azi,r_pos,value,dopler);
    }*/
}
 void  C_radar_data::getNoiseLevel()
{
//     float var = signal_map.level_m[azi][range_max-50]-noiseAverage;
//     noiseAverage += var/60.0f;
//     noiseVar += (var-noiseVar)/60;
     printf("\nnoise Level:%f",noiseAverage);
     int sum=0;
     int sumvar = 0;
     int n = 0;
     for(short azi=0;azi<MAX_AZIR;azi++)
     {
         sum += signal_map.level_m[azi][range_max-50];
         if(signal_map.level_m[azi][range_max-50])n++;
         sumvar += abs(signal_map.level_m[azi][range_max-50]-signal_map.level_m[azi][range_max-100]);
     }
     if(noiseAverage==0)noiseAverage = sum/float(n);else
     {noiseAverage+=(sum/float(n)-noiseAverage);}
     if(noiseVar==0)noiseVar = sumvar/float(n);else
     {noiseVar+=(sumvar/float(n)-noiseVar);}


}
 void  C_radar_data::setSnLevel(unsigned short azi,unsigned  short range, unsigned char level)
 {/*
     signal_map.frame[azi].raw_map[range].level = level;
     signal_map.frame[azi].raw_map[range].displaylevel  = 0;
     if(!xl_nguong)
     {
         signal_map.frame[azi].raw_map[range].displaylevel  = level;return;
     }
     if(range<4)
     {
         rainLevel = noiseAverage;
     }
     else

        rainLevel = rainLevel+krain*(level-rainLevel);

     unsigned short thresh = rainLevel+ noiseVar*kgain;
     if(level>thresh)
     {
         signal_map.frame[azi].raw_map[range].displaylevel  = level;

         //signal_map.frame[azi].raw_map[r_pos].terrain*=TERRAIN_GAIN;
         //signal_map.frame[azi].raw_map[r_pos].terrain+=TERRAIN_GAIN_1;
         // !!!!
         //if(k_vet>0)signal_map.frame[azi].raw_map[r_pos].vet = 0xff;//(0xff-signal_map.frame[azi].raw_map[r_pos].vet)>>2;
         //if(!dataOverflow)procPix(azi,r_pos);
     }
     else
     {
         signal_map.frame[azi].raw_map[range].displaylevel  = 0;

         //signal_map.frame[azi].raw_map[r_pos].displaylevel     = 0;
         //signal_map.frame[azi].raw_map[r_pos].terrain *= TERRAIN_GAIN;
         //drawVet(azi,r_pos);
     }
     //drawSgn(azi,range);*/
 }
 void  C_radar_data::setDoplerLevel(unsigned short azi,unsigned  short range, unsigned char level)
 {/*
     signal_map.frame[azi].raw_map[range].dopler = level;
*/
 }
#define RADAR_COMMAND_FEEDBACK  6
#define RADAR_DATA_HEADER       22
#define RADAR_DATA_MAX_SIZE     2688

short waitForData = 0;
unsigned char curFrameId;
unsigned char dataBuff[RADAR_DATA_HEADER + RADAR_DATA_MAX_SIZE];
void C_radar_data::GetData(unsigned short azi)
{
    //return;//!!!!
    //unsigned short r_pos;
    unsigned short i = RADAR_DATA_HEADER;
    //đọc dữ liệu xung dài
//    for(r_pos=0; r_pos < range_max;r_pos++)
//    {
//        setSnLevel(azi,r_pos,dataBuff[i+r_pos]);
//    }
    memcpy(&signal_map.level_m[azi][0],&dataBuff[i],range_max);
    i+=range_max;

    //đọc dữ liệu xung đơn
//    for(r_pos = 0; r_pos < RAD_S_PULSE_RES;r_pos++)
//    {
//        setSnLevel(azi,r_pos + RAD_M_PULSE_RES, dataBuff[i+r_pos]);
//    }
    memcpy(&signal_map.level_s[azi][0],&dataBuff[i],RAD_S_PULSE_RES);
    i += RAD_S_PULSE_RES;

    // đọc dữ liệu dopler xung dài
//    for(r_pos=0; r_pos < range_max;r_pos++)
//    {
//        short dopler_pos = r_pos/2;
//        if(r_pos&0x01)
//        {
//            setDoplerLevel(azi,r_pos,dataBuff[i+dopler_pos]&0x0f);
//        }
//        else
//        {
//            setDoplerLevel(azi,r_pos,dataBuff[i+dopler_pos]>>4);
//        }

//    }
    memcpy(&signal_map.dopler_m[azi][0],&dataBuff[i],range_max/2);
    i += range_max/2;
    //đọc dữ liệu dopler xung đơn
//    for(r_pos = 0; r_pos < RAD_S_PULSE_RES;r_pos++)
//    {
//        short dopler_pos = r_pos/2;
//        if(r_pos&0x01)
//        {
//            setDoplerLevel(azi,r_pos+RAD_M_PULSE_RES,dataBuff[i+dopler_pos]&0x0f);
//        }
//        else
//        {
//            setDoplerLevel(azi,r_pos+RAD_M_PULSE_RES,dataBuff[i+dopler_pos]>>4);
//        }

//    }
    memcpy(&signal_map.dopler_s[azi][0],&dataBuff[i],RAD_S_PULSE_RES/2);
    i += RAD_S_PULSE_RES/2;

    // bù phần dữ liệu thiếu của xung dài
    if(range_max < RAD_M_PULSE_RES)
    {
//        for(r_pos = range_max;r_pos < RAD_M_PULSE_RES-1;++r_pos)
//        {
//            setSnLevel(azi,r_pos,0);
//            //setDoplerLevel(azi,r_pos,0);
//        }
        memset(&signal_map.level_m[azi][range_max],0,RAD_M_PULSE_RES-range_max);

    }
    return ;

}

void C_radar_data::ProcessDataFrame()
{
    short azi = (0xfff & (dataBuff[4] << 8 | dataBuff[5]))/2;

    //printf("azi:%d\n",azi);
    overload = (dataBuff[4]>>4)&0x01;
    unsigned char n_clk_adc = (dataBuff[4]&(0xe0))>>5;
    if(clk_adc != n_clk_adc)
    {
        // clock adc đã thay đổi, reset dữ liệu
        if((n_clk_adc>=0)&&(n_clk_adc<=5))
        {
            clk_adc = n_clk_adc;
            isClkAdcChanged = true;
            resetData();
        }else
        {
            printf("\nClock adc is wrong.");
            return;
        }
    }
    temp = dataBuff[3]/4.0f;//
    tempType = dataBuff[2];
    sn_stat = dataBuff[1];
    // đọc lệnh phản hồi
    memcpy(command_feedback,&dataBuff[RADAR_COMMAND_FEEDBACK],8);
    //đọc giá trị cân bằng tạp
    memcpy(noise_level,&dataBuff[RADAR_COMMAND_FEEDBACK+8],8);
    //kiểm tra giá trị phương vị
    short lastazi=azi-1;//,nextazi=azi+1;
    if(lastazi<0)lastazi+=MAX_AZIR;
    if(lastazi!=curAzir)
    {
        GetData(lastazi);
        lastazi-=1;
        if(lastazi<0)lastazi+=MAX_AZIR;
        if(lastazi!=curAzir)
        {
            GetData(lastazi);
            lastazi-=1;
            if(lastazi<0)lastazi+=MAX_AZIR;
            if(lastazi!=curAzir)
            {
                GetData(lastazi);
                printf("Data lost:%d at azi = %d\n",lastazi-curAzir,curAzir);
            }
        }
    }
    curAzir = azi;
    //lấy dữ liệu
    GetData(azi);
    if(azi == 0) {if(terrain_init_time) terrain_init_time--;}



}
short drawnazi = 0;
void C_radar_data::redrawImg()
{

    while(drawnazi!=curAzir)
    {
        drawnazi++;
        if(drawnazi>=MAX_AZIR)drawnazi=0;
        drawAzi(drawnazi);
        if(!((unsigned char)(drawnazi<<4))){
            procTracks(drawnazi);
        }
        if(drawnazi==0)getNoiseLevel();
    }
}
void C_radar_data::GetDataHR(unsigned char* data,unsigned short dataLen)
{

    if((dataLen<RADAR_DATA_HEADER))return;
    char dataId = data[0]&0x0f;

    if(dataId==1)
    {
        curFrameId = data[0]&0xf0;
        range_max = (dataLen - RADAR_DATA_HEADER)*4/3 - RAD_S_PULSE_RES;
        //printf("range_max:%d\n",range_max);
        if(range_max > RAD_M_PULSE_RES)return;
        memcpy(dataBuff,data,dataLen);
        waitForData = dataLen;

    }
    else if(dataId==2)
    {
        //check if we are waiting for second half data frame
        if(!waitForData)return;
        //check if frame ID is the one that we are expecting
        short secondFrameId = data[0]&0xf0;
        if(curFrameId!=secondFrameId)return;
        // check if the data size is correct
        if(dataLen!=waitForData)return;
        //load data to buffer
        memcpy(dataBuff + waitForData,data + RADAR_DATA_HEADER,dataLen-RADAR_DATA_HEADER);
        //process data
        ProcessDataFrame();
        waitForData = 0;

    }
    //if(!dopler){frameId = data[0]>>4; }else {if(frameId =! (data[0]>>4))return;}//check id of dopler data

    /*
	short azi = 0xfff & (buff[ADDR_AZI_H] << 8 | buff[ADDR_AZI_L]);
    if(curAzir==azi) return GetData();
    curAzir = azi;
    if(curAzir==4095){
        curPeriodIndex++;
        procTracks();
    }
    for(short r = 1; r < 1023; r++)
    {
        short i = (r>>3);
        short j = (r&0x0007);
        if((buff[VIDEO_RAW_LENGTH+i]>>j & 0x1))
        {


            //signal_map.frame[azi].raw_map[r].level = signal_map.frame[azi].raw_map[r].level<<
            //if(signal_map.frame[azi].raw_map[r].level<80)
            signal_map.frame[azi].raw_map[r].displaylevel  = 1;
            signal_map.frame[azi].raw_map[r].level = buff[r];
            signal_map.frame[azi].raw_map[r].vet = float(signal_map.frame[azi].raw_map[r].vet*0.95 + 0.05);//255*0.125;

            procPix(azi,r);
        }
        else
        {
            signal_map.frame[azi].raw_map[r].displaylevel  = 0;
            signal_map.frame[azi].raw_map[r].level = buff[r];
            signal_map.frame[azi].raw_map[r].vet = float(signal_map.frame[azi].raw_map[r].vet*0.95);
            //signal_map.frame[azi].raw_map[r].level = 0;
        }

    }
	delete[] buff;
    return azi;*/

}
void C_radar_data::procMark(mark_t* pMark)
{

    if((pMark->size>size_thresh))//ENVDEP
    {
            object_t newobject;
            float ctA = pMark->sumA/pMark->size/MAX_AZIR*PI_NHAN2+trueN;
            float ctR = pMark->sumR/pMark->size;
            if(ctA >= PI_NHAN2)ctA -= PI_NHAN2;
            newobject.terrain = pMark->sumTer/(float)(pMark->size);
            newobject.az   = ctA;
            //newobject.markNum = ss;
            //newobject.daz  = pMark->maxA-pMark->minA;
            //if(newobject.daz<0)newobject.daz+=PI_NHAN2;
            newobject.rg   = ctR;
            //_________convert center AR to XY_______________//
            newobject.x = ((sinf(ctA)))*ctR;
            newobject.y = ((cosf(ctA)))*ctR;
            procObject(&newobject);
    }

}
void C_radar_data::procTracks(unsigned short curA)
{
    //process all marks
    short pr_curA = curA-1;
    if(pr_curA<0)pr_curA+=MAX_AZIR;
    for(unsigned short i = 0;i<mark_list.size();++i)
    {
        if(mark_list[i].size)
        {
            if((mark_list[i].maxA!=curA)&&(mark_list[i].maxA!=pr_curA))
            {
                procMark(&mark_list[i]);
                mark_list[i].size =0;
            }

        }
    }
    //proc_track
    float azi = (float)curA/MAX_AZIR*PI_NHAN2+trueN;
    for(unsigned short i=0;i<mTrackList.size();i++)
    {
        if(!mTrackList[i].state)continue;
        float dA = azi-mTrackList[i].estA;
        if(dA>PI) dA-=PI_NHAN2;
        else if(dA<-PI)dA+=PI_NHAN2;
        if(mTrackList[i].isProcessed)
        {
            if(abs(dA)<0.35f)//20 deg
            {
                mTrackList[i].isProcessed = false;
            }
        }
        else
        {
            if(abs(dA)>0.35f)//20 deg
            {
                mTrackList[i].isProcessed = true;
                mTrackList[i].update();
            }
        }
    }


}
void C_radar_data::getPolar(float x,float y,float *azi,float *range)
{
    *azi = atanf(x/y);//tinh azi theo chuan bac thuan kim dong ho
    if(y<0)*azi+=PI;
    if(azi<0)*azi += PI_NHAN2;
    *range = sqrt(x*x+y*y);
}
void C_radar_data::addTrackManual(float x,float y)
{
    float azi = atanf(x/y);//tinh azi,range
    if(y<0)azi+=PI;
    if(azi<0)azi += PI_NHAN2;
    float range = sqrt(x*x+y*y);
    addTrack( azi,  range, 4);
}
void C_radar_data::addTrack(float azi, float range, short state)
{
    //add new track
    object_t mObject;
    for(unsigned short i=0;i<mTrackList.size();i++)
    {
        if(!mTrackList[i].state)
        {

            mObject.x = (sinf(azi))*range;
            mObject.y = (cosf(azi))*range;
            mTrackList[i].init(azi,range,state);
            mTrackList[i].tclass = RED_OBJ;
            mTrackList[i].object_list.push_back(mObject);
            return;
        }
    }
    track_t newTrack;
    mObject.x = (sinf(azi))*range;
    mObject.y = (cosf(azi))*range;
    newTrack.init(azi,range,state);
    newTrack.object_list.push_back(mObject);
    mTrackList.push_back(newTrack);
}
void C_radar_data::deleteTrack(short trackNum)
{
    if(mTrackList.size()>trackNum)
    {
        mTrackList[trackNum].state = 0;
        if(mTrackList[trackNum].object_list.size())mTrackList[trackNum].object_list.clear();
        if(mTrackList[trackNum].suspect_list.size())mTrackList[trackNum].suspect_list.clear();
    }
}
void C_radar_data::procObject(object_t* pObject)
{
        if(pObject->rg<80) return;
        if(avtodetect)
        {
            if((pObject->terrain<TERRAIN_THRESH)&&(!terrain_init_time))
            {
                for(unsigned short i=0;i<mTrackList.size();i++)
                {
                    if(mTrackList[i].state&&(! mTrackList[i].isProcessed))
                    {
                        if(mTrackList[i].checkProb(pObject)){
                            mTrackList[i].suspect_list.push_back(*pObject);
                            return;//add object to a processing track
                        }
                    }
                }

                for(unsigned short i=0;i<mTrackList.size();i++)
                {
                    if(!mTrackList[i].state)
                    {

                        mTrackList[i].init(pObject->az,pObject->rg,2);
                        mTrackList[i].object_list.push_back(*pObject);
                        return;//add object to a empty track
                    }
                }
                //create new track
                if(mTrackList.size()<MAX_TRACKS)
                {
                    //printf("pObject->rg:%f\n",pObject->rg);
                    addTrack(pObject->az,pObject->rg,2);

                }

            }
        }
        else
        {
            for(unsigned short i=0;i<mTrackList.size();i++)
            {
                if(mTrackList[i].state&&(! mTrackList[i].isProcessed))
                {
                    if(mTrackList[i].checkProb(pObject)){
                        mTrackList[i].suspect_list.push_back(*pObject);
                        return;//add object to a processing track
                    }
                }
            }
        }

}
void C_radar_data::procPix(short proc_azi,short range)//_______signal detected, check 4 last neighbour points for nearby mark_______________//
{
/*
    short pr_proc_azi = proc_azi-1;
    if(pr_proc_azi<0)pr_proc_azi+=MAX_AZIR;
    short markIndex =-1;
    if(signal_map.frame[pr_proc_azi].raw_map[range].displaylevel)
    {
         markIndex = signal_map.frame[pr_proc_azi].raw_map[range].markIndex;

    }else if(signal_map.frame[proc_azi].raw_map[range-1].displaylevel)
    {
         markIndex = signal_map.frame[proc_azi].raw_map[range-1].markIndex;

    }
    else if(signal_map.frame[pr_proc_azi].raw_map[range-1].displaylevel)
    {
         markIndex = signal_map.frame[pr_proc_azi].raw_map[range-1].markIndex;

    }
    else if(signal_map.frame[pr_proc_azi].raw_map[range+1].displaylevel)
    {
         markIndex = signal_map.frame[pr_proc_azi].raw_map[range+1].markIndex;
    }
    if(markIndex!=-1)// add to existing marker
    {
        signal_map.frame[proc_azi].raw_map[range].markIndex = markIndex;
        mark_list[markIndex].size++;
        if(proc_azi<mark_list[markIndex].minA){
            mark_list[markIndex].sumA    +=  proc_azi + MAX_AZIR;
            mark_list[markIndex].maxA    =  proc_azi + MAX_AZIR;
        }else
        {
            mark_list[markIndex].sumA    +=  proc_azi;
            mark_list[markIndex].maxA    =  proc_azi;
        }

        mark_list[markIndex].sumR    +=  range;
        mark_list[markIndex].sumTer  +=  signal_map.frame[proc_azi].raw_map[range].terrain;

    }
    else//_________new marker found_____________//
    {

        mark_t              new_mark;
        new_mark.maxA =  new_mark.minA  = proc_azi;
        //new_mark.minR = new_mark.maxR = range;
        new_mark.size =  1;
        new_mark.sumTer = signal_map.frame[proc_azi].raw_map[range].terrain;
        new_mark.sumA =  proc_azi;
        new_mark.sumR =  range;
        bool listFull = true;

        for(unsigned short i = 0;i<mark_list.size();++i)
        {
                //  overwrite
                if(!mark_list[i].size)
                {
                    signal_map.frame[proc_azi].raw_map[range].markIndex =  i;
                    mark_list[i] = new_mark;
                    listFull = false;
                    break;
                }
        }
        if(listFull)
        {
            mark_list.push_back(new_mark);
            signal_map.frame[proc_azi].raw_map[range].markIndex = mark_list.size()-1;
            //printf("\nmark_list.size():%d",mark_list.size());
        }


    }
            //______________________________________//


*/

}
/*void C_radar_data::polarToSnXY(short *xsn, short *ysn, short azi, short range)
{
    *xsn = signal_map.frame[azi].raw_map[range].x;
    *ysn = signal_map.frame[azi].raw_map[range].y;
}
//static short ctX=0,ctY=0;
//static float dr = 0;
*/
void C_radar_data::polarToXY(float *x, float *y, float azi, float range)
{

    *x = ((sinf(azi)))*range;
    *y = ((cosf(azi)))*range;
}
void C_radar_data::raw_map_init()
{
    img_ppi->fill(Qt::transparent);
    img_alpha->fill(0);
    float theta=trueN;
    float dTheta = 2*PI/MAX_AZIR;
    for(short azir = 0; azir < MAX_AZIR; azir++)
	{

		float cost = cosf(theta);
		float sint = sinf(theta);

        for(short range = 0;range<RAD_M_PULSE_RES;range++)
		{
            signal_map.x[azir][range]     =  short(sint*(range+1)*viewScale)+RAD_M_PULSE_RES;
            signal_map.y[azir][range]    =  -short(cost*(range+1)*viewScale)+RAD_M_PULSE_RES;
            if(signal_map.x[azir][range]<0||signal_map.x[azir][range]>=img_ppi->width()||signal_map.y[azir][range]<0||signal_map.y[azir][range]>=img_ppi->height())
            {
                signal_map.x[azir][range] = -1;
                signal_map.y[azir][range] = -1;
            }
		}
		theta+=dTheta;
	}
}
void C_radar_data::resetData()
{
    terrain_init_time = 20;
    noiseAverage = 0;
    noiseVar = 0;
    kgain = 1;
    krain  = ksea = 0;
    brightness = 1;
    avtodetect = false;
    memset(signal_map.level_m,0,RAD_M_PULSE_RES);
    memset(signal_map.dopler_m,0,RAD_M_PULSE_RES);
    memset(signal_map.level_s,0,RAD_S_PULSE_RES);
    memset(signal_map.dopler_s,0,RAD_S_PULSE_RES);
//    for(short azir = 0;azir< MAX_AZIR;azir++)
//    {
//        for(short range = 0;range<RAD_M_PULSE_RES;range++)
//        {
//            signal_map.level_m[azir].raw_map[range].displaylevel  = 0;
//            signal_map.frame[azir].raw_map[range].vet = 0;//NGUONG_DIA_VAT/2;
//            signal_map.frame[azir].raw_map[range].terrain = TERRAIN_THRESH/2.0f;
//        }
//    }

}
void C_radar_data::setViewScale(float sprding)
{
    if(viewScale!=sprding)
    {
        viewScale = sprding;
        raw_map_init();

        //printf("sprding:%f",spreading);//!!!!
    }
        /*for(short azir = 0;azir< MAX_AZIR;azir++)
    {
        for(short range = 0;range<RADAR_MAX_RESOLUTION;range++)
        {
            //signal_map.frame[azir].raw_map[range].displaylevel  = 0;
            //signal_map.frame[azir].raw_map[range].level  = 0;
            signal_map.frame[azir].raw_map[range].vet = 0;//NGUONG_DIA_VAT/2;
            //signal_map.frame[azir].raw_map[range].terrain = TERRAIN_THRESH/2.0f;
        }
    }*/
    //sgn_img->fill(Qt::transparent);
}
void C_radar_data::resetTrack()
{
    for(unsigned short i=0;i<mTrackList.size();i++)
    {
        if(mTrackList[i].state)
        {
            mTrackList[i].state = 0;
        }
    }
}