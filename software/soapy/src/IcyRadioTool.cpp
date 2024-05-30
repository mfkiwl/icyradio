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

void testToneTX(SoapyIcyRadio *sdr, double fc)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, 16 * 1024 * 1024);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, 12e6);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_RF", -30);
    sdr->setGain(SOAPY_SDR_TX, 1, "TX_RF", -30);

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
void testTimedToneTX(SoapyIcyRadio *sdr, double fc)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, 16 * 1024 * 1024);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, 12e6);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_RF", -30);
    sdr->setGain(SOAPY_SDR_TX, 1, "TX_RF", -30);

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
void testAWGNTX(SoapyIcyRadio *sdr, double fc, double fs)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, fs);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, 0.5 * fs);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_RF", -30);
    sdr->setGain(SOAPY_SDR_TX, 1, "TX_RF", -30);

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
void testRFDelay(SoapyIcyRadio *sdr, double fc, double fs)
{
    sdr->setSampleRate(SOAPY_SDR_TX, 0, fs);
    sdr->setBandwidth(SOAPY_SDR_TX, 0, fs / 8);
    sdr->setBandwidth(SOAPY_SDR_RX, 0, fs / 8);
    sdr->setFrequency(SOAPY_SDR_TX, 0, fc);
    sdr->setFrequency(SOAPY_SDR_RX, 0, fc);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setAntenna(SOAPY_SDR_RX, 0, "RX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_RF", -10);
    sdr->setGain(SOAPY_SDR_RX, 0, "RX_RF", 0);

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
void testFullDuplex(SoapyIcyRadio *sdr, size_t n_chan, double fs)
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

    if(n_chan > 2)
        n_chan = 2;

    sdr->setSampleRate(SOAPY_SDR_TX, 0, fs);
    sdr->setAntenna(SOAPY_SDR_TX, 0, "TX1A");
    sdr->setAntenna(SOAPY_SDR_RX, 0, "RX1A");
    sdr->setGain(SOAPY_SDR_TX, 0, "TX_RF", -89);
    sdr->setGain(SOAPY_SDR_RX, 0, "RX_RF", 0);

    std::vector<size_t> channels(n_chan);

    for(size_t i = 0; i < n_chan; i++)
        channels[i] = i;

    double tx_full_scale = 0;
    const std::string tx_fmt = sdr->getNativeStreamFormat(SOAPY_SDR_TX, 0, tx_full_scale);
    const size_t tx_samp_sz = SoapySDR::formatToSize(tx_fmt);
    auto tx_s = sdr->setupStream(SOAPY_SDR_TX, tx_fmt, channels);

    double rx_full_scale = 0;
    const std::string rx_fmt = sdr->getNativeStreamFormat(SOAPY_SDR_RX, 0, rx_full_scale);
    const size_t rx_samp_sz = SoapySDR::formatToSize(rx_fmt);
    auto rx_s = sdr->setupStream(SOAPY_SDR_RX, rx_fmt, channels);

    const size_t tx_mtu = sdr->getStreamMTU(tx_s);
    const size_t rx_mtu = sdr->getStreamMTU(rx_s);

    std::vector<std::vector<uint8_t>> tx_buf(n_chan, std::vector<uint8_t>(tx_samp_sz * tx_mtu)); // Native
    std::vector<void *> tx_bufs(n_chan);

    std::vector<std::vector<uint8_t>> rx_buf(n_chan, std::vector<uint8_t>(rx_samp_sz * rx_mtu)); // Native
    std::vector<void *> rx_bufs(n_chan);

    for(size_t i = 0; i < n_chan; i++)
    {
        tx_bufs[i] = tx_buf[i].data();
        rx_bufs[i] = rx_buf[i].data();
    }

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

    std::cout << "Starting Full Duplex stream loop at " << (size_t)fs << " sps (" << n_chan << " channel" << (n_chan != 1 ? "s" : "") << "), press Ctrl+C to exit..." << std::endl;

    sdr->activateStream(tx_s);
    sdr->activateStream(rx_s);

    while(!g_done)
    {
        int ret = 0;
        int flags = 0;
        long long timeNs = 0;

        ret = sdr->writeStream(tx_s, tx_bufs.data(), tx_mtu, flags, timeNs, 0);

        if(ret != SOAPY_SDR_TIMEOUT)
        {
            if(ret == SOAPY_SDR_OVERFLOW)
                tx_overflows++;
            else if(ret == SOAPY_SDR_UNDERFLOW)
                tx_underflows++;
            else if(ret > 0)
                tx_n_samples += ret;
        }

        while(true)
        {
            size_t chanMask;

            ret = sdr->readStreamStatus(tx_s, chanMask, flags, timeNs, 0);

            if(ret == SOAPY_SDR_OVERFLOW)
                tx_overflows++;
            else if(ret == SOAPY_SDR_UNDERFLOW)
                tx_underflows++;
            else if(ret == SOAPY_SDR_TIME_ERROR)
                {}
            else
                break;
        }

        ret = sdr->readStream(rx_s, rx_bufs.data(), rx_mtu, flags, timeNs, 0);

        if(ret != SOAPY_SDR_TIMEOUT)
        {
            if(ret == SOAPY_SDR_OVERFLOW)
                rx_overflows++;
            else if(ret == SOAPY_SDR_UNDERFLOW)
                rx_underflows++;
            else if(ret > 0)
                rx_n_samples += ret;
        }

        while(true)
        {
            size_t chanMask;

            ret = sdr->readStreamStatus(rx_s, chanMask, flags, timeNs, 0);

            if(ret == SOAPY_SDR_OVERFLOW)
                rx_overflows++;
            else if(ret == SOAPY_SDR_UNDERFLOW)
                rx_underflows++;
            else if(ret == SOAPY_SDR_TIME_ERROR)
                {}
            else
                break;
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
            const auto tx_dr = tx_sr * n_chan * tx_samp_sz;
            const auto total_tx_dr = total_tx_sr * n_chan * tx_samp_sz;

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
            const auto rx_dr = rx_sr * n_chan * rx_samp_sz;
            const auto total_rx_dr = total_rx_sr * n_chan * rx_samp_sz;

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
void testMMWaveSynth(SoapyIcyRadio *sdr, double f_pfd, double lf_rs, double lf_cs, double lf_cp, double lf_r3, double lf_c3, double l_bw, double i_cp, double i_cp_b, uint8_t p_a, uint8_t p_b, double f)
{
    sdr->mmw_synth->powerUp();

    if(f_pfd > 0.0)
        sdr->mmw_synth->configPFD(f_pfd);

    if(lf_rs > 0.0 && lf_cs > 0.0 && lf_cp > 0.0)
    {
        sdr->mmw_synth->setLoopFilter(
            {
                .rs = lf_rs,
                .cs = lf_cs,
                .cp = lf_cp,
                .r3 = lf_r3 > 0.0 ? lf_r3 : 0.0,
                .c3 = lf_c3 > 0.0 ? lf_c3 : 0.0,
            }
        );
    }

    if(l_bw > 0.0)
        sdr->mmw_synth->setTargetLoopBandwidth(l_bw);

    if(lf_rs > 0.0)
    {
        sdr->mmw_synth->setChargePumpPositiveCurrent(i_cp);
        sdr->mmw_synth->setChargePumpNegativeCurrent(i_cp);
    }

    if(i_cp > 0.0)
    {
        sdr->mmw_synth->setChargePumpPositiveCurrent(i_cp);
        sdr->mmw_synth->setChargePumpNegativeCurrent(i_cp);
    }

    if(i_cp_b > 0.0)
        sdr->mmw_synth->setChargePumpBleederCurrent(i_cp_b);

    sdr->mmw_synth->setFrequency(f, !(i_cp > 0.0));

    sdr->mmw_synth->setRFOutputPower(IDT8V97003::RFOutput::RFOUT_A, p_a);
    sdr->mmw_synth->enableRFOutput(IDT8V97003::RFOutput::RFOUT_A, p_a > 0);

    sdr->mmw_synth->setRFOutputPower(IDT8V97003::RFOutput::RFOUT_B, p_b);
    sdr->mmw_synth->enableRFOutput(IDT8V97003::RFOutput::RFOUT_B, p_b > 0);

    sdr->mmw_synth->unmute();

    std::cout << "Starting MMWave Synth loop, press Ctrl+C to exit..." << std::endl;
    std::cout << "> ";

    std::string prev_cmd = "";
    std::string cmd = "";

    while(!g_done)
    {
        char c = std::cin.get();

        if(c == '\n')
        {
            if(cmd.empty())
                cmd = prev_cmd;

            if(cmd == "help")
            {
                std::cout << "Commands:" << std::endl;
                std::cout << "  help: Show this help" << std::endl;
                std::cout << "  status: Show the chip status" << std::endl;
                std::cout << "  freq [+/-]<f>: Set the frequency to <f> Hz" << std::endl;
                std::cout << "  lf_bw <bw>: Set the target loop bandwidth to <bw> Hz" << std::endl;
                std::cout << "  lf rs=<rs>,cs=<cs>,cp=<cp>[,r3=<r3>,c3=<c3>]: Set the loop filter" << std::endl;
                std::cout << "  cpb <i>: Set the charge pump bleeder current to <i> A" << std::endl;
                std::cout << "  cp <i>: Set the charge pump positive and negative current to <i> A" << std::endl;
                std::cout << "  rfout_a <a>: Set the RF output power for the A output" << std::endl;
                std::cout << "  rfout_b <b>: Set the RF output power for the B output" << std::endl;
            }
            else if(cmd == "status")
            {
                std::cout << "Ref. Input: " << sdr->mmw_synth->getReferenceFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Ref. Doubler Input: " << sdr->mmw_synth->getReferenceDoublerInputFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Ref. Doubler Output: " << sdr->mmw_synth->getReferenceDoublerOutputFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Ref. Mult. Input: " << sdr->mmw_synth->getReferenceMultiplierInputFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Ref. Mult. Output: " << sdr->mmw_synth->getReferenceMultiplierOutputFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Ref. R-Div. Input: " << sdr->mmw_synth->getReferenceDividerInputFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Ref. R-Div. Output: " << sdr->mmw_synth->getReferenceDividerOutputFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "PFD: " << sdr->mmw_synth->getPFDFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "VCO: " << sdr->mmw_synth->getVCOFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Output: " << sdr->mmw_synth->getFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "FB Div.: " << sdr->mmw_synth->getFeedbackDivider() << std::endl;

                IDT8V97003::VCOBand vco = sdr->mmw_synth->getCurrentVCOBand();

                std::cout << "Current VCO: " << (size_t)vco.vco << std::endl;
                std::cout << "Current VCO Band: " << (size_t)vco.dig_band << std::endl;

                IDT8V97003::LoopFilter lf = sdr->mmw_synth->getLoopFilter();

                std::cout << "LF Rs: " << lf.rs << " Ohm" << std::endl;
                std::cout << "LF Cs: " << lf.cs << " F" << std::endl;
                std::cout << "LF Cp: " << lf.cp << " F" << std::endl;
                std::cout << "LF R3: " << lf.r3 << " Ohm" << std::endl;
                std::cout << "LF C3: " << lf.c3 << " F" << std::endl;

                std::cout << "Target Loop Bw.: " << sdr->mmw_synth->getTargetLoopBandwidth() * 1e-3 << " kHz" << std::endl;

                IDT8V97003::LoopFrequencyResponse lfr = sdr->mmw_synth->getLoopFrequencyResponse();

                std::cout << "Loop Zero: " << lfr.fz * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Bw: " << lfr.fc * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 1: " << lfr.fp * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 2: " << lfr.fp2 * 1e-3 << " kHz" << std::endl;

                IDT8V97003::ChargePumpConfig cp = sdr->mmw_synth->getChargePumpConfig();

                std::cout << "CP PMOS: " << cp.icp_pmos * 1e3 << " mA" << std::endl;
                std::cout << "CP NMOS: " << cp.icp_nmos * 1e3 << " mA" << std::endl;
                std::cout << "CP Bleeder: " << cp.icp_bleeder * 1e6 << " uA" << std::endl;

                std::cout << "RFOUT A: " << (size_t)sdr->mmw_synth->getRFOutputPower(IDT8V97003::RFOutput::RFOUT_A) << std::endl;
                std::cout << "RFOUT B: " << (size_t)sdr->mmw_synth->getRFOutputPower(IDT8V97003::RFOutput::RFOUT_B) << std::endl;
            }
            else if(cmd.substr(0, 4) == "freq")
            {
                double f = sdr->mmw_synth->getFrequency();

                if(cmd.at(5) == '+')
                    f += std::stod(cmd.substr(6));
                else if(cmd.at(5) == '-')
                    f -= std::stod(cmd.substr(6));
                else
                    f = std::stod(cmd.substr(5));

                try
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    sdr->mmw_synth->setFrequency(f, true, 5000, 1000000); // Wait for up to 1 second for the PLL to lock
                    auto lock_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

                    std::cout << "Lock time: " << lock_time.count() << " us" << std::endl;
                }
                catch(const std::exception &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }

                std::cout << "VCO: " << sdr->mmw_synth->getVCOFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "Output: " << sdr->mmw_synth->getFrequency() * 1e-6 << " MHz" << std::endl;
                std::cout << "FB Div.: " << sdr->mmw_synth->getFeedbackDivider() << std::endl;

                IDT8V97003::VCOBand vco = sdr->mmw_synth->getCurrentVCOBand();

                std::cout << "Current VCO: " << (size_t)vco.vco << std::endl;
                std::cout << "Current VCO Band: " << (size_t)vco.dig_band << std::endl;

                std::cout << "Target Loop Bw.: " << sdr->mmw_synth->getTargetLoopBandwidth() * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Bw: " << sdr->mmw_synth->getLoopBandwidth() * 1e-3 << " kHz" << std::endl;

                IDT8V97003::ChargePumpConfig cp = sdr->mmw_synth->getChargePumpConfig();

                std::cout << "CP PMOS: " << cp.icp_pmos * 1e3 << " mA" << std::endl;
                std::cout << "CP NMOS: " << cp.icp_nmos * 1e3 << " mA" << std::endl;
            }
            else if(cmd.substr(0, 5) == "lf_bw")
            {
                double bw = std::stod(cmd.substr(6));

                sdr->mmw_synth->setTargetLoopBandwidth(bw);
                sdr->mmw_synth->setLoopBandwidth();

                std::cout << "Target Loop Bw.: " << sdr->mmw_synth->getTargetLoopBandwidth() * 1e-3 << " kHz" << std::endl;

                IDT8V97003::LoopFrequencyResponse lfr = sdr->mmw_synth->getLoopFrequencyResponse();

                std::cout << "Loop Zero: " << lfr.fz * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Bw: " << lfr.fc * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 1: " << lfr.fp * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 2: " << lfr.fp2 * 1e-3 << " kHz" << std::endl;

                IDT8V97003::ChargePumpConfig cp = sdr->mmw_synth->getChargePumpConfig();

                std::cout << "CP PMOS: " << cp.icp_pmos * 1e3 << " mA" << std::endl;
                std::cout << "CP NMOS: " << cp.icp_nmos * 1e3 << " mA" << std::endl;
            }
            else if(cmd.substr(0, 2) == "lf")
            {
                SoapySDR::Kwargs args = SoapySDR::KwargsFromString(cmd.substr(3));

                if(args.empty())
                    continue;

                IDT8V97003::LoopFilter lf = sdr->mmw_synth->getLoopFilter();

                lf.rs = args.count("rs") > 0 ? std::stod(args.at("rs")) : lf.rs;
                lf.cs = args.count("cs") > 0 ? std::stod(args.at("cs")) : lf.cs;
                lf.cp = args.count("cp") > 0 ? std::stod(args.at("cp")) : lf.cp;
                lf.r3 = args.count("r3") > 0 ? std::stod(args.at("r3")) : lf.r3;
                lf.c3 = args.count("c3") > 0 ? std::stod(args.at("c3")) : lf.c3;

                sdr->mmw_synth->setLoopFilter(lf);
                sdr->mmw_synth->setLoopBandwidth();

                std::cout << "LF Rs: " << lf.rs << " Ohm" << std::endl;
                std::cout << "LF Cs: " << lf.cs << " F" << std::endl;
                std::cout << "LF Cp: " << lf.cp << " F" << std::endl;
                std::cout << "LF R3: " << lf.r3 << " Ohm" << std::endl;
                std::cout << "LF C3: " << lf.c3 << " F" << std::endl;

                std::cout << "Target Loop Bw.: " << sdr->mmw_synth->getTargetLoopBandwidth() * 1e-3 << " kHz" << std::endl;

                IDT8V97003::LoopFrequencyResponse lfr = sdr->mmw_synth->getLoopFrequencyResponse();

                std::cout << "Loop Zero: " << lfr.fz * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Bw: " << lfr.fc * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 1: " << lfr.fp * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 2: " << lfr.fp2 * 1e-3 << " kHz" << std::endl;

                IDT8V97003::ChargePumpConfig cp = sdr->mmw_synth->getChargePumpConfig();

                std::cout << "CP PMOS: " << cp.icp_pmos * 1e3 << " mA" << std::endl;
                std::cout << "CP NMOS: " << cp.icp_nmos * 1e3 << " mA" << std::endl;
            }
            else if(cmd.substr(0, 3) == "cpb")
            {
                double i = std::stod(cmd.substr(4));

                sdr->mmw_synth->setChargePumpBleederCurrent(i);

                std::cout << "CP Bleeder: " << sdr->mmw_synth->getChargePumpBleederCurrent() * 1e6 << " uA" << std::endl;
            }
            else if(cmd.substr(0, 2) == "cp")
            {
                double i = std::stod(cmd.substr(3));

                sdr->mmw_synth->setChargePumpPositiveCurrent(i);
                sdr->mmw_synth->setChargePumpNegativeCurrent(i);

                IDT8V97003::LoopFrequencyResponse lfr = sdr->mmw_synth->getLoopFrequencyResponse();

                std::cout << "Loop Zero: " << lfr.fz * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Bw: " << lfr.fc * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 1: " << lfr.fp * 1e-3 << " kHz" << std::endl;
                std::cout << "Loop Pole 2: " << lfr.fp2 * 1e-3 << " kHz" << std::endl;

                IDT8V97003::ChargePumpConfig cp = sdr->mmw_synth->getChargePumpConfig();

                std::cout << "CP PMOS: " << cp.icp_pmos * 1e3 << " mA" << std::endl;
                std::cout << "CP NMOS: " << cp.icp_nmos * 1e3 << " mA" << std::endl;
            }
            else if(cmd.substr(0, 7) == "rfout_a")
            {
                uint8_t p = std::stoul(cmd.substr(8));

                sdr->mmw_synth->setRFOutputPower(IDT8V97003::RFOutput::RFOUT_A, p);
                sdr->mmw_synth->enableRFOutput(IDT8V97003::RFOutput::RFOUT_A, p > 0);

                std::cout << "RFOUT A: " << (size_t)sdr->mmw_synth->getRFOutputPower(IDT8V97003::RFOutput::RFOUT_A) << std::endl;
            }
            else if(cmd.substr(0, 7) == "rfout_b")
            {
                uint8_t p = std::stoul(cmd.substr(8));

                sdr->mmw_synth->setRFOutputPower(IDT8V97003::RFOutput::RFOUT_B, p);
                sdr->mmw_synth->enableRFOutput(IDT8V97003::RFOutput::RFOUT_B, p > 0);

                std::cout << "RFOUT B: " << (size_t)sdr->mmw_synth->getRFOutputPower(IDT8V97003::RFOutput::RFOUT_B) << std::endl;
            }

            prev_cmd = cmd;
            cmd = "";
            std::cout << "> ";

            fflush(stdout);
        }
        else
        {
            cmd += c;
        }
    }

    g_done = false;

    sdr->mmw_synth->mute();
    sdr->mmw_synth->powerDown();
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
    bool do_mmw_synth_test = false;
    SoapySDR::Kwargs test_args;

    static struct option long_options[] =
    {
        {"serial", required_argument, nullptr, 'S'},
        {"flash-file", required_argument, nullptr, 'f'},
        {"test", required_argument, nullptr, 't'},
        {"test-args", required_argument, nullptr, 'a'},
        {nullptr, no_argument, nullptr, '\0'}
    };

    int c;
    int option_index = 0;

    while((c = getopt_long(argc, argv, "S:f:t:a:", long_options, &option_index)) != -1)
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
                else if(std::string(optarg) == "mmw_synth")
                {
                    do_mmw_synth_test = true;
                }
                else
                {
                    std::cerr << "Unknown test: " << optarg << std::endl;

                    return EXIT_FAILURE;
                }
            }
            break;
            case 'a':
            {
                test_args = SoapySDR::KwargsFromString(optarg);
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
        double fc = test_args.count("fc") > 0 ? std::stod(test_args.at("fc")) : 480e6;

        std::cout << "Running tone test..." << std::endl;

        testToneTX(sdr, fc);
    }

    if(do_timed_tone_test)
    {
        double fc = test_args.count("fc") > 0 ? std::stod(test_args.at("fc")) : 480e6;

        std::cout << "Running timed tone test..." << std::endl;

        testTimedToneTX(sdr, fc);
    }

    if(do_awgn_test)
    {
        double fc = test_args.count("fc") > 0 ? std::stod(test_args.at("fc")) : 480e6;
        double fs = test_args.count("fs") > 0 ? std::stod(test_args.at("fs")) : 3e6;

        std::cout << "Running AWGN test..." << std::endl;

        testAWGNTX(sdr, fc, fs);
    }

    if(do_rfdelay_test)
    {
        double fc = test_args.count("fc") > 0 ? std::stod(test_args.at("fc")) : 480e6;
        double fs = test_args.count("fs") > 0 ? std::stod(test_args.at("fs")) : 61.44e6;

        std::cout << "Running RF delay test..." << std::endl;

        testRFDelay(sdr, fc, fs);
    }

    if(do_full_duplex_test)
    {
        size_t n_chan = test_args.count("n_chan") > 0 ? std::stod(test_args.at("n_chan")) : 2;
        double fs = test_args.count("fs") > 0 ? std::stod(test_args.at("fs")) : 30.72e6;

        std::cout << "Running full duplex test..." << std::endl;

        testFullDuplex(sdr, n_chan, fs);
    }

    if(do_mmw_synth_test)
    {
        double f_pfd = test_args.count("f_pfd") > 0 ? std::stod(test_args.at("f_pfd")) : 0;
        double lf_rs = test_args.count("lf_rs") > 0 ? std::stod(test_args.at("lf_rs")) : 0;
        double lf_cs = test_args.count("lf_cs") > 0 ? std::stod(test_args.at("lf_cs")) : 0;
        double lf_cp = test_args.count("lf_cp") > 0 ? std::stod(test_args.at("lf_cp")) : 0;
        double lf_r3 = test_args.count("lf_r3") > 0 ? std::stod(test_args.at("lf_r3")) : 0;
        double lf_c3 = test_args.count("lf_c3") > 0 ? std::stod(test_args.at("lf_c3")) : 0;
        double l_bw = test_args.count("l_bw") > 0 ? std::stod(test_args.at("l_bw")) : 0;
        double i_cp = test_args.count("i_cp") > 0 ? std::stod(test_args.at("i_cp")) : 0;
        double i_cp_b = test_args.count("i_cp_b") > 0 ? std::stod(test_args.at("i_cp_b")) : 0;
        uint8_t p_a = test_args.count("p_a") > 0 ? std::stod(test_args.at("p_a")) : 0;
        uint8_t p_b = test_args.count("p_b") > 0 ? std::stod(test_args.at("p_b")) : 0;
        double f = test_args.count("f") > 0 ? std::stod(test_args.at("f")) : 8e9;

        std::cout << "Running mmWave synthesizer test..." << std::endl;

        testMMWaveSynth(sdr, f_pfd, lf_rs, lf_cs, lf_cp, lf_r3, lf_c3, l_bw, i_cp, i_cp_b, p_a, p_b, f);
    }

    delete sdr;

    std::cout << "IcyRadio device destroyed." << std::endl;

    return EXIT_SUCCESS;
}