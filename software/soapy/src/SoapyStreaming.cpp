#include "SoapyIcyRadio.hpp"

std::vector<std::string> SoapyIcyRadio::getStreamFormats(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getStreamFormats: Unknown direction");

    if(!this->isChannelValid(direction, channel))
        throw std::runtime_error("getStreamFormats: Invalid channel specified");

    double full_scale;
    std::string native = this->getNativeStreamFormat(direction, channel, full_scale);

    (void)full_scale; // Suppress unused warning

    std::vector<std::string> formats;

    if(direction == SOAPY_SDR_RX)
        formats = SoapySDR::ConverterRegistry::listTargetFormats(native);
    else if(direction == SOAPY_SDR_TX)
        formats = SoapySDR::ConverterRegistry::listSourceFormats(native);

    if(formats.empty())
        formats.push_back(native);

    return formats;
}
std::string SoapyIcyRadio::getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getNativeStreamFormat: Unknown direction");

    if(!this->isChannelValid(direction, channel))
        throw std::runtime_error("getNativeStreamFormat: Invalid channel specified");

    fullScale = (direction == SOAPY_SDR_RX) ? 2048 : 32768;

	return SOAPY_SDR_CS16;
}
SoapySDR::ArgInfoList SoapyIcyRadio::getStreamArgsInfo(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getStreamArgsInfo: Unknown direction");

    if(!this->isChannelValid(direction, channel))
        throw std::runtime_error("getStreamArgsInfo: Invalid channel specified");

    SoapySDR::ArgInfoList args;

    return args;
}
SoapySDR::Stream *SoapyIcyRadio::setupStream(const int direction, const std::string &format, const std::vector<size_t> &channels, const SoapySDR::Kwargs &args)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setupStream: Unknown direction");

    if(channels.empty())
        throw std::runtime_error("setupStream: No channels specified");

    if(!this->isChannelVectorValid(direction, channels))
        throw std::runtime_error("setupStream: Invalid channel(s) specified");

    std::lock_guard<std::recursive_mutex> lock(this->streams_mutex);

    DLOGF(SOAPY_SDR_DEBUG, "setupStream: %s, %u channels, %s (%s)", (direction == SOAPY_SDR_RX) ? "RX" : "TX", channels.size(), format.c_str(), SoapySDR::KwargsToString(args).c_str());

    if(this->isAnyChannelBusy(direction, channels))
        throw std::runtime_error("setupStream: One or more channels in this direction are already in use");

    // Check if datapath needs reconfiguration and if we can do it
    auto rx_chs = this->getBusyChannels(SOAPY_SDR_RX);
    auto tx_chs = this->getBusyChannels(SOAPY_SDR_TX);

    for(size_t c : channels)
    {
        if(direction == SOAPY_SDR_RX)
            rx_chs.push_back(c);
        else if(direction == SOAPY_SDR_TX)
            tx_chs.push_back(c);
    }

    size_t num_chans = MAX(rx_chs.size(), tx_chs.size());

    this->validateSampleRateAndChannelCombination(this->getSampleRate(direction, channels[0]), num_chans);

    if(this->requiresDataPathReconfiguration(rx_chs, tx_chs))
    {
        if(this->getStreams(true).size() > 0)
            throw std::runtime_error("setupStream: New stream requires datapath reconfiguration, but there are active streams");

        DLOGF(SOAPY_SDR_DEBUG, "setupStream: Reconfiguring data path for new channel count %u", num_chans);

        if(num_chans > 1)
            this->reconfigureDataPath(true);
        else
            this->reconfigureDataPath(false, rx_chs.size() > 0 ? rx_chs[0] : 0, tx_chs.size() > 0 ? tx_chs[0] : 0);

        auto streams = this->getStreams();

        for(auto &s : streams)
            this->reinitStreamChannels(s);
    }

    std::unique_ptr<SoapyIcyRadio::Stream> s = std::make_unique<SoapyIcyRadio::Stream>();

    s->direction = direction;
    s->active = false;

    // Setup channels
    s->channels = std::vector<SoapyIcyRadio::Stream::Channel *>();

    this->initStreamChannels(s.get(), channels);

    // Determine if and what converter is needed
    double full_scale;
    std::string native = this->getNativeStreamFormat(direction, channels[0], full_scale);

    if(format != native)
    {
        if(direction == SOAPY_SDR_RX)
        {
            s->fmt_convert = SoapySDR::ConverterRegistry::getFunction(native, format);
            s->fmt_scale = 1.0 / full_scale;
        }
        else if(direction == SOAPY_SDR_TX)
        {
            s->fmt_convert = SoapySDR::ConverterRegistry::getFunction(format, native);
            s->fmt_scale = full_scale;
        }

        if(s->fmt_convert == nullptr)
            throw std::runtime_error("setupStream: Format " + format + " not supported");

        DLOGF(SOAPY_SDR_DEBUG, "setupStream: Converting from %s to %s", direction == SOAPY_SDR_RX ? native.c_str() : format.c_str(), direction == SOAPY_SDR_RX ? format.c_str() : native.c_str());
    }
    else
    {
        s->fmt_convert = nullptr;

        DLOGF(SOAPY_SDR_DEBUG, "setupStream: Using native format %s", native.c_str());
    }

    s->cur_buf_handle = 0;
    s->cur_buf_ptrs = std::vector<void *>(channels.size(), nullptr);
    s->cur_buf_size = 0;
    s->cur_buf_valid_size = 0;
    s->cur_buf_time = 0;
    s->cur_buf_time_valid = false;

    // Add stream to list
    SoapyIcyRadio::Stream *_s = s.release();

    this->streams.push_back(_s);

    return reinterpret_cast<SoapySDR::Stream *>(_s);
}
void SoapyIcyRadio::closeStream(SoapySDR::Stream *stream)
{
    std::lock_guard<std::recursive_mutex> lock(this->streams_mutex);

    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("closeStream: Stream not found");

    {
        std::lock_guard<std::mutex> lock(s->mutex);

        if(s->active)
            this->deactivateStream(stream);
    }

    std::remove(this->streams.begin(), this->streams.end(), s);

    delete s;
}
size_t SoapyIcyRadio::getStreamMTU(SoapySDR::Stream *stream) const
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("getStreamMTU: Stream not found");

    return ICYRADIO_DEFAULT_DMA_BUFFER_SIZE_SAMPLES; // TODO: Make configurable
}
int SoapyIcyRadio::activateStream(SoapySDR::Stream *stream, const int flags, const long long timeNs, const size_t numElems)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("activateStream: Stream not found");

    std::lock_guard<std::mutex> lock(s->mutex);

    DLOGF_S(SOAPY_SDR_DEBUG, "activateStream: Stream direction is %s", s->direction == SOAPY_SDR_RX ? "RX" : "TX");

    if(s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "activateStream: Stream is already active");

        return SOAPY_SDR_STREAM_ERROR;
    }

    uint64_t ticks = 0;

    if(flags & SOAPY_SDR_HAS_TIME)
    {
        if(s->direction != SOAPY_SDR_RX)
            return SOAPY_SDR_NOT_SUPPORTED;

        if(timeNs <= this->getHardwareTime("rf_timestamp"))
            return SOAPY_SDR_TIME_ERROR;

        ticks = SoapySDR::timeNsToTicks(timeNs, this->getSampleRate(s->direction, 0));
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "activateStream: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    s->cur_buf_handle = 0;
    s->cur_buf_ptrs = std::vector<void *>(s->channels.size(), nullptr);
    s->cur_buf_time = 0;
    s->cur_buf_time_valid = false;

    for(auto &c : s->channels)
    {
        DLOGF_S(SOAPY_SDR_TRACE, "activateStream: Channel %u, invalidate %u user buffers", c->num, c->buffers.size());

        for(auto &buf : c->buffers)
        {
            buf->valid_size = 0;
            buf->time = 0;
            buf->time_valid = false;
            buf->acquired = false;
        }

        c->next_user_buf = 0;
        c->next_user_buf_time = 0;
        c->next_user_buf_time_valid = false;
        c->next_dma_user_buf = 0;
        c->next_dma_user_buf_time = 0;
        c->next_dma_user_buf_time_valid = false;

        DLOGF_S(SOAPY_SDR_TRACE, "activateStream: DMA Controller %u", c->dma->getPeripheralID());

        if(s->direction == SOAPY_SDR_TX)
            this->axi_rf_tstamp->flushTX(c->ts_chan, 100); // Flush pending DMA transfers

        c->dma->waitIdle();

        if(s->direction == SOAPY_SDR_RX)
            c->dma->enable();

        for(auto &buf : c->dma_buffers)
        {
            if(s->direction == SOAPY_SDR_RX)
            {
                c->dma->submitTransfer(buf->xfer);

                buf->idle = false;

                DLOGF_S(SOAPY_SDR_TRACE, "activateStream: Submitted transfer ID %u", buf->xfer.id);
            }
            else
            {
                buf->idle = true;
            }
        }
    }

    if(s->direction == SOAPY_SDR_RX)
    {
        if(s->channels.size() > 1) // Multi channel, atomically work on both channels
        {
            this->axi_rf_tstamp->getCounterLatch(AXIRFTStamp::Channel::RX0); // Trigger read to clear valid status
            this->axi_rf_tstamp->getCounterLatch(AXIRFTStamp::Channel::RX1); // Trigger read to clear valid status
            this->axi_rf_tstamp->armCounterLatch();

            // Only check RX1, RX0 will be set aswell
            while(this->axi_rf_tstamp->getCounterLatchStatus(AXIRFTStamp::Channel::RX1) != AXIRFTStamp::CounterLatchStatus::LATCH_ARMED)
                std::this_thread::yield();

            // Only check the last channel, the first will be set aswell
            this->axi_rf_tstamp->waitRXDMAReady(s->channels.end()[-1]->ts_chan, 100);

            if(flags & SOAPY_SDR_HAS_TIME)
            {
                this->axi_rf_tstamp->setRXCounter(AXIRFTStamp::Channel::RX0, ticks);
                this->axi_rf_tstamp->setRXCounter(AXIRFTStamp::Channel::RX1, ticks);
                this->axi_rf_tstamp->enableRXCounter(); // Atomically enables both counters
            }
            else
            {
                this->axi_rf_tstamp->enableRX(); // Atomically enables both channels
            }
        }
        else // Single channel
        {
            AXIRFTStamp::Channel ts_chan = s->channels[0]->ts_chan;

            this->axi_rf_tstamp->getCounterLatch(ts_chan); // Trigger read to clear valid status
            this->axi_rf_tstamp->armCounterLatch(ts_chan);

            while(this->axi_rf_tstamp->getCounterLatchStatus(ts_chan) != AXIRFTStamp::CounterLatchStatus::LATCH_ARMED)
                std::this_thread::yield();

            if(flags & SOAPY_SDR_HAS_TIME)
            {
                this->axi_rf_tstamp->setRXCounter(ts_chan, ticks);
                this->axi_rf_tstamp->enableRXCounter(ts_chan);
            }
            else
            {
                this->axi_rf_tstamp->enableRX(ts_chan);
            }
        }
    }

    s->active = true;

    return 0;
}
int SoapyIcyRadio::deactivateStream(SoapySDR::Stream *stream, const int flags, const long long timeNs)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("deactivateStream: Stream not found");

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "deactivateStream: Stream is not active");

        return 0;
    }

    if(flags & SOAPY_SDR_HAS_TIME)
    {
        return SOAPY_SDR_NOT_SUPPORTED; // Not supported for now
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "deactivateStream: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    if(s->channels.size() > 1) // Multi channel, atomically work on both channels
    {
        if(s->direction == SOAPY_SDR_RX)
        {
            this->axi_rf_tstamp->disableRXCounter(); // Atomically disables both counters
            this->axi_rf_tstamp->disableRX(); // Atomically disables both channels
            this->axi_rf_tstamp->waitRXCounterDisabled(100);
            this->axi_rf_tstamp->waitRXDisabled(100);

            this->axi_rf_tstamp->disarmCounterLatch();
            this->axi_rf_tstamp->getCounterLatch(AXIRFTStamp::Channel::RX0); // Trigger read to clear valid status
            this->axi_rf_tstamp->getCounterLatch(AXIRFTStamp::Channel::RX1); // Trigger read to clear valid status
        }
        else
        {
            this->axi_rf_tstamp->disableTXCounter(); // Atomically disables both counters
            this->axi_rf_tstamp->disableTX(); // Atomically disables both channels
            this->axi_rf_tstamp->waitTXCounterDisabled(100);
            this->axi_rf_tstamp->waitTXDisabled(100);
        }
    }
    else // Single channel
    {
        AXIRFTStamp::Channel ts_chan = s->channels[0]->ts_chan;

        if(s->direction == SOAPY_SDR_RX)
        {
            this->axi_rf_tstamp->disableRXCounter(ts_chan);
            this->axi_rf_tstamp->disableRX(ts_chan);
            this->axi_rf_tstamp->waitRXCounterDisabled(ts_chan, 100);
            this->axi_rf_tstamp->waitRXDisabled(ts_chan, 100);

            this->axi_rf_tstamp->disarmCounterLatch(ts_chan);
            this->axi_rf_tstamp->getCounterLatch(ts_chan); // Trigger read to clear valid status
        }
        else
        {
            this->axi_rf_tstamp->disableTXCounter(ts_chan);
            this->axi_rf_tstamp->disableTX(ts_chan);
            this->axi_rf_tstamp->waitTXCounterDisabled(ts_chan, 100);
            this->axi_rf_tstamp->waitTXDisabled(ts_chan, 100);
        }
    }

    for(auto &c : s->channels)
    {
        DLOGF_S(SOAPY_SDR_DEBUG, "deactivateStream: Channel %u, DMA Controller %u", c->num, c->dma->getPeripheralID());

        for(auto &buf : c->dma_buffers)
        {
            c->dma->setTransferCallback(buf->xfer.id, nullptr);
        }

        c->dma->disable();

        if(s->direction == SOAPY_SDR_TX)
            this->axi_rf_tstamp->flushTX(c->ts_chan, 100);

        c->dma->waitIdle();
    }

    {
        std::lock_guard<std::mutex> lock(s->cur_buf_mutex);

        s->cur_buf_handle = 0;
        s->cur_buf_ptrs = std::vector<void *>(s->channels.size(), nullptr);
        s->cur_buf_time = 0;
        s->cur_buf_time_valid = false;
    }

    s->active = false;

    return 0;
}
int SoapyIcyRadio::readStream(SoapySDR::Stream *stream, void * const *buffs, const size_t numElems, int &flags, long long &timeNs, const long timeoutUs)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("readStream: Stream not found");

    if(s->direction != SOAPY_SDR_RX)
        return SOAPY_SDR_NOT_SUPPORTED;

    if(!s->active)
        this->activateStream(stream);

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "readStream: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "readStream: Reading %u elements", numElems);

    bool buf_acquired = false;

    std::lock_guard<std::mutex> lock(s->cur_buf_mutex);

    if(s->cur_buf_ptrs[0] == nullptr)
    {
        DLOGF_S(SOAPY_SDR_TRACE, "readStream: No current buffer, waiting for one...");

        size_t _handle;
        void *_buffs[s->channels.size()];
        int _flags;
        long long _timeNs;

        int _rem = this->acquireReadBuffer(stream, _handle, (const void **)_buffs, _flags, _timeNs, timeoutUs);

        if(_rem < 0)
            return _rem;

        s->cur_buf_handle = _handle;

        for(size_t i = 0; i < s->cur_buf_ptrs.size(); i++)
            s->cur_buf_ptrs[i] = _buffs[i];

        s->cur_buf_size = _rem;
        s->cur_buf_valid_size = _rem;
        s->cur_buf_time = _timeNs;
        s->cur_buf_time_valid = (flags & SOAPY_SDR_HAS_TIME);

        DLOGF_S(SOAPY_SDR_TRACE, "readStream: Got buffer %u with %u elements", s->cur_buf_handle, s->cur_buf_size);

        buf_acquired = true;
    }
    else
    {
        DLOGF_S(SOAPY_SDR_TRACE, "readStream: Using current buffer %u, still has %u elements", s->cur_buf_handle, s->cur_buf_valid_size);

        buf_acquired = false;
    }

    size_t _numElems = MIN(numElems, s->cur_buf_valid_size);

    for(size_t i = 0; i < s->channels.size(); i++)
    {
        if(s->fmt_convert)
            s->fmt_convert(s->cur_buf_ptrs[i], buffs[i], _numElems, s->fmt_scale);
        else
            std::memcpy(buffs[i], s->cur_buf_ptrs[i], _numElems * ICYRADIO_SAMPLE_SIZE_BYTES);
    }

    if(s->cur_buf_time_valid)
    {
        timeNs = s->cur_buf_time;
        flags |= SOAPY_SDR_HAS_TIME;
    }

    s->cur_buf_valid_size -= _numElems;

    if(!s->cur_buf_valid_size)
    {
        DLOGF_S(SOAPY_SDR_TRACE, "readStream: Current buffer is exhausted, releasing...");

        this->releaseReadBuffer(stream, s->cur_buf_handle);

        for(size_t i = 0; i < s->cur_buf_ptrs.size(); i++)
            s->cur_buf_ptrs[i] = nullptr;

        s->cur_buf_size = 0;
        s->cur_buf_handle = 0;
        s->cur_buf_time = 0;
        s->cur_buf_time_valid = false;

        if(buf_acquired)
            flags |= SOAPY_SDR_ONE_PACKET;
        else
            flags |= SOAPY_SDR_END_BURST;
    }
    else
    {
        for(size_t i = 0; i < s->cur_buf_ptrs.size(); i++)
            s->cur_buf_ptrs[i] = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(s->cur_buf_ptrs[i]) + _numElems * ICYRADIO_SAMPLE_SIZE_BYTES);

        if(s->cur_buf_time_valid)
            s->cur_buf_time += SoapySDR::ticksToTimeNs(_numElems, this->getSampleRate(SOAPY_SDR_RX, 0));
    }

    return _numElems;
}
int SoapyIcyRadio::writeStream(SoapySDR::Stream *stream, const void * const *buffs, const size_t numElems, int &flags, const long long timeNs, const long timeoutUs)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("writeStream: Stream not found");

    if(s->direction != SOAPY_SDR_TX)
        return SOAPY_SDR_NOT_SUPPORTED;

    if(!s->active)
        this->activateStream(stream);

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "writeStream: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "writeStream: Writting %u elements", numElems);

    std::lock_guard<std::mutex> lock(s->cur_buf_mutex);

    if(s->cur_buf_ptrs[0] == nullptr)
    {
        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: No current buffer, waiting for one...");

        size_t _handle;
        void *_buffs[s->channels.size()];

        int _rem = this->acquireWriteBuffer(stream, _handle, _buffs, timeoutUs);

        if(_rem < 0)
            return _rem;

        s->cur_buf_handle = _handle;

        for(size_t i = 0; i < s->cur_buf_ptrs.size(); i++)
            s->cur_buf_ptrs[i] = _buffs[i];

        s->cur_buf_size = _rem;
        s->cur_buf_valid_size = 0;
        s->cur_buf_time = 0;
        s->cur_buf_time_valid = false;

        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: Got buffer %u for %u elements", s->cur_buf_handle, s->cur_buf_size);
    }
    else
    {
        if(flags & SOAPY_SDR_ONE_PACKET)
        {
            DLOGF(SOAPY_SDR_WARNING, "writeStream: Previous burst not finished, ignoring single packet flag");

            flags &= ~SOAPY_SDR_ONE_PACKET;
        }

        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: Using current buffer %u, still has space for %u elements", s->cur_buf_handle, s->cur_buf_size - s->cur_buf_valid_size);
    }

    size_t _numElems = MIN(numElems, s->cur_buf_size - s->cur_buf_valid_size);

    for(size_t i = 0; i < s->channels.size(); i++)
    {
        if(s->fmt_convert)
            s->fmt_convert(buffs[i], s->cur_buf_ptrs[i], _numElems, s->fmt_scale);
        else
            std::memcpy(s->cur_buf_ptrs[i], buffs[i], _numElems * ICYRADIO_SAMPLE_SIZE_BYTES);
    }

    if(flags & SOAPY_SDR_HAS_TIME)
    {
        if(timeNs <= this->getHardwareTime("rf_timestamp"))
            return SOAPY_SDR_TIME_ERROR; // OK to return here, cur_buf_valid_size will not be incremented

        if(!s->cur_buf_time_valid)
        {
            s->cur_buf_time = timeNs;
            s->cur_buf_time_valid = true;
        }
        else
        {
            DLOGF(SOAPY_SDR_WARNING, "writeStream: Ignoring timeNs %llu, using previously set value %llu", timeNs, s->cur_buf_time);
        }
    }

    s->cur_buf_valid_size += _numElems;

    bool release_buf = false;

    if(s->cur_buf_valid_size == s->cur_buf_size)
    {
        release_buf = true;

        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: Current buffer is full, releasing...");
    }
    else if(flags & SOAPY_SDR_END_BURST)
    {
        release_buf = true;

        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: End of burst, releasing...");
    }
    else if(flags & SOAPY_SDR_ONE_PACKET)
    {
        release_buf = true;

        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: Single packet, releasing...");
    }

    if(release_buf)
    {
        DLOGF_S(SOAPY_SDR_TRACE, "writeStream: Releasing buffer with %u elements", s->cur_buf_valid_size);

        int _flags = 0;
        long long _timeNs = 0;

        if(s->cur_buf_time_valid)
        {
            _flags = SOAPY_SDR_HAS_TIME;
            _timeNs = s->cur_buf_time;
        }

        this->releaseWriteBuffer(stream, s->cur_buf_handle, s->cur_buf_valid_size, _flags, _timeNs);

        for(size_t i = 0; i < s->cur_buf_ptrs.size(); i++)
            s->cur_buf_ptrs[i] = nullptr;

        s->cur_buf_size = 0;
        s->cur_buf_valid_size = 0;
        s->cur_buf_handle = 0;
        s->cur_buf_time = 0;
        s->cur_buf_time_valid = false;
    }
    else
    {
        for(size_t i = 0; i < s->cur_buf_ptrs.size(); i++)
            s->cur_buf_ptrs[i] = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(s->cur_buf_ptrs[i]) + _numElems * ICYRADIO_SAMPLE_SIZE_BYTES);
    }

    return _numElems;
}
int SoapyIcyRadio::readStreamStatus(SoapySDR::Stream *stream, size_t &chanMask, int &flags, long long &timeNs, const long timeoutUs)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

