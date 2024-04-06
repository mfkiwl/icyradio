#include "SoapyIcyRadio.hpp"
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <iostream>
#include <csignal>
#include <chrono>

SoapySDR::KwargsList findIcyRadio(const SoapySDR::Kwargs &args);
SoapySDR::Device *makeIcyRadio(const SoapySDR::Kwargs &args);

static sig_atomic_t g_done = false;

static void sigHandler(const int)
{
    g_done = true;
}

bool loadSystemSoapyIcyRadio()
{
    try
    {
        for(const auto &mod : SoapySDR::listModules())
        {
            if(mod.find("/libIcyRadioSupport.so") == std::string::npos)
                continue;

            std::cout << "Loading system IcyRadio support module \"" << mod << "\"..." << std::endl;

            std::string e = SoapySDR::loadModule(mod);

            if(!e.empty())
            {
                std::cerr << "Error loading module: " << e << std::endl;

                return false;
            }

            std::cout << "Loaded version " << SoapySDR::getModuleVersion(mod) << std::endl;

            return true;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception loading local IcyRadio support module: " << ex.what() << std::endl;

        return false;
    }

    return false;
}
bool loadLocalSoapyIcyRadio()
{
    try
    {
        char cwd[256] = {0};

        char* _ = getcwd(cwd, sizeof(cwd)); // Suppress unused warning
        (void)_;

        const std::string mod = std::string(cwd) + "/libIcyRadioSupport.so";

        if(access(mod.c_str(), F_OK) == -1)
            return false;

        std::cout << "Loading local IcyRadio support module \"" << mod << "\"..." << std::endl;

        std::string e = SoapySDR::loadModule(mod);

        if(!e.empty())
        {
            std::cerr << "Error loading module: " << e << std::endl;

            return false;
        }

        std::cout << "Loaded version " << SoapySDR::getModuleVersion(mod) << std::endl;

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception loading local IcyRadio support module: " << ex.what() << std::endl;

        return false;
    }

    return false;
}
bool loadSoapyIcyRadio()
{
    if(loadLocalSoapyIcyRadio())
        return true;

    if(loadSystemSoapyIcyRadio())
        return true;

    return false;
}

void testToneTX(SoapyIcyRadio *sdr, double fc = 480e6)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, 16 * 1024 * 1024);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, 12e6);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_ATT", 30);
    sdr->setGain(SOAPY_SDR_TX, 1, "TX_ATT", 30);

    double full_scale = 0;
    const std::string fmt = sdr->getNativeStreamFormat(SOAPY_SDR_TX, 0, full_scale);
    const size_t samp_sz = SoapySDR::formatToSize(fmt);

    auto s = sdr->setupStream(SOAPY_SDR_TX, fmt, {0, 1});

    const size_t mtu = sdr->getStreamMTU(s);

    std::vector<std::vector<uint8_t>> buf(2, std::vector<uint8_t>(samp_sz * mtu)); // Native

    // Tone - fs / 8 on channel 0, fs / 16 on channel 1
    {
        std::vector<std::vector<float>> f_buf(2, std::vector<float>(2 * mtu));

        for(size_t i = 0; i < mtu; i++)
        {
            f_buf[0][2 * i + 0] = std::cos(2 * M_PI * i / 8);
            f_buf[0][2 * i + 1] = std::sin(2 * M_PI * i / 8);

            f_buf[1][2 * i + 0] = std::cos(2 * M_PI * i / 16);
            f_buf[1][2 * i + 1] = std::sin(2 * M_PI * i / 16);
        }

        SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, fmt)(f_buf[0].data(), buf[0].data(), mtu, full_scale);
        SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, fmt)(f_buf[1].data(), buf[1].data(), mtu, full_scale);

        double f0 = sdr->getFrequency(SOAPY_SDR_TX, 0) + sdr->getSampleRate(SOAPY_SDR_TX, 0) / 8;
        double f1 = sdr->getFrequency(SOAPY_SDR_TX, 0) + sdr->getSampleRate(SOAPY_SDR_TX, 0) / 16;

        std::cout << "Channel 0: Tone with f = " << (size_t)f0 << " Hz" << std::endl;
        std::cout << "Channel 1: Tone with f = " << (size_t)f1 << " Hz" << std::endl;
    }

    std::vector<void *> bufs(2);

    for(size_t i = 0; i < buf.size(); i++)
        bufs[i] = buf[i].data();

    std::cout << "Starting TX tone stream loop, press Ctrl+C to exit..." << std::endl;

    sdr->activateStream(s);

    long long timeNs = 0;

    while(!g_done)
    {
        int flags = 0;

        sdr->writeStream(s, bufs.data(), mtu, flags, timeNs);
    }

    g_done = false;

    sdr->deactivateStream(s);
    sdr->closeStream(s);
}
void testTimedToneTX(SoapyIcyRadio *sdr, double fc = 480e6)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, 16 * 1024 * 1024);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, 12e6);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_ATT", 30);
    sdr->setGain(SOAPY_SDR_TX, 1, "TX_ATT", 30);

    double full_scale = 0;
    const std::string fmt = sdr->getNativeStreamFormat(SOAPY_SDR_TX, 0, full_scale);
    const size_t samp_sz = SoapySDR::formatToSize(fmt);

    auto s = sdr->setupStream(SOAPY_SDR_TX, fmt, {0, 1});

    const size_t mtu = sdr->getStreamMTU(s);

    std::vector<std::vector<uint8_t>> buf(2, std::vector<uint8_t>(samp_sz * mtu)); // Native

    // Tone - fs / 8 on channel 0, fs / 16 on channel 1
    {
        std::vector<std::vector<float>> f_buf(2, std::vector<float>(2 * mtu));

        for(size_t i = 0; i < mtu; i++)
        {
            f_buf[0][2 * i + 0] = std::cos(2 * M_PI * i / 8);
            f_buf[0][2 * i + 1] = std::sin(2 * M_PI * i / 8);

            f_buf[1][2 * i + 0] = std::cos(2 * M_PI * i / 16);
            f_buf[1][2 * i + 1] = std::sin(2 * M_PI * i / 16);
        }

        SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, fmt)(f_buf[0].data(), buf[0].data(), mtu, full_scale);
        SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, fmt)(f_buf[1].data(), buf[1].data(), mtu, full_scale);

        double f0 = sdr->getFrequency(SOAPY_SDR_TX, 0) + sdr->getSampleRate(SOAPY_SDR_TX, 0) / 8;
        double f1 = sdr->getFrequency(SOAPY_SDR_TX, 0) + sdr->getSampleRate(SOAPY_SDR_TX, 0) / 16;

        std::cout << "Channel 0: Tone with f = " << (size_t)f0 << " Hz and ~50% duty cycle" << std::endl;
        std::cout << "Channel 1: Tone with f = " << (size_t)f1 << " Hz and ~50% duty cycle" << std::endl;
    }

    std::vector<void *> bufs(2);

    for(size_t i = 0; i < buf.size(); i++)
        bufs[i] = buf[i].data();

    std::cout << "Starting timed TX tone stream loop, press Ctrl+C to exit..." << std::endl;

    sdr->activateStream(s);

    long long timeNs = sdr->getHardwareTime();

    timeNs += 1e9; // 1 second in the future

    size_t n = 0;

    while(!g_done)
    {
        int flags = !(n % 64) ? SOAPY_SDR_HAS_TIME : 0;

        sdr->writeStream(s, bufs.data(), mtu, flags, timeNs, 10e6);

        if(flags & SOAPY_SDR_HAS_TIME)
            timeNs += 1e9; // 1 second in the future

        n++;
    }

    g_done = false;

    sdr->deactivateStream(s);
    sdr->closeStream(s);
}
void testAWGNTX(SoapyIcyRadio *sdr, double fc = 480e6, double fs = 3e6)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, fs);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, 0.5 * fs);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_ATT", 30);
    sdr->setGain(SOAPY_SDR_TX, 1, "TX_ATT", 30);

    double full_scale = 0;
    const std::string fmt = sdr->getNativeStreamFormat(SOAPY_SDR_TX, 0, full_scale);
    const size_t samp_sz = SoapySDR::formatToSize(fmt);

    auto s = sdr->setupStream(SOAPY_SDR_TX, fmt, {0, 1});

    const size_t mtu = sdr->getStreamMTU(s);

    std::vector<std::vector<uint8_t>> buf(2, std::vector<uint8_t>(samp_sz * mtu)); // Native

    // Random data on both channels
    {
        std::vector<std::vector<float>> f_buf(2, std::vector<float>(2 * mtu));

        for(size_t i = 0; i < mtu; i++)
        {
            // Scale to [-1.0, 1.0]
            f_buf[0][2 * i + 0] = 2.0 * (double)std::rand() / RAND_MAX - 1.0;
            f_buf[0][2 * i + 1] = 2.0 * (double)std::rand() / RAND_MAX - 1.0;

            f_buf[1][2 * i + 0] = 2.0 * (double)std::rand() / RAND_MAX - 1.0;
            f_buf[1][2 * i + 1] = 2.0 * (double)std::rand() / RAND_MAX - 1.0;
        }

        SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, fmt)(f_buf[0].data(), buf[0].data(), mtu, full_scale);
        SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, fmt)(f_buf[1].data(), buf[1].data(), mtu, full_scale);

        std::cout << "Channel 0: Random data (White noise) at fc = " << (size_t)fc << " Hz and bandwidth = " << (size_t)(0.5 * fs) << " Hz" << std::endl;
        std::cout << "Channel 1: Random data (White noise) at fc = " << (size_t)fc << " Hz and bandwidth = " << (size_t)(0.5 * fs) << " Hz" << std::endl;
    }

    std::vector<void *> bufs(2);

    for(size_t i = 0; i < buf.size(); i++)
        bufs[i] = buf[i].data();

    std::cout << "Starting TX AWGN stream loop, press Ctrl+C to exit..." << std::endl;

    sdr->activateStream(s);

    long long timeNs = 0;

    while(!g_done)
    {
        int flags = 0;

        sdr->writeStream(s, bufs.data(), mtu, flags, timeNs);
    }

    g_done = false;

    sdr->deactivateStream(s);
    sdr->closeStream(s);
}
void testRFDelay(SoapyIcyRadio *sdr, double fc = 480e6, double fs = 61.44e6)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, fs);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, fs / 8);
    sdr->setBandwidth(SOAPY_SDR_RX, 0, fs / 8);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setFrequency(SOAPY_SDR_RX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setAntenna(SOAPY_SDR_RX, 0, "RX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_ATT", 10);
    sdr->setGain(SOAPY_SDR_RX, 0, "RX_FE", 0);

    double tx_full_scale = 0;
    const std::string tx_fmt = sdr->getNativeStreamFormat(SOAPY_SDR_TX, 0, tx_full_scale);
    const size_t tx_samp_sz = SoapySDR::formatToSize(tx_fmt);
    auto tx_s = sdr->setupStream(SOAPY_SDR_TX, tx_fmt, {0});

    double rx_full_scale = 0;
    const std::string rx_fmt = sdr->getNativeStreamFormat(SOAPY_SDR_RX, 0, rx_full_scale);
    const size_t rx_samp_sz = SoapySDR::formatToSize(rx_fmt);
    auto rx_s = sdr->setupStream(SOAPY_SDR_RX, rx_fmt, {0});

    const size_t tx_mtu = sdr->getStreamMTU(tx_s);
    const size_t rx_mtu = sdr->getStreamMTU(rx_s);
    const size_t mtu = std::min(std::min(tx_mtu, rx_mtu), (size_t)4096U);
    const size_t tx_size = mtu / 2;
    const size_t rx_size = mtu;

    std::vector<uint8_t> tx_buf(tx_samp_sz * tx_size); // Native
    void *tx_bufs[] = {tx_buf.data()};

    std::vector<uint8_t> rx_buf(rx_samp_sz * rx_size); // Native
    void *rx_bufs[] = {rx_buf.data()};

    std::vector<float> f_buf(2 * rx_size); // RX size is bigger, we will reuse the buffer

    // Generate sinc pulse
    for(size_t i = 0; i < tx_size; i++)
    {
        float _i = i;
        float max = tx_size / 2;
        float x = 8 * (_i - max) / max;

        f_buf[2 * i + 0] = (_i == max) ? 1.0 : (std::sin(M_PI * x) / (M_PI * x)); // I
        f_buf[2 * i + 1] = 0; // Q
    }

    SoapySDR::ConverterRegistry::getFunction(SOAPY_SDR_CF32, tx_fmt)(f_buf.data(), tx_buf.data(), tx_size, tx_full_scale);

    // --------------|--------------|--------------|--------------|--------> Time
    //  rx starts -> | tx starts -> |   tx ends -> |   rx ends -> |

    long long now = sdr->getHardwareTime();
    long long rx_start = now + 100e6; // 100 ms in the future
    long long rx_stop = rx_start + SoapySDR::ticksToTimeNs(rx_size, sdr->getSampleRate(SOAPY_SDR_RX, 0));
    long long tx_start = rx_start + SoapySDR::ticksToTimeNs(tx_size / 2, sdr->getSampleRate(SOAPY_SDR_TX, 0));
    long long tx_stop = tx_start + SoapySDR::ticksToTimeNs(tx_size, sdr->getSampleRate(SOAPY_SDR_TX, 0));

    std::cout << "now: " << now << " ns, rx_start: " << rx_start << " ns, tx_start: " << tx_start << " ns, tx_stop: " << tx_stop << " ns, rx_stop: " << rx_stop << " ns" << std::endl;

    int flags = SOAPY_SDR_HAS_TIME;

    sdr->activateStream(tx_s);
    sdr->activateStream(rx_s, flags, rx_start);

    flags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_ONE_PACKET;

    sdr->writeStream(tx_s, tx_bufs, tx_size, flags, tx_start, 1e6);

    flags = 0;
    long long actual_rx_start = 0;

    sdr->readStream(rx_s, rx_bufs, rx_size, flags, actual_rx_start, 1e6);

    sdr->deactivateStream(rx_s);
    sdr->deactivateStream(tx_s);
    sdr->closeStream(rx_s);
    sdr->closeStream(tx_s);

    std::ofstream tf;

    tf.open("rfdelay_trace.py");

    tf << "import numpy as np" << std::endl;
    tf << "from matplotlib import pyplot as plt" << std::endl;

    tf << std::endl;

    tf << "tx = np.array([";

    for(size_t i = 0; i < tx_size; i++)
    {
        float mag = std::sqrt(f_buf[2 * i + 0] * f_buf[2 * i + 0] + f_buf[2 * i + 1] * f_buf[2 * i + 1]);

        tf << mag;

        if(i < tx_size - 1)
            tf << ", ";
    }

    tf << "])" << std::endl;

    SoapySDR::ConverterRegistry::getFunction(rx_fmt, SOAPY_SDR_CF32)(rx_buf.data(), f_buf.data(), rx_size, 1.0 / rx_full_scale);

    tf << "rx = np.array([";

    for(size_t i = 0; i < rx_size; i++)
    {
        float mag = std::sqrt(f_buf[2 * i + 0] * f_buf[2 * i + 0] + f_buf[2 * i + 1] * f_buf[2 * i + 1]);

        tf << mag;

        if(i < rx_size - 1)
            tf << ", ";
    }

    tf << "])" << std::endl;

    tf << std::endl;

    tf << "tx = tx / np.max(tx)" << std::endl;
    tf << "rx = rx / np.max(rx)" << std::endl;
    tf << "corr = np.correlate(tx, rx, 'full')" << std::endl;

    tf << std::endl;

    tf << "plt.figure()" << std::endl;
    tf << "plt.plot(tx, label='TX')" << std::endl;
    tf << "plt.legend()" << std::endl;

    tf << "plt.figure()" << std::endl;
    tf << "plt.plot(rx, label='RX')" << std::endl;
    tf << "plt.show()" << std::endl;

    tf << "plt.figure()" << std::endl;
    tf << "plt.plot(corr, label='Correlation')" << std::endl;
    tf << "plt.show()" << std::endl;

    tf.close();

    // TODO: Find the actual delay
}
void testFullDuplex(SoapyIcyRadio *sdr, double fs = 30.72e6)
{
    // List of sensors to monitor
    std::vector<std::pair<std::string, std::string>> sensors = {
        {"xadc_temp", ""},
        {"pmc_temp", ""},
        {"rf_phy_temp", ""},
        {"vin_reg_temp", ""},
        {"vin_reg_5v0_vout", ""},
        {"vin_reg_5v0_iout", ""},
        {"vin_reg_5v0_pout", ""},
        {"vin_reg_1v0_vout", ""},
        {"vin_reg_1v0_iout", ""},
        {"vin_reg_1v0_pout", ""}
    };

    for(auto &sensor : sensors)
    {
        auto info = sdr->getSensorInfo(sensor.first);

        sensor.second = info.name + ": %s " + info.units;

        printf("-> ");
        printf(sensor.second.c_str(), sdr->readSensor(sensor.first).c_str());
        printf("\n");
    }

    fflush(stdout);

    sdr->setSampleRate(SOAPY_SDR_TX, 0, fs);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setAntenna(SOAPY_SDR_RX, 0, "RX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_ATT", 89);
    sdr->setGain(SOAPY_SDR_RX, 0, "RX_FE", 0);

    double tx_full_scale = 0;
    const std::string tx_fmt = sdr->getNativeStreamFormat(SOAPY_SDR_TX, 0, tx_full_scale);
    const size_t tx_samp_sz = SoapySDR::formatToSize(tx_fmt);
    auto tx_s = sdr->setupStream(SOAPY_SDR_TX, tx_fmt, {0, 1});

    double rx_full_scale = 0;
    const std::string rx_fmt = sdr->getNativeStreamFormat(SOAPY_SDR_RX, 0, rx_full_scale);
    const size_t rx_samp_sz = SoapySDR::formatToSize(rx_fmt);
    auto rx_s = sdr->setupStream(SOAPY_SDR_RX, rx_fmt, {0, 1});

    const size_t tx_mtu = sdr->getStreamMTU(tx_s);
    const size_t rx_mtu = sdr->getStreamMTU(rx_s);

    std::vector<std::vector<uint8_t>> tx_buf(2, std::vector<uint8_t>(tx_samp_sz * tx_mtu)); // Native
    void *tx_bufs[] = {tx_buf[0].data(), tx_buf[1].data()};

    std::vector<std::vector<uint8_t>> rx_buf(2, std::vector<uint8_t>(rx_samp_sz * rx_mtu)); // Native
    void *rx_bufs[] = {rx_buf[0].data(), rx_buf[1].data()};

    size_t tx_overflows = 0;
    size_t rx_overflows = 0;
    size_t tx_underflows = 0;
    size_t rx_underflows = 0;
    size_t tx_n_samples = 0;
    size_t rx_n_samples = 0;
    size_t tx_total_n_samples = 0;
    size_t rx_total_n_samples = 0;
    const auto t_start = std::chrono::high_resolution_clock::now();
    auto t_last_print = std::chrono::high_resolution_clock::now();
    auto t_last_spin = std::chrono::high_resolution_clock::now();
    size_t spin_i = 0;

    std::cout << "Starting Full Duplex stream loop at " << (size_t)fs << " sps, press Ctrl+C to exit..." << std::endl;

    sdr->activateStream(tx_s);
    sdr->activateStream(rx_s);

    while(!g_done)
    {
        int ret = 0;
        int flags = 0;
        long long timeNs = 0;

        ret = sdr->writeStream(tx_s, tx_bufs, tx_mtu, flags, timeNs, 0);

        if(ret != SOAPY_SDR_TIMEOUT)
        {
            if(ret == SOAPY_SDR_OVERFLOW)
                tx_overflows++;
            else if(ret == SOAPY_SDR_UNDERFLOW)
                tx_underflows++;
            else if(ret > 0)
                tx_n_samples += ret;
        }

        ret = sdr->readStream(rx_s, rx_bufs, rx_mtu, flags, timeNs, 0);

        if(ret != SOAPY_SDR_TIMEOUT)
        {
            if(ret == SOAPY_SDR_OVERFLOW)
                rx_overflows++;
            else if(ret == SOAPY_SDR_UNDERFLOW)
                rx_underflows++;
            else if(ret > 0)
                rx_n_samples += ret;
        }

        const auto now = std::chrono::high_resolution_clock::now();

        if(t_last_spin + std::chrono::milliseconds(300) < now)
        {
            static const char spin[] = {"|/-\\"};

            printf("\b%c", spin[(spin_i++) % 4]);
            fflush(stdout);

            t_last_spin = now;
        }

        if(t_last_print + std::chrono::seconds(5) < now)
        {
            const auto total_dt = std::chrono::duration_cast<std::chrono::microseconds>(now - t_start);
            const auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - t_last_print);

            tx_total_n_samples += tx_n_samples;

            const auto tx_sr = (double)tx_n_samples / dt.count();
            const auto total_tx_sr = (double)tx_total_n_samples / total_dt.count();
            const auto tx_dr = tx_sr * tx_buf.size() * tx_samp_sz;
            const auto total_tx_dr = total_tx_sr * tx_buf.size() * tx_samp_sz;

            tx_n_samples = 0;

            printf("\bTX: %g Msps (%g MB/s, %g Mbps) [Avg: %g Msps (%g MB/s, %g Mbps)]", tx_sr, tx_dr, tx_dr * 8, total_tx_sr, total_tx_dr, total_tx_dr * 8);

            if(tx_overflows != 0)
                printf("\tOverflows %lu", tx_overflows);

            if(tx_underflows != 0)
                printf("\tUnderflows %lu", tx_underflows);

            printf("\n ");

            rx_total_n_samples += rx_n_samples;

            const auto rx_sr = (double)rx_n_samples / dt.count();
            const auto total_rx_sr = (double)rx_total_n_samples / total_dt.count();
            const auto rx_dr = rx_sr * rx_buf.size() * rx_samp_sz;
            const auto total_rx_dr = total_rx_sr * rx_buf.size() * rx_samp_sz;

            rx_n_samples = 0;

            printf("\bRX: %g Msps (%g MB/s, %g Mbps) [Avg: %g Msps (%g MB/s, %g Mbps)]", rx_sr, rx_dr, rx_dr * 8, total_rx_sr, total_rx_dr, total_rx_dr * 8);

            if(rx_overflows != 0)
                printf("\tOverflows %lu", rx_overflows);

            if(rx_underflows != 0)
                printf("\tUnderflows %lu", rx_underflows);

            printf("\n ");

            for(const auto &sensor : sensors)
            {
                printf("\b-> ");
                printf(sensor.second.c_str(), sdr->readSensor(sensor.first).c_str());
                printf("\n ");
            }

            t_last_print = now;
        }
    }

    g_done = false;

    sdr->deactivateStream(rx_s);
    sdr->deactivateStream(tx_s);
    sdr->closeStream(rx_s);
    sdr->closeStream(tx_s);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);

    // if(!loadSoapyIcyRadio())
    // {
    //     std::cerr << "Could not find a suitable IcyRadio support module." << std::endl;

    //     return EXIT_FAILURE;
    // }

    SoapySDR::Kwargs args;

    bool do_tone_test = false;
    bool do_timed_tone_test = false;
    bool do_awgn_test = false;
    bool do_rfdelay_test = false;
    bool do_full_duplex_test = false;

    static struct option long_options[] =
    {
        {"serial", required_argument, nullptr, 'S'},
        {"flash-file", required_argument, nullptr, 'f'},
        {"test", required_argument, nullptr, 't'},
        {nullptr, no_argument, nullptr, '\0'}
    };

    int c;
    int option_index = 0;

    while((c = getopt_long(argc, argv, "S:f:t:", long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'S':
            {
                args["serial"] = optarg;
            }
            break;
            case 't':
            {
                if(std::string(optarg) == "tone")
                {
                    do_tone_test = true;
                }
                else if(std::string(optarg) == "timed_tone")
                {
                    do_timed_tone_test = true;
                }
                else if(std::string(optarg) == "awgn")
                {
                    do_awgn_test = true;
                }
                else if(std::string(optarg) == "rfdelay")
                {
                    do_rfdelay_test = true;
                }
                else if(std::string(optarg) == "full_duplex")
                {
                    do_full_duplex_test = true;
                }
                else
                {
                    std::cerr << "Unknown test: " << optarg << std::endl;

                    return EXIT_FAILURE;
                }
            }
            break;
            default:
                return EXIT_FAILURE;
        }
    }

    std::cout << "Making IcyRadio device";
    std::cout << (args.size() ? " (" : "");
    std::cout << (args.size() ? SoapySDR::KwargsToString(args) : "");
    std::cout << (args.size() ? ")" : "");
    std::cout << "..." << std::endl;

    SoapyIcyRadio *sdr = nullptr;

    try
    {
        SoapySDR::Device *dev = makeIcyRadio(args);

        sdr = dynamic_cast<SoapyIcyRadio *>(dev);
    }
    catch(const std::exception &ex)
    {
        std::cerr << "Failed to make IcyRadio device: " << ex.what() << std::endl;

        return EXIT_FAILURE;
    }

    if(sdr == nullptr)
    {
        std::cerr << "Made device is not an IcyRadio device." << std::endl;

        return EXIT_FAILURE;
    }

    std::cout << "IcyRadio device created." << std::endl;

    if(do_tone_test)
    {
        std::cout << "Running tone test..." << std::endl;

        testToneTX(sdr);
    }

    if(do_timed_tone_test)
    {
        std::cout << "Running timed tone test..." << std::endl;

        testTimedToneTX(sdr);
    }

    if(do_awgn_test)
    {
        std::cout << "Running AWGN test..." << std::endl;

        testAWGNTX(sdr);
    }

    if(do_rfdelay_test)
    {
        std::cout << "Running RF delay test..." << std::endl;

        testRFDelay(sdr);
    }

    if(do_full_duplex_test)
    {
        std::cout << "Running full duplex test..." << std::endl;

        testFullDuplex(sdr);
    }

    delete sdr;

    std::cout << "IcyRadio device destroyed." << std::endl;

    return EXIT_SUCCESS;
}