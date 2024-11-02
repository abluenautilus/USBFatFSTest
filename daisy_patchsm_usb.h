#include "daisy.h"
#include "daisy_patch_sm.h"
#include "fatfs.h"

using namespace daisy;
using namespace patch_sm;

class DaisyPatchSM_USB: public DaisyPatchSM {

    public:
        daisy::USBHostHandle usbHost;
        daisy::FatFSInterface fatfs_interface;

        void PrepareMedia(
            daisy::USBHostHandle::ConnectCallback     connect_cb      = nullptr,
            daisy::USBHostHandle::DisconnectCallback  disconnect_cb   = nullptr,
            daisy::USBHostHandle::ClassActiveCallback class_active_cb = nullptr,
            daisy::USBHostHandle::ErrorCallback       error_cb        = nullptr,
            void                                     *userdata        = nullptr)
            {
                /** Initialize hardware and set user callbacks */
                daisy::USBHostHandle::Config usbcfg;
                usbcfg.connect_callback      = connect_cb;
                usbcfg.disconnect_callback   = disconnect_cb;
                usbcfg.class_active_callback = class_active_cb;
                usbcfg.error_callback        = error_cb;
                usbcfg.userdata              = userdata;
                usbHost.Init(usbcfg);

                /** Prepare FatFS -- fmount will defer until first attempt to read/write */
                daisy::FatFSInterface::Config fsi_cfg;
                fsi_cfg.media = daisy::FatFSInterface::Config::MEDIA_USB;
                fatfs_interface.Init(fsi_cfg);
                f_mount(&fatfs_interface.GetUSBFileSystem(),
                        fatfs_interface.GetUSBPath(),
                        0);

            }
};