size_t SoapyIcyRadio::getNumDirectAccessBuffers(SoapySDR::Stream *stream)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("getNumDirectAccessBuffers: Stream not found");

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "getNumDirectAccessBuffers: Stream is not active");

        return 0;
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "getNumDirectAccessBuffers: No channels in this stream");

        return 0;
    }

    if(!s->channels[0]->buffers.size())
    {
        DLOGF(SOAPY_SDR_ERROR, "getNumDirectAccessBuffers: No user buffers in this stream");

        return 0;
    }

    return s->channels[0]->buffers.size();
}
int SoapyIcyRadio::getDirectAccessBufferAddrs(SoapySDR::Stream *stream, const size_t handle, void **buffs)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("getDirectAccessBufferAddrs: Stream not found");

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "getDirectAccessBufferAddrs: Stream is not active");

        return SOAPY_SDR_STREAM_ERROR;
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "getDirectAccessBufferAddrs: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "getDirectAccessBufferAddrs: Get user buffer %u addresses", handle);

    for(auto &c : s->channels)
    {
        std::lock_guard<std::mutex> lock(c->mutex);

        if(handle >= c->buffers.size())
            DLOGF(SOAPY_SDR_WARNING, "getDirectAccessBufferAddrs: Invalid buffer %u for channel %u", handle, c->num);

        if(s->direction == SOAPY_SDR_RX && !c->buffers[handle]->valid_size)
            DLOGF(SOAPY_SDR_WARNING, "getDirectAccessBufferAddrs: User owns buffer %u for channel %u but it contains no valid data", handle, c->num);
        else if(s->direction == SOAPY_SDR_TX && c->buffers[handle]->valid_size)
            DLOGF(SOAPY_SDR_WARNING, "getDirectAccessBufferAddrs: User owns buffer %u for channel %u but it contains valid data", handle, c->num);

        if(!c->buffers[handle]->acquired)
        {
            DLOGF(SOAPY_SDR_WARNING, "getDirectAccessBufferAddrs: Buffer %u for channel %u is not acquired", handle, c->num);

            return SOAPY_SDR_NOT_SUPPORTED;
        }

        *buffs++ = c->buffers[handle]->addr;
    }

    return 0;
}
int SoapyIcyRadio::acquireReadBuffer(SoapySDR::Stream *stream, size_t &handle, const void **buffs, int &flags, long long &timeNs, const long timeoutUs)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("acquireReadBuffer: Stream not found");

    if(s->direction != SOAPY_SDR_RX)
        return SOAPY_SDR_NOT_SUPPORTED;

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "acquireReadBuffer: Stream is not active");

        return SOAPY_SDR_STREAM_ERROR;
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "acquireReadBuffer: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    size_t _handle;
    long _timeoutUs = !timeoutUs ? 1 : timeoutUs;

    while(_timeoutUs)
    {
        bool all_acquired = true;

        _handle = s->channels[0]->next_user_buf;

        for(const auto &c : s->channels)
        {
            std::lock_guard<std::mutex> lock(c->mutex);

            if(_handle != c->next_user_buf)
            {
                all_acquired = false;

                break;
            }

            if(!c->buffers[_handle]->valid_size)
            {
                all_acquired = false;

                break;
            }
        }

        if(all_acquired)
            break;

        usleep(10);

        if(_timeoutUs <= 10)
        {
            if(timeoutUs > 0)
                DLOGF(SOAPY_SDR_WARNING, "acquireReadBuffer: Timeout waiting for a buffer with valid data");

            return SOAPY_SDR_TIMEOUT;
        }

        _timeoutUs -= 10;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "acquireReadBuffer: Checking user buffer %u for any overflow", _handle);

    bool any_overflow = false;

    for(auto &c : s->channels)
    {
        std::lock_guard<std::mutex> lock(c->mutex);

        if(!c->buffers[_handle]->time_valid)
        {
            DLOGF(SOAPY_SDR_WARNING, "acquireReadBuffer: Buffer %u for channel %u has no valid timestamp", _handle, c->num);

            continue;
        }

        if(c->next_user_buf_time_valid && c->buffers[_handle]->time > c->next_user_buf_time)
        {
            DLOGF(SOAPY_SDR_WARNING, "acquireReadBuffer: Buffer %u for channel %u has timestamp %llu, expected %llu (delta %lld)", _handle, c->num, c->buffers[_handle]->time, c->next_user_buf_time, c->buffers[_handle]->time - c->next_user_buf_time);

            any_overflow = true;

            break;
        }

        c->next_user_buf_time = c->buffers[_handle]->time + c->buffers[_handle]->valid_size / ICYRADIO_SAMPLE_SIZE_BYTES; // Next buffer should be this one + buffer size
        c->next_user_buf_time_valid = true;
    }

    if(any_overflow)
    {
        DLOGF(SOAPY_SDR_WARNING, "acquireReadBuffer: Buffer %u has overflowed in at least one channel, aborting", _handle);

        for(auto &c : s->channels)
        {
            std::lock_guard<std::mutex> lock(c->mutex);

            // Invalidate all expected timestamps so we start over
            c->next_user_buf_time = 0;
            c->next_user_buf_time_valid = false;
        }

        return SOAPY_SDR_OVERFLOW;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "acquireReadBuffer: Acquiring user buffer %u", _handle);

    handle = _handle;

    uint32_t buf_valid_size = s->channels[0]->buffers[handle]->valid_size;
    uint64_t ticks = s->channels[0]->buffers[handle]->time;

    for(auto &c : s->channels)
    {
        std::lock_guard<std::mutex> lock(c->mutex);

        *buffs++ = c->buffers[handle]->addr;

        if(buf_valid_size != c->buffers[handle]->valid_size)
            DLOGF(SOAPY_SDR_WARNING, "acquireReadBuffer: Buffer size mismatch (expected %u, got %u)", buf_valid_size, c->buffers[handle]->valid_size);

        buf_valid_size = MIN(buf_valid_size, c->buffers[handle]->valid_size);

        if(c->buffers[handle]->time_valid)
        {
            if(ticks != c->buffers[handle]->time)
                DLOGF(SOAPY_SDR_WARNING, "acquireReadBuffer: Buffer time mismatch (expected %llu, got %llu)", ticks, c->buffers[handle]->time);
            else
                ticks = MIN(ticks, c->buffers[handle]->time);

            flags |= SOAPY_SDR_HAS_TIME;
        }
        else
        {
            flags &= ~SOAPY_SDR_HAS_TIME;
        }

        c->buffers[handle]->acquired = true;

        c->next_user_buf = (handle + 1) % c->buffers.size();
    }

    if(flags & SOAPY_SDR_HAS_TIME)
        timeNs = SoapySDR::ticksToTimeNs(ticks, this->getSampleRate(SOAPY_SDR_RX, 0));

    return buf_valid_size / ICYRADIO_SAMPLE_SIZE_BYTES;
}
void SoapyIcyRadio::releaseReadBuffer(SoapySDR::Stream *stream, const size_t handle)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("releaseReadBuffer: Stream not found");

    if(s->direction != SOAPY_SDR_RX)
        return;

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "releaseReadBuffer: Stream is not active");

        return;
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "releaseReadBuffer: No channels in this stream");

        return;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: Releasing user buffer %u", handle);

    bool any_dma_idle = false;

    // Check if any of the DMAs is idle, if so, we need to re-set the next dma buffer in order to keep timestamps synced
    for(auto &c : s->channels)
    {
        std::lock_guard<std::mutex> lock(c->mutex);

        if(c->dma->idle())
        {
            DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: DMA Controller %u of channel %u is idle", c->dma->getPeripheralID(), c->num);

            any_dma_idle = true;

            break;
        }
    }

    if(any_dma_idle)
    {
        DLOGF(SOAPY_SDR_WARNING, "releaseReadBuffer: At least one DMA Controller is idle");

        if(s->channels.size() > 1)
        {
            // We need to disable RX (both channels) and re-enable after, to re-sync the timestamps
            DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: Re-syncing multichannel timestamps - disable RX and RX counters");

            this->axi_rf_tstamp->disableRXCounter(); // Atomically disables both counters
            this->axi_rf_tstamp->disableRX(); // Atomically disables both channels
            this->axi_rf_tstamp->waitRXCounterDisabled(100);
            this->axi_rf_tstamp->waitRXDisabled(100);

            while(this->axi_rf_tstamp->getCounterLatchStatus(AXIRFTStamp::Channel::RX0) != AXIRFTStamp::CounterLatchStatus::LATCH_ARMED)
                this->axi_rf_tstamp->getCounterLatch(AXIRFTStamp::Channel::RX0); // Trigger read to clear valid status

            while(this->axi_rf_tstamp->getCounterLatchStatus(AXIRFTStamp::Channel::RX1) != AXIRFTStamp::CounterLatchStatus::LATCH_ARMED)
                this->axi_rf_tstamp->getCounterLatch(AXIRFTStamp::Channel::RX1); // Trigger read to clear valid status
        }

        for(auto &c : s->channels)
        {
            // No mutex locking here since we need the callbacks to run to flush the finished transfers
            c->dma->disable();
            c->dma->waitIdle();

            std::lock_guard<std::mutex> lock(c->mutex);

            DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: Channel %u, invalidate %u user buffers", c->num, c->buffers.size());

            for(auto &buf : c->buffers)
            {
                buf->valid_size = 0;
                buf->time = 0;
                buf->time_valid = false;
                buf->acquired = false;
            }

            c->next_user_buf = 0;
            c->next_dma_user_buf = 0;
            c->next_dma_user_buf_time = 0;
            c->next_dma_user_buf_time_valid = false;

            DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: DMA Controller %u", c->dma->getPeripheralID());

            c->dma->enable();

            for(auto &buf : c->dma_buffers)
            {
                c->dma->submitTransfer(buf->xfer);

                buf->idle = false;

                DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: Submitted transfer ID %u", buf->xfer.id);
            }
        }

        if(s->channels.size() > 1)
        {
            DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: Re-syncing multichannel timestamps - wait for DMAs and enable RX");

            // Only check the last channel, the first will be set aswell
            this->axi_rf_tstamp->waitRXDMAReady(s->channels.end()[-1]->ts_chan, 100);

            this->axi_rf_tstamp->enableRX(); // Atomically enables both channels
        }
    }
    else
    {
        DLOGF_S(SOAPY_SDR_TRACE, "releaseReadBuffer: No DMA Controller is idle, stream is healthy");

        for(auto &c : s->channels)
        {
            std::unique_lock<std::mutex> lock(c->mutex);

            if(handle >= c->buffers.size())
                DLOGF(SOAPY_SDR_WARNING, "releaseReadBuffer: Invalid buffer %u for channel %u", handle, c->num);

            if(!c->buffers[handle]->acquired)
            {
                DLOGF(SOAPY_SDR_WARNING, "releaseReadBuffer: Buffer %u for channel %u is not acquired", handle, c->num);

                return;
            }

            if(!c->buffers[handle]->valid_size)
            {
                DLOGF(SOAPY_SDR_WARNING, "releaseReadBuffer: Buffer %u for channel %u was acquired with no valid data", handle, c->num);

                return;
            }

            c->buffers[handle]->valid_size = 0;
            c->buffers[handle]->time_valid = false;
            c->buffers[handle]->acquired = false;
        }
    }
}
int SoapyIcyRadio::acquireWriteBuffer(SoapySDR::Stream *stream, size_t &handle, void **buffs, const long timeoutUs)
{
    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("acquireWriteBuffer: Stream not found");

    if(s->direction != SOAPY_SDR_TX)
        return SOAPY_SDR_NOT_SUPPORTED;

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "acquireWriteBuffer: Stream is not active");

        return SOAPY_SDR_STREAM_ERROR;
    }

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "acquireWriteBuffer: No channels in this stream");

        return SOAPY_SDR_STREAM_ERROR;
    }

    size_t _handle;
    long _timeoutUs = !timeoutUs ? 1 : timeoutUs;

    while(_timeoutUs)
    {
        bool all_acquired = true;

        _handle = s->channels[0]->next_user_buf;

        for(const auto &c : s->channels)
        {
            std::lock_guard<std::mutex> lock(c->mutex);

            if(_handle != c->next_user_buf)
            {
                all_acquired = false;

                break;
            }

            if(c->buffers[_handle]->valid_size)
            {
                all_acquired = false;

                break;
            }
        }

        if(all_acquired)
            break;

        usleep(10);

        if(_timeoutUs <= 10)
        {
            if(timeoutUs > 0)
                DLOGF(SOAPY_SDR_WARNING, "acquireWriteBuffer: Timeout waiting for a buffer with free space");

            return SOAPY_SDR_TIMEOUT;
        }

        _timeoutUs -= 10;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "acquireWriteBuffer: Acquiring user buffer %u", _handle);

    handle = _handle;

    uint32_t buf_size = s->channels[0]->buffers[handle]->size;

    for(auto &c : s->channels)
    {
        std::lock_guard<std::mutex> lock(c->mutex);

        *buffs++ = c->buffers[handle]->addr;

        if(buf_size != c->buffers[handle]->size)
            DLOGF(SOAPY_SDR_WARNING, "acquireWriteBuffer: Buffer size mismatch (expected %u, got %u)", buf_size, c->buffers[handle]->size);

        buf_size = MIN(buf_size, c->buffers[handle]->size);

        c->buffers[handle]->acquired = true;

        c->next_user_buf = (handle + 1) % c->buffers.size();
    }

    return buf_size / ICYRADIO_SAMPLE_SIZE_BYTES;
}
void SoapyIcyRadio::releaseWriteBuffer(SoapySDR::Stream *stream, const size_t handle, const size_t numElems, int &flags, const long long timeNs)
{
    if(!numElems)
        throw std::invalid_argument("releaseWriteBuffer: numElems must be greater than zero");

    SoapyIcyRadio::Stream *s = this->findStream(stream);

    if(s == nullptr)
        throw std::runtime_error("releaseWriteBuffer: Stream not found");

    if(s->direction != SOAPY_SDR_TX)
        return;

    std::lock_guard<std::mutex> lock(s->mutex);

    if(!s->active)
    {
        DLOGF(SOAPY_SDR_ERROR, "releaseWriteBuffer: Stream is not active");

        return;
    }

    uint64_t ticks = 0;

    if(flags & SOAPY_SDR_HAS_TIME)
        ticks = SoapySDR::timeNsToTicks(timeNs, this->getSampleRate(SOAPY_SDR_TX, 0));

    if(s->channels.empty())
    {
        DLOGF(SOAPY_SDR_ERROR, "releaseWriteBuffer: No channels in this stream");

        return;
    }

    DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Releasing user buffer %u", handle);

    bool any_dma_disabled = false;

    // Check if any of the DMAs is disabled, meaning we had an underflow and the ISR disabled them
    for(auto &c : s->channels)
    {
        std::lock_guard<std::mutex> lock(c->mutex);

        if(!c->dma->enabled())
        {
            DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: DMA Controller %u of channel %u is disabled", c->dma->getPeripheralID(), c->num);

            any_dma_disabled = true;

            break;
        }
    }

    if(any_dma_disabled)
    {
        DLOGF(SOAPY_SDR_WARNING, "releaseWriteBuffer: At least one DMA Controller is disabled, waiting for all to be");

        for(auto &c : s->channels)
        {
            // No mutex locking here since we need the callbacks to run to flush the finished transfers
            c->dma->waitIdle();

            std::lock_guard<std::mutex> lock(c->mutex);

            this->axi_rf_tstamp->disableTXCounter(c->ts_chan);
            this->axi_rf_tstamp->disableTX(c->ts_chan);
            this->axi_rf_tstamp->waitTXCounterDisabled(c->ts_chan, 100);
            this->axi_rf_tstamp->waitTXDisabled(c->ts_chan, 100);

            c->dma->disable();

            for(auto &buf : c->dma_buffers)
                buf->idle = true;
        }

        for(auto &c : s->channels)
        {
            std::lock_guard<std::mutex> lock(c->mutex);

            c->buffers[handle]->valid_size = 0;
            c->buffers[handle]->time_valid = false;
            c->buffers[handle]->acquired = false;

            DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Channel %u, DMA Controller %u", c->num, c->dma->getPeripheralID());

            c->next_dma_user_buf = (handle + 1) % c->buffers.size();

            std::memcpy(c->dma_buffers[0]->virt, c->buffers[handle]->addr, numElems * ICYRADIO_SAMPLE_SIZE_BYTES);

            c->dma_buffers[0]->xfer.size = numElems * ICYRADIO_SAMPLE_SIZE_BYTES;

            c->dma->enable();
            c->dma->submitTransfer(c->dma_buffers[0]->xfer);

            c->dma_buffers[0]->idle = false;

            DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Submitted transfer ID %u", c->dma_buffers[0]->xfer.id);

            if(flags & SOAPY_SDR_HAS_TIME)
                this->axi_rf_tstamp->setTXCounter(c->ts_chan, ticks);

            if(s->channels.size() > 1)
                continue;

            DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Single channel, just enable TX/counter");

            if(flags & SOAPY_SDR_HAS_TIME)
            {
                uint64_t hw_time_now = this->axi_rf_tstamp->getCounter(); // Hardware time now

                if(ticks > hw_time_now)
                {
                    DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Buffer time %llu is valid and not late, disabling TX and arming trigger", ticks);

                    this->axi_rf_tstamp->enableTXCounter(c->ts_chan);
                }
                else
                {
                    DLOGF(SOAPY_SDR_WARNING, "releaseWriteBuffer: Buffer time %llu is late (hw time is %llu), not arming trigger, enabling TX now", ticks, hw_time_now);

                    DLOGF_S(SOAPY_SDR_SSI, "L");

                    this->axi_rf_tstamp->enableTX(c->ts_chan);
                }
            }
            else
            {
                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Buffer does not have time, enabling TX now");

                this->axi_rf_tstamp->enableTX(c->ts_chan);
            }
        }

        if(s->channels.size() > 1)
        {
            DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Syncing multichannel timestamps - wait for DMAs and enable TX/counters");

            // Only check the last channel, the first will be set aswell
            this->axi_rf_tstamp->waitTXDMAReady(s->channels.end()[-1]->ts_chan, 100);

            if(flags & SOAPY_SDR_HAS_TIME)
            {
                uint64_t hw_time_now = this->axi_rf_tstamp->getCounter(); // Hardware time now

                if(ticks > hw_time_now)
                {
                    DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Buffer time %llu is valid and not late, disabling TX and arming trigger", ticks);

                    this->axi_rf_tstamp->enableTXCounter(); // Atomically enables both counters
                }
                else
                {
                    DLOGF(SOAPY_SDR_WARNING, "releaseWriteBuffer: Buffer time %llu is late (hw time is %llu), not arming trigger, enabling TX now", ticks, hw_time_now);

                    DLOGF_S(SOAPY_SDR_SSI, "L");

                    this->axi_rf_tstamp->enableTX(); // Atomically enables both channels
                }
            }
            else
            {
                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Buffer does not have time, enabling TX now");

                this->axi_rf_tstamp->enableTX(); // Atomically enables both channels
            }
        }
    }
    else
    {
        DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: No DMA Controller is disabled, stream is healthy");

        for(auto &c : s->channels)
        {
            std::unique_lock<std::mutex> lock(c->mutex);

            if(handle >= c->buffers.size())
                DLOGF(SOAPY_SDR_WARNING, "releaseWriteBuffer: Invalid buffer %u for channel %u", handle, c->num);

            if(!c->buffers[handle]->acquired)
            {
                DLOGF(SOAPY_SDR_WARNING, "releaseWriteBuffer: Buffer %u for channel %u is not acquired", handle, c->num);

                return;
            }

            if(c->buffers[handle]->valid_size)
            {
                DLOGF(SOAPY_SDR_WARNING, "releaseReadBuffer: Buffer %u for channel %u was acquired with valid data", handle, c->num);

                return;
            }

            bool submit_xfer = true;

            if(c->next_dma_user_buf != handle)
            {
                submit_xfer = false;

                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Buffer %u for channel %u is not the next one in the queue, not submitting transfer", handle, c->num);
            }
            else if(flags & SOAPY_SDR_HAS_TIME)
            {
                submit_xfer = false;

                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Buffer has time, not submitting transfer");
            }

            SoapyIcyRadio::Stream::Channel::DMABuffer *dma_buf = nullptr;

            if(submit_xfer)
            {
                // Find a possible free DMA buffer
                for(auto &buf : c->dma_buffers)
                {
                    if(buf->idle)
                    {
                        dma_buf = buf;

                        break;
                    }
                }

                if(!dma_buf)
                {
                    submit_xfer = false;

                    DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: DMA Controller %u has no transfer slot available, not submitting transfer", c->dma->getPeripheralID());
                }
            }

            if(submit_xfer)
            {
                // Invalidate the user buffer and return it to the pool
                c->buffers[handle]->valid_size = 0;
                c->buffers[handle]->time_valid = false;
                c->buffers[handle]->acquired = false;

                // Submit the data directly to the DMA
                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Submitting transfer for channel %u on DMA Controller %u", c->num, c->dma->getPeripheralID());

                c->next_dma_user_buf = (handle + 1) % c->buffers.size();

                std::memcpy(dma_buf->virt, c->buffers[handle]->addr, numElems * ICYRADIO_SAMPLE_SIZE_BYTES);

                dma_buf->xfer.size = numElems * ICYRADIO_SAMPLE_SIZE_BYTES;

                c->dma->submitTransfer(dma_buf->xfer);

                dma_buf->idle = false;

                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Submitted transfer ID %u", dma_buf->xfer.id);
            }
            else
            {
                // Add the buffer to the pool
                DLOGF_S(SOAPY_SDR_TRACE, "releaseWriteBuffer: Adding buffer to channel %u pool", c->num);

                c->buffers[handle]->valid_size = numElems * ICYRADIO_SAMPLE_SIZE_BYTES;

                if(flags & SOAPY_SDR_HAS_TIME)
                {
                    c->buffers[handle]->time = ticks;
                    c->buffers[handle]->time_valid = true;
                }
                else
                {
                    c->buffers[handle]->time = 0;
                    c->buffers[handle]->time_valid = false;
                }

                c->buffers[handle]->acquired = false;
            }
        }
    }
}