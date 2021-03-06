/*
**
** Copyright 2009, The Android-x86 Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Author: Niels Keeman <nielskeeman@gmail.com>
**
*/

#ifndef ANDROID_HARDWARE_CAMERA_HARDWARE_H
#define ANDROID_HARDWARE_CAMERA_HARDWARE_H

#include <utils/threads.h>
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/threads.h>

#include <utils/threads.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include <utils/threads.h>
#include <linux/videodev2.h>
#include "binder/MemoryBase.h"
#include "binder/MemoryHeapBase.h"
#include <utils/threads.h>
#include <camera/CameraParameters.h>
#include <hardware/camera.h>
#include <sys/ioctl.h>
#include <utils/threads.h>
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/threads.h>
#include "V4L2Camera.h"

#include <hardware/camera.h>

#include <sys/ioctl.h>
#include "V4L2Camera.h"

namespace android {

class CameraHardware  {
public:
    virtual sp<IMemoryHeap> getPreviewHeap() const;
    virtual sp<IMemoryHeap> getRawHeap() const;

    virtual status_t    startPreview();
    virtual void        setCallbacks(camera_notify_callback notify_cb,
                                     camera_data_callback data_cb,
                                     camera_data_timestamp_callback data_cb_timestamp,
                                     camera_request_memory get_memory,
                                     void* arg);
    /**
     * Enable a message, or set of messages.
     */
    virtual void        enableMsgType(int32_t msgType);

    virtual int setPreviewWindow( struct preview_stream_ops *window);
    /**
     * Disable a message, or a set of messages.
     */
    virtual void        disableMsgType(int32_t msgType);

    /**
     * Query whether a message, or a set of messages, is enabled.
     * Note that this is operates as an AND, if any of the messages
     * queried are off, this will return false.
     */
    virtual bool        msgTypeEnabled(int32_t msgType);

    virtual void        stopPreview();
    virtual bool        previewEnabled();

    virtual status_t    startRecording();
    virtual void        stopRecording();
    virtual bool        recordingEnabled();
    virtual void        releaseRecordingFrame(const void* opaque);

    virtual status_t    autoFocus();
    virtual status_t    cancelAutoFocus();
    virtual status_t    takePicture();
    virtual status_t    cancelPicture();
    virtual status_t    dump(int fd, const Vector<String16>& args) const;
    virtual status_t    setParameters(const CameraParameters& params);
    virtual CameraParameters  getParameters() const;
    virtual void release();
    virtual status_t sendCommand(int32_t cmd, int32_t arg1, int32_t arg2);
                        CameraHardware(int cameraId);

    virtual             ~CameraHardware();

private:


    static const int kBufferCount = 4;

    class PreviewThread : public Thread {
        CameraHardware* mHardware;
    public:
        PreviewThread(CameraHardware* hw)
            : Thread(false), mHardware(hw) { }
        virtual void onFirstRef() {
            run("CameraPreviewThread", PRIORITY_URGENT_DISPLAY);
        }
        virtual bool threadLoop() {
            mHardware->previewThread();
            // loop until we need to quit
            return true;
        }
    };

    void initDefaultParameters();
    bool initHeapLocked();

    int previewThread();

    static int beginAutoFocusThread(void *cookie);
    int autoFocusThread();

    static int beginPictureThread(void *cookie);
    int pictureThread();
    camera_request_memory   mRequestMemory;
    mutable Mutex           mLock;
    preview_stream_ops_t*  mNativeWindow;

    int                     mCameraId;
    CameraParameters        mParameters;

    sp<MemoryHeapBase>      mHeap;
    sp<MemoryBase>          mBuffer;

    sp<MemoryHeapBase>      mPreviewHeap;
    sp<MemoryHeapBase>      mRawHeap;
    sp<MemoryHeapBase>      mRecordHeap;
    sp<MemoryBase>          mRecordBuffer;

    bool                    mPreviewRunning;
    bool                    mRecordRunning;
    int                     mPreviewFrameSize;

    // protected by mLock
    sp<PreviewThread>       mPreviewThread;

    // only used from PreviewThread
    int                     mCurrentPreviewFrame;

    void *                  framebuffer;
    bool                    previewStopped;
    int                     camera_device;
    void*                   mem[4];
    int                     nQueued;
    int                     nDequeued;
    V4L2Camera              camera;
    camera_notify_callback         mNotifyFn;
    camera_data_callback           mDataFn;
    camera_data_timestamp_callback mTimestampFn;
    void*                   mUser;
    int32_t                 mMsgEnabled;

};

}; // namespace android

#endif
