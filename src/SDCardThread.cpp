#include "SDCardThread.h"
#include <chrono>
#include <cstdio>

SDCardThread::SDCardThread(Data& data) :
    m_data(data),
    m_thread(SDCARD_THREAD_PRIORITY, SDCARD_THREAD_SIZE)
{
    if (!m_sd.init()) {
        //if this fails all operations will be ignored (in case you wanna use it without sd card)
        printf("SD init failed\n");
    }
    m_data = data;
    m_buffer = (uint8_t*)malloc(SDCARD_BUFFER_SIZE);
}

SDCardThread::~SDCardThread()
{
    m_ticker.detach();
}

void SDCardThread::StartThread()
{
    m_thread.start(callback(this, &SDCardThread::run));
    m_ticker.attach(callback(this, &SDCardThread::sendThreadFlag), std::chrono::milliseconds{ static_cast<long int>( SDCARD_THREAD_TS_MS ) });
}

void SDCardThread::CloseFile() {
    m_sd.close();
    free(m_buffer);
}

void SDCardThread::run()
{

    static float buffer_f[32];
    static uint32_t buffer_u32[3+2]; //3 from param.h and 2 from time and index
    static uint8_t buffer_u8[4];
    static bool buffer_b[6];

    while(true) {
        ThisThread::flags_wait_any(m_threadFlag);

        static Timer timer;
        timer.start();
        buffer_u32[0] = std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count();
        buffer_u32[1]++;

#if SDCARD_DO_PRINTF
        printf("%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", m_data.gyro(0), m_data.gyro(1), m_data.gyro(2),
                                                                         m_data.acc(0) , m_data.acc(1) , m_data.acc(2) ,
                                                                         m_data.mag(0) , m_data.mag(1) , m_data.mag(2) );
#endif
        buffer_f[0] = m_data.gyro(0);
        buffer_f[1] = m_data.gyro(1);
        buffer_f[2] = m_data.gyro(2);
        buffer_f[3] = m_data.acc(0);
        buffer_f[4] = m_data.acc(1);
        buffer_f[5] = m_data.acc(2);
        buffer_f[6] = m_data.mag(0);
        buffer_f[7] = m_data.mag(1);
        buffer_f[8] = m_data.mag(2);

        //char rover_header[] = "itow[ms];carrSoln;lon;lat;height[m];x[mm];y[mm];z[mm];hAcc[mm];vAcc[mm];LoRa_valid;SNR;RSSI;ax;az;az;gx;gy;gz;\n";
        //m_sd.write2sd(buffer_c,data_length);
        m_sd.write_f_2_sd(buffer_f, 9);
        m_sd.write_u32_2_sd(buffer_u32, 2);

    }
}

void SDCardThread::sendThreadFlag()
{
    m_thread.flags_set(m_threadFlag);
}