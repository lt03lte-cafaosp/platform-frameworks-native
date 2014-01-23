/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/Timers.h>

#include <binder/Parcel.h>
#include <binder/IInterface.h>

#include <gui/IGraphicBufferProducer.h>

namespace android {
// ----------------------------------------------------------------------------

enum {
    REQUEST_BUFFER = IBinder::FIRST_CALL_TRANSACTION,
    SET_BUFFER_COUNT,
    DEQUEUE_BUFFER,
    QUEUE_BUFFER,
    CANCEL_BUFFER,
    QUERY,
    SET_SYNCHRONOUS_MODE,
    SET_BUFFERS_SIZE,
    CONNECT,
    DISCONNECT,
    UPDATE_BUFFERS_GEOMETRY,
};


class BpGraphicBufferProducer : public BpInterface<IGraphicBufferProducer>
{
public:
    BpGraphicBufferProducer(const sp<IBinder>& impl)
        : BpInterface<IGraphicBufferProducer>(impl)
    {
    }

    virtual status_t requestBuffer(int bufferIdx, sp<GraphicBuffer>* buf) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(bufferIdx);
        status_t result =remote()->transact(REQUEST_BUFFER, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        bool nonNull = reply.readInt32();
        if (nonNull) {
            *buf = new GraphicBuffer();
            reply.read(**buf);
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t setBufferCount(int bufferCount)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(bufferCount);
        status_t result =remote()->transact(SET_BUFFER_COUNT, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t dequeueBuffer(int *buf, sp<Fence>* fence,
            uint32_t w, uint32_t h, uint32_t format, uint32_t usage) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(w);
        data.writeInt32(h);
        data.writeInt32(format);
        data.writeInt32(usage);
        status_t result = remote()->transact(DEQUEUE_BUFFER, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        *buf = reply.readInt32();
        bool fenceWasWritten = reply.readInt32();
        if (fenceWasWritten) {
            // If the fence was written by the callee, then overwrite the
            // caller's fence here.  If it wasn't written then don't touch the
            // caller's fence.
            *fence = new Fence();
            reply.read(*(fence->get()));
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t queueBuffer(int buf,
            const QueueBufferInput& input, QueueBufferOutput* output) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(buf);
        data.write(input);
        status_t result = remote()->transact(QUEUE_BUFFER, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        const void *out_data =reply.readInplace(sizeof(*output));
        if(out_data != NULL) {
            memcpy(output, out_data, sizeof(*output));
        } else {
            return BAD_VALUE;
        }
        result = reply.readInt32();
        return result;
    }

    virtual void cancelBuffer(int buf, const sp<Fence>& fence) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(buf);
        data.write(*fence.get());
        remote()->transact(CANCEL_BUFFER, data, &reply);
    }

    virtual int query(int what, int* value) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(what);
        status_t result = remote()->transact(QUERY, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        value[0] = reply.readInt32();
        result = reply.readInt32();
        return result;
    }

    virtual status_t setSynchronousMode(bool enabled) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(enabled);
        status_t result = remote()->transact(SET_SYNCHRONOUS_MODE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t connect(int api, QueueBufferOutput* output) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(api);
        status_t result = remote()->transact(CONNECT, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        const void *out_data =reply.readInplace(sizeof(*output));
        if(out_data != NULL) {
            memcpy(output, out_data, sizeof(*output));
        } else {
            return BAD_VALUE;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t disconnect(int api) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(api);
        status_t result =remote()->transact(DISCONNECT, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t setBuffersSize(int size) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(size);
        status_t result = remote()->transact(SET_BUFFERS_SIZE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t updateBuffersGeometry(int w, int h, int f) {
        Parcel data, reply;
        data.writeInterfaceToken(IGraphicBufferProducer::getInterfaceDescriptor());
        data.writeInt32(w);
        data.writeInt32(h);
        data.writeInt32(f);
        status_t result = remote()->transact(UPDATE_BUFFERS_GEOMETRY,
                                              data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

};

IMPLEMENT_META_INTERFACE(GraphicBufferProducer, "android.gui.IGraphicBufferProducer");

// ----------------------------------------------------------------------

status_t BnGraphicBufferProducer::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case REQUEST_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int bufferIdx   = data.readInt32();
            sp<GraphicBuffer> buffer;
            int result = requestBuffer(bufferIdx, &buffer);
            reply->writeInt32(buffer != 0);
            if (buffer != 0) {
                reply->write(*buffer);
            }
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_BUFFER_COUNT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int bufferCount = data.readInt32();
            int result = setBufferCount(bufferCount);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case DEQUEUE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            uint32_t w      = data.readInt32();
            uint32_t h      = data.readInt32();
            uint32_t format = data.readInt32();
            uint32_t usage  = data.readInt32();
            int buf;
            sp<Fence> fence;
            int result = dequeueBuffer(&buf, &fence, w, h, format, usage);
            reply->writeInt32(buf);
            reply->writeInt32(fence != NULL);
            if (fence != NULL) {
                reply->write(*fence.get());
            }
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case QUEUE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int buf = data.readInt32();
            QueueBufferInput input(data);
            QueueBufferOutput* const output =
                    reinterpret_cast<QueueBufferOutput *>(
                            reply->writeInplace(sizeof(QueueBufferOutput)));
            status_t result = queueBuffer(buf, input, output);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case CANCEL_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int buf = data.readInt32();
            sp<Fence> fence = new Fence();
            data.read(*fence.get());
            cancelBuffer(buf, fence);
            return NO_ERROR;
        } break;
        case QUERY: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int value;
            int what = data.readInt32();
            int res = query(what, &value);
            reply->writeInt32(value);
            reply->writeInt32(res);
            return NO_ERROR;
        } break;
        case SET_SYNCHRONOUS_MODE: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            bool enabled = data.readInt32();
            status_t res = setSynchronousMode(enabled);
            reply->writeInt32(res);
            return NO_ERROR;
        } break;
        case SET_BUFFERS_SIZE: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int size = data.readInt32();
            status_t res = setBuffersSize(size);
            reply->writeInt32(res);
            return NO_ERROR;
        } break;
        case CONNECT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int api = data.readInt32();
            QueueBufferOutput* const output =
                    reinterpret_cast<QueueBufferOutput *>(
                            reply->writeInplace(sizeof(QueueBufferOutput)));
            status_t res = connect(api, output);
            reply->writeInt32(res);
            return NO_ERROR;
        } break;
        case DISCONNECT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int api = data.readInt32();
            status_t res = disconnect(api);
            reply->writeInt32(res);
            return NO_ERROR;
        } break;
        case UPDATE_BUFFERS_GEOMETRY: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            int w = data.readInt32();
            int h = data.readInt32();
            int f = data.readInt32();
            status_t res = updateBuffersGeometry(w, h, f);
            reply->writeInt32(res);
            return NO_ERROR;
        } break;
    }
    return BBinder::onTransact(code, data, reply, flags);
}

// ----------------------------------------------------------------------------

IGraphicBufferProducer::QueueBufferInput::QueueBufferInput(const Parcel& parcel) {
    parcel.read(*this);
}

size_t IGraphicBufferProducer::QueueBufferInput::getFlattenedSize() const
{
    return sizeof(timestamp)
         + sizeof(crop)
         + sizeof(scalingMode)
         + sizeof(transform)
         + fence->getFlattenedSize();
}

size_t IGraphicBufferProducer::QueueBufferInput::getFdCount() const
{
    return fence->getFdCount();
}

status_t IGraphicBufferProducer::QueueBufferInput::flatten(void* buffer, size_t size,
        int fds[], size_t count) const
{
    status_t err = NO_ERROR;
    char* p = (char*)buffer;
    memcpy(p, &timestamp,   sizeof(timestamp));   p += sizeof(timestamp);
    memcpy(p, &crop,        sizeof(crop));        p += sizeof(crop);
    memcpy(p, &scalingMode, sizeof(scalingMode)); p += sizeof(scalingMode);
    memcpy(p, &transform,   sizeof(transform));   p += sizeof(transform);
    err = fence->flatten(p, size - (p - (char*)buffer), fds, count);
    return err;
}

status_t IGraphicBufferProducer::QueueBufferInput::unflatten(void const* buffer,
        size_t size, int fds[], size_t count)
{
    status_t err = NO_ERROR;
    const char* p = (const char*)buffer;
    memcpy(&timestamp,   p, sizeof(timestamp));   p += sizeof(timestamp);
    memcpy(&crop,        p, sizeof(crop));        p += sizeof(crop);
    memcpy(&scalingMode, p, sizeof(scalingMode)); p += sizeof(scalingMode);
    memcpy(&transform,   p, sizeof(transform));   p += sizeof(transform);
    fence = new Fence();
    err = fence->unflatten(p, size - (p - (const char*)buffer), fds, count);
    return err;
}

}; // namespace android
