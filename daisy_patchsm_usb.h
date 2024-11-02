#include "daisy.h"
#include "daisy_patch_sm.h"
#include "fatfs.h"

/*

    Extension of DaisyPatchSM Class to include USB Host handling

*/

using namespace daisy;
using namespace patch_sm;

class DaisyPatchSM_USB: public DaisyPatchSM {

    public:
        daisy::USBHostHandle usbHost;

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

            }
};