#ifndef DATAPROCESSINGTHREAD_H
#define DATAPROCESSINGTHREAD_H
#include <QThread>
#include "C_radar_data.h"
#include "c_arpa_data.h"
#include <vector>
#include <QMutexLocker>
#include <QFile>
#include <QUdpSocket>
#include <QStringList>
#define HAVE_REMOTE
#include "pcap.h"
#ifndef CONST_NM
#define CONST_NM 1.825f
#endif
struct DataBuff
{
    short len;
    unsigned char data[1500];
};
class dataProcessingThread:public QThread
{
    Q_OBJECT
public:
    QMutex  mutex;
    char    playRate;
    DataBuff*   dataBuff;
    float   k_vet;
    void SetRadarPort( unsigned short portNumber);
    void SetARPAPort( unsigned short portNumber);
    void ReadDataBuffer();
    ~dataProcessingThread();
    dataProcessingThread();
    //QUdpSocket      *udpSendSocket;
    void PlaybackFile();
    void startRecord(QString fileName);
    void stopRecord();
    void stopThread();
    void startReplay(QString fileName);
    void togglePlayPause(bool play);
    C_radar_data* radarData;
    C_ARPA_data* arpaData;
    void run();
    bool getIsDrawn()
    {
       if(!isDrawn){isDrawn = true;return false;}
       else return true;
    }
private:

    bool isDrawn;
    bool isRecording;
    bool isPlaying;
    QFile signRepFile;
    QFile signRecFile;

    QUdpSocket      *radarDataSocket;
    QUdpSocket      *ARPADataSocket;
    void listenToRadar();
public slots:

    void processRadarData();
    void processARPAData();
    void playbackRadarData();
};

#endif // DATAPROCESSINGTHREAD_H
